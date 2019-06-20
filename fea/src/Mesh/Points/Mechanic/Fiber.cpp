#include "Mesh/Points/Mechanic/Fiber.h"

namespace fea
{
	namespace mesh
	{
		namespace points
		{
			//constructors
			Fiber::Fiber(void) : m_position_y(0), m_position_z(0), m_area(0)
			{
				return;
			}

			//destructor
			Fiber::~Fiber(void)
			{
				return;
			}

			//analysis
			void Fiber::prepare(unsigned t)
			{
				m_point.prepare(t);
			}
			
			void Fiber::update(void)
			{
				m_point.update();
			}
			void Fiber::restore(void)
			{
				m_point.restore();
			}
		}
	}
}
