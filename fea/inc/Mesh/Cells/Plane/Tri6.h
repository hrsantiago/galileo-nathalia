#pragma once

#include "Mesh/Cells/Plane/Tri.h"

namespace fea
{
	namespace mesh
	{
		namespace cells
		{
			class Tri6 : public Tri
			{
			public:
				//friends
				friend class Cell;

			protected:
				//constructors
				Tri6(void);

				//destructor
				virtual ~Tri6(void) override;

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
