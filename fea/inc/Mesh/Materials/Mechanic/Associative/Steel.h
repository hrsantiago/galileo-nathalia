#pragma once

#include "Mesh/Materials/Mechanic/Associative/Associative.h"

namespace fea
{
	namespace mesh
	{
		namespace materials
		{
			class Steel : public Associative
			{
			public:
				//friends
				friend class Material;

			protected:
				//constructors
				Steel(void);

				//destructor
				virtual ~Steel(void) override;

				//serialization
				virtual void load(FILE*) override;
				virtual void save(FILE*) const override;

			public:
				//type
				virtual materials::type type(void) const;
				
				//data
				virtual double yield_stress(void) const;
				virtual double yield_stress(double);
				virtual double break_strain(void) const;
				virtual double break_strain(double);
				virtual double break_stress(void) const;
				virtual double break_stress(double);
				virtual double poisson_ratio(void) const override;
				virtual double poisson_ratio(double);
				virtual double elastic_modulus(void) const override;
				virtual double elastic_modulus(double);
				virtual double plastic_modulus(void) const;
				virtual double plastic_modulus(double);
				virtual double reference_stress(void) const override;
				virtual double thermal_coefficient(void) const override;

			protected:
				//break criterion
				virtual bool break_criterion(const double*, unsigned) const override;

				//yield criterion
				virtual double yield_stress(const double*, unsigned) const override;
				virtual double* yield_gradient(double*, const double*, unsigned) const override;
				virtual double* yield_hessian(double*, const double*, unsigned) const override;

				//plastic potential
				virtual double plastic_potential(double) const override;
				virtual double plastic_gradient(double) const override;
				virtual double plastic_hessian(double) const override;

				//attributes
				double m_yield_stress;
				double m_break_strain;
				double m_break_stress;
				double m_poisson_ratio;
				double m_elastic_modulus;
			};
		}
	}
}
