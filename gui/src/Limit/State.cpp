//gui
#include "Limit/State.h"

namespace gui
{
	namespace limit
	{
		//constructors
		State::State(void)
		{
			m_step[0] = 0;
			m_step[1] = 0;
			m_node[0] = 0;
			m_node[1] = 0;
			m_value[0] = 0;
			m_value[1] = 0;
			m_element[0] = 0;
			m_element[1] = 0;
		}
		
		//destructor
		State::~State(void)
		{
			return;
		}
	}
}
