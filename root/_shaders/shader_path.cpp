#include "shader_path.h"
#include <fstream>
#include <string>
#include <cstring>

std::string path = "/root/_shaders/";

std::vector<ShaderPath::File> ShaderPath::files = {
    { path + "main/vert.glsl", VERTEX },
    { path + "main/frag.glsl", FRAG },
    { path + "main/color.glsl", COLOR },
    { path + "main/texture.glsl", TEXTURE },
    { path + "lightning/ambient_light.glsl", AMBIENT_LIGHT },
    { path + "lightning/point_light.glsl", POINT_LIGHT },
    { path + "skybox/skybox_vert.glsl", SKYBOX_VERT },
    { path + "skybox/skybox_frag.glsl", SKYBOX_FRAG },
    { path + "skybox/star_color.glsl", STAR_COLOR },
    { path + "skybox/star_attr.glsl", STAR_ATTR },
    { path + "effect/fresnel.glsl", FRESNEL },
    { path + "effect/noise.glsl", NOISE }
};