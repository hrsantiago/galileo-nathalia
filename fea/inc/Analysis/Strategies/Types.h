#pragma once

namespace fea
{
	namespace analysis
	{
		namespace strategies
		{
			enum class type : unsigned
			{
				control_load			= 1 << 0,
				control_state			= 1 << 1,
				arc_length_linear		= 1 << 2,
				arc_length_spheric		= 1 << 3,
				arc_length_cylindric	= 1 << 4,
				minimal_norm			= 1 << 5,
				orthogonal_residue		= 1 << 6,
				last
			};
		}
	}
}
