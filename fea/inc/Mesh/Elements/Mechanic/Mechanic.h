#pragma once

#include "Mesh/Elements/Element.h"

namespace fea
{
	namespace mesh
	{
		namespace elements
		{
			class Mechanic : public Element
			{
			protected:
				//constructors
				Mechanic(void);

				//destructor
				virtual ~Mechanic(void) override;

			public:
				//data
				static bool geometric(void);
				static bool geometric(bool);
				static bool inelastic(void);
				static bool inelastic(bool);

				//type
				virtual unsigned stresses(void) const = 0;
				virtual points::type point(void) const override;

			protected:
				//attributes
				static bool m_geometric;
				static bool m_inelastic;
			};
		}
	}
}
