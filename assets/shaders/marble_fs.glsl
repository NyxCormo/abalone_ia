#version 330

// Fragment shader pour marbre poli avec effet de pierre polie

// Inputs du vertex shader
in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragPosition;
in vec3 viewDirection;

// Output
out vec4 finalColor;

// Uniforms
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Lumières (vous pouvez en ajouter plusieurs)
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float lightIntensity;

// Paramètres du matériau pour effet marbre poli
uniform float shininess;      // Brillance (plus élevé = plus brillant)
uniform float specularStrength; // Force du reflet spéculaire
uniform float ambientStrength;  // Lumière ambiante

void main()
{
    // Normalisation des vecteurs
    vec3 normal = normalize(fragNormal);
    vec3 viewDir = normalize(viewDirection);
    
    // Couleur de base du marbre (texture)
    vec4 texColor = texture(texture0, fragTexCoord) * colDiffuse;
    
    // === ÉCLAIRAGE AMBIANT ===
    vec3 ambient = ambientStrength * lightColor;
    
    // === ÉCLAIRAGE DIFFUS (Lambert) ===
    vec3 lightDir = normalize(lightPos - fragPosition);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * lightIntensity;
    
    // === ÉCLAIRAGE SPÉCULAIRE (Blinn-Phong pour effet poli) ===
    // Blinn-Phong utilise le vecteur halfway pour un meilleur rendu
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;
    
    // === EFFET FRESNEL (reflet sur les bords) ===
    // Donne un aspect plus réaliste aux surfaces polies
    float fresnel = pow(1.0 - max(dot(viewDir, normal), 0.0), 3.0);
    vec3 fresnelColor = fresnel * 0.3 * lightColor;
    
    // === RIM LIGHTING (lumière sur les contours) ===
    float rimAmount = 1.0 - max(dot(viewDir, normal), 0.0);
    rimAmount = pow(rimAmount, 2.0);
    vec3 rimLight = rimAmount * 0.2 * lightColor;
    
    // Combinaison finale
    vec3 result = (ambient + diffuse) * texColor.rgb + specular + fresnelColor + rimLight;
    
    // Légère saturation pour un aspect "poli"
    result = mix(result, result * result, 0.1);
    
    finalColor = vec4(result, texColor.a);
}
