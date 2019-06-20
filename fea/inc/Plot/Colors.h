#pragma once

#include <cstdio>

namespace fea
{
	namespace plot
	{
		class Colors
		{
		public:
			//constructors
			Colors(void);
			
			//destructor
			virtual ~Colors(void);
			
			//serialization
			void load(FILE*);
			void save(FILE*) const;
			
			//data
			double* back(double*);
			double* nodes(double*);
			double* loads(double*);
			double* paths(double*);
			double* joints(double*);
			double* elements(double*);
			double* supports(double*);
			
			const double* back(void) const;
			const double* nodes(void) const;
			const double* loads(void) const;
			const double* paths(void) const;
			const double* joints(void) const;
			const double* elements(void) const;
			const double* supports(void) const;
			
			//info
			void info(void) const;
			
			//config
			void reset(void);
			void print(void);
		
		private:
			//attributes
			double m_back[4] = {0.00, 0.00, 0.00, 1.00};
			double m_nodes[4] = {1.00, 0.00, 0.00, 1.00};
			double m_loads[4] = {1.00, 0.39, 0.28, 1.00};
			double m_paths[4] = {0.50, 0.50, 0.50, 1.00};
			double m_joints[4] = {0.00, 1.00, 0.00, 1.00};
			double m_elements[4] = {0.00, 0.00, 1.00, 1.00};
			double m_supports[4] = {0.50, 0.50, 0.50, 1.00};
		};
	}
}
