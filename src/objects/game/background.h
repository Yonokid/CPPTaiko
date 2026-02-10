#include "../../libs/texture.h"
#include "../../libs/animation.h"
#include "../../libs/script.h"

#include <lua.hpp>
#include <spdlog/spdlog.h>

class Background {
private:
    lua_State* L;
    int lua_ref;

public:
    Background(PlayerNum player_num, float bpm, const std::string& scene_preset) {
        L = luaL_newstate();
        luaL_openlibs(L);

        std::string script_path = script_manager.get_lua_script_path("background");
        if (luaL_dofile(L, script_path.c_str()) != LUA_OK) {
            spdlog::error("Error loading background.lua: {}", lua_tostring(L, -1));
            lua_pop(L, 1);
        }

        lua_getglobal(L, "Background");
        lua_getfield(L, -1, "new");
        lua_pushinteger(L, static_cast<int>(player_num));
        lua_pushnumber(L, bpm);
        lua_pushstring(L, scene_preset.c_str());

        if (lua_pcall(L, 3, 1, 0) != LUA_OK) {
            spdlog::error("Error calling Background.new: {}", lua_tostring(L, -1));
            lua_pop(L, 1);
        }

        lua_ref = luaL_ref(L, LUA_REGISTRYINDEX);
        lua_pop(L, 1);
    }

    ~Background() {
        lua_rawgeti(L, LUA_REGISTRYINDEX, lua_ref);
        lua_getfield(L, -1, "destroy");
        if (lua_isfunction(L, -1)) {
            lua_pushvalue(L, -2);  // push self
            lua_pcall(L, 1, 0, 0);
        } else {
            lua_pop(L, 1);
        }
        lua_pop(L, 1);

        luaL_unref(L, LUA_REGISTRYINDEX, lua_ref);
        lua_close(L);
    }

    void update(double current_ms) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, lua_ref);
        lua_getfield(L, -1, "update");
        lua_pushvalue(L, -2);  // push self
        lua_pushnumber(L, current_ms);

        if (lua_pcall(L, 2, 0, 0) != LUA_OK) {
            spdlog::error("Error calling update: {}", lua_tostring(L, -1));
            lua_pop(L, 1);
        }
        lua_pop(L, 1); // pop self
    }

    void add_chibi(bool is_bad, PlayerNum player_num) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, lua_ref);
        lua_getfield(L, -1, "add_chibi");
        lua_pushvalue(L, -2);  // push self
        lua_pushboolean(L, is_bad);
        lua_pushinteger(L, static_cast<int>(player_num));

        if (lua_pcall(L, 3, 0, 0) != LUA_OK) {
            spdlog::error("Error calling add_chibi: {}", lua_tostring(L, -1));
            lua_pop(L, 1);
        }
        lua_pop(L, 1);
    }

    void add_renda() {
        lua_rawgeti(L, LUA_REGISTRYINDEX, lua_ref);
        lua_getfield(L, -1, "add_renda");
        lua_pushvalue(L, -2);  // push self

        if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
            spdlog::error("Error calling add_renda: {}", lua_tostring(L, -1));
            lua_pop(L, 1);
        }
        lua_pop(L, 1);
    }

    void draw_back() {
        lua_rawgeti(L, LUA_REGISTRYINDEX, lua_ref);
        lua_getfield(L, -1, "draw_back");
        lua_pushvalue(L, -2);  // push self

        if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
            spdlog::error("Error calling draw_back: {}", lua_tostring(L, -1));
            lua_pop(L, 1);
        }
        lua_pop(L, 1);
    }

    void draw_fore() {
        lua_rawgeti(L, LUA_REGISTRYINDEX, lua_ref);
        lua_getfield(L, -1, "draw_fore");
        lua_pushvalue(L, -2);  // push self

        if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
            spdlog::error("Error calling draw_fore: {}", lua_tostring(L, -1));
            lua_pop(L, 1);
        }
        lua_pop(L, 1);
    }
};
