#include "RenderWindow.h"

#include "RenderParameters.h"

RenderWindow::RenderWindow(
    TriangleMesh* triangleMesh,
    RenderParameters* renderParameters,
    const char* windowName
) : QWidget(nullptr),
    triangleMesh(triangleMesh),
    renderParameters(renderParameters) {
    setWindowTitle(QString(windowName));

    windowLayout = new QGridLayout(this);

    renderWidget = new RenderWidget(triangleMesh, renderParameters, this);

    modelRotator = new ArcBallWidget(this);

    renderBoundaryBox = new QCheckBox("Boundary", this);
    renderWireframeBox = new QCheckBox("Wireframe", this);
    useTexCoordsBox = new QCheckBox("UVW -> RGB", this);
    useNormalBox = new QCheckBox("Nrm -> RGB", this);
    renderTextureBox = new QCheckBox("Texture", this);

    writeUnwrappedObjFile = new QPushButton("Write unwrap .obj", this);
    writeToPng = new QPushButton("Render to .png", this);

    xTranslateSlider = new QSlider(Qt::Horizontal, this);
    yTranslateSlider = new QSlider(Qt::Vertical, this);
    zoomSlider = new QSlider(Qt::Vertical, this);

    modelRotatorLabel = new QLabel("Model", this);
    yTranslateLabel = new QLabel("Y", this);
    zoomLabel = new QLabel("Zm", this);

    // Add the widgets to the grid | Row | Column | Row Span | Column Span |
    constexpr int nStacked = 8;

    windowLayout->addWidget(renderWidget, 0, 1, nStacked, 1);
    windowLayout->addWidget(yTranslateSlider, 0, 2, nStacked, 1);
    windowLayout->addWidget(zoomSlider, 0, 4, nStacked, 1);

    // Sack in the middle
    windowLayout->addWidget(modelRotator, 0, 3, 1, 1);
    windowLayout->addWidget(modelRotatorLabel, 1, 3, 1, 1);
    windowLayout->addWidget(renderBoundaryBox, 2, 3, 1, 1);
    windowLayout->addWidget(renderWireframeBox, 3, 3, 1, 1);
    windowLayout->addWidget(useTexCoordsBox, 4, 3, 1, 1);
    windowLayout->addWidget(useNormalBox, 5, 3, 1, 1);
    windowLayout->addWidget(renderTextureBox, 6, 3, 1, 1);
    windowLayout->addWidget(writeUnwrappedObjFile, 7, 3, 1, 1);
    windowLayout->addWidget(writeToPng, 8, 3, 1, 1);

    // Translate Slider Row
    windowLayout->addWidget(xTranslateSlider, nStacked, 1, 1, 1);
    windowLayout->addWidget(yTranslateLabel, nStacked, 2, 1, 1);
    // nothing in column 3
    windowLayout->addWidget(zoomLabel, nStacked, 4, 1, 1);

    resetInterface();
}

// sets every visual control to match the model
void RenderWindow::resetInterface() const {
    // set check boxes
    renderBoundaryBox->setChecked(renderParameters->renderBoundary);
    renderWireframeBox->setChecked(renderParameters->useWireframe);
    useNormalBox->setChecked(renderParameters->useNormal);
    useTexCoordsBox->setChecked(renderParameters->useTexCoords);
    renderTextureBox->setChecked(renderParameters->renderTexture);

    // set sliders
    // x & y translate are scaled to notional unit sphere in render widgets
    // but because the slider is defined as integer, we multiply by a 100 for all sliders
    xTranslateSlider->setMinimum(static_cast<int>(TRANSLATE_MIN * PARAMETER_SCALING));
    xTranslateSlider->setMaximum(static_cast<int>(TRANSLATE_MAX * PARAMETER_SCALING));
    xTranslateSlider->setValue(static_cast<int>(renderParameters->xTranslate * PARAMETER_SCALING));

    yTranslateSlider->setMinimum(static_cast<int>(TRANSLATE_MIN * PARAMETER_SCALING));
    yTranslateSlider->setMaximum(static_cast<int>(TRANSLATE_MAX * PARAMETER_SCALING));
    yTranslateSlider->setValue(static_cast<int>(renderParameters->yTranslate * PARAMETER_SCALING));

    // zoom slider is a logarithmic scale, so we want a narrow range
    zoomSlider->setMinimum(static_cast<int>(ZOOM_SCALE_LOG_MIN * PARAMETER_SCALING));
    zoomSlider->setMaximum(static_cast<int>(ZOOM_SCALE_LOG_MAX * PARAMETER_SCALING));
    zoomSlider->setValue(static_cast<int>(std::log10(renderParameters->zoomScale) * PARAMETER_SCALING));

    // now flag them all for update
    renderWidget->update();
    modelRotator->update();
    xTranslateSlider->update();
    yTranslateSlider->update();
    zoomSlider->update();
    renderBoundaryBox->update();
    renderWireframeBox->update();
    useNormalBox->update();
    useTexCoordsBox->update();
    renderTextureBox->update();
    writeToPng->update();
}
