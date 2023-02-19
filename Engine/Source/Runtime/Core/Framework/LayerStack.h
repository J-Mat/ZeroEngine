#pragma once

#include "Core.h"
#include "Layer.h"


namespace Zero
{
	class FLayerStack
	{
	public:
		FLayerStack();
		~FLayerStack();

		void PushLayer(FLayer* Layer);
		void PushOverlay(FLayer* Overlay);
		void PopLayer(FLayer* Layer);
		void PopOverlay(FLayer* Overlay);

		std::vector<FLayer*>::iterator begin() { return m_Layers.begin(); }
		std::vector<FLayer*>::iterator end() { return m_Layers.end(); }

	private:
		std::vector<FLayer*> m_Layers;
		unsigned int m_LayerInsertIndex = 0;
	};
}