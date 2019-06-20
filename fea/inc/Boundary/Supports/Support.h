#pragma once

#include <list>
#include <functional>

namespace fea
{
	//declarations
	namespace mesh
	{
		namespace nodes
		{
			class Node;
			enum class dof : unsigned;
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

	namespace boundary
	{
		class Support
		{
		public:
			//friends
			friend class boundary::Boundary;
			friend class analysis::Assembler;

		protected:
			//constructor
			Support(void);

			//destructor
			virtual ~Support(void);

			//serialization
			virtual void load(FILE*);
			virtual void save(FILE*) const;

		public:
			//data
			bool fixed(void) const;
			bool fixed(bool);
			
			mesh::nodes::dof dof(void) const;
			mesh::nodes::dof dof(mesh::nodes::dof);
			mesh::nodes::Node* node(void) const;
			mesh::nodes::Node* node(unsigned);

			double inertia(void) const;
			double inertia(double);
			double damping(void) const;
			double damping(double);
			double stiffness(void) const;
			double stiffness(double);

			std::function<double(double)> state(void) const;
			std::function<double(double)> state(std::function<double(double)>);
			std::function<double(double)> velocity(void) const;
			std::function<double(double)> velocity(std::function<double(double)>);
			std::function<double(double)> acceleration(void) const;
			std::function<double(double)> acceleration(std::function<double(double)>);

			//index
			unsigned index(void) const;
			unsigned index_node(void) const;

			//data
			double state(double) const;
			double velocity(double) const;
			double acceleration(double) const;
			
			double reaction(void) const;

		protected:
			//assembler
			void apply_values(void) const;
			void apply_reaction(void);

			//analysis
			bool check(void) const;
			void prepare(void);
			void record(void);
			void finish(void) const;
			void add_dof(void) const;

			//results
			virtual void plot(double, const double*, const double** = nullptr) const;
			
			virtual void plot_rotation_x(double, const double*, const double*, const double*) const;
			virtual void plot_rotation_y(double, const double*, const double*, const double*) const;
			virtual void plot_rotation_z(double, const double*, const double*, const double*) const;
			virtual void plot_temperature(double, const double*, const double*, const double*) const;
			virtual void plot_translation_x(double, const double*, const double*, const double*) const;
			virtual void plot_translation_y(double, const double*, const double*, const double*) const;
			virtual void plot_translation_z(double, const double*, const double*, const double*) const;

			//attributes
			static Boundary* m_boundary;

			bool m_fix;
			unsigned m_dof;
			unsigned m_node;
			mesh::nodes::dof m_dof_type;

			double m_inertia;
			double m_damping;
			double m_stiffness;

			double m_reaction;

			std::function<double(double)> m_state;
			std::function<double(double)> m_velocity;
			std::function<double(double)> m_acceleration;
			
			static std::list<unsigned> m_update_nodes;

			std::string m_results;
		};
	}
}
