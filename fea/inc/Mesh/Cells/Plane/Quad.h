#pragma once

#include "Mesh/Cells/Plane/Plane.h"

namespace fea
{
	namespace mesh
	{
		namespace cells
		{
			class Quad : public Plane
			{
			protected:
				//constructors
				Quad(void);

				//destructor
				virtual ~Quad(void) override;

			public:
				//topology
				virtual unsigned edges(void) const override;
				
				//parametrization
				virtual void edge(double*, unsigned, double) const override;
				virtual void gradient(double*, unsigned, double) const override;
			};
		}
	}
}
