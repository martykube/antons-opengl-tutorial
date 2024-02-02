g++ hello-triangle.cpp ./glew/lib/libGLEW.a ./glfw/build/src/libglfw3.a \
    -I ./glfw/include -I glew/include/ \
    -l GL -l X11 -l Xxf86vm -l Xrandr -l pthread -l Xi  -l Xinerama -l Xcursor \
    -l dl -l Xinerama

./a.out
