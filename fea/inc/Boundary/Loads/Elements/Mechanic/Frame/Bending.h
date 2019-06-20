#pragma once

#include "Boundary/Loads/Elements/Mechanic/Mechanic.h"

namespace fea
{
	namespace boundary
	{
		namespace loads
		{
			class Bending : public Mechanic
			{
			public:
				//friends
				friend class Element;

			protected:
				//constructors
				Bending(void);

				//destructor
				virtual ~Bending(void) override;

			public:
				//type
				virtual loads::type type(void) const override;
			};
		}
	}
}
