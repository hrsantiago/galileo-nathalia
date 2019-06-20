//fea
#include "Mesh/Points/Types.h"
#include "Mesh/Elements/Mechanic/Frame/Frame.h"

namespace fea
{
	namespace mesh
	{
		namespace elements
		{
			//constructors
			Frame::Frame(void)
			{
				return;
			}

			//destructor
			Frame::~Frame(void)
			{
				return;
			}

			//types
			points::type Frame::point(void) const
			{
				return points::type::section;
			}
		}
	}
}
