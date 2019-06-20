#pragma once

#include "Mesh/Cells/Plane/Quad.h"

namespace fea
{
	namespace mesh
	{
		namespace cells
		{
			class Quad9 : public Quad
			{
			public:
				//friends
				friend class Cell;

			protected:
				//constructors
				Quad9(void);

				//destructor
				virtual ~Quad9(void) override;

			public:
				//type
				virtual cells::type type(void) const override;

				//topology
				virtual unsigned vertices(void) const override;
				
				virtual std::vector<unsigned> edge(unsigned) const override;
				virtual std::vector<unsigned> face(unsigned) const override;

				//interpolation
				virtual double* function(double*, const double*) const override;
				virtual double* gradient(double*, const double*) const override;
			};
		}
	}
}
