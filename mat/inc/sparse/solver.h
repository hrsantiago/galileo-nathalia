#pragma once

namespace mat
{
	namespace sparse
	{
		class solver
		{
		public:
			//constructors
			solver(void);
			
			//destructor
			virtual ~solver(void);
			
			//solve
			virtual void clear(void) = 0;
			virtual bool solve(void) = 0;
			virtual bool decompose(void) = 0;
			virtual bool substitute(void) = 0;
			
		protected:
			//attributes
			int m_n;
			double* m_x;
			const double* m_K;
			const double* m_f;
			const int* m_row_map;
			const int* m_col_map;
		};
	}
}
