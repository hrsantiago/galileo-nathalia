//mat
#include "misc/util.h"

//fea
#include "Mesh/Cells/Types.h"
#include "Mesh/Cells/Plane/Quad8.h"

namespace fea
{
	namespace mesh
	{
		namespace cells
		{
			//constructors
			Quad8::Quad8(void)
			{
				return;
			}

			//destructor
			Quad8::~Quad8(void)
			{
				return;
			}

			//type
			cells::type Quad8::type(void) const
			{
				return cells::type::quad8;
			}

			//topology
			unsigned Quad8::vertices(void) const
			{
				return 8;
			}
			
			std::vector<unsigned> Quad8::edge(unsigned e) const
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
			std::vector<unsigned> Quad8::face(unsigned) const
			{
				return { 0, 1, 2, 3, 4, 5, 6, 7 };
			}

			//interpolation
			double* Quad8::function(double* N, const double* p) const
			{
				//point
				const double r = p[0];
				const double s = p[1];
				//function
				N[1] = (1 - r * r) * (1 - s) / 2;
				N[3] = (1 + r) * (1 - s * s) / 2;
				N[5] = (1 - r * r) * (1 + s) / 2;
				N[7] = (r - 1) * (s * s - 1) / 2;
				N[0] = (1 - r) * (s - 1) * (1 + r + s) / 4;
				N[2] = (1 + r) * (s - 1) * (1 - r + s) / 4;
				N[4] = (1 + r) * (1 + s) * (r + s - 1) / 4;
				N[6] = (r - 1) * (1 + r - s) * (1 + s) / 4;
				return N;
			}
			double* Quad8::gradient(double* B, const double* p) const
			{
				//point
				const double r = p[0];
				const double s = p[1];
				//gradient
				B[1 + 8 * 0] = +r * (s - 1);
				B[7 + 8 * 1] = +s * (r - 1);
				B[3 + 8 * 1] = -s * (1 + r);
				B[5 + 8 * 0] = -r * (1 + s);
				B[1 + 8 * 1] = (r * r - 1) / 2;
				B[3 + 8 * 0] = (1 - s * s) / 2;
				B[5 + 8 * 1] = (1 - r * r) / 2;
				B[7 + 8 * 0] = (s * s - 1) / 2;
				B[0 + 8 * 0] = (1 - s) * (2 * r + s) / 4;
				B[0 + 8 * 1] = (1 - r) * (r + 2 * s) / 4;
				B[2 + 8 * 0] = (2 * r - s) * (1 - s) / 4;
				B[2 + 8 * 1] = (1 + r) * (2 * s - r) / 4;
				B[4 + 8 * 0] = (1 + s) * (2 * r + s) / 4;
				B[4 + 8 * 1] = (1 + r) * (r + 2 * s) / 4;
				B[6 + 8 * 0] = (2 * r - s) * (1 + s) / 4;
				B[6 + 8 * 1] = (r - 1) * (r - 2 * s) / 4;
				return B;
			}
			
			//integration
			unsigned Quad8::points(void) const
			{
				return 9;
			}
			double Quad8::point(double* x, unsigned i) const
			{
				//quadrature
				const double* s = mat::gauss_points(3);
				const double* w = mat::gauss_weights(3);
				//point
				switch(i)
				{
					case 0: x[0] = s[0]; x[1] = s[0]; return w[0] * w[0];
					case 1: x[0] = s[0]; x[1] = s[1]; return w[0] * w[1];
					case 2: x[0] = s[0]; x[1] = s[2]; return w[0] * w[2];
					case 3: x[0] = s[1]; x[1] = s[0]; return w[1] * w[0];
					case 4: x[0] = s[1]; x[1] = s[1]; return w[1] * w[1];
					case 5: x[0] = s[1]; x[1] = s[2]; return w[1] * w[2];
					case 6: x[0] = s[2]; x[1] = s[0]; return w[2] * w[0];
					case 7: x[0] = s[2]; x[1] = s[1]; return w[2] * w[1];
					case 8: x[0] = s[2]; x[1] = s[2]; return w[2] * w[2];
					default: return 0;
				}
			}
		}
	}
}
