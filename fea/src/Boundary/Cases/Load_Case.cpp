#include <vector>
#include <cstring>

#include "Boundary/Boundary.h"
#include "Boundary/Cases/Load_Case.h"
#include "Boundary/Loads/Nodes/Node.h"
#include "Boundary/Loads/Elements/Element.h"

namespace fea
{
	namespace boundary
	{
		//constructors
		Load_Case::Load_Case(void) : m_label("Loads")
		{
			loads::Load::m_boundary = m_boundary;
		}

		//destructor
		Load_Case::~Load_Case(void)
		{
			//delete loads nodes
			for(loads::Node*& load_node : m_loads_nodes)
			{
				delete load_node;
			}
			//delete loads elements
			for(loads::Element* load_element : m_loads_elements)
			{
				delete load_element;
			}
		}

		//serialization
		void Load_Case::load(FILE* file)
		{
			//load label
			fscanf(file, "%s", m_label);
			//load sizes
			unsigned nn, ne;
			fscanf(file, "%d %d", &nn, &ne);
			//load nodes
			m_loads_nodes.resize(nn);
			for(loads::Node*& load_node : m_loads_nodes)
			{
				load_node = new loads::Node;
				load_node->load(file);
			}
			//load elements
			loads::type type;
			m_loads_elements.resize(ne);
			for(loads::Element*& load_element : m_loads_elements)
			{
				fscanf(file, "%d", &type);
				loads::Element::create(load_element, type);
				load_element->load(file);
			}
		}
		void Load_Case::save(FILE* file) const
		{
			//save label
			fprintf(file, "%s ", m_label);
			//save sizes
			fprintf(file, "%04d %04d\n", (unsigned) m_loads_nodes.size(), (unsigned) m_loads_elements.size());
			//save nodes
			for(const loads::Node* load_node : m_loads_nodes)
			{
				load_node->save(file);
				fprintf(file, "\n");
			}
			//save elements
			for(const loads::Element* load_element : m_loads_elements)
			{
				fprintf(file, "%02d ", load_element->type());
				load_element->save(file);
				fprintf(file, "\n");
			}
		}

		//data
		const char* Load_Case::label(void) const
		{
			return m_label;
		}
		const char* Load_Case::label(const char* label)
		{
			return strcpy(m_label, label);
		}
		loads::Node* Load_Case::load_node(unsigned index) const
		{
			return m_loads_nodes[index];
		}
		loads::Element* Load_Case::load_element(unsigned index) const
		{
			return m_loads_elements[index];
		}

		//sizes
		unsigned Load_Case::loads_nodes(void) const
		{
			return (unsigned) m_loads_nodes.size();
		}
		unsigned Load_Case::loads_elements(void) const
		{
			return (unsigned) m_loads_elements.size();
		}

		//index
		unsigned Load_Case::index(void) const
		{
			for(unsigned i = 0; i < m_boundary->load_cases(); i++)
			{
				if(m_boundary->load_case(i) == this)
				{
					return i;
				}
			}
			return 0;
		}

		//add
		void Load_Case::add_load_node(unsigned node, mesh::nodes::dof dof, double value, std::function<double(double)> function)
		{
			loads::Node* load = new loads::Node;
			load->m_node = node;
			load->m_value = value;
			load->m_dof_type = dof;
			load->m_function = function;
			m_loads_nodes.push_back(load);
		}
		void Load_Case::add_load_element(loads::type type, std::vector<unsigned> elements, double value, std::function<double(double)> function)
		{
			loads::Element* load = nullptr;
			loads::Element::create(load, type);
			load->m_value = value;
			load->m_elements = elements;
			load->m_function = function;
			m_loads_elements.push_back(load);
		}

		//remove
		void Load_Case::remove_load_node(unsigned index)
		{
			m_loads_nodes.erase(m_loads_nodes.begin(), m_loads_nodes.begin() + index);
		}
		void Load_Case::remove_load_element(unsigned index)
		{
			m_loads_elements.erase(m_loads_elements.begin(), m_loads_elements.begin() + index);
		}

		//analysis
		bool Load_Case::check(void) const
		{
			//check node loads
			for(const loads::Node* load_node : m_loads_nodes)
			{
				if(!load_node->check())
				{
					return false;
				}
			}
			//check element loads
			for(const loads::Element* load_element : m_loads_elements)
			{
				if(!load_element->check())
				{
					return false;
				}
			}
			//return
			return true;
		}
		void Load_Case::prepare(void)
		{
			for(loads::Node* load_node : m_loads_nodes)
			{
				load_node->prepare();
			}
		}
		
		//results
		void Load_Case::plot(double size, const double* color, const double** positions) const
		{
			for(const loads::Node* load_node : m_loads_nodes)
			{
				load_node->plot(size, color, positions);
			}
			for(const loads::Element* load_element : m_loads_elements)
			{
				load_element->plot(size, color, positions);
			}
		}

		//static attributes
		Boundary* Load_Case::m_boundary = nullptr;
	}
}
