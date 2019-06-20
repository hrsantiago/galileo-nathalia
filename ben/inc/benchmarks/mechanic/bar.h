#pragma once

namespace tests
{
	namespace bar
	{
		namespace modal
		{
			void von_mises_truss_2D(void);
		}
		namespace buckling
		{
			void von_mises_truss_2D(void);
		}
		namespace static_linear
		{
			void load_point(void);
			void load_uniform(void);
		}
		namespace static_nonlinear
		{
			void tent_unit(void);
			void tent_roof(void);
			void dome_star(void);
			void dome_circle(void);
			void space_truss(void);
			void pinned_tip_force(void);
			void von_mises_truss_2D(void);
			void von_mises_truss_3D(void);
			void inelastic_tip_force(void);
		}
		namespace dynamic_linear
		{
			void free(void);
			void tent(void);
		}
		namespace dynamic_nonlinear
		{
			void tent_unit(void);
			void tent_roof(void);
			void single_pendulum_2D(void);
			void single_pendulum_3D(void);
			void double_pendulum_2D(void);
		}
	}
}
