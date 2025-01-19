#version 120

varying vec3 fragPos;
varying vec3 normal;

uniform vec3 lightPos;    // Posição da luz
uniform vec3 viewPos;     // Posição da câmera
uniform vec3 lightColor;  // Cor da luz
uniform vec3 objectColor; // Cor do objeto

void main() {
    // Calcula direção da luz
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);

    // Componente difusa
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * 0.6;

    // Componente especular (Phong)
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 42.0); // "32.0" controla o brilho
    vec3 specular = spec * lightColor * 1.7;

    // Cor final
    vec3 ambient = 0.6 * lightColor; // Luz ambiente
    vec3 result = (ambient + diffuse + specular) * objectColor;
    gl_FragColor = vec4(result, 1.0);
}