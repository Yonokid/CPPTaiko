#include "script.h"
#include <spdlog/spdlog.h>

void ScriptManager::init(fs::path script_path) {
    for (const auto& script : fs::directory_iterator(script_path)) {
        scripts[script.path().stem().string()] = script.path();
    }
    spdlog::debug("Loaded scripts:");
    for (const auto& [name, path] : scripts) {
        spdlog::debug("  {} -> {}", name, path);
    }
    spdlog::debug("Total scripts: {}", scripts.size());
}

std::string ScriptManager::get_lua_script_path(const std::string& script_name) {
    if (scripts.find(script_name) == scripts.end()) {
        throw std::runtime_error("Unable to find lua script: " + script_name);
    }
    return scripts[script_name];
}

ScriptManager script_manager;
