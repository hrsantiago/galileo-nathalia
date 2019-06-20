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
			
			
			//integration
			unsigned Quad::points(void) const
			{
				return m_rule * m_rule;
			}
			double Quad::point(double* x, unsigned i) const
			{
				double w[2];
				const unsigned a = i % m_rule;
				const unsigned b = (i - a) / m_rule;
				mat::gauss_point(x[0], w[0], m_rule, a);
				mat::gauss_point(x[1], w[1], m_rule, b);
				return w[0] * w[1];
			}
		}
	}
}
