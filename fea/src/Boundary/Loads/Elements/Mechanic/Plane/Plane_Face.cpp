#include "Boundary/Loads/Types.h"
#include "Boundary/Loads/Elements/Mechanic/Plane/Plane_Face.h"

namespace fea
{
	namespace boundary
	{
		namespace loads
		{
			//constructors
			Plane_Face::Plane_Face(void)
			{
				return;
			}

			//destructor
			Plane_Face::~Plane_Face(void)
			{
				return;
			}

			//types
			loads::type Plane_Face::type(void) const
			{
				return loads::type::plane_face;
			}
		}
	}
}
