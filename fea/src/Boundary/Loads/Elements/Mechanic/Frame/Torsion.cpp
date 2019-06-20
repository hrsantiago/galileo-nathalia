#include "Boundary/Loads/Types.h"
#include "Boundary/Loads/Elements/Mechanic/Frame/Torsion.h"

namespace fea
{
	namespace boundary
	{
		namespace loads
		{
			//constructors
			Torsion::Torsion(void)
			{
				return;
			}

			//destructor
			Torsion::~Torsion(void)
			{
				return;
			}

			//types
			loads::type Torsion::type(void) const
			{
				return loads::type::torsion;
			}
		}
	}
}
