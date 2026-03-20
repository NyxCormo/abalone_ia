#version 330

// Fragment shader pour skybox

// Input du vertex shader
in vec3 fragTexCoord;

// Output
out vec4 finalColor;

// Uniform pour la texture cubemap
uniform samplerCube environmentMap;

void main()
{
    // Échantillonnage de la cubemap avec le vecteur de direction
    vec3 color = texture(environmentMap, fragTexCoord).rgb;
    
    finalColor = vec4(color, 1.0);
}
