#include "Mesh/Cells/Types.h"
#include "Mesh/Cells/Plane/Quad9.h"

namespace fea
{
	namespace mesh
	{
		namespace cells
		{
			//constructors
			Quad9::Quad9(void)
			{
				return;
			}

			//destructor
			Quad9::~Quad9(void)
			{
				return;
			}

			//type
			cells::type Quad9::type(void) const
			{
				return cells::type::quad9;
			}

			//topology
			unsigned Quad9::vertices(void) const
			{
				return 9;
			}

			std::vector<unsigned> Quad9::edge(unsigned e) const
			{
				switch(e)
				{
					case 0: return { 0, 1, 2 };
					case 1: return { 2, 3, 4 };
					case 2: return { 4, 5, 6 };
					case 3: return { 6, 7, 0 };
					default: return {};
				}
			}
			std::vector<unsigned> Quad9::face(unsigned) const
			{
				return { 0, 1, 2, 3, 4, 5, 6, 7 };
			}

			//interpolation
			double* Quad9::function(double* N, const double* p) const
			{
				//point
				const double r = p[0];
				const double s = p[1];
				//function
				N[8] = (1 - r * r) * (1 - s * s);
				N[1] = (1 - r * r) * (s - 1) * s / 2;
				N[5] = (1 - r * r) * (s + 1) * s / 2;
				N[3] = (r + 1) * r * (1 - s * s) / 2;
				N[7] = (r - 1) * r * (1 - s * s) / 2;
				N[0] = (r - 1) * r * (s - 1) * s / 4;
				N[2] = (r + 1) * r * (s - 1) * s / 4;
				N[4] = (r + 1) * r * (s + 1) * s / 4;
				N[6] = (r - 1) * r * (s + 1) * s / 4;
				return N;
			}
			double* Quad9::gradient(double* B, const double* p) const
			{
				//point
				const double r = p[0];
				const double s = p[1];
				//gradient
				B[8 + 9 * 0] = (0 - 2 * r) * (1 - s * s);
				B[8 + 9 * 1] = (1 - r * r) * (0 - 2 * s);
				B[0 + 9 * 0] = (2 * r - 1) * (s - 1) * s / 4;
				B[0 + 9 * 1] = (r - 1) * r * (2 * s - 1) / 4;
				B[1 + 9 * 0] = (0 - 2 * r) * (s - 1) * s / 2;
				B[1 + 9 * 1] = (1 - r * r) * (2 * s - 1) / 4;
				B[2 + 9 * 0] = (2 * r + 1) * (s - 1) * s / 4;
				B[2 + 9 * 1] = (r + 1) * r * (2 * s - 1) / 4;
				B[3 + 9 * 0] = (2 * r + 1) * (1 - s * s) / 2;
				B[3 + 9 * 1] = (r + 1) * r * (0 - 2 * s) / 4;
				B[4 + 9 * 0] = (2 * r + 1) * (s + 1) * s / 4;
				B[4 + 9 * 1] = (r + 1) * r * (2 * s + 1) / 4;
				B[5 + 9 * 0] = (0 - 2 * r) * (s + 1) * s / 2;
				B[5 + 9 * 1] = (1 - r * r) * (2 * s + 1) / 4;
				B[6 + 9 * 0] = (2 * r - 1) * (s + 1) * s / 4;
				B[6 + 9 * 1] = (r - 1) * r * (2 * s + 1) / 4;
				B[7 + 9 * 0] = (2 * r - 1) * (1 - s * s) / 2;
				B[7 + 9 * 1] = (r - 1) * r * (0 - 2 * s) / 4;
				return B;
			}
		}
	}
}
