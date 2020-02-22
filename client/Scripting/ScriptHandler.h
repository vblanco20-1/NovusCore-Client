#pragma once
#include <string>
#include <filesystem>
#include <asio.hpp>
#include <entt.hpp>
#include "angelscript.h"

class ScriptHandler
{
public:
    static void LoadScriptDirectory(std::string& path);
    static void ReloadScripts();

private:
    static bool LoadScript(std::filesystem::path path);

    ScriptHandler();

private:
    static std::string _scriptFolder;
};