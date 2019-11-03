//std
#include <cstring>

//mat
#include "misc/stress.h"
#include "linear/dense.h"
#include "linear/linear.h"
#include "linear/matrix.h"

//fea
#include "Mesh/Mesh.h"

#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Nodes/Node.h"

#include "Mesh/Cells/Types.h"
#include "Mesh/Cells/Plane/Plane.h"

#include "Mesh/Materials/Mechanic/Mechanic.h"

#include "Mesh/Elements/Types.h"
#include "Mesh/Elements/Mechanic/Plane/Plane.h"

#include "Mesh/Points/Mechanic/Mechanic.h"

#include "Boundary/Loads/Types.h"
#include "Boundary/Loads/Elements/Mechanic/Plane/Plane_Edge.h"
#include "Boundary/Loads/Elements/Mechanic/Plane/Plane_Face.h"

namespace fea
{
	namespace mesh
	{
		namespace elements
		{
			//constructors
			Plane::Plane(void)
			{
				return;
			}

			//destructor
			Plane::~Plane(void)
			{
				return;
			}

			//type
			unsigned Plane::cells(void) const
			{
				return
					(unsigned) cells::type::tri3 |
					(unsigned) cells::type::tri6 |
					(unsigned) cells::type::quad4 |
					(unsigned) cells::type::quad8 |
					(unsigned) cells::type::quad9;
			}
			unsigned Plane::loads(void) const
			{
				return
					(unsigned) boundary::loads::type::plane_edge |
					(unsigned) boundary::loads::type::plane_face;
			}
			unsigned Plane::states(void) const
			{
				return 0;
			}
			unsigned Plane::stresses(void) const
			{
				return
					(unsigned) mat::stress::sxx |
					(unsigned) mat::stress::syy |
					(unsigned) mat::stress::sxy;
			}
			unsigned Plane::dofs(unsigned) const
			{
				return 
					(unsigned) nodes::dof::translation_x |
					(unsigned) nodes::dof::translation_y;
			}
			elements::type Plane::type(void) const
			{
				return elements::type::plane;
			}

			//analysis
			void Plane::apply(void)
			{
				return;
			}
			
			//strain
			void Plane::strain_gradient(double* B, const double* dNx, unsigned n) const
			{
				for(unsigned i = 0; i < n; i++)
					{
						B[0 + 3 * (2 * i + 1)] = 0;
						B[1 + 3 * (2 * i + 0)] = 0;
						B[0 + 3 * (2 * i + 0)] = dNx[i + n * 0];
						B[1 + 3 * (2 * i + 1)] = dNx[i + n * 1];
						B[2 + 3 * (2 * i + 0)] = dNx[i + n * 1];
						B[2 + 3 * (2 * i + 1)] = dNx[i + n * 0];
					}
			}

