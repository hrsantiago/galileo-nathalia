//mat
#include "linear/vector.h"

namespace mat
{
	//constructors
	vector::vector(unsigned rows) : matrix(rows, 1)
	{
		return;
	}
	vector::vector(const matrix& m) : matrix(m)
	{
		return;
	}
	vector::vector(double* ptr, unsigned rows) : matrix(ptr, rows, 1)
	{
		return;
	}
	vector::vector(const double* ptr, unsigned rows) : matrix(ptr, rows, 1)
	{
		return;
	}

	//destructor
	vector::~vector(void)
	{
		return;
	}
	
	//linear
	double vector::inner(const vector& v) const
	{
		double s = 0;
		for(unsigned i = 0; i < m_rows; i++)
		{
			s += m_ref[i] * v.m_ref[i];
		}
		return s;
	}
	matrix vector::outer(const vector& v) const
	{
		matrix m(m_rows, v.m_rows);
		for(unsigned i = 0; i < m_rows; i++)
		{
			for(unsigned j = 0; j < v.m_rows; j++)
			{
				m(i, j) = m_ref[i] * v.m_ref[j];
			}
		}
		return m;
	}
}
