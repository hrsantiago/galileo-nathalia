#include "Topology/Topology.h"
#include "Topology/Curves/Line.h"

namespace fea
{
	namespace topology
	{
		//constructors
		Line::Line(void)
		{
			return;
		}

		//destructor
		Line::~Line(void)
		{
			return;
		}

		//data
		Point* Line::point(unsigned index) const
		{
			return m_topology->point(m_points[index]);
		}
		Point* Line::point(unsigned index, unsigned point)
		{
			return m_topology->point(m_points[index] = point);
		}

		//index
		unsigned Line::index_point(unsigned index) const
		{
			return m_points[index];
		}
	}
}
