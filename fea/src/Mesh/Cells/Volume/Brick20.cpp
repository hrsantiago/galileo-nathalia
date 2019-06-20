#include "Mesh/Cells/Types.h"
#include "Mesh/Cells/Volume/Brick20.h"

namespace fea
{
	namespace mesh
	{
		namespace cells
		{
			//constructors
			Brick20::Brick20(void)
			{
				return;
			}

			//destructor
			Brick20::~Brick20(void)
			{
				return;
			}

			//type
			cells::type Brick20::type(void) const
			{
				return cells::type::brick20;
			}

			//topology
			unsigned Brick20::vertices(void) const
			{
				return 20;
			}
			
			std::vector<unsigned> Brick20::edge(unsigned) const
			{
				return {};
			}
			std::vector<unsigned> Brick20::face(unsigned) const
			{
				return {};
			}

			//interpolation
			double* Brick20::function(double* N, const double*) const
			{
				return N;
			}
			double* Brick20::gradient(double* B, const double*) const
			{
				return B;
			}
		}
	}
}
