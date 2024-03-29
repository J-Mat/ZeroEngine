#include "DialogUtils.h"
#include "Render/RendererAPI.h"

namespace Zero
{
    std::string FFileDialog::OpenFile(const char* Filter, const char* Extension)
    {
		OPENFILENAMEA Ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&Ofn, sizeof(OPENFILENAME));
		Ofn.lStructSize = sizeof(OPENFILENAME);
		//ofn.hwndOwner
		switch (FGraphic::RHI)
		{
		case ERHI::DirectX12 :
			Ofn.hwndOwner = (HWND)FApplication::Get().GetWindow().GetNativeWindow();
			break;
		case ERHI::OpenGL:
			break;
		}
		Ofn.lpstrFile = szFile;
		Ofn.nMaxFile = sizeof(szFile);
		Ofn.lpstrFilter = Filter;
		Ofn.nFilterIndex = 1;
		Ofn.lpstrDefExt = Extension;
		Ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetOpenFileNameA(&Ofn) == TRUE)
		{
			return Ofn.lpstrFile;
		}
		return std::string();
    }

	std::string FFileDialog::SaveFile(const char* Filter, const char* Extension)
    {
		OPENFILENAMEA Ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&Ofn, sizeof(OPENFILENAME));
		Ofn.lStructSize = sizeof(OPENFILENAME);
		//ofn.hwndOwner
		switch (FGraphic::RHI)
		{
		case ERHI::DirectX12 :
			Ofn.hwndOwner = (HWND)FApplication::Get().GetWindow().GetNativeWindow();
			break;
		case ERHI::OpenGL:
			break;
		}
		Ofn.lpstrFile = szFile;
		Ofn.nMaxFile = sizeof(szFile);
		Ofn.lpstrFilter = Filter;
		Ofn.nFilterIndex = 1;
		Ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		Ofn.lpstrDefExt = Extension;
		if (GetSaveFileNameA(&Ofn) == TRUE)
		{
			return Ofn.lpstrFile;
		}
        return std::string();
    }
}
