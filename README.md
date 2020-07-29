<h1 align="center">Sadenstein notso3D</h1>
<p>
</p>

An old school FPS game written in C++ inspired by `Wolfstein 3D` which implements ray tracing


Current Implementation includes a ray tracing algorithm which generates a single rendered image.  

![walkthrough](res/walkthrough.gif)

## Install

```sh
git clone https://github.com/siddhantrao23/sadenstein.git
sudo apt install -y libsdl2-dev git
cd sadenstein
mkdir build
cd build
cmake ..
make
```

## Usage

```sh
./sadenstein
```
`out.ppm` will be stored in the build directory
