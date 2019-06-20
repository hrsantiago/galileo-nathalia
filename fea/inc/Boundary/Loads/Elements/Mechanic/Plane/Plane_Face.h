#pragma once

#include "Boundary/Loads/Elements/Mechanic/Plane/Plane.h"

namespace fea
{
	namespace boundary
	{
		namespace loads
		{
			class Plane_Face : public Plane
			{
			public:
				//friends
				friend class Element;

			protected:
				//constructors
				Plane_Face(void);

				//destructor
				virtual ~Plane_Face(void) override;

			public:
				//type
				virtual loads::type type(void) const override;
			};
		}
	}
}
