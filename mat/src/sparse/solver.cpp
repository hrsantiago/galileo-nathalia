//mat
#include "sparse/solver.h"

namespace mat
{
	namespace sparse
	{
		//constructors
		solver::solver(void) : m_n(0), m_x(nullptr), m_K(nullptr), m_f(nullptr), m_row_map(nullptr), m_col_map(nullptr)
		{
			return;
		}
		
		//destructor
		solver::~solver(void)
		{
			return;
		}
	}
}
