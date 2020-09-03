<h1 align="center">Sadenstein notso3D</h1>
<p>
</p>

_Sadenstein_ is a simple, old school 2.5D FPS game written in C++ inspired by `Wolfenstein 3D` which implements ray casting.

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
Or just run the `build.sh` scirpt

## Usage

```sh
./sadenstein
```

## TODO

* [x] Remove STB library dependancy
* [ ] Add menu for startup
* [ ] Add user map drawing capability
* [ ] Kill the monsters
* [ ] Implement network multiplayer
* [ ] Add new sprites
* [ ] Add GitPod instance

Drop an issue if you want to work on any of these^
