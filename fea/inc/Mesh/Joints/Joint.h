#pragma once

#include <string>
#include <vector>

namespace fea
{
	//declarations
	namespace mesh
	{
		class Mesh;
		namespace nodes
		{
			class Node;
		}
		namespace joints
		{
			enum class type : unsigned;
			enum class state : unsigned;
		}
	}
	namespace analysis
	{
		class Assembler;
	}
	namespace boundary
	{
		class Boundary;
		class Dependency;
	}

	namespace mesh
	{
		namespace joints
		{
			class Joint
			{
			public:
				//friends
				friend class mesh::Mesh;
				friend class boundary::Boundary;
				friend class analysis::Assembler;

			protected:
				//constructors
				Joint(void);

				//destructor
				virtual ~Joint(void);

				//serialization
				virtual void load(FILE*);
				virtual void save(FILE*) const;

				//create
				static void create(Joint*&, const Joint*);
				static void create(Joint*&, joints::type, const Joint* = nullptr);

			public:
				//data
				static Mesh* mesh(void);
				
				double mass(void) const;
				double mass(double);
				
				nodes::Node* node(unsigned) const;
				nodes::Node* node(unsigned, unsigned);

				//types
				virtual unsigned states(void) const = 0;
				virtual joints::type type(void) const = 0;

				//state
				static const char* state_name(state);

				//dofs
				virtual std::vector<unsigned> dofs(void) const = 0;

				//sizes
				unsigned nodes(void) const;
				unsigned dependencies(void) const;

				//index
				unsigned index(void) const;
				unsigned index_node(unsigned) const;
				
				//add
				void add_node(unsigned);
				
				//remove
				void remove_node(unsigned);

			protected:
				//analysis
				void add_dof(void) const;

				virtual void apply(void) = 0;

				virtual bool check(void) const;
				virtual void prepare(void);
				virtual void configure(void);
				
				virtual void update(void);
				virtual void restore(void);
				
				virtual void record(void);
				virtual void finish(void) const;
				
				virtual boundary::Dependency* create(void);
				
				//formulation
				virtual double* internal_force(double*) const = 0;
				
				virtual double* damping(double*) const = 0;
				virtual double* stiffness(double*) const = 0;

				//results
				virtual void plot(double, const double*, const double** = nullptr) const = 0;

				//attributes
				double m_mass;
				
				static Mesh* m_mesh;

				std::string m_results;

				std::vector<unsigned> m_dof;
				std::vector<unsigned> m_nodes;
				
				std::vector<boundary::Dependency*> m_dependencies;
			};
		}
	}
}
