g++ hello-triangle.cpp ./glew/lib/libGLEW.a ./glfw/build/src/libglfw3.a \
    -I ./glfw/include -I glew/include/ \
    -l GL -lX11 -l Xxf86vm -l Xrandr -lpthread -l Xi  -lXinerama -lXcursor \
    -ldl -lXinerama -lXcursor

./a.out
