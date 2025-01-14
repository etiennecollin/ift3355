

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "parser.h"
#include "raytracer.h"
namespace fs = std::filesystem;

int main(int argc, char **argv) {
    //[0]: cmd
    //[1]: scene filename

    if (!(argc == 2)) {
        std::cerr << "Entry must respect the following: cmd scene_filename";
        return 0;
    }

    fs::path data_root("data");

    // File name of the specified scene
    fs::path filename_scene_input = data_root / "scene" / std::string(argv[1]);
    fs::path directory_scene_output = data_root / "output" / filename_scene_input.stem();

    if (!fs::exists(directory_scene_output)) {
        fs::create_directories(directory_scene_output);
    }

    std::cout << "Rendering " << filename_scene_input << std::endl;
    std::cout << "Output to " << directory_scene_output << std::endl;

    // Scene file analysis
    Parser parser(new std::ifstream(filename_scene_input.string().c_str()));
    if (!parser.parse()) {
        std::cout << "Scene is not found or can't be parsed." << std::endl;
    } else {
        Frame output = Frame{parser.scene.resolution[0], parser.scene.resolution[1]};

        // Render the given scene with raytracing
        Raytracer raytracer;
        raytracer.render(parser.scene, &output);

        // Save the frame
        output.show_color_to((directory_scene_output / "color.bmp").string().c_str());
        output.show_depth_to((directory_scene_output / "depth.bmp").string().c_str());

        std::cout << "Ray tracing finished with images saved." << std::endl;
    }

    // Uncomment if you use Visual Studio
    // system("pause");
    return 0;
};
