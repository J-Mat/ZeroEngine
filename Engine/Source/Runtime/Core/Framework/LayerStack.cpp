#include "LayerStack.h"

namespace Zero
{
	FLayerStack::FLayerStack()
	{

	}

	FLayerStack::~FLayerStack()
	{
		for (FLayer* Layer : m_Layers)
		{
			delete Layer;
		}
	}

	void FLayerStack::PushLayer(FLayer* Layer)
	{
		m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, Layer);
		m_LayerInsertIndex++;
		Layer->OnAttach();
	}

	void FLayerStack::PushOverlay(FLayer* Overlay)
	{
		m_Layers.emplace_back(Overlay);
		Overlay->OnAttach();
	}

	void FLayerStack::PopLayer(FLayer* Layer)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.end(), Layer);
		if (it != m_Layers.end())
		{
			m_Layers.erase(it);
			m_LayerInsertIndex--;
		}
	}

	void FLayerStack::PopOverlay(FLayer* Overlay)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.end(), Overlay);
		if (it != m_Layers.end())
		{
			m_Layers.erase(it);
		}
	}
}