@echo off

SET myapp=evaporate
SET mydp=C:\dev\evaporate

cls

mkdir %mydp%\build

del %mydp%\build\evaporate.exe 

pushd %mydp%\build

g++ %mydp%\main.cpp -std=c++11 -IC:\dev\b_libs -IC:\dev\stb -IC:\dev\mingw\include -IC:\dev\bullet\Bullet -IC:\dev\SDL2\include\SDL2 -LC:\dev\bullet\lib -lbullet -LC:\dev\mingw\lib -lopengl32 -lglu32 -lglew32 -lgdi32 -LC:\dev\SDL2\lib -w -Wl,-subsystem,windows -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -LC:\dev\mingw\lib -o %myapp%

popd