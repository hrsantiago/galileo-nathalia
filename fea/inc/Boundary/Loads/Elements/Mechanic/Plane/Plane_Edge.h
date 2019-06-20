#pragma once

#include "Boundary/Loads/Elements/Mechanic/Plane/Plane.h"

namespace fea
{
	namespace boundary
	{
		namespace loads
		{
			class Plane_Edge : public Plane
			{
			public:
				//friends
				friend class Element;

			protected:
				//constructors
				Plane_Edge(void);

				//destructor
				virtual ~Plane_Edge(void) override;

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
