#pragma once

#include "Mesh/Materials/Mechanic/Associative/Associative.h"

namespace fea
{
	namespace mesh
	{
		namespace materials
		{
			class Concrete : public Associative
			{
			public:
				//friends
				friend class Material;

			protected:
				//constructors
				Concrete(void);

				//destructor
				virtual ~Concrete(void) override;

				//serialization
				virtual void load(FILE*) override;
				virtual void save(FILE*) const override;

			public:
				//type
				virtual materials::type type(void) const override;
				
				//data
				virtual double softening(void) const;
				virtual double softening(double);
				virtual double break_strain(void) const;
				virtual double break_strain(double);
				virtual double poisson_ratio(void) const override;
				virtual double elastic_modulus(void) const override;
				virtual double elastic_modulus(double);
				virtual double yield_stress_tension(void) const;
				virtual double yield_stress_tension(double);
				virtual double yield_stress_biaxial(void) const;
				virtual double yield_stress_biaxial(double);
				virtual double yield_stress_compression(void) const;
				virtual double yield_stress_compression(double);
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
				double m_softening;
				double m_break_strain;
				double m_elastic_modulus;
				double m_yield_stress_tension;
				double m_yield_stress_biaxial;
				double m_yield_stress_compression;
			};
		}
	}
}
