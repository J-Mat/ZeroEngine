#pragma once

#include "World/Object/CoreObject.h"
#include "World/Object/ClassObject.h"
#include "World/Object/VariateProperty.h"
#include "World/Object/ClassProperty.h"
#include "World/Object/ObjectGlobal.h"
#include "BasePanel.h"

namespace Zero
{
	class FContentBrowserPanel : public FBasePanel
	{
	public:
		FContentBrowserPanel();
		virtual void Init() override;
		virtual void OnGuiRender() override;
		void ProjectViewerSystemPrintChildren(std::filesystem::path Foler);
	private:
		std::filesystem::path m_SelectedFolder;
		std::filesystem::path m_SelectedFile;
		Ref<FTexture2D> m_FolderIcon;
		Ref<FTexture2D> m_ShaderIcon;
		Ref<FTexture2D> m_ImageIcon;
		Ref<FTexture2D> m_ModelIcon;
		Ref<FTexture2D> m_FileIcon;
		Ref<FTexture2D> m_SceneIcon;
		Ref<FTexture2D> m_MaterialIcon;
		std::string m_DragType = "";
		std::filesystem::path m_AssetPath;
	private:
		bool IsTexture(const std::filesystem::path& File);
		Ref<FTexture2D> GetIcon(const std::filesystem::path& File);
		void DisplayFolder();
		void DisplayFiles();
		void DisplayAssetMenu();
		void CreateDefaultMaterial(char* AssetTempName);
	};
}