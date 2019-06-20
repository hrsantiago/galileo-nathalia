#pragma once

#include "Mesh/Cells/Plane/Tri.h"

namespace fea
{
	namespace mesh
	{
		namespace cells
		{
			class Tri3 : public Tri
			{
			public:
				//friends
				friend class Cell;

			protected:
				//constructors
				Tri3(void);

				//destructor
				virtual ~Tri3(void) override;

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
