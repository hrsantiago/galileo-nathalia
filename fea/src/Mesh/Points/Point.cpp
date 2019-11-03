//fea
#include "Mesh/Points/Point.h"
#include "Mesh/Points/Types.h"
#include "Mesh/Points/Heat/Heat.h"
#include "Mesh/Points/Mechanic/Fiber.h"
#include "Mesh/Points/Mechanic/Section.h"
#include "Mesh/Points/Mechanic/Mechanic.h"

namespace fea
{
	namespace mesh
	{
		namespace points
		{
			//constructors
			Point::Point(void)
			{
				return;
			}

			//destructor
			Point::~Point(void)
			{
				return;
			}

			//create
			void Point::create(Point*& point, const Point* base)
			{
				Point::create(point, base->type(), base);
			}
			void Point::create(Point*& point, points::type type, const Point* base)
			{
				//delete point
				delete point;
				//create point
				switch(type)
				{
					case points::type::section:
						point = base ? new Section(*(Section*) base) : new Section;
						break;
					case points::type::mechanic:
						point = base ? new Mechanic(*(Mechanic*) base) : new Mechanic;
						break;
					case points::type::heat:
						point = base ? new Heat(*(Heat*) base) : new Heat;
						break;
					default:
						break;
				}
			}

			//static attributes
			Mesh* Point::m_mesh = nullptr;
		}
	}
}
