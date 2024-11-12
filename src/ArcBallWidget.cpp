#include "ArcBallWidget.h"

#define ARCBALL_WIDGET_SIZE 100

std::array<std::array<float, 3>, 84> sphereVert{
    {
        {0, 0, -1},
        {0.5, 0, -0.86603},
        {0.86603, 0, -0.5},
        {1, 0, 0},
        {0.86603, 0, 0.5},
        {0.5, 0, 0.86603},
        {0, 0, 1},
        {0, 0, -1},
        {0.43301, 0.25, -0.86603},
        {0.75, 0.43301, -0.5},
        {0.86603, 0.5, 0},
        {0.75, 0.43301, 0.5},
        {0.43301, 0.25, 0.86603},
        {0, 0, 1},
        {0, 0, -1},
        {0.25, 0.43301, -0.86603},
        {0.43301, 0.75, -0.5},
        {0.5, 0.86603, 0},
        {0.43301, 0.75, 0.5},
        {0.25, 0.43301, 0.86603},
        {0, 0, 1},
        {0, 0, -1},
        {0, 0.5, -0.86603},
        {0, 0.86603, -0.5},
        {0, 1, 0},
        {0, 0.86603, 0.5},
        {0, 0.5, 0.86603},
        {0, 0, 1},
        {0, 0, -1},
        {-0.25, 0.43301, -0.86603},
        {-0.43301, 0.75, -0.5},
        {-0.5, 0.86603, 0},
        {-0.43301, 0.75, 0.5},
        {-0.25, 0.43301, 0.86603},
        {0, 0, 1},
        {0, 0, -1},
        {-0.43301, 0.25, -0.86603},
        {-0.75, 0.43301, -0.5},
        {-0.86603, 0.5, 0},
        {-0.75, 0.43301, 0.5},
        {-0.43301, 0.25, 0.86603},
        {0, 0, 1},
        {0, 0, -1},
        {-0.5, 0, -0.86603},
        {-0.86603, 0, -0.5},
        {-1, 0, 0},
        {-0.86603, 0, 0.5},
        {-0.5, 0, 0.86603},
        {0, 0, 1},
        {0, 0, -1},
        {-0.43301, -0.25, -0.86603},
        {-0.75, -0.43301, -0.5},
        {-0.86603, -0.5, 0},
        {-0.75, -0.43301, 0.5},
        {-0.43301, -0.25, 0.86603},
        {0, 0, 1},
        {0, 0, -1},
        {-0.25, -0.43301, -0.86603},
        {-0.43301, -0.75, -0.5},
        {-0.5, -0.86603, 0},
        {-0.43301, -0.75, 0.5},
        {-0.25, -0.43301, 0.86603},
        {0, 0, 1},
        {0, 0, -1},
        {0, -0.5, -0.86603},
        {0, -0.86603, -0.5},
        {0, -1, 0},
        {0, -0.86603, 0.5},
        {0, -0.5, 0.86603},
        {0, 0, 1},
        {0, 0, -1},
        {0.25, -0.43301, -0.86603},
        {0.43301, -0.75, -0.5},
        {0.5, -0.86603, 0},
        {0.43301, -0.75, 0.5},
        {0.25, -0.43301, 0.86603},
        {0, 0, 1},
        {0, 0, -1},
        {0.43301, -0.25, -0.86603},
        {0.75, -0.43301, -0.5},
        {0.86603, -0.5, 0},
        {0.75, -0.43301, 0.5},
        {0.43301, -0.25, 0.86603},
        {0, 0, 1}
    }
};

ArcBallWidget::ArcBallWidget(QWidget* parent)
    : QOpenGLWidget(parent) {
    // let QT know we are fixed size
    setFixedSize(QSize(ARCBALL_WIDGET_SIZE, ARCBALL_WIDGET_SIZE));
}

Matrix4 ArcBallWidget::rotationMatrix() const {
    return arcBall.getRotation();
}

void ArcBallWidget::initializeGL() {
    // no lighting, but we need depth test
    glDisable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.9, 0.9, 0.9, 1.0);
}

