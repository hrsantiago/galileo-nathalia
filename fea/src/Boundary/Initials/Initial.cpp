//std
#include <algorithm>

//mat
#include "misc/util.h"

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Nodes/Node.h"

#include "Boundary/Boundary.h"
#include "Boundary/Initials/Initial.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Solver.h"

namespace fea
{
	namespace boundary
	{
		//costructors
		Initial::Initial(void)
		{
			return;
		}

		//destructor
		Initial::~Initial(void)
		{
			return;
		}

		//serialization
		void Initial::load(FILE* file)
		{
			fscanf(file, "%d %d %lf %lf", &m_node, &m_dof_type, &m_state, &m_velocity);
		}
		void Initial::save(FILE* file) const
		{
			fprintf(file, "%04d %02d %+.6e %+.6e", m_node, m_dof_type, m_state, m_velocity);
		}

		//data
		mesh::nodes::dof Initial::dof(void) const
		{
			return m_dof_type;
		}
		mesh::nodes::dof Initial::dof(mesh::nodes::dof dof_type)
		{
			return m_dof_type = dof_type;
		}
		mesh::nodes::Node* Initial::node(void) const
		{
			return m_boundary->model()->mesh()->node(m_node);
		}
		mesh::nodes::Node* Initial::node(unsigned node)
		{
			return m_boundary->model()->mesh()->node(m_node = node);
		}
		
		double Initial::state(void) const
		{
			return m_state;
		}
		double Initial::state(double state)
		{
			return m_state = state;
		}
		double Initial::velocity(void) const
		{
			return m_velocity;
		}
		double Initial::velocity(double velocity)
		{
			return m_velocity = velocity;
		}

		//index
		unsigned Initial::index(void) const
		{
			const std::vector<Initial*>& list = m_boundary->initials();
			return std::distance(list.begin(), std::find(list.begin(), list.end(), this));
		}
		unsigned Initial::index_node(void) const
		{
			return m_node;
		}

		//analysis
		bool Initial::check(void) const
		{
			//check node
			if(m_node >= m_boundary->model()->mesh()->nodes().size())
			{
				printf("Initial %04d has out of range node!\n", index());
				return false;
			}
			return true;
		}
		void Initial::prepare(void)
		{
			//set dof
			const mesh::nodes::Node* node = m_boundary->model()->mesh()->node(m_node);
			const unsigned char p = mat::bit_index(node->m_dof_types, (unsigned) m_dof_type);
			m_dof = node->m_dof[p];
		}
		void Initial::add_dof(void) const
		{
			m_boundary->model()->mesh()->node(m_node)->add_dof_type(m_dof_type);
		}

		//analysis
		void Initial::apply(void) const
		{
			//node
			const mesh::nodes::Node* node = m_boundary->model()->mesh()->node(m_node);
			const unsigned char p = mat::bit_index(node->m_dof_types, (unsigned) m_dof_type);
			//solver
			analysis::solvers::Solver* solver = m_boundary->model()->analysis()->solver();
			const unsigned solver_set = solver->state_set();
			//set state
			if(solver_set & (unsigned) analysis::solvers::state::u)
			{
				solver->m_u[m_dof] = m_state;
				node->m_state_old[p] = m_state;
				node->m_state_new[p] = m_state;
			}
			//set velocity
			if(solver_set & (unsigned) analysis::solvers::state::v)
			{
				solver->m_v[m_dof] = m_velocity;
				node->m_velocity_old[p] = m_velocity;
				node->m_velocity_new[p] = m_velocity;
			}
		}

		//static attributes
		Boundary* Initial::m_boundary = nullptr;
	}
}
