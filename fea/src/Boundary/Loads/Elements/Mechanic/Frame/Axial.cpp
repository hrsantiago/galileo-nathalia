#include "Boundary/Loads/Types.h"
#include "Boundary/Loads/Elements/Mechanic/Frame/Axial.h"

namespace fea
{
	namespace boundary
	{
		namespace loads
		{
			//constructors
			Axial::Axial(void)
			{
				return;
			}

			//destructor
			Axial::~Axial(void)
			{
				return;
			}

			//types
			loads::type Axial::type(void) const
			{
				return loads::type::axial;
			}
		}
	}
}
