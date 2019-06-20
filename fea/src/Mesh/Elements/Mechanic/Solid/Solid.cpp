//std
#include <cmath>
#include <cstring>

//mat
#include "misc/util.h"
#include "misc/stress.h"
#include "linear/dense.h"
#include "linear/linear.h"

//fea
#include "Mesh/Mesh.h"

#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Nodes/Node.h"

#include "Mesh/Cells/Types.h"
#include "Mesh/Cells/Cells.h"
#include "Mesh/Cells/Volume/Volume.h"

#include "Mesh/Materials/Mechanic/Mechanic.h"

#include "Mesh/Elements/Types.h"
#include "Mesh/Elements/Mechanic/Solid/Solid.h"

#include "Mesh/Points/Mechanic/Mechanic.h"

#include "Boundary/Loads/Types.h"
#include "Boundary/Loads/Elements/Mechanic/Solid/Solid_Edge.h"
#include "Boundary/Loads/Elements/Mechanic/Solid/Solid_Face.h"
#include "Boundary/Loads/Elements/Mechanic/Solid/Solid_Body.h"

namespace fea
{
	namespace mesh
	{
		namespace elements
		{
			//constructors
			Solid::Solid(void)
			{
				return;
			}

			//destructor
			Solid::~Solid(void)
			{
				return;
			}

			//type
			unsigned Solid::cells(void) const
			{
				return
					(unsigned) cells::type::tetra4	|
					(unsigned) cells::type::brick8	|
					(unsigned) cells::type::brick20;
			}
			unsigned Solid::loads(void) const
			{
				return
					(unsigned) boundary::loads::type::solid_edge |
					(unsigned) boundary::loads::type::solid_face |
					(unsigned) boundary::loads::type::solid_body;
			}
			unsigned Solid::states(void) const
			{
				return 0;
			}
			unsigned Solid::stresses(void) const
			{
				return
					(unsigned) mat::stress::sxx |
					(unsigned) mat::stress::syy |
					(unsigned) mat::stress::szz |
					(unsigned) mat::stress::sxy |
					(unsigned) mat::stress::sxz |
					(unsigned) mat::stress::syz;
			}
			unsigned Solid::dofs(unsigned) const
			{
				return 
					(unsigned) nodes::dof::translation_x |
					(unsigned) nodes::dof::translation_y |
					(unsigned) nodes::dof::translation_z;
			}
			elements::type Solid::type(void) const
			{
				return elements::type::solid;
			}

			//analysis
			void Solid::apply(void)
			{
				//cell
//				const unsigned n = cell()->vertices();
				//displacements
//				arma::vec u(3 * nn);
//				state(u.memptr());
				//strain and stress
//				double p[3], J[9], C[36];
//				double dNe(nn, 3), dNx(nn, 3), B(6, 3 * nn);
//				for(unsigned i = 0; i < m_points.size(); i++)
//				{
//					point
//					cell()->point(p, i);
//					jacobian
//					cell()->jacobian(this, p, J);
//					gradient
//					cell()->gradient(p, dNe);
//					strain gradient
//					dNx = dNe * inv(J);
//					for(unsigned i = 0; i < nn; i++)
//					{
//						B(0, 3 * i + 0) = dNx(i, 0);
//						B(0, 3 * i + 1) = 0.0;
//						B(0, 3 * i + 2) = 0.0;
//						B(1, 3 * i + 0) = 0.0;
//						B(1, 3 * i + 1) = dNx(i, 1);
//						B(1, 3 * i + 2) = 0.0;
//						B(2, 3 * i + 0) = 0.0;
//						B(2, 3 * i + 1) = 0.0;
//						B(2, 3 * i + 2) = dNx(i, 2);
//						B(3, 3 * i + 0) = dNx(i, 1);
//						B(3, 3 * i + 1) = dNx(i, 0);
//						B(3, 3 * i + 2) = 0.0;
//						B(4, 3 * i + 0) = dNx(i, 2);
//						B(4, 3 * i + 1) = 0.0;
//						B(4, 3 * i + 2) = dNx(i, 0);
//						B(5, 3 * i + 0) = 0.0;
//						B(5, 3 * i + 1) = dNx(i, 2);
//						B(5, 3 * i + 2) = dNx(i, 1);
//					}
//					material point
//					points::Material& material_point = static_cast<points::Mechanic*> (m_points[i])->m_material_points[0];
//					arma::vec e(material_point.m_strain_new, 6, false, true);
//					arma::vec s(material_point.m_stress_new, 6, false, true);
//					arma::vec sr(material_point.m_stress_residual, 6, false, true);
//					strain
//					e = B * u;
//					stress
//					if(!m_inelastic)
//					{
//						if(i == 0)
//						{
//							((materials::Mechanic*)material(0))->elastic_stiffness(material_point, C);
//						}
//						s = C * e + sr;
//					}
//					else
//					{
//						((materials::Mechanic*)material(0))->return_mapping(material_point);
//					}
//				}
			}

