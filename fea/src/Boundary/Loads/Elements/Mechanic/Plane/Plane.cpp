#include "Boundary/Loads/Elements/Mechanic/Plane/Plane.h"

namespace fea
{
	namespace boundary
	{
		namespace loads
		{
			//constructors
			Plane::Plane(void) : m_direction(0)
			{
				return;
			}

			//destructor
			Plane::~Plane(void)
			{
				return;
			}

			//data
			double Plane::direction(void) const
			{
				return m_direction;
			}
			double Plane::direction(double direction)
			{
				return m_direction = direction;
			}
		}
	}
}
