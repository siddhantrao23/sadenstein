# sadenstein  
  
An old school FPS game in C++ inspired by `Wolfstein 3D`
  
(Currently you can create a 3D render in a series of images based on your current location)  
  
![sadenstein](https://user-images.githubusercontent.com/44816445/78461021-850b8580-76e3-11ea-9717-394c219b8fc9.png)
  
## Compilation
```sh
sudo apt install -y libsdl2-dev git cmake
git clone https://github.com/siddhantrao23/sadenstein.git
cd sadenstein
mkdir build
cd build
cmake ..
make
./ray_tracer
```
The images will be stored in the output folder
