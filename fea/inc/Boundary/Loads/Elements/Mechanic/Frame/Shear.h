#pragma once

#include "Boundary/Loads/Elements/Mechanic/Mechanic.h"

namespace fea
{
	namespace boundary
	{
		namespace loads
		{
			class Shear : public Mechanic
			{
			public:
				//friends
				friend class Element;

			protected:
				//constructors
				Shear(void);

				//destructor
				virtual ~Shear(void) override;

			public:
				//type
				virtual loads::type type(void) const override;
				
				//data
				unsigned axis(void) const;
				unsigned axis(unsigned);
				
			protected:
				//attributes
				unsigned m_axis;
			};
		}
	}
}