			//formulation
			double* Plane::internal_force(double* f) const
			{
				//cell
				const unsigned n = cell()->vertices();
				const double t = ((cells::Plane*) cell())->thickness();
				//data
				double w, d, p[2], J[4], C[9], e[3], s[3];
				double u[2 * n], dNe[2 * n], dNx[2 * n], B[6 * n];
				//state
				for(unsigned i = 0; i < n; i++)
				{
					u[2 * i + 0] = node(i)->state(nodes::dof::translation_x);
					u[2 * i + 1] = node(i)->state(nodes::dof::translation_y);
				}
				//internal force
				memset(f, 0, 2 * n * sizeof(double));
				for(unsigned i = 0; i < m_points.size(); i++)
				{
					//cell
					w = cell()->point(p, i);
					d = cell()->jacobian(J, this, p);
					//strain gradient
					((materials::Mechanic*) material())->elastic_stiffness(C, stresses());
					strain_gradient(B, mat::multiply(dNx, cell()->gradient(dNe, p), mat::inverse(J, 2), n, 2, 2), n);
					mat::multiply(e, B, u, 3, 2 * n, 1);
					mat::multiply(s, C, e, 3, 3, 1);
					//internal force
					for(unsigned j = 0; j < n; j++)
					{
						f[j] = 0;
						for(unsigned k = 0; k < 3; k++)
						{
							f[j] += t * w * d * B[k + 3 * j] * s[k];
						}
					}
				}
				return f;
			}
			double* Plane::inertial_force(double* f) const
			{
				return (double*) memset(f, 0, m_dof.size() * sizeof(double));
			}
			double* Plane::reference_force(double* f, const boundary::loads::Element* load) const
			{
				switch(load->type())
				{
					case boundary::loads::type::plane_edge: 
						return load_edge(f, (const boundary::loads::Plane_Edge*) load);
					case boundary::loads::type::plane_face: 
						return load_face(f, (const boundary::loads::Plane_Face*) load);
					default:
						return nullptr;
				}
			}
			double* Plane::load_edge(double* f, const boundary::loads::Plane_Edge* load) const
			{
				//load
//				const double v = load->value();
//				const unsigned e = load->edge();
//				const double q = load->direction();
				//cell
//				const unsigned n = cell()->vertices();
//				const double t = ((cells::Plane*) cell())->thickness();
//				//gauss points
//				double p[2];
//				arma::mat::fixed<2, 2> J;
//				arma::vec N(n), s(r), w(r);
//				//external force
//				memset(f, 0, 2 * n * sizeof(double));
//				for(unsigned i = 0; i < r; i++)
//				{
//					//edge
//					cell()->edge(e, p, s[i]);
//					//shape
//					cell()->function(p, N);
//					//jacobian
//					const double m = cell()->jacobian(this, p, J);
//					//contribution
//					for(unsigned j = 0; j < n; j++)
//					{
//						f[2 * j + 0] += v * t * m * w(i) * N(j) * cos(q);
//						f[2 * j + 1] += v * t * m * w(i) * N(j) * sin(q);
//					}
//				}
//				//return
				return f;
			}
			double* Plane::load_face(double* f, const boundary::loads::Plane_Face* load) const
			{
//				//load
//				const double v = load->value();
//				//thickness
//				const double t = ((cells::Plane*) cell())->thickness();
//				//direction
//				const double q = load->direction();
//				//cell
//				const unsigned n = cell()->vertices();
//				//external force
//				double x[2];
//				arma::vec N(n);
//				arma::mat::fixed<2, 2> J;
//				memset(f, 0, 2 * n * sizeof(double));
//				for(unsigned i = 0; i < m_points.size(); i++)
//				{
//					//gauss point
//					const double w = cell()->point(x, i);
//					//shape
//					cell()->function(x, N);
//					//jacobian
//					const double m = cell()->jacobian(this, x, J);
//					//contribution
//					for(unsigned j = 0; j < n; j++)
//					{
//						f[2 * j + 0] += v * t * m * w * N(j) * cos(q);
//						f[2 * j + 1] += v * t * m * w * N(j) * sin(q);
//					}
//				}
//				//return
				return f;
			}
			
			double* Plane::inertia(double* m) const
			{
				const unsigned nd = m_dof.size();
				return (double*) memset(m, 0, nd * nd * sizeof(double));
			}
			double* Plane::damping(double* c) const
			{
				const unsigned nd = m_dof.size();
				return (double*) memset(c, 0, nd * nd * sizeof(double));
			}
			double* Plane::stiffness(double* k) const
			{
				//cell
				const unsigned n = cell()->vertices();
				const double t = ((cells::Plane*) cell())->thickness();
				//data
				double p[2], J[4], C[9];
				double dNe[2 * n], dNx[2 * n], B[6 * n];
				//stiffness
				memset(k, 0, 4 * n * n * sizeof(double));
				for(unsigned i = 0; i < m_points.size(); i++)
				{
					//cell
					const double w = cell()->point(p, i);
					const double d = cell()->jacobian(J, this, p);
					cell()->gradient(dNe, p);
					//strain gradient
					strain_gradient(B, mat::multiply(dNx, dNe, mat::inverse(J, 2), n, 2, 2), n);
					//material
					((materials::Mechanic*) material())->elastic_stiffness(C, stresses());
					//stiffness
					mat::matrix(k, 2 * n, 2 * n) += t * w * d * mat::matrix(B, 3, 2 * n).transpose() * mat::matrix(C, 3, 3) * mat::matrix(B, 3, 2 * n);
				}
				return k;
			}
		}
	}
}
