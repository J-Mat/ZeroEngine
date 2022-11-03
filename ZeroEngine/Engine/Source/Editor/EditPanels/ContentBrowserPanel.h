#pragma once

#include <ZeroEngine.h>
#include "BasePanel.h"


namespace Zero
{
	class FContentBrowserPanel : public FBasePanel
	{
	public:
		FContentBrowserPanel() = default;
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
	private:
		Ref<FTexture2D> GetIcon(const std::filesystem::path& File);
		void PrintFolder();
		void PrintFiles();
	};
}