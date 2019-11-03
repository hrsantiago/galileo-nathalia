#include "Mesh/Cells/Types.h"
#include "Mesh/Cells/Volume/Brick27.h"

namespace fea
{
	namespace mesh
	{
		namespace cells
		{
			//constructors
			Brick27::Brick27(void)
			{
				return;
			}

			//destructor
			Brick27::~Brick27(void)
			{
				return;
			}

			//type
			cells::type Brick27::type(void) const
			{
				return cells::type::brick27;
			}

			//topology
			unsigned Brick27::vertices(void) const
			{
				return 8;
			}
			
			std::vector<unsigned> Brick27::edge(unsigned e) const
			{
				switch(e)
				{
					case  0: return { 0, 1 };
					case  1: return { 1, 2 };
					case  2: return { 2, 3 };
					case  3: return { 3, 0 };
					case  4: return { 0, 4 };
					case  5: return { 1, 5 };
					case  6: return { 2, 6 };
					case  7: return { 3, 7 };
					case  8: return { 4, 5 };
					case  9: return { 5, 6 };
					case 10: return { 6, 7 };
					case 11: return { 7, 4 };
					default: return {};
				}
			}
			std::vector<unsigned> Brick27::face(unsigned f) const
			{
				switch(f)
				{
					case 0: return { 0, 1, 2, 3 };
					case 1: return { 0, 4, 5, 1 };
					case 2: return { 1, 5, 6, 2 };
					case 3: return { 2, 6, 7, 3 };
					case 4: return { 3, 7, 4, 0 };
					case 5: return { 4, 7, 6, 5 };
					default: return {};
				}
			}

			//interpolation
			double* Brick27::function(double* N, const double* p) const
			{
				//point
				const double r = p[0];
				const double s = p[1];
				const double t = p[2];
				//function
				N[0] = (1 - r) * (1 - s) * (1 - t) / 8;
				N[1] = (1 + r) * (1 - s) * (1 - t) / 8;
				N[2] = (1 + r) * (1 + s) * (1 - t) / 8;
				N[3] = (1 - r) * (1 + s) * (1 - t) / 8;
				N[4] = (1 - r) * (1 - s) * (1 + t) / 8;
				N[5] = (1 + r) * (1 - s) * (1 + t) / 8;
				N[6] = (1 + r) * (1 + s) * (1 + t) / 8;
				N[7] = (1 - r) * (1 + s) * (1 + t) / 8;
				return N;
			}
			double* Brick27::gradient(double* B, const double* p) const
			{
				//point
				const double r = p[0];
				const double s = p[1];
				const double t = p[2];
				//gradient
				B[0 + 8 * 0] = -(1 - s) * (1 - t) / 8;
				B[0 + 8 * 1] = -(1 - r) * (1 - t) / 8;
				B[0 + 8 * 2] = -(1 - r) * (1 - s) / 8;
				B[1 + 8 * 0] = +(1 - s) * (1 - t) / 8;
				B[1 + 8 * 1] = -(1 + r) * (1 - t) / 8;
				B[1 + 8 * 2] = -(1 + r) * (1 - s) / 8;
				B[2 + 8 * 0] = +(1 + s) * (1 - t) / 8;
				B[2 + 8 * 1] = +(1 + r) * (1 - t) / 8;
				B[2 + 8 * 2] = -(1 + r) * (1 + s) / 8;
				B[3 + 8 * 0] = -(1 + s) * (1 - t) / 8;
				B[3 + 8 * 1] = +(1 - r) * (1 - t) / 8;
				B[3 + 8 * 2] = -(1 - r) * (1 + s) / 8;
				B[4 + 8 * 0] = -(1 - s) * (1 + t) / 8;
				B[4 + 8 * 1] = -(1 - r) * (1 + t) / 8;
				B[4 + 8 * 2] = +(1 - r) * (1 - s) / 8;
				B[5 + 8 * 0] = +(1 - s) * (1 + t) / 8;
				B[5 + 8 * 1] = -(1 + r) * (1 + t) / 8;
				B[5 + 8 * 2] = +(1 + r) * (1 - s) / 8;
				B[6 + 8 * 0] = +(1 + s) * (1 + t) / 8;
				B[6 + 8 * 1] = +(1 + r) * (1 + t) / 8;
				B[6 + 8 * 2] = +(1 + r) * (1 + s) / 8;
				B[7 + 8 * 0] = -(1 + s) * (1 + t) / 8;
				B[7 + 8 * 1] = +(1 - r) * (1 + t) / 8;
				B[7 + 8 * 2] = +(1 - r) * (1 + s) / 8;
				return B;
			}
			
			//integration
			unsigned Brick27::points(void) const
			{
				return 0;
			}
			double Brick27::point(double* x, unsigned i) const
			{
				return 0;
			}
		}
	}
}
