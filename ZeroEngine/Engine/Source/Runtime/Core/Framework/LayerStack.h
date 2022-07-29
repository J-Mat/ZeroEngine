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

		std::vector<FLayer*>::iterator begin() { return Layers.begin(); }
		std::vector<FLayer*>::iterator end() { return Layers.end(); }

	private:
		std::vector<FLayer*> Layers;
		unsigned int LayerInsertIndex = 0;
	};
}