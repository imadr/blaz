# 🔥 blaz

## Building

- [Install CMake >= 3.12](https://cmake.org/download/)
- ```git clone https://github.com/imadr/blaz.git```
- ```cd blaz```
- ```mkdir build && cd build```
- ```cmake ..``` to generate the build files
- ```cmake --build .``` to run the build **or** use the generated visual studio project file/makefile to build
You can also use **cmake-gui**

#### Building for web

- Install [emscripten](https://emscripten.org/)
- ```mkdir build_wasm && cd build_wasm```
- ```emcmake cmake ..```
- ```cmake --build .```
- To build a specific sample ```cmake --build . --target "01-hellotriangle"```

## Samples

<table border="0">
<tr>
  <td>
    <img src="samples/tests/01-hellotriangle.bmp" width="300"/><br>
    01-triangle
  </td>
  <td>
    <img src="samples/tests/02-cubes.bmp" width="300"/><br>
    02-cubes
  </td> 
  <td>
    <img src="samples/tests/03-pbr.bmp" width="300"/><br>
    03-pbr
  </td>
  <td>
    <img src="samples/tests/04-physics.bmp" width="300"/><br>
    04-physics
  </td>
</tr>
</table>
