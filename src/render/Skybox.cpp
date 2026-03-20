#include "Skybox.h"
#include <raymath.h>
#include <rlgl.h>

// rlgl.h fournit déjà toutes les définitions OpenGL nécessaires

Skybox::Skybox()
    : skyboxModel_{},
      skyboxShader_{},
      cubemapTexture_{},
      type_(SkyboxType::CUBEMAP),
      isLoaded_(false)
{
}

Skybox::~Skybox() {
    unload();
}

void Skybox::initializeSkyboxModel() {
    // Création d'un cube pour la skybox
    Mesh cube = GenMeshCube(1.0f, 1.0f, 1.0f);
    skyboxModel_ = LoadModelFromMesh(cube);

    // Chargement du shader skybox
    skyboxShader_ = LoadShader(
        "../assets/shaders/skybox_vs.glsl",
        "../assets/shaders/skybox_fs.glsl"
    );

    // Configuration des locations des uniforms
    skyboxShader_.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(skyboxShader_, "matModel");
    skyboxShader_.locs[SHADER_LOC_MATRIX_VIEW] = GetShaderLocation(skyboxShader_, "matView");
    skyboxShader_.locs[SHADER_LOC_MATRIX_PROJECTION] = GetShaderLocation(skyboxShader_, "matProjection");

    // Application du shader au modèle
    skyboxModel_.materials[0].shader = skyboxShader_;
}

bool Skybox::loadCubemap(
    const std::string& rightPath,
    const std::string& leftPath,
    const std::string& topPath,
    const std::string& bottomPath,
    const std::string& frontPath,
    const std::string& backPath)
{
    // Déchargement des ressources précédentes si nécessaire
    if (isLoaded_) {
        unload();
    }

    // Chargement des 6 images
    Image faces[6];
    faces[0] = LoadImage(rightPath.c_str());    // Droite (X+)
    faces[1] = LoadImage(leftPath.c_str());     // Gauche (X-)
    faces[2] = LoadImage(topPath.c_str());      // Haut (Y+)
    faces[3] = LoadImage(bottomPath.c_str());   // Bas (Y-)
    faces[4] = LoadImage(frontPath.c_str());    // Face (Z+)
    faces[5] = LoadImage(backPath.c_str());     // Arrière (Z-)

    // Vérification que toutes les images sont chargées
    for (int i = 0; i < 6; i++) {
        if (faces[i].data == nullptr) {
            // Déchargement des images déjà chargées
            for (int j = 0; j < i; j++) {
                UnloadImage(faces[j]);
            }
            return false;
        }
    }

    // Création de la texture cubemap
    cubemapTexture_ = createCubemapTexture(faces);

    // Déchargement des images temporaires
    for (int i = 0; i < 6; i++) {
        UnloadImage(faces[i]);
    }

    // Initialisation du modèle et du shader
    initializeSkyboxModel();

    // Application de la texture au matériau
    skyboxModel_.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = cubemapTexture_;

    type_ = SkyboxType::CUBEMAP;
    isLoaded_ = true;

    return true;
}

bool Skybox::loadPanoramic(const std::string& panoramicPath) {
    // Déchargement des ressources précédentes si nécessaire
    if (isLoaded_) {
        unload();
    }

    // Vérification de l'extension du fichier
    std::string extension = panoramicPath.substr(panoramicPath.find_last_of(".") + 1);

    Image panoramic;

    if (extension == "hdr") {
        // Pour les fichiers HDR, on doit les charger manuellement car LoadImage ne supporte pas HDR
        // Alternative : utiliser un fichier PNG/JPG panoramique au lieu de HDR
        TraceLog(LOG_WARNING, "SKYBOX: HDR files are not directly supported by LoadImage");
        TraceLog(LOG_INFO, "SKYBOX: Please use a PNG or JPG equirectangular image instead");
        TraceLog(LOG_INFO, "SKYBOX: You can convert HDR to PNG using tools like ImageMagick or Photoshop");
        return false;
    } else {
        // Chargement de l'image panoramique PNG/JPG
        panoramic = LoadImage(panoramicPath.c_str());
        if (panoramic.data == nullptr) {
            TraceLog(LOG_ERROR, "SKYBOX: Failed to load panoramic image: %s", panoramicPath.c_str());
            return false;
        }
    }

    // Conversion en cubemap
    cubemapTexture_ = convertPanoramicToCubemap(panoramic);

    // Déchargement de l'image temporaire
    UnloadImage(panoramic);

    // Initialisation du modèle et du shader
    initializeSkyboxModel();

    // Application de la texture au matériau
    skyboxModel_.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = cubemapTexture_;

    type_ = SkyboxType::PANORAMIC;
    isLoaded_ = true;

    return true;
}

