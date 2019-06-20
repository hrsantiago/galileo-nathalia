#pragma once

#include "Topology/Curves/Curve.h"

namespace fea
{
	namespace topology
	{
		//declarations
		class Point;

		//class
		class Line : public Curve
		{
		public:
			//constructors
			Line(void);

			//destructor
			~Line(void);

			//data
			Point* point(unsigned) const;
			Point* point(unsigned, unsigned);

			//index
			unsigned index_point(unsigned) const;

		protected:
			//attributes
			unsigned m_points[2];
		};
	}
}
