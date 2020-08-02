<h1 align="center">Sadenstein notso3D</h1>
<p>
</p>

An old school FPS game written in C++ inspired by `Wolfstein 3D` which implements ray tracing

<p align="center">
  <img src="https://github.com/siddhantrao23/sadenstein/blob/master/res/walkthrough.gif"/>
</p>

> Current implementation allows walking through a world infested with monsters!  

Controls are pretty straightforward:

Key | Action | 
--- | --- 
a | Turn CounterClockwise 
d | Turn Clockwise
w | Move Forward
s | Move Backward

## Installation

```sh
git clone https://github.com/siddhantrao23/sadenstein.git
sudo apt install -y libsdl2-dev git
```

## Building

```sh
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

## TODO

* [ ] Remove STB library dependancy
* [ ] Add new sprites
