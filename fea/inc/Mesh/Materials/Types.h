#pragma once

namespace fea
{
	namespace mesh
	{
		namespace materials
		{
			enum class type : unsigned
			{
				heat		= 1 << 0,
				steel		= 1 << 1,
				concrete	= 1 << 2,
				last
			};
		}
	}
}
