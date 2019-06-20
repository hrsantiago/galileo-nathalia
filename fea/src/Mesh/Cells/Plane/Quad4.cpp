#include "Mesh/Cells/Types.h"
#include "Mesh/Cells/Plane/Quad4.h"

namespace fea
{
	namespace mesh
	{
		namespace cells
		{
			//constructors
			Quad4::Quad4(void)
			{
				return;
			}

			//destructor
			Quad4::~Quad4(void)
			{
				return;
			}

			//type
			cells::type Quad4::type(void) const
			{
				return cells::type::quad4;
			}

			//topology
			unsigned Quad4::vertices(void) const
			{
				return 4;
			}
			
			std::vector<unsigned> Quad4::edge(unsigned e) const
			{
				switch(e)
				{
					case 0: return { 0, 1 };
					case 1: return { 1, 2 };
					case 2: return { 2, 3 };
					case 3: return { 3, 0 };
				}
			}
			std::vector<unsigned> Quad4::face(unsigned) const
			{
				return { 0, 1, 2, 3 };
			}

			//interpolation
			double* Quad4::function(double* N, const double* p) const
			{
				//point
				const double r = p[0];
				const double s = p[1];
				//function
				N[0] = (1 - r) * (1 - s) / 4;
				N[1] = (1 + r) * (1 - s) / 4;
				N[2] = (1 + r) * (1 + s) / 4;
				N[3] = (1 - r) * (1 + s) / 4;
				return N;
			}
			double* Quad4::gradient(double* B, const double* p) const
			{
				//point
				const double r = p[0];
				const double s = p[1];
				//gradient
				B[0 + 4 * 0] = -(1 - s) / 4;
				B[0 + 4 * 1] = -(1 - r) / 4;
				B[1 + 4 * 0] = +(1 - s) / 4;
				B[1 + 4 * 1] = -(1 + r) / 4;
				B[2 + 4 * 0] = +(1 + s) / 4;
				B[2 + 4 * 1] = +(1 + r) / 4;
				B[3 + 4 * 0] = -(1 + s) / 4;
				B[3 + 4 * 1] = +(1 - r) / 4;
				return B;
			}
		}
	}
}
