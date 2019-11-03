//std
#include <cmath>
#include <GL/gl.h>

//fea
#include "Mesh/Sections/Types.h"
#include "Mesh/Sections/Rectangle.h"
#include "Mesh/Points/Mechanic/Fiber.h"
#include "Mesh/Points/Mechanic/Mechanic.h"

namespace fea
{
	namespace mesh
	{
		namespace sections
		{
			//constructors
			Rectangle::Rectangle(void) : m_width(0.2), m_height(0.6), m_mesh_w(20), m_mesh_h(20)
			{
				return;
			}

			//destructor
			Rectangle::~Rectangle(void)
			{
				return;
			}

			//serialization
			void Rectangle::load(FILE* file)
			{
				Section::load(file);
				fscanf(file, "%lf %lf %d %d", &m_width, &m_height, &m_mesh_w, &m_mesh_h);
			}
			void Rectangle::save(FILE* file) const
			{
				Section::save(file);
				fprintf(file, "%+.2e %+.2e %02d %02d ", m_width, m_height, m_mesh_w, m_mesh_h);
			}

			//type
			sections::type Rectangle::type(void) const
			{
				return sections::type::rectangle;
			}

			//data
			double Rectangle::width(double w)
			{
				return m_width = w;
			}
			double Rectangle::height(double h)
			{
				return m_height = h;
			}
			
			unsigned Rectangle::mesh_w(void) const
			{
				return m_mesh_w;
			}
			unsigned Rectangle::mesh_w(unsigned mesh)
			{
				return m_mesh_w = mesh;
			}
			unsigned Rectangle::mesh_h(void) const
			{
				return m_mesh_h;
			}
			unsigned Rectangle::mesh_h(unsigned mesh)
			{
				return m_mesh_h = mesh;
			}

			//geometry
			double Rectangle::area(void) const
			{
				const double w = m_width;
				const double h = m_height;
				return w * h;
			}
			double Rectangle::width(void) const
			{
				return m_width;
			}
			double Rectangle::height(void) const
			{
				return m_height;
			}
			double Rectangle::inertia_y(void) const
			{
				const double w = m_width;
				const double h = m_height;
				return h * pow(w, 3) / 12;
			}
			double Rectangle::inertia_z(void) const
			{
				const double w = m_width;
				const double h = m_height;
				return w * pow(h, 3) / 12;
			}
			double Rectangle::inertia_w(void) const
			{
				const double a = std::min(m_width, m_height);
				const double b = std::max(m_width, m_height);
				return a * pow(b, 3) / 12 - b * pow(a, 3) * (1.0 / 4 - 64 / pow(M_PI, 5) * a / b * tanh(M_PI * b / 2 / a));
			}
			double Rectangle::centroid_z(void) const
			{
				return m_width / 2;
			}
			double Rectangle::centroid_y(void) const
			{
				return m_height / 2;
			}
			double Rectangle::plastic_modulus_y(void) const
			{
				const double w = m_width;
				const double h = m_height;
				return h * pow(w, 2) / 4;
			}
			double Rectangle::plastic_modulus_z(void) const
			{
				const double w = m_width;
				const double h = m_height;
				return w * pow(h, 2) / 4;
			}
			double Rectangle::shear_coefficient_y(double v) const
			{
				return 10 * (1 + v) / (12 + 11 * v);
			}
			double Rectangle::shear_coefficient_z(double v) const
			{
				return 10 * (1 + v) / (12 + 11 * v);
			}

			//fibers
			std::vector<points::Fiber> Rectangle::discretize(void) const
			{
				const unsigned nw = m_mesh_w;
				const unsigned nh = m_mesh_h;
				const double zc = m_width / 2;
				const double yc = m_height / 2;
				const double dw = m_width / nw;
				const double dh = m_height / nh;
				std::vector<points::Fiber> fibers(nw * nh);
				for(unsigned i = 0; i < nh; i++)
				{
					for(unsigned j = 0; j < nw; j++)
					{
						fibers[i * nw + j].m_area = dw * dh;
						fibers[i * nw + j].m_position_z = dw * j + dw / 2 - zc;
						fibers[i * nw + j].m_position_y = dh * i + dh / 2 - yc;
					}
				}
				return fibers;
			}
			
			//draw
			void Rectangle::draw(void) const
			{
				//data
				const double w = m_width;
				const double h = m_height;
				const unsigned nw = m_mesh_w;
				const unsigned nh = m_mesh_h;
				//core
				glBegin(GL_QUADS);
				glColor3d(0, 0, 1);
				glVertex2d(-w / 2, -h / 2);
				glVertex2d(+w / 2, -h / 2);
				glVertex2d(+w / 2, +h / 2);
				glVertex2d(-w / 2, +h / 2);
				glEnd();
				//rebars
				draw_rebars();
				//mesh
				glBegin(GL_LINES);
				glColor3d(1, 1, 1);
				for(unsigned i = 0; i <= nw; i++)
				{
					glVertex2d(i * w / nw - w / 2, -h / 2);
					glVertex2d(i * w / nw - w / 2, +h / 2);
				}
				for(unsigned i = 0; i <= nh; i++)
				{
					glVertex2d(-w / 2, i * h / nh - h / 2);
					glVertex2d(+w / 2, i * h / nh - h / 2);
				}
				glEnd();
			}
		}
	}
}