void Skybox::draw(const Camera3D& camera) const {
    if (!isLoaded_) {
        return;
    }

    // Désactivation de l'écriture dans le depth buffer
    rlDisableDepthMask();

    // Désactivation du backface culling pour la skybox
    rlDisableBackfaceCulling();

    // Activation du shader
    rlEnableShader(skyboxShader_.id);

    // Bind de la texture cubemap manuellement
    rlActiveTextureSlot(0);
    rlEnableTextureCubemap(cubemapTexture_.id);

    // Envoi de l'uniform environmentMap = 0
    int envMapLoc = GetShaderLocation(skyboxShader_, "environmentMap");
    if (envMapLoc != -1) {
        int textureUnit = 0;
        SetShaderValue(skyboxShader_, envMapLoc, &textureUnit, SHADER_UNIFORM_INT);
    }

    // Dessin du modèle avec le shader déjà actif
    DrawModel(skyboxModel_, Vector3{0.0f, 0.0f, 0.0f}, 1000.0f, WHITE);

    // Désactivation de la texture cubemap
    rlDisableTextureCubemap();

    // Réactivation du backface culling
    rlEnableBackfaceCulling();

    // Réactivation de l'écriture dans le depth buffer
    rlEnableDepthMask();
}

bool Skybox::isLoaded() const {
    return isLoaded_;
}

void Skybox::unload() {
    if (!isLoaded_) {
        return;
    }

    UnloadTexture(cubemapTexture_);
    UnloadShader(skyboxShader_);
    UnloadModel(skyboxModel_);

    isLoaded_ = false;
}

Texture2D Skybox::createCubemapTexture(const Image faces[6]) {
    // Utilisation de l'API Raylib pour créer une cubemap
    // Raylib 4.0+ supporte LoadTextureCubemap qui attend une image avec layout

    // Pour l'instant, on utilise une approche simple :
    // Créer une image avec les 6 faces disposées en layout vertical ou horizontal

    int faceSize = faces[0].width;

    // Création d'une image contenant les 6 faces en layout vertical (6x1)
    Image cubemapLayout = GenImageColor(faceSize, faceSize * 6, BLANK);

    // Copie de chaque face dans le layout
    for (int i = 0; i < 6; i++) {
        ImageDraw(&cubemapLayout, faces[i],
                  (Rectangle){0, 0, (float)faceSize, (float)faceSize},
                  (Rectangle){0, (float)(i * faceSize), (float)faceSize, (float)faceSize},
                  WHITE);
    }

    // Chargement de la cubemap depuis le layout
    Texture2D cubemap = LoadTextureCubemap(cubemapLayout, CUBEMAP_LAYOUT_LINE_VERTICAL);

    // Nettoyage
    UnloadImage(cubemapLayout);

    return cubemap;
}

Texture2D Skybox::convertPanoramicToCubemap(const Image& panoramic) {
    // Utilisation de la fonction native Raylib pour convertir
    // une image panoramique en cubemap

    // LoadTextureCubemap détecte automatiquement le format de l'image
    Texture2D cubemap = LoadTextureCubemap(panoramic, CUBEMAP_LAYOUT_AUTO_DETECT);

    return cubemap;
}