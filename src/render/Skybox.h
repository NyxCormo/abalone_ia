#ifndef ABALONE_SKYBOX_H
#define ABALONE_SKYBOX_H

#include <raylib.h>
#include <string>


class Skybox {
public:
    enum class SkyboxType {
        CUBEMAP,
        PANORAMIC
    };

    Skybox();
    ~Skybox();

    bool loadCubemap(
        const std::string& rightPath,
        const std::string& leftPath,
        const std::string& topPath,
        const std::string& bottomPath,
        const std::string& frontPath,
        const std::string& backPath
    );
    bool loadPanoramic(
        const std::string& panoramicPath
    );
    void unload();

    void draw(const Camera3D& camera) const;

    [[nodiscard]] bool isLoaded() const;

private:

    Model skyboxModel_;
    Shader skyboxShader_;
    Texture2D cubemapTexture_;
    SkyboxType type_;
    bool isLoaded_;

    void initializeSkyboxModel();

    Texture2D createCubemapTexture(const Image faces[6]);
    Texture2D convertPanoramicToCubemap(const Image& panoramic);
};


#endif //ABALONE_SKYBOX_H