#pragma once

#include "Mesh/Cells/Volume/Tetra.h"

namespace fea
{
	namespace mesh
	{
		namespace cells
		{
			class Tetra10 : public Tetra
			{
			public:
				//friends
				friend class Cell;

			protected:
				//constructors
				Tetra10(void);

				//destructor
				virtual ~Tetra10(void) override;

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
