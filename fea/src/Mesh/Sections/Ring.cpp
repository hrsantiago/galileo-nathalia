//std
#include <cmath>
#include <GL/gl.h>

//mat
#include "misc/defs.h"

//fea
#include "Mesh/Sections/Ring.h"
#include "Mesh/Sections/Types.h"
#include "Mesh/Points/Mechanic/Fiber.h"
#include "Mesh/Points/Mechanic/Mechanic.h"

namespace fea
{
	namespace mesh
	{
		namespace sections
		{
			//constructor
			Ring::Ring(void) : m_diameter(0.1), m_thickness(0.01), m_mesh_t(10), m_mesh_r(5)
			{
				return;
			}

			//destructor
			Ring::~Ring(void)
			{
				return;
			}

			//serialization
			void Ring::load(FILE* file)
			{
				Section::load(file);
				fscanf(file, "%lf %lf %d %d", &m_diameter, &m_thickness, &m_mesh_t, &m_mesh_r);
			}
			void Ring::save(FILE* file) const
			{
				Section::save(file);
				fprintf(file, "%+4.2e %+4.2e %02d %02d ", m_diameter, m_thickness, m_mesh_t, m_mesh_r);
			}

			//type
			sections::type Ring::type(void) const
			{
				return sections::type::ring;
			}

			//data
			double Ring::diameter(void) const
			{
				return m_diameter;
			}
			double Ring::diameter(double diameter)
			{
				return m_diameter = diameter;
			}
			double Ring::thickness(void) const
			{
				return m_thickness;
			}
			double Ring::thickness(double thickness)
			{
				return m_thickness = thickness;
			}
			unsigned Ring::mesh_t(void) const
			{
				return m_mesh_t;
			}
			unsigned Ring::mesh_t(unsigned mesh)
			{
				return m_mesh_t = mesh;
			}
			unsigned Ring::mesh_r(void) const
			{
				return m_mesh_r;
			}
			unsigned Ring::mesh_r(unsigned mesh)
			{
				return m_mesh_r = mesh;
			}
			
			//geometry
			double Ring::area(void) const
			{
				const double d = m_diameter;
				const double t = m_thickness;
				return M_PI * (pow(d, 2) - pow(d - 2 * t, 2)) / 4;
			}
			double Ring::width(void) const
			{
				return m_diameter;
			}
			double Ring::height(void) const
			{
				return m_diameter;
			}
			double Ring::inercia_y(void) const
			{
				const double d = m_diameter;
				const double t = m_thickness;
				return M_PI * (pow(d, 4) - pow(d - 2 * t, 4)) / 64;
			}
			double Ring::inercia_z(void) const
			{
				const double d = m_diameter;
				const double t = m_thickness;
				return M_PI * (pow(d, 4) - pow(d - 2 * t, 4)) / 64;
			}
			double Ring::inercia_w(void) const
			{
				return 0;
			}
			double Ring::centroid_z(void) const
			{
				return m_diameter / 2;
			}
			double Ring::centroid_y(void) const
			{
				return m_diameter / 2;
			}
			double Ring::plastic_modulus_y(void) const
			{
				const double t = m_thickness;
				const double ro = m_diameter / 2;
				const double ri = m_diameter / 2 - t;
				return 4 * (ro * ro * ro - ri * ri * ri) / 3;
			}
			double Ring::plastic_modulus_z(void) const
			{
				const double t = m_thickness;
				const double ro = m_diameter / 2;
				const double ri = m_diameter / 2 - t;
				return 4 * (ro * ro * ro - ri * ri * ri) / 3;
			}
			double Ring::shear_coefficient_y(double v) const
			{
				const double ro = m_diameter / 2;
				const double ri = m_diameter / 2 - m_thickness;
				const double m = ri / ro;
				return 6 * (1 + v) * pow(1 + pow(m, 2), 2) / ((7 + 6 * v) * pow(1 + pow(m, 2), 2) + (20 + 12 * v) * pow(m, 2));
			}
			double Ring::shear_coefficient_z(double v) const
			{
				const double ro = m_diameter / 2;
				const double ri = m_diameter / 2 - m_thickness;
				const double m = ri / ro;
				return 6 * (1 + v) * pow(1 + pow(m, 2), 2) / ((7 + 6 * v) * pow(1 + pow(m, 2), 2) + (20 + 12 * v) * pow(m, 2));
			}

			//fibers
			std::vector<points::Fiber> Ring::discretize(void) const
			{
				const unsigned nt = m_mesh_t;
				const unsigned nr = m_mesh_r;
				const double ro = m_diameter / 2;
				const double ri = m_diameter / 2 - m_thickness;
				const double dt = 2 * M_PI / nt;
				const double dr = (ro - ri) / nr;
				std::vector<points::Fiber> fibers(nt * nr);
				for(unsigned i = 0; i < nr; i++)
				{
					for(unsigned j = 0; j < nt; j++)
					{
						const double t = dt * j + dt / 2;
						const double r = dr * i + dr / 2 + ri;
						fibers[i * nt + j].m_area = r * dt * dr;
						fibers[i * nt + j].m_position_z = r * cos(t);
						fibers[i * nt + j].m_position_y = r * sin(t);
					}
				}
				return fibers;
			}
			
			//draw
			void Ring::draw(void) const
			{
				//data
				const unsigned np = 50;
				const unsigned nr = m_mesh_r;
				const unsigned nt = m_mesh_t;
				const double t = m_thickness;
				const double r = m_diameter / 2;
				//radius
				const double rj = r;
				const double ri = r - t;
				//core
				glBegin(GL_QUADS);
				glColor3d(0, 0, 1);
				for(unsigned i = 0; i <= np; i++)
				{
					const double qi = 2 * (i + 0) * M_PI / np;
					const double qj = 2 * (i + 1) * M_PI / np;
					glVertex2d(ri * cos(qi), ri * sin(qi));
					glVertex2d(rj * cos(qi), rj * sin(qi));
					glVertex2d(rj * cos(qj), rj * sin(qj));
					glVertex2d(ri * cos(qj), ri * sin(qj));
				}
				glEnd();
				//mesh
				glBegin(GL_LINES);
				glColor3d(1, 1, 1);
				for(unsigned i = 0; i < nt; i++)
				{
					const double q = 2 * i * M_PI / nt;
					glVertex2d(ri * cos(q), ri * sin(q));
					glVertex2d(rj * cos(q), rj * sin(q));
				}
				glEnd();
				for(unsigned i = 0; i <= nr; i++)
				{
					glBegin(GL_LINE_LOOP);
					for(unsigned j = 0; j < np; j++)
					{
						const double s = r - i * t / nr;
						const double t = 2 * j * M_PI / np;
						glVertex2d(s * cos(t), s * sin(t));
					}
					glEnd();
				}
			}
		}
	}
}
