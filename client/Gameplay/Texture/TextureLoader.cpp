#include "TextureLoader.h"
#include <Containers/StringTable.h>
#include <filesystem>

namespace fs = std::filesystem;
StringTable* TextureLoader::textureStringTable = nullptr;

bool TextureLoader::Load()
{
    fs::path absolutePath = std::filesystem::absolute("Data/extracted/Textures");
    if (!fs::is_directory(absolutePath))
    {
        NC_LOG_ERROR("Failed to find Textures folder");
        return false;
    }

    fs::path filePath = absolutePath.append("TextureStringTable.nst");
    if (!fs::exists(filePath) || filePath.extension() != ".nst")
    {
        NC_LOG_ERROR("Failed to find TextureStringTable.nst");
        return false;
    }

    FileReader stringTableFile(filePath.string(), filePath.filename().string());
    if (!stringTableFile.Open())
    {
        NC_LOG_ERROR("Failed to open TextureStringTable.nst");
        return false;
    }

    assert(textureStringTable == nullptr);
    textureStringTable = new StringTable();

    std::shared_ptr<Bytebuffer> buffer = Bytebuffer::Borrow<8388608>();
    stringTableFile.Read(buffer.get(), buffer->size);

    textureStringTable->Deserialize(buffer.get());
    assert(textureStringTable->GetNumStrings() > 0); // We always expect to have at least 1 string in our texture stringtable

    NC_LOG_SUCCESS("Loaded Texture StringTable with %u entries", textureStringTable->GetNumStrings());
    return true;
}