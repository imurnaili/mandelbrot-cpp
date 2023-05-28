@ECHO OFF
MKDIR build
CD build
cmake ..
cmake --build .
CD ..
