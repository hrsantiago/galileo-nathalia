#pragma once

#include "Boundary/Loads/Elements/Mechanic/Mechanic.h"

namespace fea
{
	namespace boundary
	{
		namespace loads
		{
			class Plane : public Mechanic
			{
			public:
				//friends
				friend class Element;

			protected:
				//constructors
				Plane(void);

				//destructor
				virtual ~Plane(void) override;

			public:
				//data
				double direction(void) const;
				double direction(double);
				
			protected:
				//attributes
				double m_direction;
			};
		}
	}
}
