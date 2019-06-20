#pragma once

#include "Boundary/Loads/Elements/Mechanic/Mechanic.h"

namespace fea
{
	namespace boundary
	{
		namespace loads
		{
			class Solid : public Mechanic
			{
			public:
				//friends
				friend class Element;

			protected:
				//constructors
				Solid(void);

				//destructor
				virtual ~Solid(void) override;

			public:
				//data
				const double* direction(void) const;
				const double* direction(double, double);
				
			protected:
				//attributes
				double m_direction[2];
			};
		}
	}
}
