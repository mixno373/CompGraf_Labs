#version 460 core
in vec3 ourColor;
out vec4 frag_colour;
void main() {
    frag_colour = vec4(ourColor, 1.0);
}