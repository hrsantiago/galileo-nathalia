#pragma once

namespace fea
{
	namespace mesh
	{
		namespace joints
		{
			enum class state : unsigned
			{
				axial		= 1 << 0,
				shear_y		= 1 << 1,
				shear_z		= 1 << 2,
				torsion		= 1 << 3, 
				bending_y	= 1 << 4,
				bending_z	= 1 << 5, 
				last
			};
		}
	}
}
