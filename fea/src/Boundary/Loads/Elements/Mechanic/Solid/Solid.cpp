#include "Boundary/Loads/Elements/Mechanic/Solid/Solid.h"

namespace fea
{
	namespace boundary
	{
		namespace loads
		{
			//constructors
			Solid::Solid(void)
			{
				m_direction[0] = 0;
				m_direction[1] = 0;
			}

			//destructor
			Solid::~Solid(void)
			{
				return;
			}

			//data
			const double* Solid::direction(void) const
			{
				return m_direction;
			}
			const double* Solid::direction(double t0, double t1)
			{
				m_direction[0] = t0;
				m_direction[1] = t1;
				return m_direction;
			}
		}
	}
}
