//std
#include <cmath>
#include <GL/gl.h>

//fea
#include "Mesh/Sections/T.h"
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
			T::T(void) :
				m_flange_width(0.1), m_flange_thickness(0.01), m_web_height(0.1), m_web_thickness(0.01),
				m_mesh_wh(10), m_mesh_fw(10), m_mesh_wt(2), m_mesh_ft(2)
			{
				return;
			}

			//destructor
			T::~T(void)
			{
				return;
			}

			//serialization
			void T::load(FILE* file)
			{
				Section::load(file);
				fscanf(file, "%lf %lf %lf %lf %d %d %d %d",
					&m_web_height,
					&m_web_thickness,
					&m_flange_width,
					&m_flange_thickness,
					&m_mesh_fw,
					&m_mesh_ft,
					&m_mesh_wh,
					&m_mesh_wt
				);
			}
			void T::save(FILE* file) const
			{
				Section::save(file);
				fprintf(file, "%+.2e %+.2e %+.2e %+.2e %02d %02d %02d %02d ",
					m_web_height,
					m_web_thickness,
					m_flange_width,
					m_flange_thickness,
					m_mesh_fw,
					m_mesh_ft,
					m_mesh_wh,
					m_mesh_wt
				);
			}

			//type
			sections::type T::type(void) const
			{
				return sections::type::T;
			}

			//data
			
			double T::web_height(void) const
			{
				return m_web_height;
			}
			double T::web_height(double web_height)
			{
				return m_web_height = web_height;
			}
			double T::web_thickness(void) const
			{
				return m_web_thickness;
			}
			double T::web_thickness(double web_thickness)
			{
				return m_web_thickness = web_thickness;
			}
			double T::flange_width(void) const
			{
				return m_flange_width;
			}
			double T::flange_width(double flange_width)
			{
				return m_flange_width = flange_width;
			}
			double T::flange_thickness(void) const
			{
				return m_flange_thickness;
			}
			double T::flange_thickness(double flange_thickness)
			{
				return m_flange_thickness = flange_thickness;
			}
			
			unsigned T::mesh_fw(void) const
			{
				return m_mesh_fw;
			}
			unsigned T::mesh_fw(unsigned mesh_fw)
			{
				return m_mesh_fw = mesh_fw;
			}
			unsigned T::mesh_ft(void) const
			{
				return m_mesh_ft;
			}
			unsigned T::mesh_ft(unsigned mesh_ft)
			{
				return m_mesh_ft = mesh_ft;
			}
			unsigned T::mesh_wh(void) const
			{
				return m_mesh_wh;
			}
			unsigned T::mesh_wh(unsigned mesh_wh)
			{
				return m_mesh_wh = mesh_wh;
			}
			unsigned T::mesh_wt(void) const
			{
				return m_mesh_wt;
			}
			unsigned T::mesh_wt(unsigned mesh_wt)
			{
				return m_mesh_wt = mesh_wt;
			}

			//geometry
			double T::area(void) const
			{
				//data
				const double wh = m_web_height;
				const double fw = m_flange_width;
				const double wt = m_web_thickness;
				const double ft = m_flange_thickness;
				//area
				return wt * wh + ft * fw;
			}
			double T::width(void) const
			{
				return m_flange_width;
			}
			double T::height(void) const
			{
				return m_flange_thickness + m_web_height;
			}
			double T::inertia_y(void) const
			{
				//web
				const double wh = m_web_height;
				const double wt = m_web_thickness;
				const double Iw = wh * pow(wt, 3) / 12;
				//flange
				const double fw = m_flange_width;
				const double ft = m_flange_thickness;
				const double If = ft * pow(fw, 3) / 12;
				// inertia
				return Iw + If;
			}
			double T::inertia_z(void) const
			{
				//data
				const double wh = m_web_height;
				const double fw = m_flange_width;
				const double wt = m_web_thickness;
				const double ft = m_flange_thickness;
				//web
				const double yw = wh / 2;
				const double Aw = wt * wh;
				const double Qw = Aw * yw;
				//flange
				const double yf = wh + ft / 2;
				const double Af = ft * fw;
				const double Qf = Af * yf;
				//centroid
				const double yc = (Qw + Qf) / (Aw + Af);
				//inertia
				const double Iw = wt * pow(wh, 3) / 12 + Aw * pow(yw - yc, 2);
				const double If = fw * pow(ft, 3) / 12 + Af * pow(yf - yc, 2);
				return Iw + If;
			}
			double T::inertia_w(void) const
			{
				return 0;
			}
			double T::centroid_z(void) const
			{
				return m_flange_width / 2;
			}
			double T::centroid_y(void) const
			{
				//data
				const double wh = m_web_height;
				const double fw = m_flange_width;
				const double wt = m_web_thickness;
				const double ft = m_flange_thickness;
				//web
				const double wy = wh / 2;
				const double wA = wt * wh;
				const double wQ = wA * wy;
				//flange
				const double fy = wh + ft / 2;
				const double fA = ft * fw;
				const double fQ = fA * fy;
				// centroid
				return (wQ + fQ) / (wA + fA);
			}
			double T::plastic_modulus_y(void) const
			{
				//data
				const double wh = m_web_height;
				const double fw = m_flange_width;
				const double wt = m_web_thickness;
				const double ft = m_flange_thickness;
				//web
				const double Aw = wh * wt;
				const double Qw = Aw * wt / 4;
				//top flange
				const double Af = fw * ft;
				const double Qf = Af * fw / 4;
				//return
				return Qw + Qf;
			}
			double T::plastic_modulus_z(void) const
			{
				//centroid
				const double yc = centroid_y();
				//web
				const double wt = m_web_thickness;
				//return
				return wt * pow(yc, 2);

			}
			double T::shear_coefficient_y(double v) const
			{
				//data
				const double wh = m_web_height;
				const double fw = m_flange_width;
				const double wt = m_web_thickness;
				const double ft = m_flange_thickness;
				//shear coefficient
				const double n = fw / wh;
				const double m = fw * ft / (wh * wt);
				const double d3 = 30 * m + 30 * m * m;
				const double d4 = 40 * m + 50 * m * m + 10 * m * m * m;
				const double d1 = 12 + 96 * m + 278 * m * m + 192 * m * m * m;
				const double d2 = 11 + 88 * m + 248 * m * m + 216 * m * m * m;
				return 10 * (1 + v) * pow(1 + 4 * m, 2) / (d1 + v * d2 + n * n * d3 + v * n * n * d4);
			}
			double T::shear_coefficient_z(double v) const
			{
				//data
				const double wh = m_web_height;
				const double fw = m_flange_width;
				const double wt = m_web_thickness;
				const double ft = m_flange_thickness;
				//shear coefficient
				const double n = fw / wh;
				const double m = fw * ft / (wh * wt);
				const double d3 = 30 * m + 30 * m * m;
				const double d4 = 40 * m + 50 * m * m + 10 * m * m * m;
				const double d1 = 12 + 96 * m + 278 * m * m + 192 * m * m * m;
				const double d2 = 11 + 88 * m + 248 * m * m + 216 * m * m * m;
				return 10 * (1 + v) * pow(1 + 4 * m, 2) / (d1 + v * d2 + n * n * d3 + v * n * n * d4);
			}

			//fibers
			std::vector<points::Fiber> T::discretize(void) const
			{
				//data
				const unsigned nw = m_mesh_fw;
				const unsigned na = m_mesh_ft;
				const unsigned nh = m_mesh_wh;
				const unsigned nb = m_mesh_wt;
				const double wh = m_web_height;
				const double fw = m_flange_width;
				const double wt = m_web_thickness;
				const double ft = m_flange_thickness;
				//fibers
				const double dw = fw / nw;
				const double da = ft / na;
				const double dh = wh / nh;
				const double db = wt / nb;
				const double yc = centroid_y();
				std::vector<points::Fiber> fibers(nw * na + nh * nb);
				for(unsigned i = 0; i < nh; i++)
				{
					for(unsigned j = 0; j < nb; j++)
					{
						fibers[i * nb + j].m_area = dh * db;
						fibers[i * nb + j].m_position_z = db * j + db / 2 - wt / 2;
						fibers[i * nb + j].m_position_y = dh * i + dh / 2 - yc;
					}
				}
				for(unsigned i = 0; i < nw; i++)
				{
					for(unsigned j = 0; j < na; j++)
					{
						fibers[i * na + j + nh * nb].m_area = dw * da;
						fibers[i * na + j + nh * nb].m_position_z = dw * i + dw / 2 - fw / 2;
						fibers[i * na + j + nh * nb].m_position_y = da * j + da / 2 - yc + wh;
					}
				}
				return fibers;
			}
			
			//draw
			void T::draw(void) const
			{
				//data
				const unsigned nw = m_mesh_fw;
				const unsigned na = m_mesh_ft;
				const unsigned nh = m_mesh_wh;
				const unsigned nb = m_mesh_wt;
				const double wh = m_web_height;
				const double fw = m_flange_width;
				const double wt = m_web_thickness;
				const double ft = m_flange_thickness;
				//core
				glBegin(GL_QUADS);
				glColor3d(0, 0, 1);
				glVertex2d(-fw / 2, (wh - ft) / 2);
				glVertex2d(+fw / 2, (wh - ft) / 2);
				glVertex2d(+fw / 2, (wh + ft) / 2);
				glVertex2d(-fw / 2, (wh + ft) / 2);
				glVertex2d(-wt / 2, -(wh + ft) / 2);
				glVertex2d(+wt / 2, -(wh + ft) / 2);
				glVertex2d(+wt / 2, +(wh - ft) / 2);
				glVertex2d(-wt / 2, +(wh - ft) / 2);
				glEnd();
				//rebars
				draw_rebars();
				//mesh
				glBegin(GL_LINES);
				glColor3d(1, 1, 1);
				for(unsigned i = 0; i <= nw; i++)
				{
					glVertex2d(-fw / 2 + i * fw / nw, (wh - ft) / 2);
					glVertex2d(-fw / 2 + i * fw / nw, (wh + ft) / 2);
				}
				for(unsigned i = 0; i <= na; i++)
				{
					glVertex2d(-fw / 2, (wh - ft) / 2 + i * ft / na);
					glVertex2d(+fw / 2, (wh - ft) / 2 + i * ft / na);
				}
				for(unsigned i = 0; i <= nh; i++)
				{
					glVertex2d(-wt / 2, -(wh + ft) / 2 + i * wh / nh);
					glVertex2d(+wt / 2, -(wh + ft) / 2 + i * wh / nh);
				}
				for(unsigned i = 0; i <= nb; i++)
				{
					glVertex2d(-wt / 2 + i * wt / nb, -(wh + ft) / 2);
					glVertex2d(-wt / 2 + i * wt / nb, +(wh - ft) / 2);
				}
				glEnd();
			}
		}
	}
}
