#version 330

// Vertex shader pour skybox

// Inputs
in vec3 vertexPosition;

// Outputs vers fragment shader
out vec3 fragTexCoord;

// Uniforms
uniform mat4 matProjection;
uniform mat4 matView;
uniform mat4 matModel;

void main()
{
    // Les coordonnées de texture sont les coordonnées locales du cube
    // Elles servent de vecteur de direction pour échantillonner la cubemap
    fragTexCoord = vertexPosition;
    
    // Calcul de la position finale
    // On retire la translation de la matrice de vue pour que la skybox
    // suive toujours la caméra
    mat4 viewWithoutTranslation = mat4(mat3(matView));
    vec4 pos = matProjection * viewWithoutTranslation * matModel * vec4(vertexPosition, 1.0);
    
    // Astuce : on force Z = W pour que la skybox soit toujours au fond
    // (depth = 1.0 après division de perspective)
    gl_Position = pos.xyww;
}
