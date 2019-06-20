#include "Mesh/Points/Types.h"
#include "Mesh/Points/Heat/Heat.h"

namespace fea
{
	namespace mesh
	{
		namespace points
		{
			//constructors
			Heat::Heat(void)
			{
				return;
			}

			//destructor
			Heat::~Heat(void)
			{
				return;
			}

			//type
			points::type Heat::type(void) const
			{
				return points::type::heat;
			}

			//analysis
			void Heat::prepare(const elements::Element*)
			{
				return;
			}
			void Heat::update(void)
			{
				return;
			}
			void Heat::restore(void)
			{
				return;
			}
		}
	}
}
