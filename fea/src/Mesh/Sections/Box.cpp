//std
#include <cmath>
#include <GL/gl.h>

//fea
#include "Mesh/Sections/Box.h"
#include "Mesh/Sections/Types.h"
#include "Mesh/Points/Mechanic/Fiber.h"
#include "Mesh/Points/Mechanic/Mechanic.h"

namespace fea
{
	namespace mesh
	{
		namespace sections
		{
			//constructors
			Box::Box(void) : m_width(0.2), m_height(0.6), m_thickness(0.01), m_mesh_w(20), m_mesh_h(20), m_mesh_t(2)
			{
				return;
			}

			//destructor
			Box::~Box(void)
			{
				return;
			}

			//serialization
			void Box::load(FILE* file)
			{
				Section::load(file);
				fscanf(file, "%lf %lf %lf %d %d %d", &m_width, &m_thickness, &m_height, &m_mesh_w, &m_mesh_h, &m_mesh_t);
			}
			void Box::save(FILE* file) const
			{
				Section::save(file);
				fprintf(file, "%+.6e %+.6e %+.6e %02d %02d %02d ", m_width, m_height, m_thickness, m_mesh_w, m_mesh_h, m_mesh_t);
			}

			//type
			sections::type Box::type(void) const
			{
				return sections::type::box;
			}

			//data
			double Box::width(double width)
			{
				return m_width = width;
			}
			double Box::height(double height)
			{
				return m_height = height;
			}
			double Box::thickness(void) const
			{
				return m_thickness;
			}
			double Box::thickness(double width_thickness)
			{
				return m_thickness = width_thickness;
			}
			
			unsigned Box::mesh_w(void) const
			{
				return m_mesh_w;
			}
			unsigned Box::mesh_w(unsigned mesh)
			{
				return m_mesh_w = mesh;
			}
			unsigned Box::mesh_h(void) const
			{
				return m_mesh_h;
			}
			unsigned Box::mesh_h(unsigned mesh)
			{
				return m_mesh_h = mesh;
			}
			unsigned Box::mesh_t(void) const
			{
				return m_mesh_t;
			}
			unsigned Box::mesh_t(unsigned mesh)
			{
				return m_mesh_t = mesh;
			}

			//geometry
			double Box::area(void) const
			{
				const double w = m_width;
				const double h = m_height;
				const double t = m_thickness;
				return 2 * (w + h) * t - 4 * t * t;
			}
			double Box::width(void) const
			{
				return m_width;
			}
			double Box::height(void) const
			{
				return m_height;
			}
			double Box::inercia_y(void) const
			{
				const double w = m_width;
				const double h = m_height;
				const double t = m_thickness;
				return t * pow(w, 3) / 6 + (h - 2 * t) * (pow(t, 3) / 6 + t * pow(w - t, 2) / 2);
			}
			double Box::inercia_z(void) const
			{
				const double w = m_width;
				const double h = m_height;
				const double t = m_thickness;
				return t * pow(h, 3) / 6 + (w - 2 * t) * (pow(t, 3) / 6 + t * pow(h - t, 2) / 2);
			}
			double Box::inercia_w(void) const
			{
				const double w = m_width;
				const double h = m_height;
				const double t = m_thickness;
				const double Ix = 2 * pow(w - t, 2) * pow(h - t, 2) * t / (w + h - 2 * t);
				const double Iy = t * pow(w, 3) / 6 + (h - 2 * t) * (pow(t, 3) / 6 + t * pow(w - t, 2) / 2);
				const double Iz = t * pow(h, 3) / 6 + (w - 2 * t) * (pow(t, 3) / 6 + t * pow(h - t, 2) / 2);
				return Iy + Iz - Ix;
			}
			double Box::centroid_z(void) const
			{
				return m_width / 2;
			}
			double Box::centroid_y(void) const
			{
				return m_height / 2;
			}
			double Box::plastic_modulus_y(void) const
			{
				const double w = m_width;
				const double h = m_height;
				const double t = m_thickness;
				return w * t * w / 2 + t * (h - 2 * t)*(w - t);
			}
			double Box::plastic_modulus_z(void) const
			{
				const double w = m_width;
				const double h = m_height;
				const double t = m_thickness;
				return t * pow(h, 2) / 2 + t * (w - 2 * t)*(h - t);
			}
			double Box::shear_coefficient_y(double v) const
			{
				const double w = m_width;
				const double h = m_height;
				const double n = w / h;
				const double d1 = 12 + 72 * n + 150 * pow(n, 2) + 90 * pow(n, 3);
				const double d2 = 11 + 66 * n + 135 * pow(n, 2) + 90 * pow(n, 3);
				const double d3 = 10 * (3 + v) * n + 30 * pow(n, 2);
				return 10 * (1 + v) * pow(1 + 3 * n, 2) / (d1 + v * d2 + pow(n, 2) * d3);
			}
			double Box::shear_coefficient_z(double v) const
			{
				const double w = m_width;
				const double h = m_height;
				const double n = w / h;
				const double d1 = 12 + 72 * n + 150 * pow(n, 2) + 90 * pow(n, 3);
				const double d2 = 11 + 66 * n + 135 * pow(n, 2) + 90 * pow(n, 3);
				const double d3 = 10 * (3 + v) * n + 30 * pow(n, 2);
				return 10 * (1 + v) * pow(1 + 3 * n, 2) / (d1 + v * d2 + pow(n, 2) * d3);
			}

