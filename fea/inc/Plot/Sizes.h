#pragma once

#include <cstdio>

namespace fea
{
	namespace plot
	{
		class Sizes
		{
		public:
			//constructors
			Sizes(void);
			
			//destructor
			virtual ~Sizes(void);
			
			//serialization
			void load(FILE*);
			void save(FILE*) const;
			
			//data
			double loads(double);
			double joints(double);
			double supports(double);
			
			double loads(void) const;
			double joints(void) const;
			double supports(void) const;
			
			//info
			void info(void) const;
			
			//config
			void reset(void);
			
		private:
			//attributes
			double m_loads = 0.05;
			double m_joints = 0.03;
			double m_supports = 0.03;
		};
	}
}
