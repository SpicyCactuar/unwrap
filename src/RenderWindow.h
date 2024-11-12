#ifndef RENDER_WINDOW_H
#define RENDER_WINDOW_H

#include <QtWidgets>

#include "ArcBallWidget.h"
#include "RenderWidget.h"
#include "TriangleMesh.h"

// window that displays a geometric model with controls
class RenderWindow : public QWidget {
    TriangleMesh* triangleMesh;

    RenderParameters* renderParameters;

    QGridLayout* windowLayout;

    ArcBallWidget* modelRotator;
    RenderWidget* renderWidget;

    QCheckBox* renderBoundaryBox;
    QCheckBox* renderWireframeBox;
    QCheckBox* useNormalBox;
    QCheckBox* useTexCoordsBox;
    QCheckBox* renderTextureBox;

    QPushButton* writeUnwrappedObjFile;
    QPushButton* writeToPng;

    QSlider* xTranslateSlider;
    QSlider* yTranslateSlider;
    QSlider* zoomSlider;

    QLabel* modelRotatorLabel;
    QLabel* yTranslateLabel;
    QLabel* zoomLabel;

public:
    RenderWindow(
        TriangleMesh* triangleMesh,
        RenderParameters* renderParameters,
        const char* windowName = "Mesh Viewer"
    );

    void resetInterface() const;

    // declare the render controller class a friend so it can access the UI elements
    friend class RenderController;
};

#endif
