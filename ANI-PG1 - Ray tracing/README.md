# Ray tracer 

 ANI-PG1 course on FIT CTU

 Author: Bc. Josef Jech

 Tutor: Ing. Jakub Votrubec

---
### Features
This ray tracer is implemented in C++ purely for CPU. It features traditional Whitted ray tracing but enhanced by fresnel equasion done by Schlick's approximation to correctly mix reflexted and transmitted radiance.
Albedo textures are sampled by bilinear interpolation and their mipmaps trilinear by interpolation. It is also possible to plug in normal map. For BRDF you can choose from Phong and GGX illumination, where by GGX is apllied gamma correction and post-processing. The whole renderer is based on the Kd-Tree for faster triangle ray intersection query.

![](renders/PNG/main.png)



### Used libraries:

- tiny obj - https://github.com/tinyobjloader/tinyobjloader
    - for parsing obj and mtl scene files
- stb image - https://github.com/nothings/stb
    - for loading textures to arrays
- json - https://github.com/nlohmann/json
    - for parsing of json config file
- poxy - https://github.com/marzer/poxy#overview
    - for documentation
---
### Project stucture
- external - external libraries
- output - place for .ppm outputs and render logs saved in times.log
- scenes - place for textures and whole scenes based in folders
- src - renderer source code
---
### What to RUN?
- Always be in the folder Ray tracer/

#### CMakeList
1. To build the project at first -> cmake -B build
2. To compile what was edited -> cmake --build build
3. To compile and run -> cmake --build build && cmake --build build --target run
4. To clean -> rm -rf build/
- defined by CMakeLists.txt

#### Makefile
1. To build the project -> make
2. To compile and run -> make run
3. To delete makefile files -> make clean
- defined by Makefile

#### Poxy
1. If you downloaded it via pip, run -> poxy
2. Folder html/ will be generated
- defined by poxy.toml

### Config
To change setting of render you can set in config.json:

1. Width
2. Height
3. Scene Path - path to the .obj file with wanted scene
4. Material Path - path to the folder, where lies related .mtl file
5. camera - there are two setups, one for box scene and second for all others
    1. "CAMERA": {
    "pos": [278, 273, -1000],
    "up":  [0.0, 1.0, 0.0],
    "dir": [0.0, 0.0, 1.0],
    "fov": 0.6
    },
    2.   "CAMERA": {
        "pos": [0.0, 1.0, 4.42],
        "up":  [0.0, 1.0, 0.0],
        "dir": [0.0, 0.0, -1.0],
        "fov": 0.6
    },
6. DMAX - max ray traycing recursion depth
7. light samples - number of samples per each light
8. GGX - if true - GGX, else Phong is applied
9. Exposure - intensity of post-processing, applied with GGX on
10. DIST - max distance from camera to render
    - for Box scene - 1000
    - for Hall - 20
    - others - 6  

**Renders - Phong left, GGX right**
![](renders/PNG/6.png)

![](renders/PNG/66.png)

