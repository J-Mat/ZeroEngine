#include "AssignedFileDetailsMapping.h"
#include "Editor.h" 
#include "Dialog/DialogUtils.h"	

namespace Zero
{
	void FAssignedFileDetailsMapping::UpdateDetailsWidgetImpl(UProperty* Property, const char* ProperyTag)
	{
		static char Buffer[256] = {0};
		std::string* AssignedFilePtr = Property->GetData<FShaderFileHandle>();
		strcpy_s(Buffer, sizeof(Buffer), AssignedFilePtr->c_str());
		ImGui::TextDisabled(Buffer);
		std::string FormatValue;
		if (Property->GetClassCollection().FindMetas("Format", FormatValue))
		{
			ImGui::SameLine();
			if (ImGui::Button("..."))
			{
				if (FormatValue == "hlsl")
				{
					std::string FilePath = FFileDialog::OpenFile("Zero Scene (*.hlsl)\0*.hlsl\0", "hlsl");
					if (FilePath.ends_with(FormatValue))
					{
						*AssignedFilePtr = ZConfig::GetAssetReletivePath(FilePath).string();
						m_bEdited = true;
					}
				}
			}
		}
	}
}
