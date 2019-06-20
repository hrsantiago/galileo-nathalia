#pragma once

namespace fea
{
	namespace mesh
	{
		namespace sections
		{
			enum class type : unsigned
			{
				generic		= 1 << 0,
				rectangle	= 1 << 1,
				circle		= 1 << 2,
				box			= 1 << 3,
				ring		= 1 << 4,
				T			= 1 << 5,
				I			= 1 << 6,
				last
			};
		}
	}
}
