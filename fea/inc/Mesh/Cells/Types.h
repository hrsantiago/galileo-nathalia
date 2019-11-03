#pragma once

namespace fea
{
	namespace mesh
	{
		namespace cells
		{
			enum class type : unsigned
			{
				//1D
				bar		= 1 <<  0,
				beam	= 1 <<  1,
				//2D
				tri3	= 1 <<  2,
				tri6	= 1 <<  3,
				quad4	= 1 <<  4,
				quad8	= 1 <<  5,
				quad9	= 1 <<  6,
				//3D
				tetra4	= 1 <<  7,
				tetra10	= 1 <<  8,
				brick8	= 1 <<  9,
				brick20	= 1 << 10,
				brick27	= 1 << 11,
				//last
				last
			};
		}
	}
}
