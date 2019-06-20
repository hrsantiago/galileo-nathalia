#include "Topology/Topology.h"
#include "Topology/Surfaces/Surface.h"

namespace fea
{
	namespace topology
	{
		//constructors
		Surface::Surface(void)
		{
			return;
		}

		//destructor
		Surface::~Surface(void)
		{
			return;
		}

		//index
		unsigned Surface::index(void) const
		{
			for(unsigned i = 0; i < m_topology->surfaces(); i++)
			{
				if(m_topology->surface(i) == this)
				{
					return i;
				}
			}
			return 0;
		}

		//static attributes
		Topology* Surface::m_topology = nullptr;
	}
}
