#pragma once

#include "Core.h"
#include "Layer.h"


namespace SIByL
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

		std::vector<Layer*>::iterator begin() { return Layers.begin(); }
		std::vector<Layer*>::iterator end() { return Layers.end(); }

	private:
		std::vector<Layer*> Layers;
		unsigned int LayerInsertIndex = 0;
	};
}