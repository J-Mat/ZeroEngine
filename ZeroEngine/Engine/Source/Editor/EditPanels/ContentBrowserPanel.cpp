#include "ContentBrowserPanel.h"
#include "ZConfig.h"
#include "Editor.h"

namespace Zero
{
	void FContentBrowserPanel::Init()
	{
		m_SelectedFolder.clear();
		m_SelectedFile.clear();

		m_FolderIcon = FEditor::CreateIcon("folder.png");
		m_ShaderIcon = FEditor::CreateIcon("shader.png");
		m_ImageIcon = FEditor::CreateIcon("image.png");
		m_ModelIcon = FEditor::CreateIcon("model.png");
		m_FileIcon = FEditor::CreateIcon("file.png");
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
					
				
				if (ImGui::TreeNodeEx(Path.string().c_str(), NodeFlags, "%s", FileName.c_str()))
				{
					if (ImGui::IsItemClicked())
					{
						m_SelectedFolder = Child;
						m_SelectedFile = "";
					}

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
		while (CurPath != ZConfig::AssetsDir && !CurPath.empty())
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
					m_SelectedFile = "";
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

	Ref<FTexture2D> FContentBrowserPanel::GetIcon(const std::filesystem::path& File)
	{
		const std::string FileName = File.string();
		if (FileName.ends_with(".hlsl"))
			return m_ShaderIcon;

		if (FileName.ends_with(".obj") || FileName.ends_with(".fbx"))
			return m_ModelIcon;
	

		if (FileName.ends_with(".png") || FileName.ends_with(".jpg") || FileName.ends_with(".tga"))
		{
			auto Texture = FRenderer::GraphicFactroy->CreateTexture2D(File.filename().string());
			Texture->RegistGuiShaderResource();
			return Texture;
		}
		return m_FileIcon;
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
			ImGui::BeginGroup();
			auto& Child = Paths[i];
			std::string FilePathStr = Child.filename().string();
			ImGui::PushID(FilePathStr.c_str());
			
			Ref<FTexture2D> IconTexture = std::filesystem::is_directory(Child) ? m_FileIcon : GetIcon(Child);
			ImTextureID TextureID = (ImTextureID)IconTexture->GetGuiShaderReseource();
			ImVec4 Tint = m_SelectedFile == Child ? ImVec4{ 0.65f, 0.65f, 1.0f, 1.f } : ImVec4{ 1,1,1,1 };
			if (m_SelectedFile == Child)
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 1));
			else
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 1));
						
			ImGui::ImageButton(TextureID, ButtonSize, { 0,0 }, { 1,1 }, 0, Style.Colors[ImGuiCol_WindowBg], Tint);
			std::string FileName = Child.stem().string();

			char* DropDragType = "";
			if (IconTexture == m_ModelIcon)
			{
				DropDragType = ASSEST_PANEL_OBJ;
			}
			
			
			if (ImGui::BeginDragDropSource())
			{
				ImGui::Image(TextureID, ButtonSize, { 0,0 }, { 1,1 }	, Style.Colors[ImGuiCol_WindowBg], Tint);
				ImGui::TextWrapped(FileName.c_str());
				const char* ItemPath = FilePathStr.c_str();
				ImGui::SetDragDropPayload(DropDragType, ItemPath, FilePathStr.length() + 1);
				ImGui::EndDragDropSource();
			}

			Utils::NameShrink(FileName, 9);

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (std::filesystem::is_directory(Child))
				{
					m_SelectedFile = "";
					m_SelectedFolder = Child;
				}
			}
			if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				m_SelectedFile = Child;
			}


			ImGui::TextWrapped(FileName.c_str());
			ImGui::PopStyleColor();
			ImGui::EndGroup();

			float LastX = ImGui::GetItemRectMax().x;
			float NextX = LastX + Style.ItemSpacing.x + ButtonSize.x;
			if (i + 1 < Paths.size() && NextX < WindowVisibleMax_X)
				ImGui::SameLine();
			ImGui::PopID();
		}
	}

	void FContentBrowserPanel::OnGuiRender()
	{
		std::filesystem::path AssetsFolder = ZConfig::AssetsDir;
		if (ImGui::Begin("Folders"))
		{
			ProjectViewerSystemPrintChildren(AssetsFolder);
		}
		ImGui::End();
		
		
		if (ImGui::Begin("Assest"))
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
