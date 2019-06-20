#pragma once

#include "Mesh/Elements/Mechanic/Mechanic.h"

namespace fea
{
	namespace mesh
	{
		namespace elements
		{
			class Frame : public Mechanic
			{
			public:

				//constructors
				Frame(void);

				//destructor
				virtual ~Frame(void) override;

				//types
				virtual points::type point(void) const override;
			};
		}
	}
}
