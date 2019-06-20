//std
#include <cmath>

//mat
#include "misc/util.h"
#include "misc/stress.h"
#include "linear/dense.h"

//fea
#include "Mesh/Materials/Types.h"
#include "Mesh/Materials/Mechanic/Associative/Concrete.h"

namespace fea
{
	namespace mesh
	{
		namespace materials
		{
			//constructors
			Concrete::Concrete(void) :
			m_softening(50), m_break_strain(9e-3), m_elastic_modulus(26.1e9), 
			m_yield_stress_compression(30e6), m_yield_stress_biaxial(50e6), m_yield_stress_tension(3e6)
			{
				m_specific_mass = 2.5e3;
			}

			//destructor
			Concrete::~Concrete(void)
			{
				return;
			}

			//serialization
			void Concrete::load(FILE* file)
			{
				Material::load(file);
				fscanf(file, "%lf %lf %lf %lf %lf %lf",
					&m_softening,
					&m_break_strain,
					&m_elastic_modulus,
					&m_yield_stress_tension,
					&m_yield_stress_biaxial,
					&m_yield_stress_compression);
			}
			void Concrete::save(FILE* file) const
			{
				Material::save(file);
				fprintf(file, "%+.6e %+.6e %+.6e %+.6e %+.6e %+.6e %+.6e ",
					m_softening,
					m_break_strain,
					m_elastic_modulus,
					m_yield_stress_tension,
					m_yield_stress_biaxial,
					m_yield_stress_compression);
			}

			//type
			materials::type Concrete::type(void) const
			{
				return materials::type::concrete;
			}

			//data
			double Concrete::softening(void) const
			{
				return m_softening;
			}
			double Concrete::softening(double u)
			{
				return m_softening = u;
			}
			double Concrete::break_strain(void) const
			{
				return m_break_strain;
			}
			double Concrete::break_strain(double eu)
			{
				return m_break_strain = eu;
			}
			double Concrete::poisson_ratio(void) const
			{
				return 0.2;
			}
			double Concrete::elastic_modulus(void) const
			{
				return m_elastic_modulus;
			}
			double Concrete::elastic_modulus(double E)
			{
				return m_elastic_modulus = E;
			}
			double Concrete::yield_stress_tension(void) const
			{
				return m_yield_stress_tension;
			}
			double Concrete::yield_stress_tension(double st)
			{
				return m_yield_stress_tension = st;
			}
			double Concrete::yield_stress_biaxial(void) const
			{
				return m_yield_stress_biaxial;
			}
			double Concrete::yield_stress_biaxial(double sb)
			{
				return m_yield_stress_biaxial = sb;
			}
			double Concrete::yield_stress_compression(void) const
			{
				return m_yield_stress_compression;
			}
			double Concrete::yield_stress_compression(double sc)
			{
				return m_yield_stress_compression = sc;
			}
			double Concrete::reference_stress(void) const
			{
				return m_yield_stress_compression;
			}
			double Concrete::thermal_coefficient(void) const
			{
				return 1e-5;
			}
			
			//break criterion
			bool Concrete::break_criterion(const double* e, unsigned t) const
			{
				return mat::norm(e, mat::bit_find(t, (unsigned) mat::stress::last)) > m_break_strain;
			}

			//yield criterion
			double Concrete::yield_stress(const double* s, unsigned t) const
			{
				//properties
				const double st = m_yield_stress_tension;
				const double sb = m_yield_stress_biaxial;
				const double sc = m_yield_stress_compression;
				//parameters
				const double C = (3 * sb * st - sb * sc - 2 * sc * st) / (sc + st) / (2 * sb - sc) / (2 * sb + st);
				const double B = (st - sc) * (4 * sb * sb + sc * st - sb * (sc + st)) / (sc + st) / (2 * sb - sc) / (2 * sb + st);
				//von mises stress
				const double se = mat::von_mises_stress(s, t);
				//first invariant
				const double I1 = mat::stress_invariant_I1(s, t);
				//yield stress
				return se - B * I1 - C * I1 * I1;
			}
			double* Concrete::yield_gradient(double* dfs, const double* s, unsigned t) const
			{
				//properties
				const double st = m_yield_stress_tension;
				const double sb = m_yield_stress_biaxial;
				const double sc = m_yield_stress_compression;
				//componentes
				const unsigned n = mat::bit_find(t, (unsigned) mat::stress::last);
				//parameters
				const double C = (3 * sb * st - sb * sc - 2 * sc * st) / (sc + st) / (2 * sb - sc) / (2 * sb + st);
				const double B = (st - sc) * (4 * sb * sb + sc * st - sb * (sc + st)) / (sc + st) / (2 * sb - sc) / (2 * sb + st);
				//first invariant
				const double I1 = mat::stress_invariant_I1(s, t);
				//yield gradient
				double dI1[6];
				return mat::sub(mat::von_mises_gradient(dfs, s, t), mat::mul(mat::stress_gradient_I1(dI1, s, t), B + 2 * C * I1, n), n);
			}
			double* Concrete::yield_hessian(double* dfss, const double* s, unsigned t) const
			{
				//properties
				const double st = m_yield_stress_tension;
				const double sb = m_yield_stress_biaxial;
				const double sc = m_yield_stress_compression;
				//componentes
				const unsigned n = mat::bit_find(t, (unsigned) mat::stress::last);
				//parameters
				const double C = (3 * sb * st - sb * sc - 2 * sc * st) / (sc + st) / (2 * sb - sc) / (2 * sb + st);
				//yield hessian
				double dI1[6], dP1[36];
				return mat::sub(mat::von_mises_hessian(dfss, s, t), mat::mul(mat::outer(dP1, mat::stress_gradient_I1(dI1, s, t), n), 2 * C, n * n), n * n);
			}

			//plastic potential
			double Concrete::plastic_potential(double a) const
			{
				//properties
				const double st = m_yield_stress_tension;
				const double sb = m_yield_stress_biaxial;
				const double sc = m_yield_stress_compression;
				//parameters
				const double u = m_softening;
				const double A = sc * sb * st * (st + 8 * sb - 3 * sc) / (sc + st) / (2 * sb - sc) / (2 * sb + st);
				//plastic potential
				return -A / u * exp(-u * a);
			}
			double Concrete::plastic_gradient(double a) const
			{
				//properties
				const double st = m_yield_stress_tension;
				const double sb = m_yield_stress_biaxial;
				const double sc = m_yield_stress_compression;
				//parameters
				const double u = m_softening;
				const double A = sc * sb * st * (st + 8 * sb - 3 * sc) / (sc + st) / (2 * sb - sc) / (2 * sb + st);
				//plastic gradient
				return A * exp(-u * a);
			}
			double Concrete::plastic_hessian(double a) const
			{
				//properties
				const double st = m_yield_stress_tension;
				const double sb = m_yield_stress_biaxial;
				const double sc = m_yield_stress_compression;
				//parameters
				const double u = m_softening;
				const double A = sc * sb * st * (st + 8 * sb - 3 * sc) / (sc + st) / (2 * sb - sc) / (2 * sb + st);
				//plastic hessian
				return -u * A * exp(-u * a);
			}
		}
	}
}
