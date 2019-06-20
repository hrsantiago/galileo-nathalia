#pragma once

#include "Boundary/Loads/Elements/Mechanic/Solid/Solid.h"

namespace fea
{
	namespace boundary
	{
		namespace loads
		{
			class Solid_Face : public Solid
			{
			public:
				//friends
				friend class Element;

			protected:
				//constructors
				Solid_Face(void);

				//destructor
				virtual ~Solid_Face(void) override;

			public:
				//type
				virtual loads::type type(void) const override;
				
				//data
				unsigned face(void) const;
				unsigned face(unsigned);
				
			protected:
				//attributes
				unsigned m_face;
			};
		}
	}
}
