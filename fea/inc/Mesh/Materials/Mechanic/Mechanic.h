#pragma once

#include "Mesh/Materials/Material.h"

namespace fea
{
	namespace mesh
	{
		//declarations
		namespace points
		{
			class Mechanic;
		}
		
		//class
		namespace materials
		{
			class Mechanic : public Material
			{
			public:
				//constructors
				Mechanic(void);

				//destructor
				virtual ~Mechanic(void);

				//properties
				virtual double bulk_modulus(void) const;
				virtual double shear_modulus(void) const;
				virtual double poisson_ratio(void) const = 0;
				virtual double elastic_modulus(void) const = 0;
				virtual double reference_stress(void) const = 0;
				virtual double thermal_coefficient(void) const = 0;

				virtual double* elastic_stiffness(double*, unsigned) const;
				virtual double* elastic_flexibility(double*, unsigned) const;

				//break criterion
				virtual bool break_criterion(const double*, unsigned) const = 0;

				//stress
				virtual double* stress(double*, const double*, const double*, const points::Mechanic&) const;

				//return mapping
				virtual double* return_mapping(double*, double*, const double*, const double*, points::Mechanic&) const = 0;
			};
		}
	}
}
