#include "ContentBrowserPanel.h"

namespace Zero
{
	FContentBrowserPanel::FContentBrowserPanel()
	{
	}
	void FContentBrowserPanel::Init()
	{
		m_SelectedFolder.clear();
		m_SelectedFile.clear();
		Ref<FImage> FolderImage = CreateRef<FImage>(FConfig::GetInstance().GetEditorContentFullPath("folder.png").string());
		m_FolderIcon = FRenderer::GraphicFactroy->CreateTexture2D(FolderImage);
		m_FolderIcon->RegistGuiShaderResource();
	}
	void FContentBrowserPanel::ProjectViewerSystemPrintChildren(std::filesystem::path Folder)
	{
		for (auto& Child : std::filesystem::directory_iterator(Folder))
		{
			if (std::filesystem::is_directory(Child))
			{
				const auto& Path = Child.path();
				std::string FileName = Path.filename().string();

				static constexpr ImGuiTreeNodeFlags NodeBaseFlags =
					ImGuiTreeNodeFlags_OpenOnArrow
					| ImGuiTreeNodeFlags_OpenOnDoubleClick
					| ImGuiTreeNodeFlags_SpanAvailWidth;

				ImGuiTreeNodeFlags NodeFlags = NodeBaseFlags;
				if (m_SelectedFolder == Child)
					NodeFlags |= ImGuiTreeNodeFlags_Selected;
				
				if (Utils::IsParentFolder(Child.path(), m_SelectedFolder))
					ImGui::SetNextItemOpen(true, ImGuiCond_Always);
				
				if (ImGui::TreeNodeEx(Path.string().c_str(), NodeBaseFlags, "%s", FileName.c_str()))
				{
					if (ImGui::IsItemClicked())
						m_SelectedFolder = Child;

					ProjectViewerSystemPrintChildren(Child.path());
					ImGui::TreePop();
				}
			}
		}
	}

	void FContentBrowserPanel::PrintFolder()
	{
		auto CurPath = m_SelectedFolder;
		std::vector<std::filesystem::path> Paths;
		while (CurPath != FConfig::GetInstance().GetAssetsFolder() && !CurPath.empty())
		{
			Paths.push_back(CurPath);
			CurPath = CurPath.parent_path();
		}
		for (int32_t i = int32_t(Paths.size()) - 1; i >= 0; --i)
		{
			const auto& Path = Paths[i];
			if (i > 0)
			{
				if (ImGui::SmallButton(Path.stem().string().c_str()))
				{
					m_SelectedFolder = Path;
				}
				ImGui::SameLine();
				ImGui::Text(">");
				ImGui::SameLine();
			}
			else
			{
				ImGui::Text(Path.stem().string().c_str());
			}
		}
		ImGui::Separator();
	}

	void FContentBrowserPanel::PrintFiles()
	{
		ImGuiStyle& Style = ImGui::GetStyle();
		static ImVec2 ButtonSize(64, 64);

		std::vector<std::filesystem::path> Paths;
		for (auto& Child : std::filesystem::directory_iterator(m_SelectedFolder))
		{
			if (std::filesystem::is_directory(Child))
				Paths.push_back(Child);
		}
		for (auto& Child : std::filesystem::directory_iterator(m_SelectedFolder))
		{
			if (!std::filesystem::is_directory(Child))
				Paths.push_back(Child);
		}

		float WindowVisibleMax_X = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

		for (size_t i = 0; i < Paths.size(); ++i)
		{
			auto& Child = Paths[i];
			ImGui::BeginGroup();
			{
				ImTextureID TextureID = 0;
				if (std::filesystem::is_directory(Child))
				{
					TextureID = (ImTextureID)m_FolderIcon->GetGuiShaderReseource();
					ImVec4 Tint = m_SelectedFile == Child ? ImVec4{ 0.65f, 0.65f, 0.65f, 1.f } : ImVec4{ 1,1,1,1 };
					if (ImGui::ImageButton(TextureID, ButtonSize, { 0,0 }, { 1,1 }, 0, Style.Colors[ImGuiCol_WindowBg], Tint))
					{
						m_SelectedFile = Child;
					}
					std::string FileName = Child.stem().string();
					Utils::NameShrink(FileName, 9);
					ImGui::Text(FileName.c_str());
				}
			}
			ImGui::EndGroup();
			float LastX = ImGui::GetItemRectMax().x;
			float NextX = LastX + Style.ItemSpacing.x + ButtonSize.x;
			if (i + 1 < Paths.size() && NextX < WindowVisibleMax_X)
				ImGui::SameLine();
		}
	}

	void FContentBrowserPanel::OnGuiRender()
	{
		std::filesystem::path AssetsFolder = FConfig::GetInstance().GetAssetsFolder();
		if (ImGui::Begin("Assests"))
		{
			ProjectViewerSystemPrintChildren(AssetsFolder);
		}
		ImGui::End();
		
		
		if (ImGui::Begin("Folder"))
		{
			if (!m_SelectedFolder.empty())
			{
				PrintFolder();
				PrintFiles();
			}
		}
		ImGui::End();
	}
}
