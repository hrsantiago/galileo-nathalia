//std
#include <cstring>

//mat
#include "misc/stress.h"

//fea
#include "Mesh/Mesh.h"

#include "Mesh/Nodes/Dofs.h"

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
//				cell
//				const unsigned nn = cell()->vertices();
//				displacements
//				arma::vec u(2 * nn);
//				state(u.memptr());
//				strain and stress
//				double x[2];
//				arma::mat::fixed<2, 2> J;
//				arma::mat::fixed<3, 3> C;
//				arma::mat dNe(nn, 2), dNx(nn, 2), B(3, 2 * nn);
//				for(unsigned i = 0; i < m_points.size(); i++)
//				{
//					point
//					cell()->point(x, i);
//					jacobian
//					cell()->jacobian(this, x, J);
//					gradient
//					cell()->gradient(x, dNe);
//					strain gradient
//					dNx = dNe * arma::inv(J);
//					strain_gradient(dNx, B);
//					material point
//					points::Material& material_point = ((points::Mechanic*) m_points[i])->m_material_points[0];
//					arma::vec e(material_point.m_strain_new, 3, false, true);
//					arma::vec s(material_point.m_stress_new, 3, false, true);
//					arma::vec sr(material_point.m_stress_residual, 3, false, true);
//					strain
//					e = B * u;
//					stress
//					if(!m_inelastic)
//					{
//						if(i == 0)
//						{
//							((materials::Mechanic*) material(0))->elastic_stiffness(material_point, C);
//						}
//						s = C * e + sr;
//					}
//					else
//					{
//						((materials::Mechanic*) material(0))->return_mapping(material_point);
//					}
//				}
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
//				cell
//				const unsigned nn = cell()->vertices();
//				thickness
//				const double t = ((cells::Plane*) cell())->thickness();
//				internal force
//				double x[2];
//				arma::mat::fixed<2, 2> J;
//				arma::mat::fixed<3, 3> C;
//				arma::vec Fi(f, 2 * nn, false, true);
//				arma::mat dNe(nn, 2), dNx(nn, 2), B(3, 2 * nn);
//				memset(f, 0, 2 * nn * sizeof(double));
//				for(unsigned i = 0; i < m_points.size(); i++)
//				{
//					gauss point
//					const double h = cell()->point(x, i);
//					jacobian
//					const double w = cell()->jacobian(this, x, J);
//					gradient
//					cell()->gradient(x, dNe);
//					strain gradient
//					dNx = dNe * arma::inv(J);
//					strain_gradient(dNx, B);
//					material point
//					points::Material& material_point = ((points::Mechanic*)m_points[i])->m_material_points[0];
//					stress
//					arma::vec s(material_point.m_stress_new, 3, false, true);
//					internal force
//					Fi += t * w * h * B.t() * s;
//				}
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
					case boundary::loads::type::plane_edge: return load_edge(f, (const boundary::loads::Plane_Edge*) load);
					case boundary::loads::type::plane_face: return load_face(f, (const boundary::loads::Plane_Face*) load);
				}
			}
			double* Plane::load_edge(double* f, const boundary::loads::Plane_Edge* load) const
			{
//				//load
//				const double v = load->value();
//				//thickness
//				const double t = ((cells::Plane*) cell())->thickness();
//				//direction
//				const double q = load->direction();
//				//rule
//				const unsigned r = cell()->rule();
//				//edge
//				const unsigned e = load->edge();
//				//cell
//				const unsigned n = cell()->vertices();
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
//				cell
//				const unsigned nn = cell()->vertices();
//				thickness
//				const double t = ((cells::Plane*)cell())->thickness();
//				internal force
//				double x[2];
//				arma::mat::fixed<2, 2> J;
//				arma::mat::fixed<3, 3> C;
//				arma::mat dNe(nn, 2), dNx(nn, 2), B(3, 2 * nn), K(k, 2 * nn, 2 * nn, false, true);
//				memset(k, 0, 4 * nn * nn * sizeof(double));
//				for(unsigned i = 0; i < m_points.size(); i++)
//				{
//					gauss point
//					const double h = cell()->point(x, i);
//					jacobian
//					const double w = cell()->jacobian(this, x, J);
//					gradient
//					cell()->gradient(x, dNe);
//					strain gradient
//					dNx = dNe * arma::inv(J);
//					strain_gradient(dNx, B);
//					material point
//					points::Material& material_point = ((points::Mechanic*)m_points[i])->m_material_points[0];
//					tangent modulus
//					if(!m_inelastic && i == 0)
//					{
//						((materials::Mechanic*)material(0))->elastic_stiffness(material_point, C);
//					}
//					else
//					{
//						((materials::Mechanic*)material(0))->tangent_modulus(material_point, C);
//					}
//					internal force
//					K += t * w * h * B.t() * C * B;
//				}
				return k;
			}
		}
	}
}
