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
#include "Boundary/Supports/Support.h"
#include "Boundary/Dependencies/Dependency.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Solver.h"

namespace fea
{
	namespace boundary
	{
		//constructors
		Dependency::Dependency(void) :
			m_state([] (double* d) { return d[0]; }), 
			m_gradient([] (double*, unsigned) { return 1; }), 
			m_hessian([] (double*, unsigned, unsigned) { return 0; }),
			m_slave_node(0), m_slave_dof_index(0), m_slave_dof(mesh::nodes::dof::translation_x),
			m_masters_node({ 0 }), m_masters_dof_index({ 0 }), m_masters_dof({ mesh::nodes::dof::translation_y })
		{
			return;
		}

		//destructor
		Dependency::~Dependency(void)
		{
			return;
		}

		//serialization
		void Dependency::load(FILE* file)
		{
			unsigned n;
			fscanf(file, "%d %d %d", &m_slave_node, &m_slave_dof, &n);
			m_masters_dof.resize(n);
			m_masters_node.resize(n);
			for(unsigned i = 0; i < n; i++)
			{
				fscanf(file, "%d %d", &m_masters_node[i], &m_masters_dof[i]);
			}
		}
		void Dependency::save(FILE* file) const
		{
			fprintf(file, "%04d %02d %02d ", m_slave_node, m_slave_dof, m_masters_dof.size());
			for(unsigned i = 0; i < m_masters_dof.size(); i++)
			{
				fprintf(file, "%04d %02d ", m_masters_node[i], m_masters_dof[i]);
			}
		}

		//data
		mesh::nodes::dof Dependency::slave_dof(void) const
		{
			return m_slave_dof;
		}
		mesh::nodes::dof Dependency::slave_dof(mesh::nodes::dof slave_dof)
		{
			return m_slave_dof = slave_dof;
		}
		
		mesh::nodes::dof Dependency::master_dof(unsigned index) const
		{
			return m_masters_dof[index];
		}
		mesh::nodes::dof Dependency::master_dof(unsigned index, mesh::nodes::dof dof)
		{
			return m_masters_dof[index] = dof;
		}
		
		mesh::nodes::Node* Dependency::slave_node(void) const
		{
			return m_boundary->model()->mesh()->node(m_slave_node);
		}
		mesh::nodes::Node* Dependency::slave_node(unsigned slave_node)
		{
			return m_boundary->model()->mesh()->node(m_slave_node = slave_node);
		}
		
		mesh::nodes::Node* Dependency::master_node(unsigned index) const
		{
			return m_boundary->model()->mesh()->node(m_masters_node[index]);
		}
		mesh::nodes::Node* Dependency::master_node(unsigned index, unsigned master_node)
		{
			return m_boundary->model()->mesh()->node(m_masters_node[index] = master_node);
		}
		
		std::function<double(double*)> Dependency::state(void) const
		{
			return m_state;
		}
		std::function<double(double*)> Dependency::state(std::function<double(double*)> state)
		{
			return m_state = state;
		}
		std::function<double(double*, unsigned)> Dependency::gradient(void) const
		{
			return m_gradient;
		}
		std::function<double(double*, unsigned)> Dependency::gradient(std::function<double(double*, unsigned)> gradient)
		{
			return m_gradient = gradient;
		}
		std::function<double(double*, unsigned, unsigned)> Dependency::hessian(void) const
		{
			return m_hessian;
		}
		std::function<double(double*, unsigned, unsigned)> Dependency::hessian(std::function<double(double*, unsigned, unsigned)> hessian)
		{
			return m_hessian = hessian;
		}
		
		//index
		unsigned Dependency::index(void) const
		{
			const std::vector<Dependency*>& list = m_boundary->dependencies();
			return std::distance(list.begin(), std::find(list.begin(), list.end(), this));
		}
		unsigned Dependency::index_slave_dof(void) const
		{
			return m_slave_dof_index;
		}
		unsigned Dependency::index_master_dof(unsigned i) const
		{
			return m_masters_dof_index[i];
		}
		
		//sizes
		unsigned Dependency::masters(void) const
		{
			return m_masters_node.size();
		}
		
		//add
		void Dependency::add_master(unsigned node, mesh::nodes::dof dof)
		{
			m_masters_dof.push_back(dof);
			m_masters_node.push_back(node);
		}

		//remove
		void Dependency::remove_master(unsigned index)
		{
			m_masters_dof.erase(m_masters_dof.begin() + index);
			m_masters_node.erase(m_masters_node.begin() + index);
		}

