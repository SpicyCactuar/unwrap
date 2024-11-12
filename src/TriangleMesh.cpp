#include "TriangleMesh.h"

#include <iostream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "Cartesian3.h"

#define MAXIMUM_LINE_LENGTH 1024

#define N_ITERATIONS 100000

#define PREVIOUS_EDGE(x) ((x) % 3) ? ((x) - 1) : ((x) + 2)
#define NEXT_EDGE(x) (((x) % 3) == 2) ? ((x) - 2) : ((x) + 1)

constexpr unsigned int NO_SUCH_ELEMENT = std::numeric_limits<unsigned int>::max();

TriangleMesh::TriangleMesh()
    : centreOfGravity(0.0f, 0.0f, 0.0f),
      objectSize(0.0f) {
    vertices.clear();
    colours.clear();
    normals.clear();
    textureCoords.clear();
    firstDirectedEdge.clear();
    faceVertices.clear();
    otherHalf.clear();
    boundary.clear();
    boundaryVertices.clear();
    boundaryVerticesLookup.clear();
}

void TriangleMesh::computeCentreOfGravity() {
    // compute centre of gravity
    centreOfGravity = {0.0f, 0.0f, 0.0f};

    if (vertices.empty()) {
        return;
    }

    // accumulate vertex displacements
    for (const auto& vertex : vertices) {
        centreOfGravity = centreOfGravity + vertex;
    }

    // divide through by the number to get the barycentre
    centreOfGravity = centreOfGravity / vertices.size();

    // start with 0 radius
    objectSize = 0.0f;

    // now compute the largest distance from the origin to a vertex
    for (const auto& vertex : vertices) {
        const float distance = (vertex - centreOfGravity).length();

        if (distance > objectSize) {
            objectSize = distance;
        }
    }
}

bool TriangleMesh::readFromObjFile(std::istream& objFile) {
    while (!objFile.eof()) {
        std::string token;
        objFile >> token;

        // Switch on the string token
        if (token == "#") {
            // Comment line, consume
            objFile.ignore(MAXIMUM_LINE_LENGTH, '\n');
        } else if (token == "v") {
            // Vertex read
            Cartesian3 vertex;
            objFile >> vertex;
            vertices.push_back(vertex);
        } else if (token == "vc") {
            // Colour read
            Cartesian3 colour;
            objFile >> colour;
            colours.push_back(colour);
        } else if (token == "vn") {
            // Normal read
            Cartesian3 normal;
            objFile >> normal;
            normals.push_back(normal);
        } else if (token == "vt") {
            // Texture coordinate read
            Cartesian3 texCoord;
            objFile >> texCoord;
            textureCoords.push_back(texCoord);
        } else if (token == "f") {
            // Face data, assume a single triangle per line
            unsigned int vertexId;
            for (unsigned int v = 0; v < 3; v++) {
                objFile >> vertexId;
                faceVertices.push_back(vertexId - 1);
            }
        }
    }

    centreOfGravity = Cartesian3(0.0, 0.0, 0.0);

    computeCentreOfGravity();
    computeHalfEdge();
    computeNormals();
    computeBoundary();

    return true;
}

void TriangleMesh::computeHalfEdge() {
    // Reset the half-edge structures
    firstDirectedEdge.assign(vertices.size(), NO_SUCH_ELEMENT);
    otherHalf.assign(faceVertices.size(), NO_SUCH_ELEMENT);

    // Compute otherHalf & firstDirectedEdge
    for (unsigned int edgeId = 0; edgeId < faceVertices.size(); edgeId++) {
        const EdgeId from = faceVertices[PREVIOUS_EDGE(edgeId)];
        const EdgeId to = faceVertices[edgeId];

        // Assign FDE if not present
        if (firstDirectedEdge[from] == NO_SUCH_ELEMENT) {
            firstDirectedEdge[from] = edgeId;
        }

        // Linear search for other half
        for (unsigned int otherEdgeId = 0; otherEdgeId < faceVertices.size(); otherEdgeId++) {
            const EdgeId otherFrom = faceVertices[PREVIOUS_EDGE(otherEdgeId)];
            const EdgeId otherTo = faceVertices[otherEdgeId];

            // Fails silently on non-manifold meshes
            if (from == otherTo && to == otherFrom) {
                otherHalf[edgeId] = otherEdgeId;
                otherHalf[otherEdgeId] = edgeId;
                break;
            }
        }
    }
}

