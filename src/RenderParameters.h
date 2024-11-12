#ifndef RENDER_PARAMETERS_H
#define RENDER_PARAMETERS_H

#include "Matrix4.h"

class RenderParameters {
public:
    float xTranslate, yTranslate;
    float zoomScale;

    Matrix4 rotationMatrix;

    bool renderBoundary;
    bool useWireframe;
    bool useNormal;
    bool useTexCoords;
    bool renderTexture;
    bool renderNormalMap;

    const std::string unwrappedObjFilePath;
    const std::string pngFilePath;

    RenderParameters(
        const std::string& unwrappedObjFilePath,
        const std::string& pngFilePath
    ) : xTranslate(0.0f),
        yTranslate(0.0f),
        zoomScale(1.0f),
        renderBoundary(false),
        useWireframe(false),
        useNormal(false),
        useTexCoords(false),
        renderTexture(false),
        renderNormalMap(false),
        unwrappedObjFilePath(unwrappedObjFilePath),
        pngFilePath(pngFilePath) {
        rotationMatrix = Matrix4::identity();
    }
};

// Define some macros for bounds on parameters
#define TRANSLATE_MIN (-1.0f)
#define TRANSLATE_MAX 1.0f

#define ZOOM_SCALE_LOG_MIN (-2.0)
#define ZOOM_SCALE_LOG_MAX 2.0
#define ZOOM_SCALE_MIN 0.01f
#define ZOOM_SCALE_MAX 100.0f

// Scale to/from integer values
#define PARAMETER_SCALING 100

#endif
