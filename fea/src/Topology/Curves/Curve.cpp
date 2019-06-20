#include "Topology/Topology.h"
#include "Topology/Curves/Curve.h"

namespace fea
{
	namespace topology
	{
		//constructors
		Curve::Curve(void)
		{
			return;
		}

		//destructor
		Curve::~Curve(void)
		{
			return;
		}

		//index
		unsigned Curve::index(void) const
		{
			for(unsigned i = 0; i < m_topology->curves(); i++)
			{
				if(m_topology->curve(i) == this)
				{
					return i;
				}
			}
			return 0;
		}

		//static attributes
		Topology* Curve::m_topology = nullptr;
	}
}
