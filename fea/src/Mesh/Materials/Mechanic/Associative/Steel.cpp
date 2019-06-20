//mat
#include "misc/util.h"
#include "misc/stress.h"
#include "linear/dense.h"

//fea
#include "Mesh/Materials/Types.h"
#include "Mesh/Materials/Mechanic/Associative/Steel.h"

namespace fea
{
	namespace mesh
	{
		namespace materials
		{
			//constructors
			Steel::Steel(void) : m_yield_stress(400e6), m_break_strain(0.2), m_break_stress(500e6), m_poisson_ratio(0.3), m_elastic_modulus(200e9)
			{
				m_specific_mass = 7.85e3;
			}

			//destructor
			Steel::~Steel(void)
			{
				return;
			}

			//serialization
			void Steel::load(FILE* file)
			{
				Material::load(file);
				fscanf(file, "%lf %lf %lf %lf %lf",
							&m_elastic_modulus,
							&m_poisson_ratio,
							&m_yield_stress,
							&m_break_stress,
							&m_break_strain
							);
			}
			void Steel::save(FILE* file) const
			{
				Material::save(file);
				fprintf(file, "%+.6e %+.6e %+.6e %+.6e %+.6e ",
							 m_elastic_modulus,
							 m_poisson_ratio,
							 m_yield_stress,
							 m_break_stress,
							 m_break_strain
							 );
			}

			//type
			materials::type Steel::type(void) const
			{
				return materials::type::steel;
			}

			//data
			double Steel::yield_stress(void) const
			{
				return m_yield_stress;
			}
			double Steel::yield_stress(double sy)
			{
				m_yield_stress = sy;
			}
			double Steel::break_strain(void) const
			{
				return m_break_strain;
			}
			double Steel::break_strain(double eu)
			{
				m_break_strain = eu;
			}
			double Steel::break_stress(void) const
			{
				return m_break_stress;
			}
			double Steel::break_stress(double su)
			{
				m_break_stress = su;
			}
			double Steel::poisson_ratio(void) const
			{
				return m_poisson_ratio;
			}
			double Steel::poisson_ratio(double n)
			{
				m_poisson_ratio = n;
			}
			double Steel::elastic_modulus(void) const
			{
				return m_elastic_modulus;
			}
			double Steel::elastic_modulus(double E)
			{
				m_elastic_modulus = E;
			}
			double Steel::plastic_modulus(void) const
			{
				const double sy = m_yield_stress;
				const double su = m_break_stress;
				const double eu = m_break_strain;
				const double E = m_elastic_modulus;
				return  E * (su - sy) / (E * eu - su);
			}
			double Steel::plastic_modulus(double K)
			{
				const double sy = m_yield_stress;
				const double eu = m_break_strain;
				const double E = m_elastic_modulus;
				m_break_stress = sy + K / (E + K) * (E * eu - sy);
			}
			double Steel::reference_stress(void) const
			{
				return m_yield_stress;
			}
			double Steel::thermal_coefficient(void) const
			{
				return 1.2e-5;
			}

			//break criterion
			bool Steel::break_criterion(const double* e, unsigned t) const
			{
				return mat::norm(e, mat::bit_find(t, (unsigned) mat::stress::last)) > m_break_strain;
			}

			//yield criterion
			double Steel::yield_stress(const double* s, unsigned t) const
			{
				return mat::von_mises_stress(s, t);
			}
			double* Steel::yield_gradient(double* dfs, const double* s, unsigned t) const
			{
				return mat::von_mises_gradient(dfs, s, t);
			}
			double* Steel::yield_hessian(double* dfss, const double* s, unsigned t) const
			{
				return mat::von_mises_hessian(dfss, s, t);
			}

			//plastic potential
			double Steel::plastic_potential(double a) const
			{
				const double sy = m_yield_stress;
				const double K = plastic_modulus();
				return sy * a + K * a * a / 2;
			}
			double Steel::plastic_gradient(double a) const
			{
				const double sy = m_yield_stress;
				const double K = plastic_modulus();
				return sy + K * a;
			}
			double Steel::plastic_hessian(double) const
			{
				return plastic_modulus();
			}
		}
	}
}
