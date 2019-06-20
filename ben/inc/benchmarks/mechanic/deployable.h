#pragma once

namespace tests
{
	namespace deployable
	{
		namespace state
		{
			void slut_unit(void);
			void slut_line(void);
			void slut_grid(void);
			void slut_roof(void);
			void arch_polar(void);
			void arch_trans(void);
		}
		namespace static_linear
		{
			void arch_polar(void);
			void arch_trans(void);
		}
		namespace static_nonlinear
		{
			void tent_unit(void);
			void tent_size(void);
			void slut_fric(void);
			void slut_mult(void);
			void slut_beam(void);
			void slut_unit(void);
			void slut_line(void);
			void slut_wall(void);
			void slut_roof(void);
			void slut_test(void);
			void arch_polar(void);
			void arch_trans(void);
		}
	}
}
