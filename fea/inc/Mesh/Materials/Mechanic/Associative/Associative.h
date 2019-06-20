#pragma once

#include "Mesh/Materials/Mechanic/Mechanic.h"

namespace fea
{
	namespace mesh
	{
		namespace materials
		{
			class Associative : public Mechanic
			{
			protected:
				//constructors
				Associative(void);

				//destructor
				virtual ~Associative(void) override;

			public:
				//return mapping
				virtual double* return_mapping(double*, double*, const double*, const double*, points::Mechanic&) const override;

			protected:
				//yield criterion
				virtual double yield_stress(const double*, unsigned) const = 0;
				virtual double* yield_gradient(double*, const double*, unsigned) const = 0;
				virtual double* yield_hessian(double*, const double*, unsigned) const = 0;

				//plastic potential
				virtual double plastic_potential(double) const = 0;
				virtual double plastic_gradient(double) const = 0;
				virtual double plastic_hessian(double) const = 0;
			};
		}
	}
}
