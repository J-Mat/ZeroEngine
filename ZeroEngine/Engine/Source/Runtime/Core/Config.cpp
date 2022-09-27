
#include "Config.h"

namespace Zero
{
    FConfig::FConfig()
    {
        m_RootFolder =  ENGINE_ROOT_DIR ;
        m_AssetsFolder = m_RootFolder / "Assets";
        m_ShadersFolder = m_AssetsFolder / "Shader";
        m_TexturesFolder = m_AssetsFolder / "Textures";
        m_ObjFolder = m_AssetsFolder / "Obj";
#ifdef  EDITOR_MODE
        m_EditorContentFolder = m_RootFolder / "Engine" / "Source" / "Editor" / "Content";
#endif
    }

    void FConfig::Clear()
    {
        m_RootFolder.clear();
        m_AssetsFolder.clear();
        m_ShadersFolder.clear();
    }

    const std::filesystem::path& FConfig::GetRootFolder() const
    {
        return m_RootFolder;
    }

    const std::filesystem::path& FConfig::GetAssetsFolder() const
    { 
        return m_AssetsFolder;
    }

    const std::filesystem::path& FConfig::GetShadersFolder() const
    { 
        return m_ShadersFolder;
    }
    const std::filesystem::path& FConfig::GetObjFolder() const
    {
        return m_ObjFolder;
    }
    const std::filesystem::path FConfig::GetShaderFullPath(const std::string& RelativePath) const
    {
        return m_ShadersFolder / RelativePath;
    }
    const std::filesystem::path FConfig::GetTextureFullPath(const std::string& RelativePath) const
    {
        return m_TexturesFolder / RelativePath;
    }
    const std::filesystem::path FConfig::GetObjFullPath(const std::string& RelativePath) const
    {
        return m_ObjFolder / RelativePath;
    }
#ifdef  EDITOR_MODE
    const std::filesystem::path FConfig::GetEditorContentFolder() const
    {
        return m_EditorContentFolder;
    }
    const std::filesystem::path FConfig::GetEditorContentFullPath(const std::string& RelativePath) const
    {
        return m_EditorContentFolder / RelativePath;
    }
#endif
}