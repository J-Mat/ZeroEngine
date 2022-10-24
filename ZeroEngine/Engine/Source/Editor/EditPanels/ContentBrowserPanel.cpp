#include "ContentBrowserPanel.h"
#include "ZConfig.h"

namespace Zero
{
	FContentBrowserPanel::FContentBrowserPanel()
	{
	}

	Ref<FTexture2D> FContentBrowserPanel::CreateIcon(const std::string&& FileName)
	{
		Ref<FImage> Image = CreateRef<FImage>(ZConfig::GetEditorContentFullPath(FileName).string());
		auto Icon = FRenderer::GraphicFactroy->CreateTexture2D(Image);
		Icon->RegistGuiShaderResource();
		return Icon;
	}

	void FContentBrowserPanel::Init()
	{
		m_SelectedFolder.clear();
		m_SelectedFile.clear();

		m_FolderIcon = CreateIcon("folder.png");
		m_ShaderIcon = CreateIcon("shader.png");
		m_ImageIcon = CreateIcon("image.png");
		m_ModelIcon = CreateIcon("model.png");
		m_FileIcon = CreateIcon("file.png");
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
			auto& Child = Paths[i];
			ImGui::PushID(Child.c_str());
			ImGui::BeginGroup();
			{
				ImTextureID TextureID = std::filesystem::is_directory(Child) ? (ImTextureID)m_FolderIcon->GetGuiShaderReseource() : (ImTextureID)GetIcon(Child)->GetGuiShaderReseource();
				ImVec4 Tint = m_SelectedFile == Child ? ImVec4{ 0.65f, 0.65f, 1.0f, 1.f } : ImVec4{ 1,1,1,1 };
				if (m_SelectedFile == Child)
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 1));
				else
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 1));
						
				ImGui::ImageButton(TextureID, ButtonSize, { 0,0 }, { 1,1 }, 0, Style.Colors[ImGuiCol_WindowBg], Tint);
				std::string FileName = Child.stem().string();
				Utils::NameShrink(FileName, 9);


				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && std::filesystem::is_directory(Child))
				{
					m_SelectedFile = "";
					m_SelectedFolder = Child;
				}
				if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				{
					std::cout << Child.string() << std::endl;
					m_SelectedFile = Child;
				}


				ImGui::TextWrapped(FileName.c_str());
				ImGui::PopStyleColor();
			}
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
