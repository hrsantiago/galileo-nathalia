#pragma once

#include "Mesh/Cells/Line/Line.h"

namespace fea
{
	namespace mesh
	{
		namespace cells
		{
			class Bar : public Line
			{
			public:
				//friends
				friend class Cell;

			protected:
				//constructors
				Bar(void);

				//destructor
				virtual ~Bar(void) override;

			public:
				//type
				virtual cells::type type(void) const override;

				//topology
				virtual unsigned vertices(void) const override;

				virtual std::vector<unsigned> edge(unsigned) const override;
				
				//interpolation
				virtual double* function(double*, const double*) const override;
				virtual double* gradient(double*, const double*) const override;
				
				//integration
				virtual unsigned points(void) const override;
				virtual double point(double*, unsigned) const override;
			};
		}
	}
}
