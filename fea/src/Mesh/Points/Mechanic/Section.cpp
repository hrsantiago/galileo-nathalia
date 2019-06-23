//std
#include <cstring>

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Points/Types.h"
#include "Mesh/Cells/Line/Line.h"
#include "Mesh/Sections/Section.h"
#include "Mesh/Points/Mechanic/Fiber.h"
#include "Mesh/Points/Mechanic/Section.h"
#include "Mesh/Points/Mechanic/Mechanic.h"
#include "Mesh/Elements/Mechanic/Frame/Frame.h"

namespace fea
{
	namespace mesh
	{
		namespace points
		{
			//constructors
			Section::Section(void)
			{
				memset(m_strain_old, 0, 6 * sizeof(double));
				memset(m_strain_new, 0, 6 * sizeof(double));
			}

			//destructor
			Section::~Section(void)
			{
				return;
			}

			//type
			points::type Section::type(void) const
			{
				return points::type::section;
			}

			//analysis
			void Section::prepare(const elements::Element* element)
			{
				//stress
				const unsigned t = ((const elements::Frame*) element)->stresses();
				//discretize
				m_fibers = ((cells::Line*) element->cell())->section()->discretize();
				//prepare
				for(Fiber& fiber : m_fibers)
				{
					fiber.prepare(t);
				}
			}
			
			void Section::update(void)
			{
				for(Fiber& fiber : m_fibers)
				{
					fiber.update();
				}
				memcpy(m_strain_old, m_strain_new, 6 * sizeof(double));
			}
			void Section::restore(void)
			{
				for(Fiber& fiber : m_fibers)
				{
					fiber.restore();
				}
				memcpy(m_strain_new, m_strain_old, 6 * sizeof(double));
			}
		}
	}
}
