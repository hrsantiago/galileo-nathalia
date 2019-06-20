#include "Boundary/Loads/Types.h"
#include "Boundary/Loads/Elements/Mechanic/Solid/Solid_Edge.h"

namespace fea
{
	namespace boundary
	{
		namespace loads
		{
			//constructors
			Solid_Edge::Solid_Edge(void) : m_edge(0)
			{
				return;
			}

			//destructor
			Solid_Edge::~Solid_Edge(void)
			{
				return;
			}

			//types
			loads::type Solid_Edge::type(void) const
			{
				return loads::type::solid_edge;
			}
			
			//data
			unsigned Solid_Edge::edge(void) const
			{
				return m_edge;
			}
			unsigned Solid_Edge::edge(unsigned edge)
			{
				return m_edge = edge;
			}
		}
	}
}
