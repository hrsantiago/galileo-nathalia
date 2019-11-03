#pragma once

#include "Mesh/Cells/Volume/Volume.h"

namespace fea
{
	namespace mesh
	{
		namespace cells
		{
			class Tetra : public Volume
			{
			protected:
				//constructors
				Tetra(void);

				//destructor
				virtual ~Tetra(void) override;

			public:
				//topology
				virtual unsigned faces(void) const override;
				virtual unsigned edges(void) const override;
				
				//parametrization
				virtual void edge(double*, unsigned, double) const override;
				virtual void face(double*, unsigned, double, double) const override;
				
				virtual void gradient(double*, unsigned, double) const override;
				virtual void gradient(double*, unsigned, double, double) const override;
			};
		}
	}
}
