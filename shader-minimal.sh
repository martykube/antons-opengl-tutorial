set -e
g++ shader-minimal.cpp shadermanager.cpp gllogging.cpp \
    ../glew/lib/libGLEW.a ../glfw/build/src/libglfw3.a \
    -I ../glfw/include -I ../glew/include/ \
    -lGL -lX11 -lGLU \
    -o shader-minimal
./shader-minimal
 