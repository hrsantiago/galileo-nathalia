#include "Mesh/Elements/Mechanic/Mechanic.h"

#include "Mesh/Points/Types.h"

namespace fea
{
	namespace mesh
	{
		namespace elements
		{
			//constructors
			Mechanic::Mechanic(void)
			{
				return;
			}

			//destructor
			Mechanic::~Mechanic(void)
			{
				return;
			}

			//data
			bool Mechanic::geometric(void)
			{
				return m_geometric;
			}
			bool Mechanic::geometric(bool geometric)
			{
				return m_geometric = geometric;
			}
			bool Mechanic::inelastic(void)
			{
				return m_inelastic;
			}
			bool Mechanic::inelastic(bool inelastic)
			{
				return m_inelastic = inelastic;
			}

			//type
			points::type Mechanic::point(void) const
			{
				return points::type::mechanic;
			}

			//static attributes
			bool Mechanic::m_geometric = false;
			bool Mechanic::m_inelastic = false;
		}
	}
}
