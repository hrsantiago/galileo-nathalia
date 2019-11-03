#pragma once

#include <cstdio>
#include <string>

namespace fea
{
	//declarations
	namespace mesh
	{
		namespace nodes
		{
			enum class dof : unsigned;
		}
	}
	namespace boundary
	{
		class Initial;
		class Support;
	}
	namespace analysis
	{
		class Analysis;
		class Assembler;
		namespace solvers
		{
			class Watch_Dof;
			enum class type : unsigned;
		}
	}

	namespace analysis
	{
		namespace solvers
		{
			//enumerations
			enum class state : unsigned
			{
				u = 1 << 0,
				v = 1 << 1,
				a = 1 << 2
			};
			enum class force : unsigned
			{
				r = 1 << 0,
				R = 1 << 1,
				k = 1 << 2,
				Fi = 1 << 3,
				Fn = 1 << 4,
				Fe = 1 << 5,
				Fr = 1 << 6,
				Fd = 1 << 7
			};
			enum class tangent : unsigned
			{
				f = 1 << 0,
				K = 1 << 1,
				C = 1 << 2,
				M = 1 << 3
			};
			
			//class
			class Solver
			{
			public:
				//friends
				friend class boundary::Initial;
				friend class boundary::Support;
				friend class analysis::Analysis;
				friend class analysis::Assembler;

			protected:
				//constructors
				Solver(void);

				//destructor
				virtual ~Solver(void);

				//create
				static void create(Solver*&, solvers::type);

				//serialization
				virtual void load(FILE*);
				virtual void save(FILE*) const;

			public:
				//data
				bool dead(void) const;
				bool dead(bool);
				
				double dof_min(void) const;
				double dof_min(double);
				double dof_max(void) const;
				double dof_max(double);
				
				unsigned step_max(void) const;
				unsigned step_max(unsigned);
				unsigned load_case(void) const;
				unsigned load_case(unsigned);
				unsigned dead_case(void) const;
				unsigned dead_case(unsigned);
				
				const Watch_Dof* watch_dof(void);
				const Watch_Dof* watch_dof(unsigned, mesh::nodes::dof);

				//type
				virtual solvers::type type(void) const = 0;

				//name
				virtual const char* name(void) const;
				virtual const char* parameter(void) const;

				//sets
				virtual unsigned state_set(void) const = 0;
				virtual unsigned force_set(void) const = 0;
				virtual unsigned tangent_set(void) const = 0;

				//loops
				unsigned step(void) const;

				//dof
				double dof(void) const;

				//data
				double time(void) const;
				double time_min(void) const;
				double time_min(double);
				double time_max(void) const;
				double time_max(double);
				double time_increment(void) const;
				double time_increment_max(void) const;
				double time_increment_max(double);
				
				double load(void) const;
				double load_increment(void) const;
				double load_predictor(void) const;
				double load_corrector(void) const;
				
				const double* state(void) const;
				const double* state_increment(void) const;
				const double* state_predictor(void) const;
				const double* state_corrector_residue(void) const;
				const double* state_corrector_tangent(void) const;

				const double* velocity(void) const;
				const double* velocity_increment(void) const;

				const double* acceleration(void) const;
				const double* acceleration_increment(void) const;
				
				const double* residue(void) const;
				const double* reaction(void) const;
				
				const double* inertial_force(bool = true) const;
				const double* internal_force(bool = true) const;
				const double* external_force(bool = true) const;
				const double* reference_force(bool = true) const;
				
				const double* inertia(void) const;
				const double* damping(void) const;
				const double* stiffness(void) const;
				
			protected:
				//analysis
				virtual bool check(void) const;
				virtual void prepare(void);
				
				virtual void run(void) = 0;
				
				virtual bool stop(bool) const;
				
				virtual void record(void) = 0;
				virtual void finish(void) const = 0;
				
				//linear
				void lindel(void);
				bool lindec(const double*);
				bool linsub(double*, const double*, const double*);
				bool linsolve(double*, const double*, const double*);
				
				//attributes
				static Analysis* m_analysis;

				bool m_dead;

				void* m_sym;
				void* m_num;
				
				unsigned m_step;
				unsigned m_step_max;
				unsigned m_load_case;
				unsigned m_dead_case;

				double m_dof_min;
				double m_dof_max;
				
				double m_time_min;
				double m_time_max;
				double m_time_increment_max;

				double m_t, m_dt;
				double m_l, m_dl, m_dl0, m_ddl;

				double *m_f, *m_K, *m_C, *m_M;
				double *m_u, *m_v, *m_a, *m_du, *m_dv, *m_da, *m_dut, *m_ddu, *m_ddur, *m_ddut;
				double *m_r, *m_R, *m_k, *m_Fiu, *m_Fik, *m_Fnu, *m_Fnk, *m_Feu, *m_Fek, *m_Fru, *m_Frk, *m_Fdu, *m_Fdk;

				Watch_Dof* m_watch_dof;

				std::string m_results[2];
			};
		}
	}
}
