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
			enum class dof : unsigned;
		}
		namespace elements
		{
			class Element;
		}
		namespace joints
		{
			class Joint;
		}
	}
	namespace boundary
	{
		class Initial;
		class Support;
		class Constraint;
		class Dependency;
		namespace loads
		{
			class Node;
		}
	}
	namespace analysis
	{
		class Assembler;
		namespace solvers
		{
			class Solver;
		}
	}

	namespace mesh
	{
		namespace nodes
		{
			class Node
			{
			public:
				//friends
				friend class mesh::Mesh;
				friend class mesh::joints::Joint;
				friend class mesh::elements::Element;
				
				friend class boundary::Initial;
				friend class boundary::Support;
				friend class boundary::Constraint;
				friend class boundary::Dependency;
				friend class boundary::loads::Node;
				
				friend class analysis::Assembler;
				friend class analysis::solvers::Solver;

			protected:
				//constructors
				Node(void);

				//destructor
				virtual ~Node(void);

				//serialization
				void load(FILE*);
				void save(FILE*) const;

			public:
				//data
				static Mesh* mesh(void);
				
				const double* coordinates(void) const;
				const double* coordinates(const double*);
				const double* coordinates(double, unsigned);
				const double* coordinates(double, double, double);

				//index
				unsigned index(void) const;

				//kinematics
				double* position(double*) const;
				const double* quaternion(void) const;
				const double* rotation(unsigned = 0) const;
				const double* translation(unsigned = 0) const;

				//state
				double state(dof, unsigned = 1) const;
				double velocity(dof, unsigned = 1) const;
				double acceleration(dof, unsigned = 1) const;

				//dof
				static const char* dof_name(dof);
				
				unsigned dofs(void) const;
				unsigned dof_index(dof) const;
				unsigned dof_index(unsigned) const;
				unsigned dof_types(void) const;

			protected:
				//analysis
				void add_dof_type(dof);
				void add_dof_type(unsigned);

				bool check(void) const;
				void prepare(unsigned&);
				void finish(void) const;

				void record(void);
				void update(void);
				void restore(void);
				
				void update_rotation(void);

				//apply
				void apply_state(void);
				void apply_velocity(void);
				void apply_acceleration(void);

				//increment
				void increment_state(void);
				void increment_velocity(void);
				void increment_acceleration(void);

				//results
				void plot(const double*, const double* = nullptr) const;

				//attributes
				static Mesh* m_mesh;

				double m_coordinates[3];

				unsigned m_dof_types;
				std::vector<unsigned> m_dof;

				double* m_quat_new;
				double* m_quat_old;
				double* m_state_old;
				double* m_state_new;
				double* m_velocity_old;
				double* m_velocity_new;
				double* m_acceleration_old;
				double* m_acceleration_new;

				std::string m_results[3];
			};
		}
	}
}
