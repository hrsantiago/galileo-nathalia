#pragma once

namespace fea
{
	namespace mesh
	{
		namespace joints
		{
			enum class type : unsigned
			{
				hinge		= 1 << 0,
				rigid		= 1 << 1,
				spring		= 1 << 2,
				pinned		= 1 << 3,
				spherical	= 1 << 4,
				last
			};
		}
	}
}
