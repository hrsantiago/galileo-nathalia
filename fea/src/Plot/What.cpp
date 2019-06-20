#include "Plot/What.h"

namespace fea
{
	namespace plot
	{
		//constructors
		What::What(void)
		{
			return;
		}
		
		//destructor
		What::~What(void)
		{
			return;
		}
		
		//serialization
		void What::load(FILE* file)
		{
			fscanf(file, "%d", &m_axes);
			fscanf(file, "%d", &m_scale);
			fscanf(file, "%d", &m_nodes);
			fscanf(file, "%d", &m_loads);
			fscanf(file, "%d", &m_joints);
			fscanf(file, "%d", &m_elements);
			fscanf(file, "%d", &m_supports);
		}
		void What::save(FILE* file) const
		{
			fprintf(file, "%d ", m_axes);
			fprintf(file, "%d ", m_scale);
			fprintf(file, "%d ", m_nodes);
			fprintf(file, "%d ", m_loads);
			fprintf(file, "%d ", m_joints);
			fprintf(file, "%d ", m_elements);
			fprintf(file, "%d ", m_supports);
			fprintf(file, "\n");
		}
		
		//data
		bool What::axes(bool axes)
		{
			return m_axes = axes;
		}
		bool What::scale(bool scale)
		{
			return m_scale = scale;
		}
		bool What::nodes(bool nodes)
		{
			return m_nodes = nodes;
		}
		bool What::loads(bool loads)
		{
			return m_loads = loads;
		}
		bool What::joints(bool joints)
		{
			return m_joints = joints;
		}
		bool What::elements(bool elements)
		{
			return m_elements = elements;
		}
		bool What::supports(bool supports)
		{
			return m_supports = supports;
		}
		
		bool What::axes(void) const
		{
			return m_axes;
		}
		bool What::scale(void) const
		{
			return m_scale;
		}
		bool What::nodes(void) const
		{
			return m_nodes;
		}
		bool What::loads(void) const
		{
			return m_loads;
		}
		bool What::joints(void) const
		{
			return m_joints;
		}
		bool What::elements(void) const
		{
			return m_elements;
		}
		bool What::supports(void) const
		{
			return m_supports;
		}
		
		//info
		void What::info(void) const
		{
			printf("What\n");
			printf("\taxes:\t\t%d\n", m_axes);
			printf("\tscale:\t\t%d\n", m_scale);
			printf("\tnodes:\t\t%d\n", m_nodes);
			printf("\tloads:\t\t%d\n", m_loads);
			printf("\tjoints:\t\t%d\n", m_joints);
			printf("\telements:\t%d\n", m_elements);
			printf("\tsupports:\t%d\n", m_supports);
		}
		
		//config
		void What::reset(void)
		{
			m_axes = true;
			m_scale = true;
			m_nodes = true;
			m_loads = true;
			m_joints = true;
			m_elements = true;
			m_supports = true;
		}
	}
}
