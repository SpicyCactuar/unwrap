#include "RenderController.h"

#include <cstdio>
#include <fstream>

RenderController::RenderController(
    RenderParameters* renderParameters,
    RenderWindow* renderWindow
) : renderParameters(renderParameters),
    renderWindow(renderWindow),
    dragButton(Qt::NoButton) {
    QObject::connect(renderWindow->modelRotator, SIGNAL(rotationChanged()),
                     this, SLOT(objectRotationChanged()));

    // signals for main widget to control arcball
    QObject::connect(renderWindow->renderWidget, SIGNAL(beginScaledDrag(int, float, float)),
                     this, SLOT(beginScaledDrag(int, float, float)));
    QObject::connect(renderWindow->renderWidget, SIGNAL(continueScaledDrag(float, float)),
                     this, SLOT(continueScaledDrag(float, float)));
    QObject::connect(renderWindow->renderWidget, SIGNAL(endScaledDrag(float, float)),
                     this, SLOT(endScaledDrag(float, float)));

    // signal for zoom slider
    QObject::connect(renderWindow->zoomSlider, SIGNAL(valueChanged(int)),
                     this, SLOT(zoomChanged(int)));

    // signal for x translate sliders
    QObject::connect(renderWindow->xTranslateSlider, SIGNAL(valueChanged(int)),
                     this, SLOT(xTranslateChanged(int)));

    // signal for y translate slider
    QObject::connect(renderWindow->yTranslateSlider, SIGNAL(valueChanged(int)),
                     this, SLOT(yTranslateChanged(int)));

    // signal for check box for normals
    QObject::connect(renderWindow->renderBoundaryBox, SIGNAL(stateChanged(int)),
                     this, SLOT(renderBoundaryCheckChanged(int)));

    // signal for check box for normals
    QObject::connect(renderWindow->renderWireframeBox, SIGNAL(stateChanged(int)),
                     this, SLOT(useWireframeCheckChanged(int)));

    // signal for check box for normals
    QObject::connect(renderWindow->useNormalBox, SIGNAL(stateChanged(int)),
                     this, SLOT(useNormalCheckChanged(int)));

    // signal for check box for texture coordinates
    QObject::connect(renderWindow->useTexCoordsBox, SIGNAL(stateChanged(int)),
                     this, SLOT(useTexCoordsCheckChanged(int)));

    // signal for check box for texture render
    QObject::connect(renderWindow->renderTextureBox, SIGNAL(stateChanged(int)),
                     this, SLOT(renderTextureCheckChanged(int)));

    // signal for file write buttons
    QObject::connect(renderWindow->writeUnwrappedObjFile, SIGNAL(released()),
                     this, SLOT(writeUnwrappedObj()));
    QObject::connect(renderWindow->writeToPng, SIGNAL(released()),
                     this, SLOT(writeToPng()));

    // copy the rotation matrix from the widgets to the model
    renderParameters->rotationMatrix = renderWindow->modelRotator->rotationMatrix();
}

void RenderController::objectRotationChanged() const {
    renderParameters->rotationMatrix = renderWindow->modelRotator->rotationMatrix();

    renderWindow->resetInterface();
}

void RenderController::zoomChanged(const int value) const {
    renderParameters->zoomScale =
            std::clamp(std::pow(10.0f, static_cast<float>(value) / 100.0f), ZOOM_SCALE_MIN, ZOOM_SCALE_MAX);

    renderWindow->resetInterface();
}

void RenderController::xTranslateChanged(int value) const {
    renderParameters->xTranslate =
            std::clamp(static_cast<float>(value) / 100.0f, TRANSLATE_MIN, TRANSLATE_MAX);

    renderWindow->resetInterface();
}

void RenderController::yTranslateChanged(const int value) const {
    renderParameters->yTranslate =
            std::clamp(static_cast<float>(value) / 100.0f, TRANSLATE_MIN, TRANSLATE_MAX);

    renderWindow->resetInterface();
}

void RenderController::renderBoundaryCheckChanged(const int state) const {
    renderParameters->renderBoundary = state == Qt::Checked;

    renderWindow->resetInterface();
}

// slot for toggling wireframe
void RenderController::useWireframeCheckChanged(const int state) const {
    renderParameters->useWireframe = state == Qt::Checked;

    renderWindow->resetInterface();
}

void RenderController::useNormalCheckChanged(const int state) const {
    renderParameters->useNormal = state == Qt::Checked;

    renderWindow->resetInterface();
}

void RenderController::useTexCoordsCheckChanged(const int state) const {
    renderParameters->useTexCoords = state == Qt::Checked;

    renderWindow->resetInterface();
}

void RenderController::renderTextureCheckChanged(int state) const {
    renderParameters->renderTexture = state == Qt::Checked;

    renderWindow->resetInterface();
}

void RenderController::writeUnwrappedObj() const {
    if (std::ofstream unwrappedObjFile(renderParameters->unwrappedObjFilePath);
        unwrappedObjFile.good()) {
        renderWindow->triangleMesh->writeToObjFile(unwrappedObjFile);
        std::cout << "Write to " << renderParameters->unwrappedObjFilePath << std::endl;
    } else {
        std::cerr << "Failed to write to " << renderParameters->unwrappedObjFilePath << std::endl;
    }
}

void RenderController::writeToPng() const {
    renderWindow->renderWidget->grab().save(QString::fromStdString(renderParameters->pngFilePath));
    std::cout << "Written to: " << renderParameters->pngFilePath << std::endl;

    renderWindow->resetInterface();
}

void RenderController::beginScaledDrag(const int whichButton, const float x, const float y) {
    // Remember drag button
    dragButton = whichButton;

    switch (dragButton) {
        case Qt::LeftButton:
            renderWindow->modelRotator->beginDrag(x, y);
            break;
        default:
            break;
    }

    renderWindow->resetInterface();
}

void RenderController::continueScaledDrag(const float x, const float y) const {
    switch (dragButton) {
        case Qt::LeftButton:
            renderWindow->modelRotator->continueDrag(x, y);
            break;
        default:
            break;
    }

    renderWindow->resetInterface();
}

void RenderController::endScaledDrag(const float x, const float y) {
    switch (dragButton) {
        case Qt::LeftButton:
            renderWindow->modelRotator->endDrag(x, y);
            break;
        default:
            break;
    }

    // Forget drag button
    dragButton = Qt::NoButton;

    renderWindow->resetInterface();
}
