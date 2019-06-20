#pragma once

#include <cstdio>

namespace fea
{
	namespace plot
	{
		class Gradient
		{
		public:
			//constructors
			Gradient(void);
			
			//destructor
			virtual ~Gradient(void);
			
			//serialization
			void load(FILE*);
			void save(FILE*) const;
			
			//data
			double* color_min(double*);
			double* color_max(double*);
			double* color_mean(double*);
			
			const double* color_min(void) const;
			const double* color_max(void) const;
			const double* color_mean(void) const;
			
			//info
			void info(void) const;
			
			//config
			void reset(void);
			
		private:
			//attributes
			double m_color_min[4] = {1.00, 0.00, 0.00, 0.50};
			double m_color_max[4] = {0.00, 0.00, 1.00, 0.50};
			double m_color_mean[4] = {1.00, 1.00, 1.00, 0.50};
		};
	}
}
