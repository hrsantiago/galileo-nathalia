#pragma once

namespace tests
{
	namespace cable
	{
		namespace modal
		{
			void tower(void);
			void cable(void);
			void mixed(void);
		}
		namespace buckling
		{
			void tower(void);
			void cable(void);
			void mixed(void);
		}
		namespace static_nonlinear
		{
			void line_bridge(void);
			void line_weight(void);
			void grid_bridge(void);
			void grid_weight(void);
		}
		namespace dynamic_nonlinear
		{
			void line_bridge(void);
			void line_weight(void);
			void grid_bridge(void);
			void grid_weight(void);
		}
	}
}
