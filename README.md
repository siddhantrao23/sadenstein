<h1 align="center">Sadenstein notso3D</h1>
<p>
</p>

An old school FPS game written in C++ inspired by `Wolfstein 3D` which implements ray tracing


Current Implementation includes a ray tracing algorithm which generates a single rendered image.
<br>
![sadenstein](https://user-images.githubusercontent.com/44816445/78461021-850b8580-76e3-11ea-9717-394c219b8fc9.png)

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
