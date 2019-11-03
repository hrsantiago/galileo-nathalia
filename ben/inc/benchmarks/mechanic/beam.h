#pragma once

namespace tests
{
	namespace beam
	{
		namespace state
		{
			void axial(void);
			void shear(void);
			void torsion(void);
			void bending(void);
			void rigid_body(void);
		}
		namespace buckling
		{
			void column_buckling(void);
		}
		namespace static_linear
		{
			void load_axial(void);
			void load_shear(void);
			void load_torsion(void);
			void load_bending(void);
		}
		namespace static_nonlinear
		{
			namespace elastic
			{
				void self_weight(void);
				void hockling_cable(void);
				void column_buckling(void);
				void von_mises_buckling(void);
				void cantilever_tip_force(void);
				void cantilever_tip_moment(void);
				void cantilever_right_angle(void);
				void frame_lee(void);
				void frame_williams(void);
				void frame_right_angle(void);
				void frame_portal(void);
				void frame_roof(void);
				void frame_square(void);
				void frame_buckling(void);
				void arc_circle(void);
				void arc_asymmetric(void);
				void arc_shallow_pinned(void);
				void arc_shallow_clamped(void);
				void arc_cantilever(void);
			}
			namespace inelastic
			{
				namespace steel
				{
					void point_test_3(void);
					void point_test_4(void);
					void column_buckling(void);
					void cantilever_tip_force(void);
					void cantilever_tip_moment(void);
					void propped_force_midspan(void);
					void propped_force_distributed(void);
				}
				namespace concrete
				{
					void pinned_tip_moment(void);
					void cantilever_tip_force(void);
					void column_buckling(void);
					void frame_square(void);
				}
			}
		}
		namespace dynamic_nonlinear
		{
			void frame_lee(void);
			void pinned_free(void);
			void blade_rotation(void);
			void cantilever_tip_force(void);
			void arc_shallow_clamped(void);
		}
	}
}
