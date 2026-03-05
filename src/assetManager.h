#pragma once
#include "shader.h"
#include "memory"
class AssetManager {
public:
    AssetManager(){}
    void LoadShader(const std::string& name, const std::string& vert, const std::string& frag) {
        m_Shaders[name] = std::make_unique<Shader>(vert, frag);
    }

    Shader* GetShader(const std::string& name) {
        auto it = m_Shaders.find(name);
        if (it == m_Shaders.end()) {
            std::cout << "Warning: shader '" << name << "' not found\n";
            return m_Shaders["default"].get(); // fallback
        }
        return it->second.get();
    }

private:
    std::unordered_map<std::string, std::unique_ptr<Shader>>  m_Shaders;
    //std::unordered_map<std::string, std::unique_ptr<Texture>> m_Textures;
};