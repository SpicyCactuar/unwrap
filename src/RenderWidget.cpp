#include "RenderWidget.h"

#include <cmath>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#define REMAP_TO_UNIT_INTERVAL(x) (0.5 + (0.5 * x))
#define REMAP_FROM_UNIT_INTERVAL(x) (-1.0 + (2.0 * x))

RenderWidget::RenderWidget(
    TriangleMesh* triangleMesh,
    RenderParameters* renderParameters,
    QWidget* parent
) : QOpenGLWidget(parent),
    renderParameters(renderParameters),
    triangleMesh(triangleMesh) {
}

void RenderWidget::initializeGL() {
    // set lighting parameters (may be reset later)
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

    // background is yellowish-grey
    glClearColor(0.8, 0.8, 0.6, 1.0);

    // enable depth-buffering
    glEnable(GL_DEPTH_TEST);
}

void RenderWidget::resizeGL(const int width, const int height) {
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    const float aspectRatio = static_cast<float>(width) / static_cast<float>(height);

    // we want to capture a sphere of radius 1.0 without distortion
    // so we set the ortho projection based on whether the window is portrait (> 1.0) or landscape
    if (aspectRatio > 1.0) {
        // portrait ratio is wider, so make bottom & top -1.0 & 1.0
        glOrtho(-aspectRatio, aspectRatio, -1.0, 1.0, -1.1, 1.1);
    } else {
        // otherwise, make left & right -1.0 & 1.0
        glOrtho(-1.0, 1.0, -1.0 / aspectRatio, 1.0 / aspectRatio, -1.1, 1.1);
    }
}

void RenderWidget::paintGL() {
    // clear the buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set model view matrix based on stored translation, rotation &c.
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // start with lighting turned off
    glDisable(GL_LIGHTING);

    if (renderParameters->renderTexture) {
        // ignore translation
        glTranslatef(0.0f, 0.0f, 0.0f);
    } else {
        // translate by the visual translation
        glTranslatef(renderParameters->xTranslate, renderParameters->yTranslate, 0.0f);
    }

    if (renderParameters->renderTexture) {
        // fix rotation to identity
        const Matrix4 identity = Matrix4::identity();
        glMultMatrixf(reinterpret_cast<GLfloat*>(identity.columnMajor().coordinates));
    } else {
        // apply rotation matrix from arcball
        glMultMatrixf(reinterpret_cast<GLfloat*>(renderParameters->rotationMatrix.columnMajor().coordinates));
    }

    // draw triangle mesh
    renderMesh();
}

void RenderWidget::renderMesh() const {
    // make sure that textures are disabled
    glDisable(GL_TEXTURE_2D);

    // Centre on object
    float scale = renderParameters->zoomScale;
    scale /= triangleMesh->objectSize;

    if (renderParameters->renderTexture) {
        glTranslatef(0.0f, 0.0f, 0.0f);
    } else {
        const auto& centreOfGravity = triangleMesh->centreOfGravity;
        glTranslatef(-centreOfGravity.x * scale, -centreOfGravity.y * scale, -centreOfGravity.z * scale);
    }

    // Show (or not) wireframe
    if (renderParameters->useWireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // start rendering
    glBegin(GL_TRIANGLES);

    // loop through the faces
    for (unsigned int face = 0; face < triangleMesh->faceVertices.size(); face += 3) {
        // If setting is enabled, skip internal faces
        if (renderParameters->renderBoundary && !triangleMesh->isBoundaryFace(face)) {
            continue;
        }

        // loop over three vertices
        for (unsigned int vertex = 0; vertex < 3; vertex++) {
            // Vertex colour
            if (renderParameters->useNormal) {
                glColor3f(
                    REMAP_TO_UNIT_INTERVAL(triangleMesh->normals[triangleMesh->faceVertices[face + vertex]].x),
                    REMAP_TO_UNIT_INTERVAL(triangleMesh->normals[triangleMesh->faceVertices[face + vertex]].y),
                    REMAP_TO_UNIT_INTERVAL(triangleMesh->normals[triangleMesh->faceVertices[face + vertex]].z)
                );
            } else if (renderParameters->useTexCoords) {
                glColor3f(
                    triangleMesh->textureCoords[triangleMesh->faceVertices[face + vertex]].x,
                    triangleMesh->textureCoords[triangleMesh->faceVertices[face + vertex]].y,
                    triangleMesh->textureCoords[triangleMesh->faceVertices[face + vertex]].z
                );
            } else {
                glColor3f(
                    triangleMesh->colours[triangleMesh->faceVertices[face + vertex]].x,
                    triangleMesh->colours[triangleMesh->faceVertices[face + vertex]].y,
                    triangleMesh->colours[triangleMesh->faceVertices[face + vertex]].z
                );
            }

            // Vertex position
            if (renderParameters->renderTexture) {
                const Cartesian3 uv = triangleMesh->textureCoords[triangleMesh->faceVertices[face + vertex]];
                // Render UV coordinates at Z = 0.0f
                glVertex3f(
                    REMAP_FROM_UNIT_INTERVAL(uv.x),
                    REMAP_FROM_UNIT_INTERVAL(uv.y),
                    0.0f
                );
            } else {
                // Render scaled Vertex coordinates
                glVertex3f(
                    scale * triangleMesh->vertices[triangleMesh->faceVertices[face + vertex]].x,
                    scale * triangleMesh->vertices[triangleMesh->faceVertices[face + vertex]].y,
                    scale * triangleMesh->vertices[triangleMesh->faceVertices[face + vertex]].z
                );
            }
        }
    }

    // close off the triangles
    glEnd();

    // revert render mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void RenderWidget::mousePressEvent(QMouseEvent* event) {
    int whichButton = event->button();
    const float size = width() > height() ? height() : width();
    // scale both coordinates
    const float x = (2.0f * event->x() - size) / size;
    const float y = (size - 2.0f * event->y()) / size;

    // force mouse buttons to allow shift-click to be the same as right-click
    if (const int modifiers = event->modifiers();
        modifiers & Qt::ShiftModifier) {
        whichButton = Qt::RightButton;
        }

    emit beginScaledDrag(whichButton, x, y);
}

void RenderWidget::mouseMoveEvent(QMouseEvent* event) {
    const float size = width() > height() ? height() : width();
    // scale both coordinates
    const float x = (2.0f * event->x() - size) / size;
    const float y = (size - 2.0f * event->y()) / size;

    emit continueScaledDrag(x, y);
}

void RenderWidget::mouseReleaseEvent(QMouseEvent* event) {
    const float size = width() > height() ? height() : width();
    // scale both coordinates
    const float x = (2.0f * event->x() - size) / size;
    const float y = (size - 2.0f * event->y()) / size;

    emit endScaledDrag(x, y);
}
