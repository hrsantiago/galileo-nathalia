#pragma once

namespace fea
{
	namespace mesh
	{
		namespace elements
		{
			enum class type : unsigned
			{
				bar			= 1 << 0,
				rope		= 1 << 1,
				heat		= 1 << 2,
				cable		= 1 << 3,
				beam2		= 1 << 4, 
				beam3		= 1 << 5,
				plane		= 1 << 6,
				plate		= 1 << 7,
				shell		= 1 << 8,
				solid		= 1 << 9,
				membrane	= 1 << 10,
				last
			};
		}
	}
}
