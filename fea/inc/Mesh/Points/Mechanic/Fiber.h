#pragma once

#include <vector>

#include "Mesh/Points/Mechanic/Mechanic.h"

namespace fea
{
	namespace mesh
	{
		namespace points
		{
			//declarations
			class Mechanic;
			
			//class
			class Fiber
			{
			public:
				//constructors
				Fiber(void);

				//destructor
				virtual ~Fiber(void);

				//analysis
				void prepare(unsigned);
				
				void update(void);
				void restore(void);

				//attributes
				double m_area;
				double m_position_z;
				double m_position_y;
				
				Mechanic m_point;
			};
		}
	}
}