			//formulation
			double* Solid::inertial_force(double* f) const
			{
				const unsigned nd = m_dof.size();
				return (double*) memset(f, 0, nd * sizeof(double));
			}
			double* Solid::internal_force(double* f) const
			{
				//cell
				const unsigned n = cell()->vertices();
				const unsigned m = cells::Cell::max_nodes();
				//stress
				const unsigned t = 
					(unsigned) mat::stress::sxx |
					(unsigned) mat::stress::syy |
					(unsigned) mat::stress::szz |
					(unsigned) mat::stress::sxy |
					(unsigned) mat::stress::sxz |
					(unsigned) mat::stress::syz ;
				//data
				double p[3], u[3], e[6];
				double s[6], J[9], B[18 * m];
				double C[36], dNe[3 * m], dNx[3 * m];
				memset(f, 0, 3 * n * sizeof(double));
				//stiffness
				((materials::Mechanic*) material())->elastic_stiffness(C, t);
				//points
				for(unsigned i = 0; i < m_points.size(); i++)
				{
					//point
					const double h = cell()->point(p, i);
					//jacobian
					const double w = cell()->jacobian(J, this, p);
					//gradient
					mat::multiply(dNx, cell()->gradient(dNe, p), mat::inv(J, 3), n, 3, 3);
					//strains
					mat::clean(e, 6);
					for(unsigned j = 0; j < n; j++)
					{
						//displacements
						node(j)->displacement(u);
						//kinematic matrix
						B[0 + 6 * 1 + 18 * j] = 0;
						B[0 + 6 * 2 + 18 * j] = 0;
						B[1 + 6 * 0 + 18 * j] = 0;
						B[1 + 6 * 2 + 18 * j] = 0;
						B[2 + 6 * 0 + 18 * j] = 0;
						B[2 + 6 * 1 + 18 * j] = 0;
						B[3 + 6 * 2 + 18 * j] = 0;
						B[4 + 6 * 1 + 18 * j] = 0;
						B[5 + 6 * 0 + 18 * j] = 0;
						B[0 + 6 * 0 + 18 * j] = dNx[j + n * 0];
						B[1 + 6 * 1 + 18 * j] = dNx[j + n * 1];
						B[2 + 6 * 2 + 18 * j] = dNx[j + n * 2];
						B[3 + 6 * 0 + 18 * j] = dNx[j + n * 1];
						B[3 + 6 * 1 + 18 * j] = dNx[j + n * 0];
						B[4 + 6 * 0 + 18 * j] = dNx[j + n * 2];
						B[4 + 6 * 2 + 18 * j] = dNx[j + n * 0];
						B[5 + 6 * 1 + 18 * j] = dNx[j + n * 2];
						B[5 + 6 * 2 + 18 * j] = dNx[j + n * 1];
						//strains
						mat::multiply(e, B + 18 * j, u, 6, 3, 1, true);
					}
					//stress
					mat::multiply(s, C, e, 6, 6);
					//internal force
					for(unsigned j = 0; j < n; j++)
					{
						for(unsigned k = 0; k < 3; k++)
						{
							for(unsigned q = 0; q < 6; q++)
							{
								f[3 * j + k] += w * h * B[q + 6 * k + 18 * j] * s[q];
							}
						}
					}
				}
				//return
				return f;
			}
			
