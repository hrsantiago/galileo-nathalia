//std
#include <cmath>
#include <GL/gl.h>

//fea
#include "Mesh/Mesh.h"
#include "Model/Model.h"
#include "Mesh/Nodes/Node.h"

//gui
#include "Canvas/Bound.h"

namespace gui
{
	namespace canvas
	{
		//constructors
		Bound::Bound(void) : m_x_min(0), m_y_min(0), m_z_min(0), m_x_max(0), m_y_max(0), m_z_max(0)
		{
			return;
		}
	
		//destructor
		Bound::~Bound(void)
		{
			return;
		}
	
		//data
		double Bound::size(void) const
		{
			const double dx = (m_x_max - m_x_min) / 2;
			const double dy = (m_y_max - m_y_min) / 2;
			const double dz = (m_z_max - m_z_min) / 2;
			return fmax(fmax(dx, dy), dz);
		}
		double Bound::x_min(void) const
		{
			return m_x_min;
		}
		double Bound::y_min(void) const
		{
			return m_y_min;
		}
		double Bound::z_min(void) const
		{
			return m_z_min;
		}
		double Bound::x_max(void) const
		{
			return m_x_max;
		}
		double Bound::y_max(void) const
		{
			return m_y_max;
		}
		double Bound::z_max(void) const
		{
			return m_z_max;
		}
		
		//transform
		void Bound::apply(double a) const
		{
			//data
			const double s = a / size();
			const double x = -(m_x_max + m_x_min) / 2;
			const double y = -(m_y_max + m_y_min) / 2;
			const double z = -(m_z_max + m_z_min) / 2;
			//transform
			glScaled(s, s, s);
			glTranslated(x, y, z);
		}
	
		//update
		void Bound::update(const fea::models::Model* model)
		{
			//size
			const unsigned nn = model->mesh()->nodes().size();
			//check
			if(nn == 0)
			{
				m_x_min = m_y_min = m_z_min = -1;
				m_x_max = m_y_max = m_z_max = +1;
				return;
			}
			//loop
			const double* p = model->mesh()->node(0)->coordinates();
			m_x_min = m_x_max = p[0];
			m_y_min = m_y_max = p[1];
			m_z_min = m_z_max = p[2];
			for(unsigned i = 1; i < nn; i++)
			{
				const double* p = model->mesh()->node(i)->coordinates();
				m_x_min = fmin(m_x_min, p[0]);
				m_y_min = fmin(m_y_min, p[1]);
				m_z_min = fmin(m_z_min, p[2]);
				m_x_max = fmax(m_x_max, p[0]);
				m_y_max = fmax(m_y_max, p[1]);
				m_z_max = fmax(m_z_max, p[2]);
			}
			//check
			if(m_x_min == m_x_max && m_y_min == m_y_max && m_z_min == m_z_max)
			{
				m_x_min -= 1;
				m_y_min -= 1;
				m_z_min -= 1;
				m_x_max += 1;
				m_y_max += 1;
				m_z_max += 1;
			}
		}
		void Bound::update(const double*** data, unsigned ns, unsigned nn)
		{
			//check
			if(ns == 0 || nn == 0)
			{
				m_x_min = m_y_min = m_z_min = -1;
				m_x_max = m_y_max = m_z_max = +1;
				return;
			}
			//loop
			m_x_min = m_x_max = data[0][0][0];
			m_y_min = m_y_max = data[0][0][1];
			m_z_min = m_z_max = data[0][0][2];
			for(unsigned i = 0; i < ns; i++)
			{
				for(unsigned j = 0; j < nn; j++)
				{
					m_x_min = fmin(m_x_min, data[i][j][0]);
					m_y_min = fmin(m_y_min, data[i][j][1]);
					m_z_min = fmin(m_z_min, data[i][j][2]);
					m_x_max = fmax(m_x_max, data[i][j][0]);
					m_y_max = fmax(m_y_max, data[i][j][1]);
					m_z_max = fmax(m_z_max, data[i][j][2]);
				}
			}
		}
	}
}
