#include "Boundary/Loads/Types.h"
#include "Boundary/Loads/Elements/Mechanic/Frame/Shear.h"

namespace fea
{
	namespace boundary
	{
		namespace loads
		{
			//constructors
			Shear::Shear(void) : m_axis(2)
			{
				return;
			}

			//destructor
			Shear::~Shear(void)
			{
				return;
			}

			//types
			loads::type Shear::type(void) const
			{
				return loads::type::shear;
			}
			
			//data
			unsigned Shear::axis(void) const
			{
				return m_axis;
			}
			unsigned Shear::axis(unsigned axis)
			{
				return m_axis = axis;
			}
		}
	}
}
