#include <cstring>

#include "Plot/Colors.h"

namespace fea
{
	namespace plot
	{
		//constructors
		Colors::Colors(void)
		{
			return;
		}
		
		//destructor
		Colors::~Colors(void)
		{
			return;
		}
		
		//serialization
		void Colors::load(FILE* file)
		{
			double* data[] = 
			{
				m_back,
				m_nodes,
				m_loads,
				m_paths,
				m_joints,
				m_elements,
				m_supports
			};
			for(unsigned i = 0; i < sizeof(data) / sizeof(double*); i++)
			{
				for(unsigned j = 0; j < 4; j++)
				{
					fscanf(file, "%lf", &data[i][j]);
				}
			}
		}
		void Colors::save(FILE* file) const
		{
			const double* data[] = 
			{
				m_back,
				m_nodes,
				m_loads,
				m_paths,
				m_joints,
				m_elements,
				m_supports
			};
			for(unsigned i = 0; i < sizeof(data) / sizeof(double*); i++)
			{
				for(unsigned j = 0; j < 4; j++)
				{
					fprintf(file, "%.2f ", data[i][j]);
				}
				fprintf(file, "\n");
			}
		}
		
		//data
		double* Colors::back(double* back)
		{
			return (double*) memcpy(m_back, back, 4 * sizeof(double));
		}
		double* Colors::nodes(double* nodes)
		{
			return (double*) memcpy(m_nodes, nodes, 4 * sizeof(double));
		}
		double* Colors::loads(double* loads)
		{
			return (double*) memcpy(m_loads, loads, 4 * sizeof(double));
		}
		double* Colors::paths(double* paths)
		{
			return (double*) memcpy(m_paths, paths, 4 * sizeof(double));
		}
		double* Colors::joints(double* joints)
		{
			return (double*) memcpy(m_joints, joints, 4 * sizeof(double));
		}
		double* Colors::elements(double* elements)
		{
			return (double*) memcpy(m_elements, elements, 4 * sizeof(double));
		}
		double* Colors::supports(double* supports)
		{
			return (double*) memcpy(m_supports, supports, 4 * sizeof(double));
		}
		
		const double* Colors::back(void) const
		{
			return m_back;
		}
		const double* Colors::nodes(void) const
		{
			return m_nodes;
		}
		const double* Colors::loads(void) const
		{
			return m_loads;
		}
		const double* Colors::paths(void) const
		{
			return m_paths;
		}
		const double* Colors::joints(void) const
		{
			return m_joints;
		}
		const double* Colors::elements(void) const
		{
			return m_elements;
		}
		const double* Colors::supports(void) const
		{
			return m_supports;
		}
		
		//info
		void Colors::info(void) const
		{
			printf("Colors\n");
			printf("\tback:\t\t%.2f %.2f %.2f %.2f\n", m_back[0], m_back[1], m_back[2], m_back[3]);
			printf("\tnodes:\t\t%.2f %.2f %.2f %.2f\n", m_nodes[0], m_nodes[1], m_nodes[2], m_nodes[3]);
			printf("\tloads:\t\t%.2f %.2f %.2f %.2f\n", m_loads[0], m_loads[1], m_loads[2], m_loads[3]);
			printf("\tpaths:\t\t%.2f %.2f %.2f %.2f\n", m_paths[0], m_paths[1], m_paths[2], m_paths[3]);
			printf("\tjoints:\t\t%.2f %.2f %.2f %.2f\n", m_joints[0], m_joints[1], m_joints[2], m_joints[3]);
			printf("\telements:\t%.2f %.2f %.2f %.2f\n", m_elements[0], m_elements[1], m_elements[2], m_elements[3]);
			printf("\tsupports:\t%.2f %.2f %.2f %.2f\n", m_supports[0], m_supports[1], m_supports[2], m_supports[3]);
		}
		
		//config
		void Colors::reset(void)
		{
			double* data[] = 
			{
				m_back,
				m_nodes,
				m_loads,
				m_paths,
				m_joints,
				m_elements,
				m_supports
			};
			const double colors[][4] = {
				{0.00, 0.00, 0.00, 1.00},
				{1.00, 0.00, 0.00, 1.00},
				{1.00, 0.39, 0.28, 1.00},
				{0.50, 0.50, 0.50, 1.00},
				{0.00, 1.00, 0.00, 1.00},
				{0.00, 0.00, 1.00, 1.00},
				{0.50, 0.50, 0.50, 1.00}
			};
			for(unsigned i = 0; i < sizeof(data) / sizeof(double*); i++)
			{
				memcpy(data[i], colors[i], 4 * sizeof(double));
			}
		}
		void Colors::print(void)
		{
			double* data[] = 
			{
				m_back,
				m_nodes,
				m_loads,
				m_paths,
				m_joints,
				m_elements,
				m_supports
			};
			const double colors[][4] = {
				{1.00, 1.00, 1.00, 1.00},
				{0.70, 0.70, 0.70, 1.00},
				{1.00, 0.00, 0.00, 1.00},
				{0.00, 0.40, 0.00, 1.00},
				{0.00, 0.00, 0.00, 1.00},
				{0.00, 0.00, 1.00, 1.00},
				{0.70, 0.70, 0.70, 1.00}
			};
			for(unsigned i = 0; i < sizeof(data) / sizeof(double*); i++)
			{
				memcpy(data[i], colors[i], 4 * sizeof(double));
			}
		}
	}
}
