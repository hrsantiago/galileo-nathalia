#pragma once

#include "Boundary/Loads/Elements/Mechanic/Solid/Solid.h"

namespace fea
{
	namespace boundary
	{
		namespace loads
		{
			class Solid_Edge : public Solid
			{
			public:
				//friends
				friend class Element;

			protected:
				//constructors
				Solid_Edge(void);

				//destructor
				virtual ~Solid_Edge(void) override;

			public:
				//type
				virtual loads::type type(void) const override;
				
				//data
				unsigned edge(void) const;
				unsigned edge(unsigned);
				
			protected:
				//attributes
				unsigned m_edge;
			};
		}
	}
}
