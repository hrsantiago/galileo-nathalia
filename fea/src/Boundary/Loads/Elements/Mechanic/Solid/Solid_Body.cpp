#include "Boundary/Loads/Types.h"
#include "Boundary/Loads/Elements/Mechanic/Solid/Solid_Body.h"

namespace fea
{
	namespace boundary
	{
		namespace loads
		{
			//constructors
			Solid_Body::Solid_Body(void)
			{
				return;
			}

			//destructor
			Solid_Body::~Solid_Body(void)
			{
				return;
			}

			//types
			loads::type Solid_Body::type(void) const
			{
				return loads::type::solid_body;
			}
		}
	}
}