void TriangleMesh::computeBoundary() {
    // Reset boundary
    boundary.clear();
    boundaryVertices.clear();
    boundaryVerticesLookup.clear();

    // Find first boundary edge
    // Hard assumptions: It always exists and it's unique
    long firstBoundaryEdge = NO_SUCH_ELEMENT;
    for (unsigned int edgeId = 0; edgeId < faceVertices.size(); edgeId++) {
        if (otherHalf[edgeId] == NO_SUCH_ELEMENT) {
            firstBoundaryEdge = edgeId;
            break;
        }
    }

    // Loop around boundary
    long boundaryEdge = firstBoundaryEdge;
    do {
        // Store boundary edge
        boundary.insert(boundaryEdge);

        // Collect boundary vertex
        boundaryVertices.push_back(faceVertices[boundaryEdge]);
        boundaryVerticesLookup.insert(faceVertices[boundaryEdge]);

        // Go around the neighbourhood until I hit the boundary
        while (otherHalf[NEXT_EDGE(boundaryEdge)] != NO_SUCH_ELEMENT) {
            boundaryEdge = otherHalf[NEXT_EDGE(boundaryEdge)];
        }

        // Move to next boundary neighbourhood
        boundaryEdge = NEXT_EDGE(boundaryEdge);
    } while (boundaryEdge != firstBoundaryEdge);
}

void TriangleMesh::unwrap() {
    std::cout << "Relabelling vertices for Floater's..." << std::endl;

    // Prepare vertices for Floater's
    const std::vector<VertexId> exteriorVertices = boundaryVertices;
    std::vector<VertexId> interiorVertices;
    for (VertexId v = 0; v < vertices.size(); v++) {
        // Skip non-connected vertices and boundary vertices
        // The latter are already computed in ComputeBoundary()
        if (firstDirectedEdge[v] == NO_SUCH_ELEMENT || isBoundaryVertex(v)) {
            continue;
        }

        interiorVertices.push_back(v);
    }

    // Re-label vertices
    const VertexId b = exteriorVertices.size();
    const VertexId n = interiorVertices.size();
    std::vector<Cartesian3> relabelledVertices(b + n);
    std::vector<Cartesian3> relaballedColours(b + n);
    std::vector<EdgeId> relabelledFirstDirectedEdge(b + n);
    std::unordered_map<VertexId, VertexId> floatersMapping;
    for (VertexId v = 0; v < relabelledVertices.size(); v++) {
        VertexId vertexId = v < b ? exteriorVertices[v] : interiorVertices[v - b];
        floatersMapping[vertexId] = v;
        relabelledVertices[v] = vertices[vertexId];
        relaballedColours[v] = colours[vertexId];
        relabelledFirstDirectedEdge[v] = firstDirectedEdge[vertexId];
    }

    // Re-label faceVertices
    std::vector<VertexId> relabelledFaceVertices = faceVertices;
    for (VertexId fv = 0; fv < faceVertices.size(); fv++) {
        relabelledFaceVertices[fv] = floatersMapping[faceVertices[fv]];
    }

    // Re-assign fields
    vertices = relabelledVertices;
    faceVertices = relabelledFaceVertices;
    colours = relaballedColours;
    firstDirectedEdge = relabelledFirstDirectedEdge;

    boundaryVertices.clear();
    boundaryVerticesLookup.clear();
    for (VertexId bv = 0; bv < exteriorVertices.size(); bv++) {
        boundaryVertices.push_back(bv);
        boundaryVerticesLookup.insert(bv);
    }

    std::cout << "Finished relabelling vertices" << std::endl;

    std::cout << "Running Floater's..." << std::endl;

    // Reset textureCoords
    textureCoords.resize(vertices.size(), {});

    // Fix exterior vertices onto the unit square [0..1] x [0..1]
    // Going around CCW: bottom, right, top, left
    // It's important to be consistent with the order of the boundary traversal
    for (unsigned int bv = 0; bv < b; bv++) {
        // Map [0..1) -> [0..4) to match unit square perimeter
        const float t = 4.0f * bv / b;

        if (t < 1.0f) {
            // First segment: x = {0..1} and y = 0
            // t = [0..1) => f(t) = (t, 0)
            textureCoords[bv] = Cartesian3(t, 0.0f, 0.0f);
        } else if (t < 2.0f) {
            // Second segment: x = 1 and y = {0..1}
            // t = [1..2) => f(t) = (1, t - 1)
            textureCoords[bv] = Cartesian3(1.0f, t - 1.0f, 0.0f);
        } else if (t < 3.0f) {
            // Third segment: x = {1..0} and y = 1
            // t = [2..3) => f(t) = (1 - (t - 2), 1)
            textureCoords[bv] = Cartesian3(1.0f - (t - 2.0f), 1.0f, 0.0f);
        } else {
            // Fourth segment: x = 0 and y = {1..0}
            // t = [3..4) => f(t) = (0, 1 - (t - 3))
            textureCoords[bv] = Cartesian3(0.0f, 1.0f - (t - 3.0f), 0.0f);
        }
    }

    // Initially, set all internal vertices to centre of the texture
    // In addition, compute neighbourhood of each internal vertex
    std::vector<std::vector<VertexId>> neighbourhoods;
    for (VertexId iv = b; iv < (b + n); iv++) {
        textureCoords[iv] = {0.5f, 0.5f, 0.0f};
        neighbourhoods.push_back(interiorNeighbourhoodOf(iv));
    }

    // Compute internal vertices UVs using Floater's loop
    for (unsigned int i = 0; i < N_ITERATIONS; i++) {
        for (VertexId iv = b; iv < (b + n); iv++) {
            Cartesian3 weightedUv;
            const auto& neighbourhood = neighbourhoods[iv - b];

            for (const unsigned int neighbour : neighbourhood) {
                weightedUv = weightedUv + textureCoords[neighbour];
            }
            weightedUv = weightedUv / neighbourhood.size();

            textureCoords[iv] = weightedUv;
        }
    }

    std::cout << "Finished Floater's" << std::endl;
}

