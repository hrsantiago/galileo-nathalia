//fea
#include "Mesh/Cells/Types.h"
#include "Mesh/Cells/Line/Beam.h"

namespace fea
{
	namespace mesh
	{
		namespace cells
		{
			//constructors
			Beam::Beam(void)
			{
				return;
			}

			//destructor
			Beam::~Beam(void)
			{
				return;
			}

			//type
			cells::type Beam::type(void) const
			{
				return cells::type::beam;
			}

			//topology
			unsigned Beam::vertices(void) const
			{
				return 2;
			}
			
			std::vector<unsigned> Beam::edge(unsigned) const
			{
				return {0, 1};
			}

			//interpolation
			double* Beam::function(double* N, const double* p) const
			{
				//point
				const double r = p[0];
				//function
				N[0] = (1 - r) / 2;
				N[3] = (1 + r) / 2;
				N[2] = (1 - r) * (1 - r * r) / 4;
				N[5] = (1 + r) * (r * r - 1) / 4;
				N[1] = (2 - 3 * r + r * r * r) / 4;
				N[4] = (2 + 3 * r - r * r * r) / 4;
				return N;
			}
			double* Beam::gradient(double* B, const double* p) const
			{
				//point
				const double r = p[0];
				//interpolate
				B[0] = -(double) 1 / 2;
				B[3] = +(double) 1 / 2;
				B[1] = 3 * (r * r - 1) / 4;
				B[4] = 3 * (1 - r * r) / 4;
				B[2] = (3 * r * r - 2 * r - 1) / 4;
				B[5] = (3 * r * r + 2 * r - 1) / 4;
				return B;
			}
		}
	}
}
