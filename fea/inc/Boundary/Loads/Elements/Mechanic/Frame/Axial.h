#pragma once

#include "Boundary/Loads/Elements/Mechanic/Mechanic.h"

namespace fea
{
	namespace boundary
	{
		namespace loads
		{
			class Axial : public Mechanic
			{
			public:
				//friends
				friend class Element;

			protected:
				//constructors
				Axial(void);

				//destructor
				virtual ~Axial(void) override;

			public:
				//type
				virtual loads::type type(void) const override;
			};
		}
	}
}
