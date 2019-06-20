#pragma once

#include <vector>

namespace fea
{
	//forward declarations
	namespace mesh
	{
		class Mesh;
	}
	namespace boundary
	{
		class Dependency;
	}
	namespace analysis
	{
		class Analysis;
		namespace solvers
		{
			class Solver;
		}
	}

	namespace analysis
	{
		class Assembler
		{
		public:
			//friends
			friend class mesh::Mesh;
			friend class analysis::Analysis;
			friend class analysis::solvers::Solver;

		protected:
			//constructors
			Assembler(void);

			//destructor
			~Assembler(void);

			//dof
			void map_dof(void);
			void add_dof(unsigned, unsigned);
			
			void sort_dof(void);
			void save_dof(void);
			void apply_dof(void);
			void count_dof(unsigned);
			void count_dof(const std::vector<unsigned>&);

			//memory
			void allocate(void);

		public:
			//analysis
			bool check(void) const;
			bool symmetric(void) const;
			void prepare(void);
			void finish(void) const;

			void record(void) const;
			void update(void) const;
			void restore(void) const;

			//dof
			unsigned dof_know(void) const;
			unsigned dof_local(void) const;
			unsigned dof_total(void) const;
			unsigned dof_unknow(void) const;
			unsigned dof_triplet(void) const;
			unsigned dof_nonzero(void) const;
			unsigned dof_dependent(void) const;

			//map
			const int* row_map(void) const;
			const int* col_map(void) const;
			const int* row_triplet(void) const;
			const int* col_triplet(void) const;

			//assembly
			void assembly_state(void) const;
			void assembly_velocity(void) const;
			void assembly_acceleration(void) const;

			void assembly_internal_force(void) const;
			void assembly_inertial_force(void) const;

			void assembly_dead_force(void) const;
			void assembly_external_force(void) const;
			void assembly_external_force(double) const;
			void assembly_reference_force(void) const;

			void assembly_inertia(void) const;
			void assembly_damping(void) const;
			void assembly_stiffness(void) const;

			//apply
			void apply_state(void) const;
			void apply_velocity(void) const;
			void apply_acceleration(void) const;

			void apply_initials(void) const;
			void apply_supports(void) const;
			void apply_dependencies(void) const;
			void apply_configurations(void) const;
			
			void apply_reactions(void) const;

			//increment
			void increment_state(void) const;
			void increment_velocity(void) const;
			void increment_acceleration(void) const;

		protected:
			//assembly
			void assembly_number(double, unsigned, double*, double*) const;
			void assembly_number(double, unsigned, unsigned, double*) const;
			void assembly_vector(const double*, const std::vector<unsigned>&, double*, double*) const;
			void assembly_matrix(const double*, const std::vector<unsigned>&, double*) const;
			
			void assembly_dependency(double, const boundary::Dependency*, double*, double*) const;
			void assembly_dependency(double, unsigned, const boundary::Dependency*, double*) const;
			void assembly_dependency(double, const boundary::Dependency*, unsigned, double*) const;
			void assembly_dependency(double, const boundary::Dependency*, const boundary::Dependency*, double*) const;

			void assembly_dead_force_nodes(double*, double*) const;
			void assembly_dead_force_elements(double*, double*) const;

			void assembly_internal_force_joints(double*, double*) const;
			void assembly_internal_force_elements(double*, double*) const;
			void assembly_internal_force_supports(double*, double*) const;

			void assembly_inertial_force_elements(double*, double*) const;
			void assembly_inertial_force_supports(double*, double*) const;

			void assembly_external_force_nodes(double*, double*, double) const;
			void assembly_external_force_elements(double*, double*, double) const;

			void assembly_reference_force_nodes(double*, double*) const;
			void assembly_reference_force_elements(double*, double*) const;

			void assembly_inertia_elements(double*) const;
			void assembly_inertia_supports(double*) const;

			void assembly_damping_elements(double*) const;
			void assembly_damping_supports(double*) const;

			void assembly_stiffness_joints(double*) const;
			void assembly_stiffness_elements(double*) const;
			void assembly_stiffness_supports(double*) const;

			//attributes
			static Analysis* m_analysis;

			int* m_row_map;
			int* m_col_map;
			int* m_row_triplet;
			int* m_col_triplet;

			mutable double m_qe;
			mutable double* m_ue;
			mutable double* m_ve;
			mutable double* m_ae;
			mutable double* m_fe;
			mutable double* m_ke;
			mutable unsigned* m_de;
			
			unsigned m_dof_know;
			unsigned m_dof_local;
			unsigned m_dof_total;
			unsigned m_dof_unknow;
			unsigned m_dof_triplet;
			unsigned m_dof_dependent;
			
			
		};
	}
}
