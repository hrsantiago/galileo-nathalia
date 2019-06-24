//std
#include <cmath>
#include <algorithm>

//gl
#include <GL/gl.h>
#include <GL/glu.h>

//mat
#include "linear/dense.h"
#include "linear/linear.h"

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
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
			reset();
		}
	
		//destructor
		Bound::~Bound(void)
		{
			return;
		}
	
		//data
		double Bound::size(void) const
		{
			const double pi[] = {m_x_min, m_y_min, m_z_min};
			const double pj[] = {m_x_max, m_y_max, m_z_max};
			return mat::norm(pj, pi, 3);
		}
	
		double Bound::zoom(void) const
		{
			return m_zoom;
		}
		double Bound::zoom(double zoom, bool increment)
		{
			if(!increment)
			{
				return m_zoom = zoom;
			}
			else
			{
				return m_zoom *= zoom;
			}
		}
		const double* Bound::pan(void) const
		{
			return m_pan;
		}
		const double* Bound::pan(const double* pan, bool increment)
		{
			if(!increment)
			{
				return mat::set(m_pan, pan, 3);
			}
			else
			{
				return mat::add(m_pan, pan, 3);
			}
		}
		const double* Bound::rotation(void) const
		{
			return m_rotation;
		}
		const double* Bound::rotation(const double* rotation, bool increment)
		{
			double r0[16];
			if(!increment)
			{
				return mat::set(m_rotation, rotation, 16);
			}
			else
			{
				return mat::multiply(m_rotation, mat::set(r0, m_rotation, 16), rotation, 4, 4, 4);
			}
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
	
		//center
		double* Bound::center(double* c) const
		{
			c[0] = (m_x_max + m_x_min) / 2;
			c[1] = (m_y_max + m_y_min) / 2;
			c[2] = (m_z_max + m_z_min) / 2;
			return c;
		}
	
		//limits
		void Bound::apply(void)
		{
			//size
			const double s = size();
			//pan
			const double xp = m_pan[0];
			const double yp = m_pan[1];
			const double zp = m_pan[2];
			//center
			const double xc = (m_x_max + m_x_min) / 2;
			const double yc = (m_y_max + m_y_min) / 2;
			const double zc = (m_z_max + m_z_min) / 2;
			//set modelview matrix
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glTranslated(xc, yc, zc);
			glMultMatrixd(m_rotation);
			glTranslated(xp - xc, yp - yc, zp - zc);
			//set projection matrix
			glMatrixMode(GL_PROJECTION);
			const double dx = s / m_zoom;
			const double dy = s / m_zoom;
			const double dz = s / std::min((double) 1, m_zoom);
			double p[] = {2 / dx, 0, 0, 0, 0, 2 / dy, 0, 0, 0, 0, 2 / dz, 0, -2 * xc / dx, -2 * yc / dy, -2 * zc / dz, 1};
			glLoadIdentity();
			glMultMatrixd(p);
		}
		void Bound::reset(void)
		{
			m_zoom = 0.95;
			mat::clean(m_pan, 3);
			mat::eye(m_rotation, 4);
		}
	
		void Bound::update(fea::models::Model* model)
		{
			//size
			const unsigned nn = model->mesh()->nodes();
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
				m_x_min = std::min(m_x_min, p[0]);
				m_y_min = std::min(m_y_min, p[1]);
				m_z_min = std::min(m_z_min, p[2]);
				m_x_max = std::max(m_x_max, p[0]);
				m_y_max = std::max(m_y_max, p[1]);
				m_z_max = std::max(m_z_max, p[2]);
			}
		}
		void Bound::update(const double** data, unsigned nd)
		{
			//check
			if(nd == 0)
			{
				m_x_min = m_y_min = m_z_min = -1;
				m_x_max = m_y_max = m_z_max = +1;
				return;
			}
			//loop
			m_x_min = m_x_max = data[0][0];
			m_y_min = m_y_max = data[0][1];
			m_z_min = m_z_max = data[0][2];
			for(unsigned i = 1; i < nd; i++)
			{
				m_x_min = std::min(m_x_min, data[i][0]);
				m_y_min = std::min(m_y_min, data[i][1]);
				m_z_min = std::min(m_z_min, data[i][2]);
				m_x_max = std::max(m_x_max, data[i][0]);
				m_y_max = std::max(m_y_max, data[i][1]);
				m_z_max = std::max(m_z_max, data[i][2]);
			}
		}
		void Bound::update(const double*** data, unsigned ns, unsigned nd)
		{
			//check
			if(ns == 0 || nd == 0)
			{
				m_x_min = m_y_min = m_z_min = -1;
				m_x_max = m_y_max = m_z_max = +1;
				return;
			}
			//loop
			m_x_min = m_x_max = data[0][0][0];
			m_y_min = m_y_max = data[0][0][1];
			m_z_min = m_z_max = data[0][0][2];
			for(unsigned k = 0; k < ns; k++)
			{
				for(unsigned i = 0; i < nd; i++)
				{
					m_x_min = std::min(m_x_min, data[k][i][0]);
					m_y_min = std::min(m_y_min, data[k][i][1]);
					m_z_min = std::min(m_z_min, data[k][i][2]);
					m_x_max = std::max(m_x_max, data[k][i][0]);
					m_y_max = std::max(m_y_max, data[k][i][1]);
					m_z_max = std::max(m_z_max, data[k][i][2]);
				}
			}
		}
	
		//rotation
		double* Bound::rotation(double* r, const double* a, double t)
		{
			const double ax = a[0];
			const double ay = a[1];
			const double az = a[2];
			const double ct = cos(t);
			const double st = sin(t);
			r[0] = (1 - ct) * ax * ax + ct;			r[4] = (1 - ct) * ax * ay - st * az;	r[ 8] = (1 - ct) * ax * az + st * ay;	r[12] = 0;
			r[1] = (1 - ct) * ax * ay + st * az;	r[5] = (1 - ct) * ay * ay + ct;			r[ 9] = (1 - ct) * ay * az - st * ax;	r[13] = 0;
			r[2] = (1 - ct) * ax * az - st * ay;	r[6] = (1 - ct) * ay * az + st * ax;	r[10] = (1 - ct) * az * az + ct;		r[14] = 0;
			r[3] = 0;								r[7] = 0;								r[11] = 0;								r[15] = 1;
			return r;
		}
	}
}
