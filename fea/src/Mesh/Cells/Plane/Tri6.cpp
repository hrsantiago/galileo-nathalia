#include "Mesh/Cells/Types.h"
#include "Mesh/Cells/Plane/Tri6.h"

namespace fea
{
	namespace mesh
	{
		namespace cells
		{
			//constructors
			Tri6::Tri6(void)
			{
				return;
			}

			//destructor
			Tri6::~Tri6(void)
			{
				return;
			}

			//type
			cells::type Tri6::type(void) const
			{
				return cells::type::tri6;
			}

			//topology
			unsigned Tri6::vertices(void) const
			{
				return 6;
			}
			
			std::vector<unsigned> Tri6::edge(unsigned e) const
			{
				switch(e)
				{
					case 0: return { 0, 1, 2 };
					case 1: return { 2, 3, 4 };
					case 2: return { 4, 5, 0 };
					default: return {};
				}
			}
			std::vector<unsigned> Tri6::face(unsigned) const
			{
				return { 0, 1, 2, 3, 4, 5 };
			}

			//interpolation
			double* Tri6::function(double* N, const double* p) const
			{
				//point
				const double r = p[0];
				const double s = p[1];
				//interpolation
				N[2] = r * (1 + r) / 2;
				N[4] = s * (1 + s) / 2;
				N[3] = +(1 + r) * (1 + s);
				N[5] = -(r + s) * (1 + s);
				N[1] = -(r + s) * (1 + r);
				N[0] = (r + s) * (1 + r + s) / 2;
				return N;
			}
			double* Tri6::gradient(double* B, const double* p) const
			{
				//point
				const double r = p[0];
				const double s = p[1];
				//gradient
				B[2 + 6 * 1] = 0;
				B[4 + 6 * 0] = 0;
				B[3 + 6 * 0] = +(1 + s);
				B[3 + 6 * 1] = +(1 + r);
				B[1 + 6 * 1] = -(1 + r);
				B[5 + 6 * 0] = -(1 + s);
				B[2 + 6 * 0] = +(1 + 2 * r) / 2;
				B[4 + 6 * 1] = +(1 + 2 * s) / 2;
				B[1 + 6 * 0] = -(1 + 2 * r + s);
				B[5 + 6 * 1] = -(1 + r + 2 * s);
				B[0 + 6 * 0] = (1 + 2 * r + 2 * s) / 2;
				B[0 + 6 * 1] = (1 + 2 * r + 2 * s) / 2;
				return B;
			}
		}
	}
}
