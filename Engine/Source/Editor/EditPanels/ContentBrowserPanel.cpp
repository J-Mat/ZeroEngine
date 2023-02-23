#include "ContentBrowserPanel.h"
#include "ZConfig.h"
#include "Editor.h"
#include "Data/Asset/AssetManager.h"
#include "Data/Asset/AssetObject/MaterialAsset.h"
#include "Render/Moudule/Texture/TextureManager.h"

namespace Zero
{
	FContentBrowserPanel::FContentBrowserPanel()
	{
		m_SelectedFolder.clear();
		m_SelectedFile.clear();

		m_FolderIcon = FEditor::CreateIcon("EditorRes/folder.png");
		m_ShaderIcon = FEditor::CreateIcon("EditorRes/shader.png");
		m_ImageIcon = FEditor::CreateIcon("EditorRes/image.png");
		m_ModelIcon = FEditor::CreateIcon("EditorRes/model.png");
		m_FileIcon = FEditor::CreateIcon("EditorRes/file.png");
		m_SceneIcon = FEditor::CreateIcon("EditorRes/scene.png");
		FEditor::m_HasMaterialtexutre =  m_MaterialIcon = FEditor::CreateIcon("EditorRes/material.png");
		FEditor::m_NullMaterialtexutre = FEditor::CreateIcon("EditorRes/none.png");
	}
	void FContentBrowserPanel::Init()
	{
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

	void FContentBrowserPanel::DisplayFolder()
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

	bool FContentBrowserPanel::IsTexture(const std::filesystem::path& File)
	{
		const std::string FileName = File.string();
		return FileName.ends_with(".png") || FileName.ends_with(".jpg") || FileName.ends_with(".tga");
	}

	Ref<FTexture2D> FContentBrowserPanel::GetIcon(const std::filesystem::path& File)
	{
		const std::string FileName = File.string();
		if (FileName.ends_with(".hlsl"))
		{
			m_DragType = ASSET_PANEL_SHADER;
			return m_ShaderIcon;
		}

		if (FileName.ends_with(".obj") || FileName.ends_with(".fbx"))
		{
			m_DragType = ASSET_PANEL_OBJ;
			return m_ModelIcon;
		}

		if (FileName.ends_with(".scene"))
		{
			m_DragType = ASSET_PANEL_FILE;
			return m_SceneIcon;
		}
		if (FileName.ends_with(".mtl"))
		{
			m_DragType = ASSET_PANEL_MATERIAL;
			return m_MaterialIcon;
		}
	

		if (FileName.ends_with(".png") || FileName.ends_with(".jpg") || FileName.ends_with(".tga"))
		{
			m_DragType = ASSET_PANEL_IMAGE;
			auto TextureHandle = FTextureManager::Get().LoadTexture(m_AssetPath.string());
			return FTextureManager::Get().GetTextureByHandle(TextureHandle);
		}
		return m_FileIcon;
	}


	void FContentBrowserPanel::DisplayFiles()
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
			auto& Path = Paths[i];
			std::string FileName = Path.filename().string();
			ImGui::PushID(FileName.c_str());
			
			m_DragType = "";
			m_AssetPath = ZConfig::GetAssetReletivePath(Path);
			Ref<FTexture2D> IconTexture = std::filesystem::is_directory(Path) ? m_FolderIcon : GetIcon(Path);
			ImTextureID TextureID = (ImTextureID)IconTexture->GetGuiShaderReseource();
			ImVec4 Tint = m_SelectedFile == Path ? ImVec4{ 0.65f, 0.65f, 1.0f, 1.f } : ImVec4{ 1,1,1,1 };
			if (m_SelectedFile == Path)
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 1));
			else
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 1));
						
			ImGui::ImageButton(TextureID, ButtonSize, { 0,0 }, { 1,1 }, 0, Style.Colors[ImGuiCol_WindowBg], Tint);
	
			std::string AssetPathStr = m_AssetPath.string();
			if (m_DragType != "" && ImGui::BeginDragDropSource())
			{
				ImGui::Image(TextureID, ButtonSize);
				ImGui::TextWrapped(m_AssetPath.filename().string().c_str());
				const char* ItemPath = AssetPathStr.c_str();
				ImGui::SetDragDropPayload(m_DragType.c_str() , ItemPath, strlen(ItemPath) + 1);
				ImGui::EndDragDropSource();
			}

			Utils::NameShrink(FileName, 9);

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (std::filesystem::is_directory(Path))
				{
					m_SelectedFile = "";
					m_SelectedFolder = Path;
				}
			}
			if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				m_SelectedFile = Path;
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

	void FContentBrowserPanel::DisplayAssetMenu()
	{
		static bool bShowContentMenu = false;
		static bool bShowNewMaterial = false;
		if (!IsMouseOutOfWindow())
		{
			if (MouseButtonReleased(ImGuiMouseButton_::ImGuiMouseButton_Right))
			{
				bShowContentMenu = true;
			}
		}
		if (bShowContentMenu)
		{
			ImGui::OpenPopup("ContentMenu");
		}
		if (ImGui::BeginPopup("ContentMenu"))
		{
            ImGui::Text("New Asset");
			ImGui::Separator();
			if (ImGui::BeginMenu("New"))
			{
				if (ImGui::MenuItem("Material"))
				{
					bShowNewMaterial = true;
				}
				ImGui::EndMenu();
			}
			ImGui::Separator();
			ImGui::EndPopup();
		}
		if (MouseButtonReleased(ImGuiMouseButton_::ImGuiMouseButton_Right))
		{
			bShowContentMenu = false;
		}

		if (bShowNewMaterial)
		{
			ImGui::OpenPopup("New Material");
			if (ImGui::BeginPopupModal("New Material"))
			{
				ImGui::Text("Type Matrial Name");
				static char Buffer[256] = "NewMaterial";
				ImGui::Text("Material Name:");
				ImGui::SameLine();
				ImGui::InputText("##MaterialName", Buffer, sizeof(Buffer));
				if (ImGui::Button("OK"))
				{
					CreateDefaultMaterial(Buffer);
					bShowNewMaterial = false;
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel"))
				{
					bShowNewMaterial = false;
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
		}
	}

	void FContentBrowserPanel::CreateDefaultMaterial(char* AssetTempName)
	{
		std::stringstream Stream;
		Stream << AssetTempName;
		
		std::filesystem::path TempAssetPath = m_SelectedFolder / (Stream.str() + ".mtl");
		while (std::filesystem::exists(TempAssetPath))
		{
			Stream.str("");
			Stream << AssetTempName << "_1";
			TempAssetPath = m_SelectedFolder / (Stream.str() + ".mtl");
		}
		std::filesystem::path AssetPath = ZConfig::GetAssetReletivePath(TempAssetPath);
		auto* MaterialAsset = FAssetManager::Get().NewAsset<UMaterialAsset>(AssetPath.string());
		FAssetManager::Get().Serialize(MaterialAsset);
		auto* Asset = FAssetManager::Get().Remove(AssetPath.string());
		delete Asset;
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
			PreInitWindow();
			if (!m_SelectedFolder.empty())
			{
				DisplayFolder();
				DisplayFiles();
				DisplayAssetMenu();
			}
		}
		ImGui::End();
	}
}
