#pragma once

namespace gui
{
	namespace limit
	{
		struct Dof
		{
			//constructors
			Dof(void);
			
			//destructor
			virtual ~Dof(void);
			
			//attributes
			double m_value[2];
			unsigned m_step[2];
			unsigned m_node[2];
		};
	}
}