		//state
		double* Dependency::master_state(double* d) const
		{
			for(unsigned i = 0; i < m_masters_dof.size(); i++)
			{
				d[i] = m_boundary->model()->mesh()->node(m_masters_node[i])->state(m_masters_dof[i]);
			}
			return d;
		}
		double* Dependency::master_velocity(double* v) const
		{
			for(unsigned i = 0; i < m_masters_dof.size(); i++)
			{
				v[i] = m_boundary->model()->mesh()->node(m_masters_node[i])->velocity(m_masters_dof[i]);
			}
			return v;
		}
		double* Dependency::master_acceleration(double* a) const
		{
			for(unsigned i = 0; i < m_masters_dof.size(); i++)
			{
				a[i] = m_boundary->model()->mesh()->node(m_masters_node[i])->acceleration(m_masters_dof[i]);
			}
			return a;
		}

		//analysis
		bool Dependency::check(void) const
		{
			//sizes
			const unsigned nm = m_masters_dof.size();
			const unsigned nn = m_boundary->model()->mesh()->nodes().size();
			//size consistency
			if(nm != m_masters_node.size())
			{
				printf("master nodes and dofs have different sizes!\n");
				return false;
			}
			//inner consistency
			for(unsigned i = 0; i < nm; i++)
			{
				if(m_slave_node == m_masters_node[i] && m_slave_dof == m_masters_dof[i])
				{
					const char msg[] = "Dependency %02d (Node: %04d dof: %s) is inner inconsistent!\n";
					printf(msg, index(), m_slave_node, mesh::nodes::Node::dof_name(m_slave_dof));
					return false;
				}
			}
			//check nodes
			if(m_slave_node >= nn)
			{
				printf("Dependency %02d has out of range slave node!\n", index());
				return false;
			}
			for(unsigned i = 0; i < nm; i++)
			{
				if(m_masters_node[i] >= nn)
				{
					printf("Dependency %02d has out of range master node!\n", index());
					return false;
				}
			}
			//check initials
			for(const Initial* initial : m_boundary->initials())
			{
				if(m_slave_node == initial->index_node() && m_slave_dof == initial->dof())
				{
					printf("Dependency %02d is incompatible with initial %02d!\n", index(), initial->index());
					return false;
				}
			}
			//check supports
			for(const Support* support : m_boundary->supports())
			{
				if(support->fixed() && m_slave_node == support->index_node() && m_slave_dof == support->dof())
				{
					printf("Dependency %02d is incompatible with support %02d!\n", index(), support->index());
					return false;
				}
			}
			//check dependencies
			for(Dependency* dependency : m_boundary->dependencies())
			{
				if(dependency != this)
				{
					if(m_slave_node == dependency->m_slave_node && m_slave_dof == dependency->m_slave_dof)
					{
						const char msg[] = "Dependency %02d (Node: %04d dof: %s) is incompatible with dependency %02d!\n";
						printf(msg, index(), m_slave_node, mesh::nodes::Node::dof_name(m_slave_dof), dependency->index());
						return false;
					}
				}
			}
			return true;
		}
		void Dependency::prepare(void)
		{
			m_masters_dof_index.resize(m_masters_node.size());
			m_slave_dof_index = slave_node()->dof_index(m_slave_dof);
			for(unsigned i = 0; i < m_masters_dof.size(); i++)
			{
				m_masters_dof_index[i] = master_node(i)->dof_index(m_masters_dof[i]);
			}
		}
		void Dependency::add_dof(void) const
		{
			slave_node()->add_dof_type(m_slave_dof);
			for(unsigned i = 0; i < m_masters_dof.size(); i++)
			{
				m_boundary->model()->mesh()->node(m_masters_node[i])->add_dof_type(m_masters_dof[i]);
			}
		}

		//assembler
		void Dependency::apply(double* d, double* v, double* a) const
		{
			//slave data
			const mesh::nodes::Node* ns = slave_node();
			double* nd[] = {ns->m_state_new, ns->m_velocity_new, ns->m_acceleration_new};
			const unsigned char ps = mat::bit_index(ns->m_dof_types, (unsigned) m_slave_dof);
			//master data
			const unsigned n = m_masters_dof.size();
			//solver set
			const unsigned ss = m_boundary->model()->analysis()->solver()->state_set();
			//state
			if(ss & (unsigned) analysis::solvers::state::u)
			{
				master_state(d);
				nd[0][ps] = m_state(d);
			}
			//state
			if(ss & (unsigned) analysis::solvers::state::v)
			{
				nd[1][ps] = 0;
				master_velocity(v);
				for(unsigned i = 0; i < n; i++)
				{
					nd[1][ps] += m_gradient(d, i) * v[i];
				}
			}
			//state
			if(ss & (unsigned) analysis::solvers::state::a)
			{
				nd[2][ps] = 0;
				master_acceleration(a);
				for(unsigned i = 0; i < n; i++)
				{
					nd[2][ps] += m_gradient(d, i) * a[i];
					for(unsigned j = 0; j < n; j++)
					{
						nd[2][ps] += m_hessian(d, i, j) * v[i] * v[j];
					}
				}
			}
		}

		//static attributes
		Boundary* Dependency::m_boundary = nullptr;
		std::list<unsigned> Dependency::m_update_nodes;
	}
}
