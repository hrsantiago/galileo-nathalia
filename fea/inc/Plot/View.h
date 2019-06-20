#pragma once

#include <cmath>
#include <cstdio>

namespace fea
{
	namespace plot
	{
		class View
		{
		public:
			//constructors
			View(void);
			
			//destructor
			virtual ~View(void);
			
			//serialization
			void load(FILE*);
			void save(FILE*) const;
			
			//data
			double zoom(void) const;
			double zoom(double, bool = false);
			
			const double* pan(void) const;
			const double* pan(const double*, bool = false);
			
			const double* rotation(void) const;
			const double* rotation(const double*, bool = false);

			//view
			void view_x(void);
			void view_y(void);
			void view_z(void);
			void view_i(void);
			
			//rotate
			void rotate_x(bool);
			void rotate_y(bool);
			void rotate_z(bool);
			
			//rotation
			static double* rotation(double*, const double*, double);
			
			//info
			void info(void) const;
			
			//config
			void reset(void);
		
		private:
			//attributes
			double m_zoom;
			double m_pan[3];
			double m_rotation[16];
		};
	}
}
