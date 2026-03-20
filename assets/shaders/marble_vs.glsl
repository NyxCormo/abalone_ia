#version 330

// Vertex shader pour marbre poli

// Inputs
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;

// Outputs vers fragment shader
out vec2 fragTexCoord;
out vec3 fragNormal;
out vec3 fragPosition;
out vec3 viewDirection;

// Uniforms
uniform mat4 mvp;
uniform mat4 matModel;
uniform mat4 matNormal;
uniform vec3 viewPos;

void main()
{
    // Position transformée
    vec4 worldPosition = matModel * vec4(vertexPosition, 1.0);
    fragPosition = worldPosition.xyz;
    
    // Normale transformée (pour l'éclairage)
    fragNormal = normalize(mat3(matNormal) * vertexNormal);
    
    // Coordonnées de texture
    fragTexCoord = vertexTexCoord;
    
    // Direction de vue (pour les reflets spéculaires)
    viewDirection = normalize(viewPos - fragPosition);
    
    // Position finale
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
