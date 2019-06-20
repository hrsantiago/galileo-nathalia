//std
#include <cstring>

//mat
#include "misc/util.h"
#include "misc/stress.h"
#include "linear/dense.h"
#include "linear/linear.h"

//fea
#include "Mesh/Points/Mechanic/States.h"
#include "Mesh/Points/Mechanic/Mechanic.h"

#include "Mesh/Materials/Mechanic/Associative/Associative.h"

namespace fea
{
	namespace mesh
	{
		namespace materials
		{
			//constructors
			Associative::Associative(void)
			{
				return;
			}

			//destructor
			Associative::~Associative(void)
			{
				return;
			}

			//return mapping
			double* Associative::return_mapping(double* s, double* C, const double* e, const double* sr, points::Mechanic& mp) const
			{
				//data
				double fp, kp, dg; 
				double r[6], q[6], ds[6], ee[6], se[6], dfs[6], D[36], dfss[36];
				//componentes
				const unsigned t = mp.m_stress_types;
				const unsigned n = mat::bit_find(t, (unsigned) mat::stress::last);
				//elastic properties
				mat::inv(C, elastic_flexibility(D, t), n);
				//reference_stress
				const double s0 = reference_stress();
				//old configuration
				const double a_old = mp.m_hardening_old;
				const double* ep_old = mp.m_plastic_strain_old;
				const points::state st_old = mp.m_state_old;
				//new configuration
				double& a = mp.m_hardening_new;
				double* ep = mp.m_plastic_strain_new;
				points::state& st = mp.m_state_new;
				//check broken
				if(st_old == points::state::rupture || break_criterion(e, t))
				{
					a = a_old;
					mat::clean(C, n * n);
					mat::set(ep, ep_old, n);
					st = points::state::rupture;
					return mat::clean(s, n);
				}
				//predictor
				a = a_old;
				mat::set(ep, ep_old, n);
				mat::sub(ee, e, ep, n);
				mat::multiply(se, C, ee, n, n);
				mat::add(s, se, sr, n);
				fp = yield_stress(s, t) - plastic_gradient(a);
				//corrector
				if(fp < -1e-5 * s0)
				{
					st = points::state::elastic;
				}
				else
				{
					st = points::state::plastic;
					while(true)
					{
						//set state
						dg = a - a_old;
						kp = plastic_hessian(a);
						yield_gradient(dfs, s, t);
						yield_hessian(dfss, s, t);
						fp = yield_stress(s, t) - plastic_gradient(a);
						mat::multisub(mat::sub(r, ee, dfs, n, dg), D, mat::sub(se, s, sr, n), n, n);
						mat::inv(mat::add(mat::add(mat::outer(C, dfs, n, 1 / kp), D, n * n), dfss, n * n, dg), n);
						//check solution
						if(mat::norm(r, n) < 1e-5 && fp / s0 < 1e-5)
						{
							mat::add(ep, ep_old, dfs, n, dg);
							break;
						}
						//update solution
						mat::sub(q, r, dfs, n, fp / kp);
						mat::add(s, mat::multiply(ds, C, q, n, n), n);
						a += (fp + mat::dot(dfs, ds, n)) / kp;
					}
				}
				//return
				return s;
			}
		}
	}
}
