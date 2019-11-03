//mat
#include "misc/util.h"
#include "misc/stress.h"
#include "linear/dense.h"
#include "linear/linear.h"

//fea
#include "Mesh/Points/Mechanic/States.h"
#include "Mesh/Points/Mechanic/Mechanic.h"

#include "Mesh/Elements/Mechanic/Mechanic.h"

#include "Mesh/Materials/Mechanic/Mechanic.h"

namespace fea
{
	namespace mesh
	{
		namespace materials
		{
			//constructors
			Mechanic::Mechanic(void)
			{
				return;
			}

			//destructor
			Mechanic::~Mechanic(void)
			{
				return;
			}

			//properties
			double Mechanic::bulk_modulus(void) const
			{
				const double n = poisson_ratio();
				const double E = elastic_modulus();
				return E / (1 - 2 * n) / 3;
			}
			double Mechanic::shear_modulus(void) const
			{
				const double n = poisson_ratio();
				const double E = elastic_modulus();
				return E / (1 + n) / 2;
			}
			
			double* Mechanic::elastic_stiffness(double* C, unsigned t) const
			{
				//properties
				const double n = poisson_ratio();
				const double E = elastic_modulus();
				//stiffness
				if(mat::bit_set(t, (unsigned) mat::stress::last))
				{
					C[0 + 6 * 0] = 1 - n;
					C[1 + 6 * 1] = 1 - n;
					C[2 + 6 * 2] = 1 - n;
					C[3 + 6 * 3] = (1 - 2 * n) / 2;
					C[4 + 6 * 4] = (1 - 2 * n) / 2;
					C[5 + 6 * 5] = (1 - 2 * n) / 2;
					C[3 + 6 * 0] = C[0 + 6 * 3] = 0;
					C[4 + 6 * 0] = C[0 + 6 * 4] = 0;
					C[5 + 6 * 0] = C[0 + 6 * 5] = 0;
					C[3 + 6 * 1] = C[1 + 6 * 3] = 0;
					C[4 + 6 * 1] = C[1 + 6 * 4] = 0;
					C[5 + 6 * 1] = C[1 + 6 * 5] = 0;
					C[3 + 6 * 2] = C[2 + 6 * 3] = 0;
					C[4 + 6 * 2] = C[2 + 6 * 4] = 0;
					C[5 + 6 * 2] = C[2 + 6 * 5] = 0;
					C[4 + 6 * 3] = C[3 + 6 * 4] = 0;
					C[5 + 6 * 3] = C[3 + 6 * 5] = 0;
					C[5 + 6 * 4] = C[4 + 6 * 5] = 0;
					C[1 + 6 * 0] = C[0 + 6 * 1] = n;
					C[2 + 6 * 0] = C[0 + 6 * 2] = n;
					C[2 + 6 * 1] = C[1 + 6 * 2] = n;
					return mat::mul(C, E / (1 + n) / (1 - 2 * n), 36);
				}
				else
				{
					return mat::inverse(elastic_flexibility(C, t), mat::bit_count(t));
				}
			}
			double* Mechanic::elastic_flexibility(double* D, unsigned t) const
			{
				//properties
				const double n = poisson_ratio();
				const double E = elastic_modulus();
				//flexibility
				double Dc[36];
				Dc[0 + 6 * 0] = 1;
				Dc[1 + 6 * 1] = 1;
				Dc[2 + 6 * 2] = 1;
				Dc[3 + 6 * 3] = 2 * (1 + n);
				Dc[4 + 6 * 4] = 2 * (1 + n);
				Dc[5 + 6 * 5] = 2 * (1 + n);
				Dc[3 + 6 * 0] = Dc[0 + 6 * 3] = 0;
				Dc[4 + 6 * 0] = Dc[0 + 6 * 4] = 0;
				Dc[5 + 6 * 0] = Dc[0 + 6 * 5] = 0;
				Dc[3 + 6 * 1] = Dc[1 + 6 * 3] = 0;
				Dc[4 + 6 * 1] = Dc[1 + 6 * 4] = 0;
				Dc[5 + 6 * 1] = Dc[1 + 6 * 5] = 0;
				Dc[3 + 6 * 2] = Dc[2 + 6 * 3] = 0;
				Dc[4 + 6 * 2] = Dc[2 + 6 * 4] = 0;
				Dc[5 + 6 * 2] = Dc[2 + 6 * 5] = 0;
				Dc[4 + 6 * 3] = Dc[3 + 6 * 4] = 0;
				Dc[5 + 6 * 3] = Dc[3 + 6 * 5] = 0;
				Dc[5 + 6 * 4] = Dc[4 + 6 * 5] = 0;
				Dc[1 + 6 * 0] = Dc[0 + 6 * 1] = -n;
				Dc[2 + 6 * 0] = Dc[0 + 6 * 2] = -n;
				Dc[2 + 6 * 1] = Dc[1 + 6 * 2] = -n;
				//return
				return mat::stress_matrix(D, Dc, t, E);
			}
			
			//stress
			double* Mechanic::stress(double* s, const double* e, const double* sr, const points::Mechanic& mp) const
			{
				//components
				const unsigned t = mp.m_stress_types;
				const unsigned n = mat::bit_count(t);
				//stress
				double ee[6], se[6], C[36];
				const double* ep = mp.m_plastic_strain_new;
				if(!elements::Mechanic::inelastic())
				{
					return mat::add(s, mat::multiply(se, elastic_stiffness(C, t), e, n, n), sr, n);
				}
				else
				{
					return mat::add(s, mat::multiply(se, elastic_stiffness(C, t), mat::sub(ee, e, ep, n), n, n), sr, n);
				}
			}
		}
	}
}
