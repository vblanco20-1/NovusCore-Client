#include "TextureLoader.h"
#include <Containers/StringTable.h>
#include <filesystem>
#include <entt.hpp>

#include "../../ECS/Components/Singletons/TextureSingleton.h"

namespace fs = std::filesystem;

bool TextureLoader::Load(entt::registry* registry)
{
    TextureSingleton& textureSingleton = registry->set<TextureSingleton>();

    fs::path relativeParentPath = "Data/extracted/Textures";
    fs::path absolutePath = std::filesystem::absolute(relativeParentPath);
    if (!fs::is_directory(absolutePath))
    {
        NC_LOG_ERROR("Failed to find Textures folder");
        return false;
    }

    for (const auto& entry : std::filesystem::recursive_directory_iterator(absolutePath))
    {
        auto path = std::filesystem::path(entry.path());

        if (fs::is_directory(path) || path.extension() != ".dds")
            continue;

        fs::path relativePath = fs::relative(path, relativeParentPath);
        std::string texturePath = relativePath.string();

        u32 pathHash = StringUtils::fnv1a_32(texturePath.c_str(), texturePath.length());

        auto itr = textureSingleton.textureHashToPath.find(pathHash);
        if (itr != textureSingleton.textureHashToPath.end())
        {
            NC_LOG_ERROR("Found duplicate texture hash (%u) Existing Texture Path: (%s), New Texture Path: (%s)", pathHash, itr->second.c_str(), texturePath.c_str());
        }

        textureSingleton.textureHashToPath[pathHash] = (relativeParentPath / relativePath).string();
    }

    NC_LOG_SUCCESS("Loaded Texture %u entries", textureSingleton.textureHashToPath.size());
    return true;
}