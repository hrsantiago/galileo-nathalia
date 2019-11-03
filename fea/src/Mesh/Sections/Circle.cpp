//std
#include <cmath>
#include <GL/gl.h>

//fea
#include "Mesh/Sections/Types.h"
#include "Mesh/Sections/Circle.h"
#include "Mesh/Points/Mechanic/Fiber.h"
#include "Mesh/Points/Mechanic/Mechanic.h"

namespace fea
{
	namespace mesh
	{
		namespace sections
		{
			//constructors
			Circle::Circle(void) : m_diameter(0.1), m_mesh_t(10), m_mesh_r(5)
			{
				return;
			}

			//destructor
			Circle::~Circle(void)
			{
				return;
			}

			//serialization
			void Circle::load(FILE* file)
			{
				Section::load(file);
				fscanf(file, "%lf %d %d", &m_diameter, &m_mesh_t, &m_mesh_r);
			}
			void Circle::save(FILE* file) const
			{
				Section::save(file);
				fprintf(file, "%+.6e %02d %02d ", m_diameter, m_mesh_t, m_mesh_r);
			}

			//type
			sections::type Circle::type(void) const
			{
				return sections::type::circle;
			}

			//data
			double Circle::diameter(void) const
			{
				return m_diameter;
			}
			double Circle::diameter(double d)
			{
				return m_diameter = d;
			}
			
			unsigned Circle::mesh_t(void) const
			{
				return m_mesh_t;
			}
			unsigned Circle::mesh_t(unsigned mesh)
			{
				return m_mesh_t = mesh;
			}
			unsigned Circle::mesh_r(void) const
			{
				return m_mesh_r;
			}
			unsigned Circle::mesh_r(unsigned mesh)
			{
				return m_mesh_r = mesh;
			}

			//geometry
			double Circle::area(void) const
			{
				return M_PI * pow(m_diameter, 2) / 4;
			}
			double Circle::width(void) const
			{
				return m_diameter;
			}
			double Circle::height(void) const
			{
				return m_diameter;
			}
			double Circle::inertia_y(void) const
			{
				return M_PI * pow(m_diameter, 4) / 64;
			}
			double Circle::inertia_z(void) const
			{
				return M_PI * pow(m_diameter, 4) / 64;
			}
			double Circle::inertia_w(void) const
			{
				return 0;
			}
			double Circle::centroid_z(void) const
			{
				return m_diameter / 2;
			}
			double Circle::centroid_y(void) const
			{
				return m_diameter / 2;
			}
			double Circle::plastic_modulus_y(void) const
			{
				return 4 * pow(m_diameter, 3) / 3;
			}
			double Circle::plastic_modulus_z(void) const
			{
				return 4 * pow(m_diameter, 3) / 3;
			}
			double Circle::shear_coefficient_y(double v) const
			{
				return 6 * (1 + v) / (7 + 6 * v);
			}
			double Circle::shear_coefficient_z(double v) const
			{
				return 6 * (1 + v) / (7 + 6 * v);
			}

			//fibers
			std::vector<points::Fiber> Circle::discretize(void) const
			{
				const unsigned nt = m_mesh_t;
				const unsigned nr = m_mesh_r;
				const double dt = 2 * M_PI / nt;
				const double dr = m_diameter / 2 / nr;
				std::vector<points::Fiber> fibers(nt * nr);
				for(unsigned i = 0; i < nr; i++)
				{
					for(unsigned j = 0; j < nt; j++)
					{
						const double t = dt * j + dt / 2;
						const double r = dr * i + dr / 2;
						fibers[i * nt + j].m_area = r * dt * dr;
						fibers[i * nt + j].m_position_z = r * cos(t);
						fibers[i * nt + j].m_position_y = r * sin(t);
					}
				}
				return fibers;
			}
			
			//draw
			void Circle::draw(void) const
			{
				//data
				const unsigned np = 50;
				const unsigned nr = m_mesh_r;
				const unsigned nt = m_mesh_t;
				const double r = m_diameter / 2;
				//core
				glColor3d(0, 0, 1);
				glBegin(GL_TRIANGLE_FAN);
				glVertex2d(0, 0);
				for(unsigned i = 0; i <= np; i++)
				{
					glVertex2d(r * cos(2 * i * M_PI / np), r * sin(2 * i * M_PI / np));
				}
				glEnd();
				//mesh
				glBegin(GL_LINES);
				glColor3d(1, 1, 1);
				for(unsigned i = 0; i < nt; i++)
				{
					glVertex2d(0, 0);
					glVertex2d(r * cos(2 * i * M_PI / nt), r * sin(2 * i * M_PI / nt));
				}
				glEnd();
				for(unsigned i = 0; i < nr; i++)
				{
					glBegin(GL_LINE_LOOP);
					for(unsigned j = 0; j < np; j++)
					{
						const double s = (i + 1) * r / nr;
						const double t = 2 * j * M_PI / np;
						glVertex2d(s * cos(t), s * sin(t));
					}
					glEnd();
				}
			}
		}
	}
}
