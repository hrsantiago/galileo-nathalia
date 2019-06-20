#pragma once

#include "Boundary/Loads/Elements/Mechanic/Solid/Solid.h"

namespace fea
{
	namespace boundary
	{
		namespace loads
		{
			class Solid_Body : public Solid
			{
			public:
				//friends
				friend class Element;

			protected:
				//constructors
				Solid_Body(void);

				//destructor
				virtual ~Solid_Body(void) override;

			public:
				//type
				virtual loads::type type(void) const override;
			};
		}
	}
}
