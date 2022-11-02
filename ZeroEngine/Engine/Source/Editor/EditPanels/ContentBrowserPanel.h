#pragma once

#include <ZeroEngine.h>


namespace Zero
{
	class FContentBrowserPanel
	{
	public:
		FContentBrowserPanel();
		void Init();
		void ProjectViewerSystemPrintChildren(std::filesystem::path Foler);
		void OnGuiRender();
	private:
		std::filesystem::path m_SelectedFolder;
		std::filesystem::path m_SelectedFile;
		Ref<FTexture2D> m_FolderIcon;
		Ref<FTexture2D> m_ShaderIcon;
		Ref<FTexture2D> m_ImageIcon;
		Ref<FTexture2D> m_ModelIcon;
		Ref<FTexture2D> m_FileIcon;
	private:
		Ref<FTexture2D> GetIcon(const std::filesystem::path& File);
		void PrintFolder();
		void PrintFiles();
	};
}