void ArcBallWidget::resizeGL(int width, int height) {
    glViewport(0, 0, width, height);
    // set projection matrix to have range of -1.0 - 1.0 in x, y, z
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
}

void ArcBallWidget::paintGL() {
    // clear the buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set model view matrix based on arcball rotation
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // draw a single white quad to block back half of arcball
    glColor3f(0.92, 0.92, 0.92);
    glBegin(GL_QUADS);
    glVertex3f(-1.0, -1.0, 0.0);
    glVertex3f(1.0, -1.0, 0.0);
    glVertex3f(1.0, 1.0, 0.0);
    glVertex3f(-1.0, 1.0, 0.0);
    glEnd();

    // set colour to black
    glColor3f(0.0, 0.0, 0.0);
    // draw a circle around the edge of the sphere
    glScalef(0.8, 0.8, 0.8);
    glBegin(GL_LINE_LOOP);
    for (int j = 3; j < sphereVert.size(); j += 7) {
        glVertex3f(sphereVert[j][0], sphereVert[j][1], 0.5);
    }
    glEnd();

    // retrieve rotation from arcball & apply
    Matrix4 rotMatrix = arcBall.getRotation();
    glMultMatrixf(reinterpret_cast<GLfloat*>(rotMatrix.columnMajor().coordinates));

    // loop through verticals of sphere
    for (int i = 0; i < 12; i++) {
        // loop drawing verticals
        glBegin(GL_LINE_STRIP);
        for (int j = i * 7; j < 7 + i * 7; j++) {
            glVertex3fv(sphereVert[j].data());
        }
        glEnd();
    }
    // loop through horizontals
    for (int i = 1; i < 6; i++) {
        // loop for horizontals
        glBegin(GL_LINE_LOOP);
        for (int j = i; j < sphereVert.size(); j += 7) {
            glVertex3fv(sphereVert[j].data());
        }
        glEnd();
    }
}

void ArcBallWidget::mousePressEvent(QMouseEvent* event) {
    const float x = event->x();
    const float y = event->y();
    const float width = this->width();
    const float height = this->height();
    const float scaledX = (2.0f * x - width) / width;
    // flip from Qt coordinates to Cartesian
    const float scaledY = (height - 2.0f * y) / height;
    // set the initial rotation for the drag
    arcBall.beginDrag(scaledX, scaledY);
    // signal we've changed
    emit rotationChanged();
}

void ArcBallWidget::mouseMoveEvent(QMouseEvent* event) {
    const float x = event->x();
    const float y = event->y();
    const float width = this->width();
    const float height = this->height();
    const float scaledX = (2.0f * x - width) / width;
    // flip from Qt coordinates to Cartesian
    const float scaledY = (height - 2.0f * y) / height;
    // set the mid point of the drag
    arcBall.continueDrag(scaledX, scaledY);
    // signal we've changed
    emit rotationChanged();
}

void ArcBallWidget::mouseReleaseEvent(QMouseEvent* event) {
    const float x = event->x();
    const float y = event->y();
    const float width = this->width();
    const float height = this->height();
    const float scaledX = (2.0f * x - width) / width;
    // flip from Qt coordinates to Cartesian
    const float scaledY = (height - 2.0f * y) / height;
    // set the final rotation for the drag
    arcBall.endDrag(scaledX, scaledY);
    // signal we've changed
    emit rotationChanged();
}

// routines called to allow synchronised rotation with other widget
// coordinates are assumed to be in range of [-1..1] in x,y
// if outside that range, will be clamped
void ArcBallWidget::beginDrag(const float x, const float y) {
    arcBall.beginDrag(x, y);

    emit rotationChanged();
}

void ArcBallWidget::continueDrag(const float x, const float y) {
    arcBall.continueDrag(x, y);

    emit rotationChanged();
}

void ArcBallWidget::endDrag(const float x, const float y) {
    arcBall.endDrag(x, y);

    emit rotationChanged();
}
