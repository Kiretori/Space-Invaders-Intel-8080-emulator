# Space Invaders (1978) / Intel 8080 Microprocessor Emulator     

An emulator for the 8080 Intel pmicroprocessor mainly for emulating the Space Invaders arcade game.

Might make it a full 8080 emulator eventually (work in progress)

## Requirements
* SDL2 and SDL2_mixer
* CMake (optional)
* Space Invaders ROM files 
* Space Invaders sound files (not included)


## How To Build
`cmake -S . -B build -DCMAKE_BUILD_TYPE=Release`  
`cmake --build build` 


## How To Run
### Linux
`cd build`   
`./invaders`

### Windows
`cd build`    
`invaders.exe`


## How To Play
|Key|Action|
|---|------|
|`C`|Insert Coin|
|`ENTER`|1P Start|
|`P`|2P Start|
|`SPACE`|Shoot|
|`LEFT`|Move Left|
|`RIGHT`|Move Right|
|`T`|Tilt|

## References and Help
[Emulator 101](http://www.emulator101.com/welcome.html)  
[Computer Archaeology](https://www.computerarcheology.com/Arcade/SpaceInvaders/)  
[Kurtjd's SDL Implementation](https://github.com/kurtjd/space-invaders-emulator)
