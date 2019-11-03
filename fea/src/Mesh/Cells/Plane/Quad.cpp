//mat
#include "misc/util.h"

//fea
#include "Mesh/Cells/Plane/Quad.h"

namespace fea
{
	namespace mesh
	{
		namespace cells
		{
			//constructors
			Quad::Quad(void)
			{
				return;
			}

			//destructor
			Quad::~Quad(void)
			{
				return;
			}
			
			//topology
			unsigned Quad::edges(void) const
			{
				return 4;
			}

			//parametrization
			void Quad::edge(double* p, unsigned e, double t) const
			{
				switch(e)
				{
					case 0:
						p[0] = +t;
						p[1] = -1;
						break;
					case 1:
						p[0] = +1;
						p[1] = +t;
						break;
					case 2:
						p[0] = -t;
						p[1] = +1;
						break;
					case 3:
						p[0] = -1;
						p[1] = -t;
						break;
				}
			}
			void Quad::gradient(double* dp, unsigned e, double t) const
			{
				switch(e)
				{
					case 0:
						dp[0] = +1;
						dp[1] = +0;
						break;
					case 1:
						dp[0] = +0;
						dp[1] = +1;
						break;
					case 2:
						dp[0] = -1;
						dp[1] = +0;
						break;
					case 3:
						dp[0] = +0;
						dp[1] = -1;
						break;
				}
			}
		}
	}
}
