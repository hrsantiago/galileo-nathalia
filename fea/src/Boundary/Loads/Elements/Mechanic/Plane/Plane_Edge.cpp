#include "Boundary/Loads/Types.h"
#include "Boundary/Loads/Elements/Mechanic/Plane/Plane_Edge.h"

namespace fea
{
	namespace boundary
	{
		namespace loads
		{
			//constructors
			Plane_Edge::Plane_Edge(void) : m_edge(0)
			{
				return;
			}

			//destructor
			Plane_Edge::~Plane_Edge(void)
			{
				return;
			}

			//types
			loads::type Plane_Edge::type(void) const
			{
				return loads::type::plane_edge;
			}
			
			//data
			unsigned Plane_Edge::edge(void) const
			{
				return m_edge;
			}
			unsigned Plane_Edge::edge(unsigned edge)
			{
				return m_edge = edge;
			}
		}
	}
}
