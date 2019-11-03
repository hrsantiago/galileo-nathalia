#pragma once

//mat
#include "linear/vec3.h"
#include "linear/quat.h"

namespace gui
{
	namespace canvas
	{
		struct Click
		{
			//constructors
			Click(void);
			
			//destructor
			virtual ~Click(void);
			
			//attributes
			int m_x;
			int m_y;
			bool m_pan;
			bool m_rot;
			mat::vec3 m_p;
			mat::quat m_q;
		};
	}
}

