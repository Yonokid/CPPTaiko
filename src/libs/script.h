#pragma once

#include "global_data.h"

class ScriptManager {
private:
    std::map<std::string, std::string> scripts;

public:
    void init(fs::path script_path);

    std::string get_lua_script_path(const std::string& script_name);
};

extern ScriptManager script_manager;
