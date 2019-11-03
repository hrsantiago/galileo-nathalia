//gui
#include "Limit/Dof.h"

namespace gui
{
	namespace limit
	{
		//constructors
		Dof::Dof(void)
		{
			m_step[0] = 0;
			m_step[1] = 0;
			m_node[0] = 0;
			m_node[1] = 0;
			m_value[0] = 0;
			m_value[1] = 0;
		}
		
		//destructor
		Dof::~Dof(void)
		{
			return;
		}
	}
}
