//std
#include <cmath>
#include <GL/gl.h>

//fea
#include "Mesh/Sections/Rebar.h"

namespace fea
{
	namespace mesh
	{
		namespace sections
		{
			//constructors
			Rebar::Rebar(double d, double z, double y) : m_diameter(d), m_position_z(z), m_position_y(y)
			{
				return;
			}

			//destructors
			Rebar::~Rebar(void)
			{
				return;
			}
			
			//serialization
			void Rebar::load(FILE* file)
			{
				fscanf(file, "%lf %lf %lf", &m_diameter, &m_position_z, &m_position_y);
			}
			void Rebar::save(FILE* file) const
			{
				fprintf(file, "%+.2e %+.2e %+.2e\n", m_diameter, m_position_z, m_position_y);
			}
			
			//draw
			void Rebar::draw(void) const
			{
				//data
				const unsigned n = 50;
				const double z = m_position_z;
				const double y = m_position_y;
				const double r = m_diameter / 2;
				//draw
				glColor3d(0.5, 0.5, 0.5);
				glBegin(GL_TRIANGLE_FAN);
				glVertex2d(z, y);
				for(unsigned i = 0; i <= n; i++)
				{
					glVertex2d(z + r * cos(2 * i * M_PI / n), y + r * sin(2 * i * M_PI / n));
				}
				glEnd();
			}
			
			//data
			double Rebar::diameter(void) const
			{
				return m_diameter;
			}
			double Rebar::diameter(double diameter)
			{
				return m_diameter = diameter;
			}
			double Rebar::position_z(void) const
			{
				return m_position_z;
			}
			double Rebar::position_z(double position_z)
			{
				return m_position_z = position_z;
			}
			double Rebar::position_y(void) const
			{
				return m_position_y;
			}
			double Rebar::position_y(double position_y)
			{
				return m_position_y = position_y;
			}
		}
	}
}
