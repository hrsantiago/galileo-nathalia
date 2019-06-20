#include "Mesh/Cells/Volume/Tetra.h"

namespace fea
{
	namespace mesh
	{
		namespace cells
		{
			//constructors
			Tetra::Tetra(void)
			{
				return;
			}

			//destructor
			Tetra::~Tetra(void)
			{
				return;
			}
			
			//topology
			unsigned Tetra::faces(void) const
			{
				return 4;
			}
			unsigned Tetra::edges(void) const
			{
				return 6;
			}
			
			//parametrization
			void Tetra::edge(double* p, unsigned e, double t) const
			{
				switch(e)
				{
					case  0: p[0] = +t;	p[1] = -1;	p[2] = -1;	break;
					case  1: p[0] = -t;	p[1] = +t;	p[2] = -1;	break;
					case  2: p[0] = -1;	p[1] = +t;	p[2] = -1;	break;
					case  3: p[0] = -1;	p[1] = -1;	p[2] = +t;	break;
					case  4: p[0] = -t;	p[1] = -1;	p[2] = +t;	break;
					case  5: p[0] = -1;	p[1] = -t;	p[2] = +t;	break;
				}
			}
			void Tetra::face(double* p, unsigned f, double u, double v) const
			{
				switch(f)
				{
					case 0: p[0] = +u;	p[1] = +v;	p[2] = -1;	break;
					case 1: p[0] = +u;	p[1] = -1;	p[2] = +v;	break;
					case 2: p[0] = +1;	p[1] = +u;	p[2] = +v;	break;
					case 3: p[0] = +u;	p[1] = +1;	p[2] = +v;	break;
				}
			}
			
			void Tetra::gradient(double*, unsigned, double) const
			{
				
			}
			void Tetra::gradient(double*, unsigned, double, double) const
			{
				
			}
			
			//integration
			unsigned Tetra::points(void) const
			{
				return 0;
			}
			double Tetra::point(double* x, unsigned i) const
			{
				return 0;
			}
		}
	}
}
