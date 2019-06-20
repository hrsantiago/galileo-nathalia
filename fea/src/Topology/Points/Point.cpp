#include <string.h>

#include "Topology/Topology.h"
#include "Topology/Points/Point.h"

namespace fea
{
	namespace topology
	{
		//constructors
		Point::Point(void)
		{
			memset(m_coordinates, 0, 3 * sizeof(double));
		}

		//destructor
		Point::~Point(void)
		{
			return;
		}

		//data
		const double* Point::coordinates(void) const
		{
			return m_coordinates;
		}
		const double* Point::coordinates(const double* coordinates)
		{
			return (const double*) memcpy(m_coordinates, coordinates, 3 * sizeof(double));
		}
		const double* Point::coordinates(double x, double y, double z)
		{
			m_coordinates[0] = x;
			m_coordinates[1] = y;
			m_coordinates[2] = z;
			return m_coordinates;
		}

		//index
		unsigned Point::index(void) const
		{
			for(unsigned i = 0; i < m_topology->points(); i++)
			{
				if(m_topology->point(i) == this)
				{
					return i;
				}
			}
			return 0;
		}

		//static attributes
		Topology* Point::m_topology = nullptr;
	}
}
