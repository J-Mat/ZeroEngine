#pragma once

#include "Base/PublicSingleton.h"
#include "Core.h"

#include <filesystem>

namespace Zero
{
    class FConfig final : public IPublicSingleton<FConfig>
    {
    public:
        FConfig();
        FConfig(const FConfig&) = delete;
        FConfig& operator=(const FConfig&) = delete;

    public:
        void Clear();

        const std::filesystem::path& GetRootFolder() const;
        const std::filesystem::path& GetAssetsFolder() const;
        const std::filesystem::path& GetShadersFolder() const;
        const std::filesystem::path& GetObjFolder() const;
        const std::filesystem::path GetShaderFullPath(const std::string& RelativePath) const;
        const std::filesystem::path GetTextureFullPath(const std::string& RelativePath) const;
        const std::filesystem::path GetObjFullPath(const std::string& RelativePath) const;
#ifdef  EDITOR_MODE
        const std::filesystem::path GetEditorContentFolder() const;
        const std::filesystem::path GetEditorContentFullPath(const std::string& RelativePath) const;
#endif //  EDITOR_MODE

    private:
        std::filesystem::path m_RootFolder;
        std::filesystem::path m_AssetsFolder;
        std::filesystem::path m_ShadersFolder;
        std::filesystem::path m_TexturesFolder;
        std::filesystem::path m_ObjFolder;
#ifdef  EDITOR_MODE
        std::filesystem::path m_EditorContentFolder;
#endif //  EDITOR_MODE
    };
}