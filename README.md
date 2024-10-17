# 🔥 blaz

3D engine with no dependencies, all code is written from scratch

<p align="center">
<img alt="GitHub repo size" src="https://img.shields.io/github/repo-size/imadr/blaz">
<img alt="GitHub commit activity" src="https://img.shields.io/github/commit-activity/w/imadr/blaz">
<img alt="GitHub contributors" src="https://img.shields.io/github/contributors/imadr/blaz">
</p>

## Building

- [Install CMake >= 3.12](https://cmake.org/download/)
- ```git clone https://github.com/imadr/blaz.git```
- ```cd blaz```
- ```mkdir build && cd build```
- ```cmake ..``` to generate the build files
- ```cmake --build .``` to run the build **or** use the generated visual studio project file/makefile to build<br>You can also use **cmake-gui**

## Samples


| <b>Sample</b>                      |<b>Screenshot</b>                                                                               |
|-----------------------------------------------------|------------------------------------------------------------------------------------------------------------|
| 01-triangle           | <img src="/samples/tests/01-hellotriangle.png" width="300"/><br>                       |
| 02-cubes              | <img src="/samples/tests/02-cubes.png" width="300"/><br>                               |
| 03-pbr                | <img src="/samples/tests/03-pbr.png" width="300"/><br>                                 |
| 04-raymarching        | <img src="/samples/tests/04-raymarching.png" width="300"/><br>                         |
| 05-raytracing         | <img src="/samples/tests/05-raytracing.png" width="300"/><br>                          |
| 06-shadow             | <img src="/samples/tests/06-shadow.png" width="300"/><br>                          |

## Building for web

- Install [emscripten](https://emscripten.org/)
- ```emsdk_env.bat``` or ```emsdk_env.sh```
- ```mkdir build_wasm && cd build_wasm```
- ```emcmake cmake ..```
- ```cmake --build .```
- To build a specific sample ```cmake --build . --target "01-hellotriangle"```
