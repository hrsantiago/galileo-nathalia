#include "Mesh/Cells/Types.h"
#include "Mesh/Cells/Plane/Tri3.h"

namespace fea
{
	namespace mesh
	{
		namespace cells
		{
			//constructors
			Tri3::Tri3(void)
			{
				return;
			}

			//destructor
			Tri3::~Tri3(void)
			{
				return;
			}

			//type
			cells::type Tri3::type(void) const
			{
				return cells::type::tri3;
			}

			//topology
			unsigned Tri3::vertices(void) const
			{
				return 3;
			}
			
			std::vector<unsigned> Tri3::edge(unsigned e) const
			{
				switch(e)
				{
					case 0: return { 0, 1 };
					case 1: return { 1, 2 };
					case 2: return { 2, 0 };
					default: return {};
				}
			}
			std::vector<unsigned> Tri3::face(unsigned) const
			{
				return { 0, 1, 2 };
			}

			//interpolation
			double* Tri3::function(double* N, const double* p) const
			{
				//point
				const double r = p[0];
				const double s = p[1];
				//interpolation
				N[0] = -(r + s) / 2;
				N[1] = +(1 + r) / 2;
				N[2] = +(1 + s) / 2;
				return N;
			}
			double* Tri3::gradient(double* B, const double*) const
			{
				//gradient
				B[0 + 3 * 0] = -0.5;
				B[0 + 3 * 1] = -0.5;
				B[1 + 3 * 0] = +0.5;
				B[1 + 3 * 1] = +0.0;
				B[2 + 3 * 0] = +0.0;
				B[2 + 3 * 1] = +0.5;
				return B;
			}
		}
	}
}
