//std
#include <cmath>
#include <GL/gl.h>

//fea
#include "Mesh/Sections/I.h"
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
			I::I(void) :
				m_web_height(0.1), m_web_thickness(0.01), m_flange_width(0.1), m_flange_thickness(0.01),
				m_mesh_fw(10), m_mesh_ft(2), m_mesh_wh(10), m_mesh_wt(2)
			{
				return;
			}

			//destructor
			I::~I(void)
			{
				return;
			}

			//serialization
			void I::load(FILE* file)
			{
				Section::load(file);
				fscanf(file, "%lf %lf %lf %lf %d %d %d %d",
							&m_flange_width,
							&m_flange_thickness,
							&m_web_height,
							&m_web_thickness,
							&m_mesh_fw,
							&m_mesh_ft,
							&m_mesh_wh,
							&m_mesh_wt);
			}
			void I::save(FILE* file) const
			{
				Section::save(file);
				fprintf(file, "%+4.2e %+4.2e %+4.2e %+4.2e %02d %02d %02d %02d ",
							 m_flange_width,
							 m_flange_thickness,
							 m_web_height,
							 m_web_thickness,
							 m_mesh_fw,
							 m_mesh_ft,
							 m_mesh_wh,
							 m_mesh_wt);
			}

			//types
			sections::type I::type(void) const
			{
				return sections::type::I;
			}

			//data
			double I::web_height(void) const
			{
				return m_web_height;
			}
			double I::web_height(double web_height)
			{
				return m_web_height = web_height;
			}
			double I::web_thickness(void) const
			{
				return m_web_thickness;
			}
			double I::web_thickness(double web_thickness)
			{
				return m_web_thickness = web_thickness;
			}
			double I::flange_width(void) const
			{
				return m_flange_width;
			}
			double I::flange_width(double flange_width)
			{
				return m_flange_width = flange_width;
			}
			double I::flange_thickness(void) const
			{
				return m_flange_thickness;
			}
			double I::flange_thickness(double flange_thickness)
			{
				return m_flange_thickness = flange_thickness;
			}
			
			unsigned I::mesh_fw(void) const
			{
				return m_mesh_fw;
			}
			unsigned I::mesh_fw(unsigned mesh)
			{
				return m_mesh_fw = mesh;
			}
			unsigned I::mesh_ft(void) const
			{
				return m_mesh_ft;
			}
			unsigned I::mesh_ft(unsigned mesh)
			{
				return m_mesh_ft = mesh;
			}
			unsigned I::mesh_wh(void) const
			{
				return m_mesh_wh;
			}
			unsigned I::mesh_wh(unsigned mesh)
			{
				return m_mesh_wh = mesh;
			}
			unsigned I::mesh_wt(void) const
			{
				return m_mesh_wt;
			}
			unsigned I::mesh_wt(unsigned mesh)
			{
				return m_mesh_wt = mesh;
			}

			//geometry
			double I::area(void) const
			{
				//flange
				const double wf = m_flange_width;
				const double tf = m_flange_thickness;
				const double Af = tf*wf;
				//web
				const double hw = m_web_height;
				const double tw = m_web_thickness;
				const double Aw = tw*hw;
				//centroid
				return Af + Aw;
			}
			double I::width(void) const
			{
				return m_flange_width;
			}
			double I::height(void) const
			{
				return m_web_height + 2 * m_flange_thickness;
			}
			double I::inercia_y(void) const
			{
				//web
				const double hw = m_web_height;
				const double tw = m_web_thickness;
				const double Iw = hw * pow(tw, 3) / 12;
				//flange
				const double wf = m_flange_width;
				const double tf = m_flange_thickness;
				const double If = tf * pow(wf, 3) / 12;
				//inercia
				return Iw + 2 * If;
			}
			double I::inercia_z(void) const
			{
				//web
				const double hw = m_web_height;
				const double tw = m_web_thickness;
				const double Aw = tw*hw;
				const double Iw = tw * pow(hw, 3) / 12;
				//flange
				const double wf = m_flange_width;
				const double tf = m_flange_thickness;
				const double Af = tf*wf;
				const double If = wf * pow(tf, 3) / 12 + Af * pow((hw + tf) / 2, 2);
				//inercia
				return Iw + 2 * If;
			}
			double I::inercia_w(void) const
			{
				return 0.0;
			}
			double I::centroid_z(void) const
			{
				return m_flange_width / 2;
			}
			double I::centroid_y(void) const
			{
				return m_web_height / 2 + m_flange_thickness;
			}
			double I::plastic_modulus_y(void) const
			{
				//web
				const double hw = m_web_height;
				const double tw = m_web_thickness;
				const double Aw = hw*tw;
				const double Qw = Aw * tw / 4;
				//flange
				const double wf = m_flange_width;
				const double tf = m_flange_thickness;
				const double Af = wf*tf;
				const double Qf = Af * wf / 4;
				//return
				return Qw + 2 * Qf;
			}
			double I::plastic_modulus_z(void) const
			{
				//web
				const double hw = m_web_height;
				const double tw = m_web_thickness;
				const double Aw = hw*tw;
				const double Qw = Aw * hw / 4;
				//flange
				const double wf = m_flange_width;
				const double tf = m_flange_thickness;
				const double Af = wf*tf;
				const double Qf = Af * (hw + tf) / 2;
				//return
				return Qw + 2 * Qf;
			}
			double I::shear_coefficient_y(double v) const
			{
				//dimensions
				const double hw = m_web_height;
				const double tw = m_web_thickness;
				const double wf = m_flange_width;
				const double tf = m_flange_thickness;
				//parameters
				const double m = 2 * wf * tf / ((hw + tf) * tw);
				const double n = wf / (hw + tf);
				const double p1 = 12 + 72 * m + 150 * pow(m, 2) + 90 * pow(m, 3);
				const double p2 = 11 + 66 * m + 135 * pow(m, 2) + 90 * pow(m, 3);
				const double p3 = 30 * m + 30 * pow(m, 2);
				const double p4 = 40 * m + 45 * pow(m, 2);
				//return
				return 10 * (1 + v) * pow(1 + 3 * m, 2) / (p1 + v * p2 + pow(n, 2) * p3 + v * pow(n, 2) * p4);
			}
			double I::shear_coefficient_z(double v) const
			{
				//dimensions
				const double hw = m_web_height;
				const double tw = m_web_thickness;
				const double wf = m_flange_width;
				const double tf = m_flange_thickness;
				//parameters
				const double m = 2 * wf * tf / ((hw + tf) * tw);
				const double n = wf / (hw + tf);
				const double p1 = 12 + 72 * m + 150 * pow(m, 2) + 90 * pow(m, 3);
				const double p2 = 11 + 66 * m + 135 * pow(m, 2) + 90 * pow(m, 3);
				const double p3 = 30 * m + 30 * pow(m, 2);
				const double p4 = 40 * m + 45 * pow(m, 2);
				//return
				return 10 * (1 + v) * pow(1 + 3 * m, 2) / (p1 + v * p2 + pow(n, 2) * p3 + v * pow(n, 2) * p4);
			}

			//fibers
			std::vector<points::Fiber> I::discretize(void) const
			{
				const unsigned nw = m_mesh_fw;
				const unsigned na = m_mesh_ft;
				const unsigned nh = m_mesh_wh;
				const unsigned nb = m_mesh_wt;
				const double wf = m_flange_width;
				const double tf = m_flange_thickness;
				const double hw = m_web_height;
				const double tw = m_web_thickness;
				const double dw = wf / nw;
				const double da = tf / na;
				const double dh = hw / nh;
				const double db = tw / nb;
				const double yc = tf + hw / 2;
				std::vector<points::Fiber> fibers(2 * nw * na + nh * nb);
				for(unsigned i = 0; i < nw; i++)
				{
					for(unsigned j = 0; j < na; j++)
					{
						fibers[i * na + j].m_area = dw * da;
						fibers[i * na + j].m_position_z = dw * i + dw / 2 - wf / 2;
						fibers[i * na + j].m_position_y = da * j + da / 2 - yc;
						fibers[i * na + j + nw * na + nh * nb].m_area = dw * da;
						fibers[i * na + j + nw * na + nh * nb].m_position_z = dw * i + dw / 2 - wf / 2;
						fibers[i * na + j + nw * na + nh * nb].m_position_y = da * j + da / 2 - yc + tf + hw;
					}
				}
				for(unsigned i = 0; i < nh; i++)
				{
					for(unsigned j = 0; j < nb; j++)
					{
						fibers[i * nb + j + nw * na].m_area = dh * db;
						fibers[i * nb + j + nw * na].m_position_z = db * j + db / 2 - tw / 2;
						fibers[i * nb + j + nw * na].m_position_y = dh * i + dh / 2 - yc + tf;
					}
				}
				return fibers;
			}
			
			//draw
			void I::draw(void) const
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
				glVertex2d(-wt / 2, -wh / 2);
				glVertex2d(+wt / 2, -wh / 2);
				glVertex2d(+wt / 2, +wh / 2);
				glVertex2d(-wt / 2, +wh / 2);
				glVertex2d(+fw / 2, -wh / 2);
				glVertex2d(-fw / 2, -wh / 2);
				glVertex2d(-fw / 2, -wh / 2 - ft);
				glVertex2d(+fw / 2, -wh / 2 - ft);
				glVertex2d(-fw / 2, +wh / 2);
				glVertex2d(+fw / 2, +wh / 2);
				glVertex2d(+fw / 2, +wh / 2 + ft);
				glVertex2d(-fw / 2, +wh / 2 + ft);
				glEnd();
				//rebars
				draw_rebars();
				//mesh
				glBegin(GL_LINES);
				glColor3d(1, 1, 1);
				for(unsigned i = 0; i <= nw; i++)
				{
					glVertex2d(-fw / 2 + i * fw / nw, -wh / 2);
					glVertex2d(-fw / 2 + i * fw / nw, -wh / 2 - ft);
					glVertex2d(-fw / 2 + i * fw / nw, +wh / 2);
					glVertex2d(-fw / 2 + i * fw / nw, +wh / 2 + ft);
				}
				for(unsigned i = 0; i <= na; i++)
				{
					glVertex2d(-fw / 2, -wh / 2 - i * ft / na);
					glVertex2d(+fw / 2, -wh / 2 - i * ft / na);
					glVertex2d(-fw / 2, +wh / 2 + i * ft / na);
					glVertex2d(+fw / 2, +wh / 2 + i * ft / na);
				}
				for(unsigned i = 0; i <= nh; i++)
				{
					glVertex2d(-wt / 2, -wh / 2 + i * wh / nh);
					glVertex2d(+wt / 2, -wh / 2 + i * wh / nh);
				}
				for(unsigned i = 0; i <= nb; i++)
				{
					glVertex2d(-wt / 2 + i * wt / nb, -wh / 2);
					glVertex2d(-wt / 2 + i * wt / nb, +wh / 2);
				}
				glEnd();
			}
		}
	}
}
