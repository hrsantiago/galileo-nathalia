#pragma once

#include "linear/matrix.h"

namespace mat
{
	class vector : public matrix
	{
	public:
		//constructors
		vector(unsigned);
		vector(const matrix&);
		vector(double*, unsigned);
		vector(const double*, unsigned);

		//destructor
		virtual ~vector(void);
		
		//linear
		double inner(const vector&) const;
		matrix outer(const vector&) const;
	};
}
