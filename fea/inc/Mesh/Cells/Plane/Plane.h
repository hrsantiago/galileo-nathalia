#pragma once

#include "Mesh/Cells/Cell.h"

namespace fea
{
	namespace mesh
	{
		namespace cells
		{
			class Plane : public Cell
			{
			protected:
				//constructors
				Plane(void);

				//destructor
				virtual ~Plane(void) override;

				//serialization
				virtual void load(FILE*) override;
				virtual void save(FILE*) const override;

			public:
				//data
				double thickness(void) const;
				double thickness(double);

				//topology
				virtual unsigned faces(void) const override;
				virtual unsigned dimension(void) const override;
				
				//parametrization
				virtual void face(double*, unsigned, double, double) const override;
				virtual void gradient(double*, unsigned, double, double) const override;

			protected:
				//results
				virtual void plot(const elements::Element*, const double*, const double** = nullptr, const double* = nullptr, bool = false) const override;
				
				//attributes
				double m_thickness;
			};
		}
	}
}
