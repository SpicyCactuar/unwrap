#include <iostream>
#include <fstream>
#include <filesystem>

#include "RenderWindow.h"
#include "TriangleMesh.h"
#include "RenderParameters.h"
#include "RenderController.h"

int main(int argc, char** argv) {
    QApplication renderApp(argc, argv);

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <.obj>" << std::endl;
        return EXIT_FAILURE;
    }

    TriangleMesh triangleMesh;
    std::ifstream meshFile(argv[1]);

    if (!meshFile.good() || !triangleMesh.readFromObjFile(meshFile)) {
        std::cerr << "Read failed for object: " << argv[1] << std::endl;
        return EXIT_FAILURE;
    }

    const auto outputPath = std::filesystem::current_path() / "out";
    if (!exists(outputPath) && !create_directories(outputPath)) {
        std::cerr << "Failed to create output folder: " << outputPath << std::endl;
        std::cerr << "Only writes will fail, program will not abort" << std::endl;
    }

    std::filesystem::path objFilePath = outputPath / argv[1];

    std::cout << "Unwrapping..." << std::endl;

    triangleMesh.unwrap();

    std::cout << "Finished unwrapping" << std::endl;

    std::string unwrappedObjPath = outputPath / (objFilePath.stem().string() + "_unwrapped.obj");
    std::string pngFilePath = outputPath / objFilePath.stem().replace_extension(".png");

    RenderParameters renderParameters(unwrappedObjPath, pngFilePath);
    RenderWindow renderWindow(&triangleMesh, &renderParameters, argv[1]);
    RenderController renderController(&renderParameters, &renderWindow);

    renderWindow.resize(826, 700);
    renderWindow.show();

    return renderApp.exec();
}
