#version 120

varying vec3 fragPos;  // Posição do fragmento no espaço do mundo
varying vec3 normal;   // Normal do fragmento

void main() {
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    fragPos = vec3(gl_ModelViewMatrix * gl_Vertex);  // Posição no espaço do mundo
    normal = (gl_NormalMatrix * gl_Normal); // Normal transformada
}