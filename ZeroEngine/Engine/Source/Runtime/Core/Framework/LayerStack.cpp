#include "LayerStack.h"

namespace Zero
{
	FLayerStack::FLayerStack()
	{

	}

	FLayerStack::~FLayerStack()
	{
		for (FLayer* Layer : Layers)
		{
			delete Layer;
		}
	}

	void FLayerStack::PushLayer(FLayer* Layer)
	{
		Layers.emplace(Layers.begin() + LayerInsertIndex, Layer);
		LayerInsertIndex++;
	}

	void FLayerStack::PushOverlay(FLayer* Overlay)
	{
		Layers.emplace_back(Overlay);
	}

	void FLayerStack::PopLayer(FLayer* Layer)
	{
		auto it = std::find(Layers.begin(), Layers.end(), Layer);
		if (it != Layers.end())
		{
			Layers.erase(it);
			LayerInsertIndex--;
		}
	}

	void FLayerStack::PopOverlay(FLayer* Overlay)
	{
		auto it = std::find(Layers.begin(), Layers.end(), Overlay);
		if (it != Layers.end())
		{
			Layers.erase(it);
		}
	}
}