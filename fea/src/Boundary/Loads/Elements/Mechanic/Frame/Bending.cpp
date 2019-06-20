#include "Boundary/Loads/Types.h"
#include "Boundary/Loads/Elements/Mechanic/Frame/Bending.h"

namespace fea
{
	namespace boundary
	{
		namespace loads
		{
			//constructors
			Bending::Bending(void)
			{
				return;
			}

			//destructor
			Bending::~Bending(void)
			{
				return;
			}

			//types
			loads::type Bending::type(void) const
			{
				return loads::type::bending;
			}
		}
	}
}
