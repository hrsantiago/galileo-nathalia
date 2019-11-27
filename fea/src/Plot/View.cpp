//std
#include <cstring>

//mat
#include "linear/dense.h"

//fea
#include "Plot/View.h"

namespace fea
{
	namespace plot
	{
		//constructors
		View::View(void)
		{
			reset();
		}
		
		//destructor
		View::~View(void)
		{
			return;
		}
		
		//serialization
		void View::load(FILE* file)
		{
			fscanf(file, "%lf", &m_zoom);
			for(unsigned i = 0; i < 3; i++)
			{
				fscanf(file, "%lf", &m_pan[i]);
			}
			for(unsigned i = 0; i < 16; i++)
			{
				fscanf(file, "%lf", &m_rotation[i]);
			}
		}
		void View::save(FILE* file) const
		{
			fprintf(file, "%++.2f\n", m_zoom);
			for(unsigned i = 0; i < 3; i++)
			{
				fprintf(file, "%+.2f ", m_pan[i]);
			}
			fprintf(file, "\n");
			for(unsigned i = 0; i < 4; i++)
			{
				for(unsigned j = 0; j < 4; j++)
				{
					fprintf(file, "%+.2f ", m_rotation[i + 4 * j]);
				}
				fprintf(file, "\n");
			}
		}
		
		//data
		double View::zoom(void) const
		{
			return m_zoom;
		}
		double View::zoom(double zoom, bool increment)
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
			
		const double* View::pan(void) const
		{
			return m_pan;
		}
		const double* View::pan(const double* pan, bool increment)
		{
			if(!increment)
			{
				return mat::copy(m_pan, pan, 3);
			}
			else
			{
				return mat::add(m_pan, pan, 3);
			}
		}
		
		const double* View::rotation(void) const
		{
			return m_rotation;
		}
		const double* View::rotation(const double* rotation, bool increment)
		{
			double r0[16];
			if(!increment)
			{
				return mat::copy(m_rotation, rotation, 16);
			}
			else
			{
				return mat::multiply(m_rotation, mat::copy(r0, m_rotation, 16), rotation, 4, 4, 4);
			}
		}
		
		//view
		void View::view_x(void)
		{
			double r[16];
			const double e1[] = {1, 0, 0};
			const double e3[] = {0, 0, 1};
			rotation(View::rotation(r, e1, -M_PI / 2));
			rotation(View::rotation(r, e3, -M_PI / 2), true);
		}
		void View::view_y(void)
		{
			double r[16];
			const double e1[] = {1, 0, 0};
			rotation(View::rotation(r, e1, -M_PI / 2));
		}
		void View::view_z(void)
		{
			double r[16];
			const double e1[] = {1, 0, 0};
			rotation(View::rotation(r, e1, 0));
		}
		void View::view_i(void)
		{
			double r[16];
			const double ti = 35+.264;
			const double e1[] = {1, 0, 0};
			const double e2[] = {0, 1, 0};
			const double e3[] = {0, 0, 1};
			rotation(View::rotation(r, e1, +ti * M_PI / 180));
			rotation(View::rotation(r, e2, -45 * M_PI / 180), true);
			rotation(View::rotation(r, e1, -90 * M_PI / 180), true);
			rotation(View::rotation(r, e3, -90 * M_PI / 180), true);
		}
		
		//rotate
		void View::rotate_x(bool increment)
		{
			double r[16];
			const double a[] = {1, 0, 0};
			const double t = 5 * M_PI / 180;
			rotation(View::rotation(r, a, increment ? +t : -t), true);
		}
		void View::rotate_y(bool increment)
		{
			double r[16];
			const double a[] = {0, 1, 0};
			const double t = 5 * M_PI / 180;
			rotation(View::rotation(r, a, increment ? +t : -t), true);
		}
		void View::rotate_z(bool increment)
		{
			double r[16];
			const double a[] = {0, 0, 1};
			const double t = 5 * M_PI / 180;
			rotation(View::rotation(r, a, increment ? +t : -t), true);
		}
		
		//rotation
		double* View::rotation(double* r, const double* a, double t)
		{
			r[ 3] = 0;
			r[ 7] = 0;
			r[11] = 0;
			r[12] = 0;
			r[13] = 0;
			r[14] = 0;
			r[15] = 1;
			const double ax = a[0];
			const double ay = a[1];
			const double az = a[2];
			const double ct = cos(t);
			const double st = sin(t);
			r[ 0] = (1 - ct) * ax * ax + ct;	
			r[ 5] = (1 - ct) * ay * ay + ct;
			r[10] = (1 - ct) * az * az + ct;
			r[ 4] = (1 - ct) * ax * ay - st * az;
			r[ 8] = (1 - ct) * ax * az + st * ay;
			r[ 1] = (1 - ct) * ax * ay + st * az;
			r[ 9] = (1 - ct) * ay * az - st * ax;
			r[ 2] = (1 - ct) * ax * az - st * ay;
			r[ 6] = (1 - ct) * ay * az + st * ax;
			return r;
		}
		
		//info
		void View::info(void) const
		{
			printf("View\n");
			printf("\tzoom:\t\t%++.2f\n", m_zoom);
			printf("\tpan:\t\t%+.2f %+.2f %+.2f\n", m_pan[0], m_pan[1], m_pan[2]);
			printf("\trotation:");
			printf("\t%+.2f %+.2f %+.2f %+.2f\n", m_rotation[ 0], m_rotation[ 1], m_rotation[ 2], m_rotation[ 3]);
			printf("\t\t\t%+.2f %+.2f %+.2f %+.2f\n", m_rotation[ 4], m_rotation[ 5], m_rotation[ 6], m_rotation[ 7]);
			printf("\t\t\t%+.2f %+.2f %+.2f %+.2f\n", m_rotation[ 8], m_rotation[ 9], m_rotation[10], m_rotation[11]);
			printf("\t\t\t%+.2f %+.2f %+.2f %+.2f\n", m_rotation[12], m_rotation[13], m_rotation[14], m_rotation[15]);
		}
		
		//config
		void View::reset(void)
		{
			m_zoom = 0.95;
			mat::clean(m_pan, 3);
			mat::eye(m_rotation, 4);
		}
	}
}