			//fibers
			std::vector<points::Fiber> Box::discretize(void) const
			{
				//mesh
				const unsigned nw = m_mesh_w;
				const unsigned nh = m_mesh_h;
				const unsigned nt = m_mesh_t;
				//geometry
				const double w = m_width;
				const double h = m_height;
				const double t = m_thickness;
				//data
				const double zc = w / 2;
				const double yc = h / 2;
				const double dw = w / nw;
				const double dt = t / nt;
				const double dh = (h - 2 * t) / nh;
				std::vector<points::Fiber> fibers(2 * nt * (nw + nh));
				//fibers
				for(unsigned i = 0; i < nw; i++)
				{
					for(unsigned j = 0; j < nt; j++)
					{
						fibers[i * nt + j].m_area = dw * dt;
						fibers[i * nt + j].m_position_y = dt * j + dt / 2 - yc;
						fibers[i * nt + j].m_position_z = dw * i + dw / 2 - zc;
						fibers[i * nt + j + nw * nt + 2 * nh * nt].m_area = dw * dt;
						fibers[i * nt + j + nw * nt + 2 * nh * nt].m_position_z = dw * i + dw / 2 - zc;
						fibers[i * nt + j + nw * nt + 2 * nh * nt].m_position_y = dt * j + dt / 2 - yc + h - t;
					}
				}
				for(unsigned i = 0; i < nh; i++)
				{
					for(unsigned j = 0; j < nt; j++)
					{
						fibers[i * nt + j + nw * nt].m_area = dh * dt;
						fibers[i * nt + j + nw * nt].m_position_z = dt * j + dt / 2 - zc;
						fibers[i * nt + j + nw * nt].m_position_y = dh * i + dh / 2 - yc + t;
						fibers[i * nt + j + nw * nt + nh * nt].m_area = dh * dt;
						fibers[i * nt + j + nw * nt + nh * nt].m_position_z = dt * j + dt / 2 - zc + w - t;
						fibers[i * nt + j + nw * nt + nh * nt].m_position_y = dh * i + dh / 2 - yc + t;
					}
				}
				return fibers;
			}
			
			//draw
			void Box::draw(void) const
			{
				//data
				const double w = m_width;
				const double h = m_height;
				const double t = m_thickness;
				const unsigned nw = m_mesh_w;
				const unsigned nh = m_mesh_h;
				const unsigned nt = m_mesh_t;
				//core
				glBegin(GL_QUADS);
				glColor3d(0, 0, 1);
				glVertex2d(-w / 2, -h / 2);
				glVertex2d(+w / 2, -h / 2);
				glVertex2d(+w / 2, -h / 2 + t);
				glVertex2d(-w / 2, -h / 2 + t);
				glVertex2d(-w / 2, +h / 2);
				glVertex2d(+w / 2, +h / 2);
				glVertex2d(+w / 2, +h / 2 - t);
				glVertex2d(-w / 2, +h / 2 - t);
				glVertex2d(-w / 2, +h / 2);
				glVertex2d(-w / 2, -h / 2);
				glVertex2d(-w / 2 + t, -h / 2);
				glVertex2d(-w / 2 + t, +h / 2);
				glVertex2d(+w / 2, +h / 2);
				glVertex2d(+w / 2, -h / 2);
				glVertex2d(+w / 2 - t, -h / 2);
				glVertex2d(+w / 2 - t, +h / 2);
				glEnd();
				//rebars
				draw_rebars();
				//mesh
				glBegin(GL_LINES);
				glColor3d(1, 1, 1);
				
				for(unsigned i = 0; i <= nt; i++)
				{
					glVertex2d(-w / 2, i * t / nt - h / 2);
					glVertex2d(+w / 2, i * t / nt - h / 2);
				}
				for(unsigned i = 0; i <= nt; i++)
				{
					glVertex2d(-w / 2, h / 2 - i * t / nt);
					glVertex2d(+w / 2, h / 2 - i * t / nt);
				}
				for(unsigned i = 0; i <= nw; i++)
				{
					glVertex2d(i * w / nw - w / 2, +h / 2);
					glVertex2d(i * w / nw - w / 2, +h / 2 - t);
				}
				for(unsigned i = 0; i <= nw; i++)
				{
					glVertex2d(i * w / nw - w / 2, -h / 2);
					glVertex2d(i * w / nw - w / 2, -h / 2 + t);
				}
				for(unsigned i = 0; i <= nt; i++)
				{
					glVertex2d(-w / 2 + i * t / nt, -h / 2 + t);
					glVertex2d(-w / 2 + i * t / nt, +h / 2 - t);
				}
				for(unsigned i = 0; i <= nt; i++)
				{
					glVertex2d(+w / 2 - i * t / nt, -h / 2 + t);
					glVertex2d(+w / 2 - i * t / nt, +h / 2 - t);
				}
				for(unsigned i = 0; i <= nh; i++)
				{
					glVertex2d(-w / 2, -h / 2 + t + i * (h - 2 * t) / nh);
					glVertex2d(-w / 2 + t, -h / 2 + t + i * (h - 2 * t) / nh);
				}
				for(unsigned i = 0; i <= nh; i++)
				{
					glVertex2d(+w / 2, -h / 2 + t + i * (h - 2 * t) / nh);
					glVertex2d(+w / 2 - t, -h / 2 + t + i * (h - 2 * t) / nh);
				}
				glEnd();
			}
		}
	}
}
