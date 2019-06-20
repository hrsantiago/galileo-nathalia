#pragma once

namespace tests
{
	namespace joint
	{
		namespace pinned
		{
			void frame(void);
			void shear(void);
			void torsion(void);
			void scissor(void);
			void cantilever(void);
			void multi_scissor(void);
		}
		namespace hinge
		{
			void axial(void);
			void shear(void);
			void torsion(void);
			void bending(void);
			void cantilever(void);
			void two_beams_axial(void);
			void two_beams_torsion(void);
		}
		namespace rigid
		{
			void motion(void);
			void spring(void);
		}
	}
}
