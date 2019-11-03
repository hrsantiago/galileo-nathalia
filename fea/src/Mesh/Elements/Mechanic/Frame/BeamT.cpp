//std
#include <cmath>
#include <cstring>

//mat
#include "misc/util.h"
#include "misc/stress.h"
#include "linear/lin3.h"
#include "linear/vec3.h"
#include "linear/mat3.h"
#include "linear/quat.h"
#include "linear/dense.h"
#include "linear/linear.h"
#include "linear/vector.h"

//fea
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Nodes/Node.h"

#include "Mesh/Cells/Types.h"
#include "Mesh/Cells/Line/Line.h"

#include "Mesh/Materials/Mechanic/Mechanic.h"

#include "Mesh/Sections/Section.h"

#include "Mesh/Elements/Types.h"
#include "Mesh/Elements/States.h"
#include "Mesh/Elements/Mechanic/Frame/BeamT.h"

#include "Mesh/Points/Mechanic/Fiber.h"
#include "Mesh/Points/Mechanic/Section.h"
#include "Mesh/Points/Mechanic/Mechanic.h"

namespace fea
{
	namespace mesh
	{
		namespace elements
		{
			//constructors
			BeamT::BeamT(void)
			{
				m_orientation[0] = 0;
				m_orientation[1] = 0;
				m_orientation[2] = 1;
				memset(m_f, 0,  6 * sizeof(double));
				memset(m_k, 0, 36 * sizeof(double));
			}

			//destructor
			BeamT::~BeamT(void)
			{
				return;
			}
			
			//serialization
			void BeamT::load(FILE* file)
			{
				Frame::load(file);
				fscanf(file, "%lf %lf %lf", &m_orientation[0], &m_orientation[1], &m_orientation[2]);
			}
			void BeamT::save(FILE* file) const
			{
				Frame::save(file);
				fprintf(file, "%+.6e %+.6e %+.6e ", m_orientation[0], m_orientation[1], m_orientation[2]);
			}

			//data
			const double* BeamT::orientation(void) const
			{
				return m_orientation;
			}
			const double* BeamT::orientation(const double* orientation)
			{
				return (const double*) memcpy(m_orientation, orientation, 3 * sizeof(double));
			}
			const double* BeamT::orientation(double s1, double s2, double s3)
			{
				m_orientation[0] = s1;
				m_orientation[1] = s2;
				m_orientation[2] = s3;
				return m_orientation;
			}

			//types
			unsigned BeamT::cells(void) const
			{
				return (unsigned) cells::type::beam;
			}
			unsigned BeamT::loads(void) const
			{
				return 0;
			}
			unsigned BeamT::states(void) const
			{
				return 
					(unsigned) elements::state::axial |
					(unsigned) elements::state::shear_y |
					(unsigned) elements::state::shear_z |
					(unsigned) elements::state::torsion |
					(unsigned) elements::state::bending_y |
					(unsigned) elements::state::bending_z;
			}
			unsigned BeamT::stresses(void) const
			{
				return 
					(unsigned) mat::stress::sxx |
					(unsigned) mat::stress::sxy |
					(unsigned) mat::stress::sxz;
			}
			unsigned BeamT::dofs(unsigned) const
			{
				return
					(unsigned) nodes::dof::rotation_x |
					(unsigned) nodes::dof::rotation_y |
					(unsigned) nodes::dof::rotation_z |
					(unsigned) nodes::dof::translation_x |
					(unsigned) nodes::dof::translation_y |
					(unsigned) nodes::dof::translation_z;
			}
			elements::type BeamT::type(void) const
			{
				return elements::type::beamT;
			}

			//analysis
			bool BeamT::symmetric(void) const
			{
				return false;
			}
			
			void BeamT::apply(void)
			{
				return;
			}
			
			bool BeamT::check(void) const
			{
				if(Element::check())
				{
					double q[3], s1[3];
					const double* s3 = m_orientation;
					const double* Xi = node(0)->coordinates();
					const double* Xj = node(1)->coordinates();
					if(mat::norm(mat::cross(q, s3, mat::normalize(s1, Xi, Xj, 3)), 3) < 1e-5)
					{
						printf("\tElement %04d (BeamT) has axis and orientation too close!\n", index());
						return false;
					}
					else
					{
						return true;
					}
				}
				else
				{
					return false;
				}
				
			}
			void BeamT::prepare(void)
			{
				//base call
				Element::prepare();
				//coordinates
				const mat::vec3 Xi = node(0)->coordinates();
				const mat::vec3 Xj = node(1)->coordinates();
				//orientation
				mat::vec3(m_orientation).project((Xj - Xi).normalize());
			}
			
			void BeamT::record(void)
			{
				return;
			}

			//formulation
			double* BeamT::inertial_force(double* f) const
			{
				return (double*) memset(f, 0, 12 * sizeof(double));
			}
			double* BeamT::internal_force(double* f) const
			{
				//kinematics
				double pi[3], pj[3];
				const mat::vec3 xi = node(0)->position(pi);
				const mat::vec3 xj = node(1)->position(pj);
				const mat::vec3 Xi = node(0)->coordinates();
				const mat::vec3 Xj = node(1)->coordinates();
				//length
				const double L = (Xj - Xi).norm();
				const double l = (xj - xi).norm();
				//initial triad
				const mat::vec3 s1 = (Xj - Xi) / L;
				const mat::vec3 s3 = m_orientation;
				const mat::vec3 s2 = mat::cross(s3, s1);
				//nodal quaternions
				const mat::quat qi = node(0)->quaternion();
				const mat::quat qj = node(1)->quaternion();
				//nodal triad
				const mat::vec3 ri3 = qi.rotate(s3);
				const mat::vec3 rj3 = qj.rotate(s3);
				//return
				return f;
			}
			double* BeamT::reference_force(double* f, const boundary::loads::Element* load) const
			{
				return nullptr;
			}
			
			double* BeamT::inertia(double* m) const
			{
				return (double*) memset(m, 0, 144 * sizeof(double));
			}
			double* BeamT::damping(double* c) const
			{
				return (double*) memset(c, 0, 144 * sizeof(double));
			}
			double* BeamT::stiffness(double* k) const
			{
				//return
				return k;
			}
		}
	}
}
