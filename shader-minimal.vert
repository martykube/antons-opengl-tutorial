#version 410

in vec3 vp;
void main() {
    gl_Position = vec4(vp.x -0.3, vp.y + 0.3, vp.z, 1.0);
};
