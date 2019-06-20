#include "Boundary/Loads/Types.h"
#include "Boundary/Loads/Elements/Mechanic/Solid/Solid_Face.h"

namespace fea
{
	namespace boundary
	{
		namespace loads
		{
			//constructors
			Solid_Face::Solid_Face(void)
			{
				return;
			}

			//destructor
			Solid_Face::~Solid_Face(void)
			{
				return;
			}

			//types
			loads::type Solid_Face::type(void) const
			{
				return loads::type::solid_face;
			}
			
			//data
			unsigned Solid_Face::face(void) const
			{
				return m_face;
			}
			unsigned Solid_Face::face(unsigned face)
			{
				return m_face = face;
			}
		}
	}
}
