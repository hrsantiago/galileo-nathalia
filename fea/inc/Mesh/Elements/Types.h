#pragma once

namespace fea
{
	namespace mesh
	{
		namespace elements
		{
			enum class type : unsigned
			{
				//1D
				bar			= 1 << 0,
				beam2		= 1 << 1, 
				beam3		= 1 << 2,
				beamT		= 1 << 3,
				//2D
				plane		= 1 << 4,
				plate		= 1 << 5,
				shell		= 1 << 6,
				membrane	= 1 << 7,
				//3D
				solid		= 1 << 8,
				//heat
				heat		= 1 << 9,
				//last
				last
			};
		}
	}
}