			double* Solid::reference_force(double* f, const boundary::loads::Element* load) const
			{
				switch(load->type())
				{
					case boundary::loads::type::solid_edge: return edge_force(f, (const boundary::loads::Solid_Edge*) load);
					case boundary::loads::type::solid_face: return face_force(f, (const boundary::loads::Solid_Face*) load);
					case boundary::loads::type::solid_body: return body_force(f, (const boundary::loads::Solid_Body*) load);
				}
			}
			double* Solid::edge_force(double* f, const boundary::loads::Solid_Edge* load) const
			{
//				//load
//				const double v = load->value();
//				//direction
//				const double* q = load->direction();
//				//rule
//				const unsigned r = cell()->rule();
//				//edge
//				const unsigned e = load->edge();
//				//cell
//				const unsigned n = cell()->vertices();
//				//gauss points
//				double p[3];
//				arma::mat::fixed<3, 3> J;
//				arma::vec N(n), s(r), w(r);
//				//external force
//				memset(f, 0, 3 * n * sizeof(double));
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
//						f[3 * j + 0] += v * m * w(i) * N(j) * cos(q[1]) * cos(q[0]);
//						f[3 * j + 1] += v * m * w(i) * N(j) * cos(q[1]) * sin(q[0]);
//						f[3 * j + 2] += v * m * w(i) * N(j) * sin(q[1]);
//					}
//				}
				//return
				return f;
			}
			double* Solid::face_force(double* f, const boundary::loads::Solid_Face* load) const
			{
				//load
				const double v = load->value();
				const unsigned e = load->face();
				const double* q = load->direction();
				//cell
				const unsigned r = cell()->rule();
				const unsigned n = cell()->vertices();
				//gauss points
				double p[3], J[9], g[6];
				double N[n], s[r], w[r];
				double du[3], dv[3], dn[3];
				//external force
				memset(f, 0, 3 * n * sizeof(double));
				for(unsigned i = 0; i < r; i++)
				{
					for(unsigned j = 0; j < r; j++)
					{
						//point
						mat::gauss_point(s[0], w[0], r, i);
						mat::gauss_point(s[1], w[1], r, j);
						//face
						cell()->face(p, e, s[0], s[1]);
						//shape
						cell()->function(N, p);
						//jacobian
						cell()->jacobian(J, this, p);
						cell()->gradient(g, e, s[0], s[1]);
						mat::multiply(du, J, g + 0, 3, 3);
						mat::multiply(dv, J, g + 3, 3, 3);
						const double h = mat::norm(mat::cross(dn, du, dv), 3);
						//contribution
						for(unsigned k = 0; k < n; k++)
						{
							f[3 * k + 2] += v * h * w[0] * w[1] * N[k] * sin(q[1]);
							f[3 * k + 0] += v * h * w[0] * w[1] * N[k] * cos(q[1]) * cos(q[0]);
							f[3 * k + 1] += v * h * w[0] * w[1] * N[k] * cos(q[1]) * sin(q[0]);
						}
					}
				}
				//return
				return f;
			}
			double* Solid::body_force(double* f, const boundary::loads::Solid_Body* load) const
			{
//				//load
//				const double v = load->value();
//				//direction
//				const double* q = load->direction();
//				//rule
//				const unsigned r = cell()->rule();
//				//cell
//				const unsigned n = cell()->vertices();
//				//external force
//				double p[3];
//				arma::vec N(n);
//				arma::mat::fixed<3, 3> J;
//				memset(f, 0, 3 * n * sizeof(double));
//				for(unsigned i = 0; i < m_points.size(); i++)
//				{
//					//gauss point
//					const double w = cell()->point(p, i);
//					//shape
//					cell()->function(p, N);
//					//jacobian
//					const double m = cell()->jacobian(this, p, J);
//					//contribution
//					for(unsigned j = 0; j < n; j++)
//					{
//						f[3 * j + 0] += v * m * N(j) * w * cos(q[1]) * cos(q[0]);
//						f[3 * j + 1] += v * m * N(j) * w * cos(q[1]) * cos(q[0]);
//						f[3 * j + 2] += v * m * N(j) * w * sin(q[1]);
//					}
//				}
				//return
				return f;
			}
			
