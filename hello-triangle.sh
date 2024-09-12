set -e
g++ hello-triangle.cpp ../glew/lib/libGLEW.a ../glfw/build/src/libglfw3.a \
    -I ../glfw/include -I ../glew/include/ \
    -lGL -lX11 -lGLU \
    -o hello-triangle
./hello-triangle
 