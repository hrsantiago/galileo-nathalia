#pragma once

#include "Mesh/Cells/Cell.h"

namespace fea
{
	namespace mesh
	{
		namespace cells
		{
			class Volume : public Cell
			{
			protected:
				//constructors
				Volume(void);

				//destructor
				virtual ~Volume(void) override;

				//serialization
				virtual void load(FILE*) override;
				virtual void save(FILE*) const override;

			public:
				//topology
				virtual unsigned dimension(void) const override;
				
			protected:
				//results
				virtual void plot(const elements::Element*, const double*, const double** = nullptr, const double* = nullptr, bool = false) const override;
			};
		}
	}
}
