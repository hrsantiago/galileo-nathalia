#pragma once

#include <list>
#include <vector>
#include <functional>

namespace fea
{
	//declarations
	namespace mesh
	{
		namespace nodes
		{
			enum class dof : unsigned;
		}
		namespace joints
		{
			class Joint;
		}
	}
	namespace boundary
	{
		class Boundary;
	}
	namespace analysis
	{
		class Assembler;
	}

	//class
	namespace boundary
	{
		class Dependency
		{
		public:
			//friends
			friend class boundary::Boundary;
			friend class analysis::Assembler;
			friend class mesh::joints::Joint;

		protected:
			//constructors
			Dependency(void);

			//destructor
			virtual ~Dependency(void);

			//serialization
			void load(FILE*);
			void save(FILE*) const;

		public:
			//data
			mesh::nodes::dof slave_dof(void) const;
			mesh::nodes::dof slave_dof(mesh::nodes::dof);
			
			mesh::nodes::dof master_dof(unsigned) const;
			mesh::nodes::dof master_dof(unsigned, mesh::nodes::dof);
			
			mesh::nodes::Node* slave_node(void) const;
			mesh::nodes::Node* slave_node(unsigned);
			
			mesh::nodes::Node* master_node(unsigned) const;
			mesh::nodes::Node* master_node(unsigned, unsigned);
			
			std::function<double(double*)> state(void) const;
			std::function<double(double*)> state(std::function<double(double*)>);
			
			std::function<double(double*, unsigned)> gradient(void) const;
			std::function<double(double*, unsigned)> gradient(std::function<double(double*, unsigned)>);
			
			std::function<double(double*, unsigned, unsigned)> hessian(void) const;
			std::function<double(double*, unsigned, unsigned)> hessian(std::function<double(double*, unsigned, unsigned)>);

			//index
			unsigned index(void) const;
			unsigned index_slave_dof(void) const;
			unsigned index_master_dof(unsigned) const;
			
			//sizes
			unsigned masters(void) const;

			//add
			void add_master(unsigned, mesh::nodes::dof);

			//remove
			void remove_master(unsigned);

		protected:
			//state
			double* master_state(double*) const;
			double* master_velocity(double*) const;		
			double* master_acceleration	(double*) const;
		
			//analysis
			bool check(void) const;
			void prepare(void);
			void add_dof(void) const;

			//assembler
			void apply(double*, double*, double*) const;

			//attributes
			static Boundary* m_boundary;

			unsigned m_slave_node;
			unsigned m_slave_dof_index;
			mesh::nodes::dof m_slave_dof;
			
			std::vector<unsigned> m_masters_node;
			std::vector<unsigned> m_masters_dof_index;
			std::vector<mesh::nodes::dof> m_masters_dof;
			
			std::function<double(double*)> m_state;
			std::function<double(double*, unsigned)> m_gradient;
			std::function<double(double*, unsigned, unsigned)> m_hessian;
			
			static std::list<unsigned> m_update_nodes;
		};
	}
}
