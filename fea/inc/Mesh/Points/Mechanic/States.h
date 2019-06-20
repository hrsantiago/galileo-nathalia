#pragma once

namespace fea
{
	namespace mesh
	{
		namespace points
		{
			enum class state : unsigned
			{
				elastic,
				plastic,
				rupture
			};
		}
	}
}
