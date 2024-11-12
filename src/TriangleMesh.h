#ifndef TRIANGLE_MESH
#define TRIANGLE_MESH

#include <unordered_set>
#include <vector>
#include <iostream>

#include "Cartesian3.h"

typedef unsigned int VertexId;
typedef unsigned int EdgeId;
typedef unsigned int FaceIndex;

class TriangleMesh {
public:
    std::vector<Cartesian3> vertices;
    // Holds rgb colours in [0.0..1.0] range
    std::vector<Cartesian3> colours;
    std::vector<Cartesian3> normals;
    // [u, v, 0] triplets
    std::vector<Cartesian3> textureCoords;
    std::vector<VertexId> faceVertices;
    std::vector<EdgeId> firstDirectedEdge;
    std::vector<EdgeId> otherHalf;

    Cartesian3 centreOfGravity;
    float objectSize;

    std::unordered_set<EdgeId> boundary;
    std::vector<VertexId> boundaryVertices;
    // Redundantly contains all of the boundaryVertices for O(1) lookup
    std::unordered_set<VertexId> boundaryVerticesLookup;

    TriangleMesh();

    bool readFromObjFile(std::istream& objFile);

    // Run Floater's Algorithm to generate textureCoords
    void unwrap();

    void writeToObjFile(std::ostream& objFile) const;

    // Returns true if any of its edges are boundary
    bool isBoundaryFace(FaceIndex faceIndex) const;

private:
    void computeCentreOfGravity();

    void computeHalfEdge();

    void computeBoundary();

    void computeNormals();

    // Returns true if edgeId is registered as boundary
    bool isBoundaryEdge(EdgeId edgeId) const;

    // Returns true if any of its incident edges are boundary, or if it's not connected
    bool isBoundaryVertex(VertexId vertexId) const;

    // Returns a vector containing all the neighbouring vertices of vertexId
    // Assumes the vertex is interior (ie: non-boundary)
    std::vector<VertexId> interiorNeighbourhoodOf(VertexId interiorVertexId) const;
};

#endif
