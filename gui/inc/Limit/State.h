#pragma once

namespace gui
{
	namespace limit
	{
		struct State
		{
			//constructors
			State(void);
			
			//destructor
			virtual ~State(void);
			
			//attributes
			double m_value[2];
			unsigned m_step[2];
			unsigned m_node[2];
			unsigned m_element[2];
		};
	}
}