			double* Solid::inertia(double* m) const
			{
				const unsigned nd = m_dof.size();
				return (double*) memset(m, 0, nd * nd * sizeof(double));
			}
			double* Solid::damping(double* c) const
			{
				const unsigned nd = m_dof.size();
				return (double*) memset(c, 0, nd * nd * sizeof(double));
			}
			double* Solid::stiffness(double* k) const
			{
				//cell
				const unsigned n = cell()->vertices();
				const unsigned m = cells::Cell::max_nodes();
				//stress
				const unsigned t = 
					(unsigned) mat::stress::sxx |
					(unsigned) mat::stress::syy |
					(unsigned) mat::stress::szz |
					(unsigned) mat::stress::sxy |
					(unsigned) mat::stress::sxz |
					(unsigned) mat::stress::syz ;
				//data
				double p[3], J[9], B[18 * m];
				double C[36], dNe[3 * m], dNx[3 * m];
				memset(k, 0, 9 * n * n * sizeof(double));
				//stiffness
				((materials::Mechanic*) material())->elastic_stiffness(C, t);
				//points
				for(unsigned i = 0; i < m_points.size(); i++)
				{
					//point
					const double h = cell()->point(p, i);
					//jacobian
					const double w = cell()->jacobian(J, this, p);
					//gradient
					mat::multiply(dNx, cell()->gradient(dNe, p), mat::inv(J, 3), n, 3, 3);
					//kinematic matrix
					for(unsigned j = 0; j < n; j++)
					{
						B[0 + 6 * 1 + 18 * j] = 0;
						B[0 + 6 * 2 + 18 * j] = 0;
						B[1 + 6 * 0 + 18 * j] = 0;
						B[1 + 6 * 2 + 18 * j] = 0;
						B[2 + 6 * 0 + 18 * j] = 0;
						B[2 + 6 * 1 + 18 * j] = 0;
						B[3 + 6 * 2 + 18 * j] = 0;
						B[4 + 6 * 1 + 18 * j] = 0;
						B[5 + 6 * 0 + 18 * j] = 0;
						B[0 + 6 * 0 + 18 * j] = dNx[j + n * 0];
						B[1 + 6 * 1 + 18 * j] = dNx[j + n * 1];
						B[2 + 6 * 2 + 18 * j] = dNx[j + n * 2];
						B[3 + 6 * 0 + 18 * j] = dNx[j + n * 1];
						B[3 + 6 * 1 + 18 * j] = dNx[j + n * 0];
						B[4 + 6 * 0 + 18 * j] = dNx[j + n * 2];
						B[4 + 6 * 2 + 18 * j] = dNx[j + n * 0];
						B[5 + 6 * 1 + 18 * j] = dNx[j + n * 2];
						B[5 + 6 * 2 + 18 * j] = dNx[j + n * 1];
					}
					//stiffness
					for(unsigned ci = 0; ci < 3 * n; ci++)
					{
						for(unsigned cj = 0; cj < 3 * n; cj++)
						{
							for(unsigned di = 0; di < 6; di++)
							{
								for(unsigned dj = 0; dj < 6; dj++)
								{
									k[ci + 3 * n * cj] += w * h * B[di + 6 * ci] * B[dj + 6 * cj] * C[di + 6 * dj];
								}
							}
						}
					}
				}
				//return
				return k;
			}
		}
	}
}
