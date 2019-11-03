//mat
#include "misc/util.h"

//fea
#include "Mesh/Cells/Volume/Brick.h"

namespace fea
{
	namespace mesh
	{
		namespace cells
		{
			//constructors
			Brick::Brick(void)
			{
				return;
			}

			//destructor
			Brick::~Brick(void)
			{
				return;
			}
			
			//topology
			unsigned Brick::faces(void) const
			{
				return 6;
			}
			unsigned Brick::edges(void) const
			{
				return 12;
			}
			
			//parametrization
			void Brick::edge(double* p, unsigned e, double t) const
			{
				switch(e)
				{
					case  0: p[0] = +t;	p[1] = -1;	p[2] = -1;	break;
					case  1: p[0] = +1;	p[1] = +t;	p[2] = -1;	break;
					case  2: p[0] = -t;	p[1] = +1;	p[2] = -1;	break;
					case  3: p[0] = -1;	p[1] = -t;	p[2] = -1;	break;
					case  4: p[0] = -1;	p[1] = -1;	p[2] = +t;	break;
					case  5: p[0] = +1;	p[1] = -1;	p[2] = +t;	break;
					case  6: p[0] = +1;	p[1] = +1;	p[2] = +t;	break;
					case  7: p[0] = -1;	p[1] = +1;	p[2] = +t;	break;
					case  8: p[0] = +t;	p[1] = -1;	p[2] = +1;	break;
					case  9: p[0] = +1;	p[1] = +t;	p[2] = +1;	break;
					case 10: p[0] = -t;	p[1] = +1;	p[2] = +1;	break;
					case 11: p[0] = -1;	p[1] = -t;	p[2] = +1;	break;
				}
			}
			void Brick::face(double* p, unsigned f, double u, double v) const
			{
				switch(f)
				{
					case 0: p[0] = -u;	p[1] = +v;	p[2] = -1;	break;
					case 1: p[0] = +u;	p[1] = -1;	p[2] = +v;	break;
					case 2: p[0] = +1;	p[1] = +u;	p[2] = +v;	break;
					case 3: p[0] = -u;	p[1] = +1;	p[2] = +v;	break;
					case 4: p[0] = -1;	p[1] = -u;	p[2] = +v;	break;
					case 5: p[0] = +u;	p[1] = +v;	p[2] = +1;	break;
				}
			}
			
			void Brick::gradient(double* dp, unsigned e, double) const
			{
				switch(e)
				{
					case  0: dp[0] = +1; dp[1] = +0; dp[2] = +0; break;
					case  1: dp[0] = +0; dp[1] = +1; dp[2] = +0; break;
					case  2: dp[0] = -1; dp[1] = +0; dp[2] = +0; break;
					case  3: dp[0] = +0; dp[1] = -1; dp[2] = +0; break;
					case  4: dp[0] = +0; dp[1] = +0; dp[2] = +1; break;
					case  5: dp[0] = +0; dp[1] = +0; dp[2] = +1; break;
					case  6: dp[0] = +0; dp[1] = +0; dp[2] = +1; break;
					case  7: dp[0] = +0; dp[1] = +0; dp[2] = +1; break;
					case  8: dp[0] = +1; dp[1] = +0; dp[2] = +0; break;
					case  9: dp[0] = +0; dp[1] = +1; dp[2] = +0; break;
					case 10: dp[0] = -1; dp[1] = +0; dp[2] = +0; break;
					case 11: dp[0] = +0; dp[1] = -1; dp[2] = +0; break;
				}
			}
			void Brick::gradient(double* dp, unsigned f, double, double) const
			{
				switch(f)
				{
					case 0: dp[0] = -1;	dp[1] = +0;	dp[2] = +0;	dp[3] = +0;	dp[4] = +1;	dp[5] = +0;	break;
					case 1: dp[0] = +1;	dp[1] = +0;	dp[2] = +0;	dp[3] = +0;	dp[4] = +0;	dp[5] = +1;	break;
					case 2: dp[0] = +0;	dp[1] = +1;	dp[2] = +0;	dp[3] = +0;	dp[4] = +0;	dp[5] = +1;	break;
					case 3: dp[0] = -1;	dp[1] = +0;	dp[2] = +0;	dp[3] = +0;	dp[4] = +0;	dp[5] = +1;	break;
					case 4: dp[0] = +0;	dp[1] = -1;	dp[2] = +0;	dp[3] = +0;	dp[4] = +0;	dp[5] = +1;	break;
					case 5: dp[0] = +1;	dp[1] = +0;	dp[2] = +0;	dp[3] = +0;	dp[4] = +1;	dp[5] = +0;	break;
				}
			}
		}
	}
}
