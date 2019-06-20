#include "Models/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Elements/Element.h"

#include "Boundary/Boundary.h"
#include "Boundary/Loads/Loads.h"
#include "Boundary/Cases/Load_Case.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Solver.h"

namespace fea
{
	namespace boundary
	{
		namespace loads
		{
			//constructor
			Element::Element(void)
			{
				return;
			}

			//destructor
			Element::~Element(void)
			{
				return;
			}

			//serialization
			void Element::load(FILE* file)
			{
				//base call
				Load::load(file);
				//load elements
				unsigned n;
				fscanf(file, "%d", &n);
				m_elements.resize(n);
				for(unsigned i = 0; i < m_elements.size(); i++)
				{
					fscanf(file, "%d", &m_elements[i]);
				}
			}
			void Element::save(FILE* file) const
			{
				//base call
				Load::save(file);
				//save elements
				fprintf(file, "%04d ", (unsigned) m_elements.size());
				for(unsigned e : m_elements)
				{
					fprintf(file, "%04d ", e);
				}
			}

			//create
			void Element::create(Element*& load, loads::type type)
			{
				//delete load
				delete load;
				//create
				switch(type)
				{
					case loads::type::axial:
						load = new Axial;
						break;
					case loads::type::shear:
						load = new Shear;
						break;
					case loads::type::torsion:
						break;
					case loads::type::bending:
						load = new Bending;
						break;
					case loads::type::plane_edge:
						load = new Plane_Edge;
						break;
					case loads::type::plane_face:
						load = new Plane_Face;
						break;
					case loads::type::solid_edge:
						load = new Solid_Edge;
						break;	
					case loads::type::solid_face:
						load = new Solid_Face;
						break;
					case loads::type::solid_body:
						load = new Solid_Body;
						break;
					case loads::type::heat_flux:
						break;
					case loads::type::heat_source:
						break;
				}
			}

			//add
			void Element::add_element(unsigned element)
			{
				m_elements.push_back(element);
			}

			//remove
			void Element::remove_element(unsigned index)
			{
				m_elements.erase(m_elements.begin() + index);
			}

			//index
			unsigned Element::index(void) const
			{
				const unsigned k = m_boundary->model()->analysis()->solver()->load_case();
				for(unsigned i = 0; i < m_boundary->load_case(k)->loads_elements(); i++)
				{
					if(this == m_boundary->load_case(k)->load_element(i))
					{
						return i;
					}
				}
				return 0;
			}

			//analysis
			bool Element::check(void) const
			{
				for(unsigned i = 0; i < m_elements.size(); i++)
				{
					if(m_elements[i] >= m_boundary->model()->mesh()->elements())
					{
						printf("Element load %02d has elements out of range\n", index());
						return false;
					}
				}
				return true;
			}
			
			//results
			void Element::plot(double, const double*, const double**) const
			{
				return;
			}
		}
	}
}
