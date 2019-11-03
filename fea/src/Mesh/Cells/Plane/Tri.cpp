#include "Mesh/Cells/Plane/Tri.h"

namespace fea
{
	namespace mesh
	{
		namespace cells
		{
			//constructors
			Tri::Tri(void)
			{
				return;
			}

			//destructor
			Tri::~Tri(void)
			{
				return;
			}
			
			//topology
			unsigned Tri::edges(void) const
			{
				return 3;
			}

			//parametrization
			void Tri::edge(double* p, unsigned e, double t) const
			{
				switch(e)
				{
					case 0:
						p[0] = +t;
						p[1] = -1;
						break;
					case 1:
						p[0] = 1 - 2 * t;
						p[1] = 2 * t - 1;
						break;
					case 2:
						p[0] = -1;
						p[1] = -t;
						break;
				}
			}
			void Tri::gradient(double* dp, unsigned e, double t) const
			{
				switch(e)
				{
					case 0:
						dp[0] = +1;
						dp[1] = +0;
						break;
					case 1:
						dp[0] = -2;
						dp[1] = +2;
						break;
					case 2:
						dp[0] = +0;
						dp[1] = -1;
						break;
				}
			}
		}
	}
}
