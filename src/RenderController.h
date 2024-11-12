#ifndef RENDER_CONTROLLER_H
#define RENDER_CONTROLLER_H

#include <QtGui>

#include "RenderWindow.h"
#include "TriangleMesh.h"
#include "RenderParameters.h"

class RenderController : public QObject {
    Q_OBJECT

    RenderParameters* renderParameters;
    RenderWindow* renderWindow;

    int dragButton;

public:
    RenderController(
        RenderParameters* renderParameters,
        RenderWindow* renderWindow
    );

public slots:
    // slot for responding to arcball rotation for object
    void objectRotationChanged() const;

    // slots for responding to zoom & translate sliders
    void zoomChanged(int value) const;

    void xTranslateChanged(int value) const;

    void yTranslateChanged(int value) const;

    // slots for responding to check boxes
    void renderBoundaryCheckChanged(int state) const;

    void useWireframeCheckChanged(int state) const;

    void useNormalCheckChanged(int state) const;

    void useTexCoordsCheckChanged(int state) const;

    void renderTextureCheckChanged(int state) const;

    // slot for file writes
    void writeUnwrappedObj() const;

    void writeToPng() const;

    // slots for responding to arcball manipulations
    // these are general purpose signals which pass the mouse moves to the controller
    // after scaling to the notional unit sphere
    void beginScaledDrag(int whichButton, float x, float y);

    void continueScaledDrag(float x, float y) const;

    void endScaledDrag(float x, float y);
};

#endif
