#pragma once

#include "Boundary/Loads/Elements/Mechanic/Mechanic.h"

namespace fea
{
	namespace boundary
	{
		namespace loads
		{
			class Torsion : public Mechanic
			{
			public:
				//friends
				friend class Element;

			protected:
				//constructors
				Torsion(void);

				//destructor
				virtual ~Torsion(void) override;

			public:
				//type
				virtual loads::type type(void) const override;
			};
		}
	}
}
