#include <cstring>

#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Cells/Types.h"
#include "Mesh/Points/Types.h"
#include "Mesh/Elements/Types.h"
#include "Mesh/Elements/States.h"
#include "Mesh/Elements/Heat/Heat.h"
#include "Mesh/Materials/Heat/Heat.h"

#include "Boundary/Loads/Types.h"

namespace fea
{
	namespace mesh
	{
		namespace elements
		{
			//constructors
			Heat::Heat(void)
			{
				return;
			}

			//destructor
			Heat::~Heat(void)
			{
				return;
			}

			//types
			unsigned Heat::cells(void) const
			{
				return
					(unsigned) cells::type::tri3 |
					(unsigned) cells::type::tri6 |
					(unsigned) cells::type::quad4 |
					(unsigned) cells::type::quad8 |
					(unsigned) cells::type::quad9;
			}
			unsigned Heat::loads(void) const
			{
				return
					(unsigned) boundary::loads::type::heat_flux |
					(unsigned) boundary::loads::type::heat_source;
			}
			unsigned Heat::states(void) const
			{
				return (unsigned) elements::state::heat_flux;
			}
			unsigned Heat::dofs(unsigned) const
			{
				return (unsigned) nodes::dof::temperature;
			}
			points::type Heat::point(void) const
			{
				return points::type::heat;
			}
			elements::type Heat::type(void) const
			{
				return elements::type::heat;
			}

			//analysis
			void Heat::apply(void)
			{
				return;
			}

			//formulation
			double* Heat::internal_force(double* f) const
			{
//				//cell
//				const unsigned nd = cell()->dimension();
//				const unsigned nv = cell()->vertices();
//				//thickness
//				const double t = cell()->dimension() == 2 ? ((cells::Plane*) cell())->thickness() : 1;
//				//temperature
//				arma::vec d(nv);
//				state(d.memptr());
//				//material
//				const double k = ((materials::Heat*) material(0))->condutivity();
//				//internal force
//				double x[2];
//				arma::mat dN(nv, nd);
//				arma::mat J(nd, nd);
//				arma::mat B(nd, nv);
//				arma::vec Fi(f, nv, false, true);
//				memset(f, 0, nv * sizeof(double));
//				for(unsigned i = 0; i < m_points.size(); i++)
//				{
//					const double w = cell()->point(x, i);
//					const double j = cell()->jacobian(J.memptr(), this, x);
//					cell()->gradient(dN.memptr(), x);
//					B = arma::inv(J) * dN.t();
//					Fi += w * j * t * k * B.t() * B * d;
//				}
				//return
				return f;
			}
			double* Heat::inertial_force(double* f) const
			{
				return (double*) memset(f, 0, m_dof.size() * sizeof(double));
			}
			
			double* Heat::reference_force(double* f, const boundary::loads::Element*) const
			{
				return (double*) memset(f, 0, m_dof.size() * sizeof(double));
			}
			
			double* Heat::inertia(double* m) const
			{
				const unsigned nd = m_dof.size();
				return (double*) memset(m, 0, nd * nd * sizeof(double));
			}
			double* Heat::damping(double* m) const
			{
//				//cell
//				const unsigned nd = cell()->dimension();
//				const unsigned nv = cell()->vertices();
//				//thickness
//				const double t = cell()->dimension() == 2 ? ((cells::Plane*) cell())->thickness() : 1;
//				//material
//				const double r = material(0)->specific_mass();
//				const double c = ((materials::Heat*) material(0))->capacity();
//				//damping
//				double x[2];
//				arma::vec N(nv);
//				arma::mat J(nd, nd);
//				arma::mat C(m, nv, nv, false, true);
//				memset(m, 0, nv * nv * sizeof(double));
//				for(unsigned i = 0; i < m_points.size(); i++)
//				{
//					const double w = cell()->point(x, i);
//					const double j = cell()->jacobian(J.memptr(), this, x);
//					cell()->function(N.memptr(), x);
//					C += w * j * t * r * c * N.t() * N;
//				}
				//return
				return m;
			}
			double* Heat::stiffness(double* m) const
			{
//				//cell
//				const unsigned nd = cell()->dimension();
//				const unsigned nv = cell()->vertices();
//				//thickness
//				const double t = cell()->dimension() == 2 ? ((cells::Plane*) cell())->thickness() : 1;
//				//material
//				const double k = ((materials::Heat*) material(0))->condutivity();
//				//stiffness
//				double x[2];
//				arma::mat dN(nv, nd);
//				arma::mat J(nd, nd);
//				arma::mat B(nd, nv);
//				arma::mat K(m, nv, nv, false, true);
//				memset(m, 0, nv * nv * sizeof(double));
//				for(unsigned i = 0; i < m_points.size(); i++)
//				{
//					const double w = cell()->point(x, i);
//					const double j = cell()->jacobian(J.memptr(), this, x);
//					cell()->gradient(dN.memptr(), x);
//					B = inv(J) * dN.t();
//					K += w * j * t * k * B.t() * B;
//				}
				//return
				return m;
			}
		}
	}
}
