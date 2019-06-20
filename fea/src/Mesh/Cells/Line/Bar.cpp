//fea
#include "Mesh/Cells/Types.h"
#include "Mesh/Cells/Line/Bar.h"

namespace fea
{
	namespace mesh
	{
		namespace cells
		{
			//constructors
			Bar::Bar(void)
			{
				m_rule = 1;
			}

			//destructor
			Bar::~Bar(void)
			{
				return;
			}

			//type
			cells::type Bar::type(void) const
			{
				return cells::type::bar;
			}

			//topology
			unsigned Bar::vertices(void) const
			{
				return 2;
			}
			
			std::vector<unsigned> Bar::edge(unsigned) const
			{
				return {0, 1};
			}

			//interpolation
			double* Bar::function(double* N, const double* p) const
			{
				N[0] = (1 - p[0]) / 2;
				N[1] = (1 + p[0]) / 2;
				return N;
			}
			double* Bar::gradient(double* B, const double*) const
			{
				B[0] = -(double) 1 / 2;
				B[1] = +(double) 1 / 2;
				return B;
			}
		}
	}
}
