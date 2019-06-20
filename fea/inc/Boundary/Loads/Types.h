#pragma once

namespace fea
{
	namespace boundary
	{
		namespace loads
		{
			enum class type : unsigned
			{
				axial		= 1 << 0,
				shear		= 1 << 1,
				torsion		= 1 << 2,
				bending		= 1 << 3,
				plane_edge	= 1 << 4,
				plane_face	= 1 << 5,
				solid_edge	= 1 << 6,
				solid_face	= 1 << 7,
				solid_body	= 1 << 8,
				heat_flux	= 1 << 9,
				heat_source	= 1 << 10,
				last
			};
		}
	}
}
