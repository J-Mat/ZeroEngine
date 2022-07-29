#include <ZeroEngine.h>
#include <Core/Framework/EntryPoint.h>

#include "EditorLayer.h"

namespace Zero {

	class FZeroEditor : public FApplication
	{
	public:
		FZeroEditor(HINSTANCE hInst)
			: FApplication(hInst, "ZeorEditor")
		{
			PushLayer(new FEditorLayer());
		}

		~FZeroEditor()
		{
		}
	};

	FApplication* CreateApplication(HINSTANCE hInst)
	{
		return new FZeroEditor(hInst);
	}

}