void TriangleMesh::computeNormals() {
    // Based on https://iquilezles.org/articles/normals/
    normals.assign(vertices.size(), {0.0f, 0.0f, 0.0f});

    for (unsigned int f = 0; f < faceVertices.size(); f += 3) {
        const VertexId v0 = faceVertices[f];
        const VertexId v1 = faceVertices[f + 1];
        const VertexId v2 = faceVertices[f + 2];

        Cartesian3 e1 = vertices[v1] - vertices[v0];
        Cartesian3 e2 = vertices[v2] - vertices[v0];
        Cartesian3 n = e1.cross(e2);

        normals[v0] = normals[v0] + n;
        normals[v1] = normals[v1] + n;
        normals[v2] = normals[v2] + n;
    }

    for (unsigned int v = 0; v < vertices.size(); v++) {
        normals[v] = normals[v].unit();
    }
}

void TriangleMesh::writeToObjFile(std::ostream& objFile) const {
    objFile << "# " << faceVertices.size() / 3 << " triangles" << std::endl;
    objFile << std::endl;

    objFile << "# " << vertices.size() << " vertices" << std::endl;
    for (const auto& vertex : vertices) {
        objFile << "v  " << std::fixed << vertex << std::endl;
    }

    objFile << "# " << colours.size() << " vertex colours" << std::endl;
    for (const auto& colour : colours) {
        objFile << "vc " << std::fixed << colour << std::endl;
    }

    objFile << "# " << normals.size() << " vertex normals" << std::endl;
    for (const auto& normal : normals) {
        objFile << "vn " << std::fixed << normal << std::endl;
    }

    objFile << "# " << textureCoords.size() << " vertex tex coords" << std::endl;
    for (const auto& textureCoord : textureCoords) {
        objFile << "vt " << std::fixed << textureCoord << std::endl;
    }

    for (unsigned int face = 0; face < faceVertices.size(); face += 3) {
        objFile << "f";

        for (unsigned int vertex = 0; vertex < 3; vertex++) {
            objFile << " "
                    << faceVertices[face + vertex] + 1;
        }
        objFile << std::endl;
    }
}

bool TriangleMesh::isBoundaryEdge(const EdgeId edgeId) const {
    return boundary.find(edgeId) != boundary.end();
}

bool TriangleMesh::isBoundaryFace(const FaceIndex faceIndex) const {
    return isBoundaryEdge(faceIndex) ||
           isBoundaryEdge(faceIndex + 1) ||
           isBoundaryEdge(faceIndex + 2);
}

bool TriangleMesh::isBoundaryVertex(const VertexId vertexId) const {
    return boundaryVerticesLookup.find(vertexId) != boundaryVerticesLookup.end();
}

std::vector<unsigned int> TriangleMesh::interiorNeighbourhoodOf(const VertexId interiorVertexId) const {
    std::vector<unsigned int> neighbourhood;
    const EdgeId firstEdge = firstDirectedEdge[interiorVertexId];
    EdgeId currentEdge = firstEdge;

    do {
        neighbourhood.push_back(faceVertices[currentEdge]);
        currentEdge = NEXT_EDGE(otherHalf[currentEdge]);
    } while (otherHalf[currentEdge] != NO_SUCH_ELEMENT && currentEdge != firstEdge);

    return neighbourhood;
}
