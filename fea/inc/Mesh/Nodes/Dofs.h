#pragma once

namespace fea
{
	namespace mesh
	{
		namespace nodes
		{
			enum class dof : unsigned
			{
				translation_x 	= 1 << 0,
				translation_y 	= 1 << 1,
				translation_z 	= 1 << 2,
				rotation_x 		= 1 << 3,
				rotation_y 		= 1 << 4,
				rotation_z 		= 1 << 5,
				temperature		= 1 << 6,
				last
			};
		}
	}
}
