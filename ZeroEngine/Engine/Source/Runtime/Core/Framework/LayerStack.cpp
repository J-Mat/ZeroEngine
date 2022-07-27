#include "LayerStack.h"

namespace SIByL
{
	FLayerStack::FLayerStack()
	{

	}

	FLayerStack::~FLayerStack()
	{
		for (FLayer* Layer : Layers)
			delete Layer;
	}

	void FLayerStack::PushLayer(FLayer* Layer)
	{
		Layers.emplace(m_Layers.begin() + LayerInsertIndex, Layer);
		LayerInsertIndex++;
	}

	void FLayerStack::PushOverlay(Layer* Overlay)
	{
		Layers.emplace_back(Overlay);
	}

	void FLayerStack::PopLayer(Layer* Layer)
	{
		auto it = std::find(Layers.begin(), Layers.end(), layer);
		if (it != Layers.end())
		{
			Layers.erase(it);
			LayerInsertIndex--;
		}
	}

	void LayerStack::PopOverlay(Layer* overlay)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.end(), overlay);
		if (it != m_Layers.end())
			Layers.erase(it);
	}
}