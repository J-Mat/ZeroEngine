#include "ResourceLibrary.h"


namespace Zero
{
	template< >
	Ref<Texture2D> FLibrary<Texture2D>::Fetch(const std::string& id)
	{
		if (Mapper.find(id) != Mapper.end())
		{
			return Mapper[id];
		}
		else
		{
			if (IsIdentifierFromPath(id))
			{
				Ref<Texture2D> texture = Texture2D::Create(IdentifierToPath(id));
				return texture;
			}
		}

		return nullptr;
	}


	template< >
	Ref<TextureCubemap> FLibrary<TextureCubemap>::Fetch(const std::string& id)
	{
		if (Mapper.find(id) != Mapper.end())
		{
			return Mapper[id];
		}
		else
		{
			if (IsIdentifierFromPath(id))
			{
				Ref<TextureCubemap> texture = TextureCubemap::Create(IdentifierToPath(id));
				return texture;
			}
		}

		return nullptr;
	}
}