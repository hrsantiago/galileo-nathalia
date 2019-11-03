//fea
#include "Model/Model.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Assembler/Assembler.h"
#include "Analysis/Solvers/Dynamic_Linear.h"

namespace fea
{
	namespace analysis
	{
		namespace solvers
		{
			//static data
			static unsigned n;
			static double b, g;
			static Assembler* assembler;
			
			//constructors
			Dynamic_Linear::Dynamic_Linear(void)
			{
				return;
			}

			//destructor
			Dynamic_Linear::~Dynamic_Linear(void)
			{
				return;
			}
			
			//serialization
			void Dynamic_Linear::load(FILE* file)
			{
				Solver::load(file);
				Time::load(file);
			}
			void Dynamic_Linear::save(FILE* file) const
			{
				Solver::save(file);
				Time::save(file);
			}

			//type
			solvers::type Dynamic_Linear::type(void) const
			{
				return solvers::type::dynamic_linear;
			}

			//sets
			unsigned Dynamic_Linear::state_set(void) const
			{
				return 
					(unsigned) solvers::state::u | 
					(unsigned) solvers::state::v | 
					(unsigned) solvers::state::a;
			}
			unsigned Dynamic_Linear::force_set(void) const
			{
				return 
					(unsigned) solvers::force::R | 
					(unsigned) solvers::force::Fi | 
					(unsigned) solvers::force::Fn | 
					(unsigned) solvers::force::Fe;
			}
			unsigned Dynamic_Linear::tangent_set(void) const
			{
				return 
					(unsigned) solvers::tangent::K | 
					(unsigned) solvers::tangent::C | 
					(unsigned) solvers::tangent::M;
			}

			//analysis
			void Dynamic_Linear::prepare(void)
			{
				Time::prepare();
			}
			
			void Dynamic_Linear::run(void)
			{
				//assembler
				assembler = m_analysis->assembler();
				//apply state
				assembler->apply_initials();
				assembler->apply_supports();
				assembler->apply_dependencies();
				assembler->apply_configurations();
				//assembly data
				assembler->assembly_inertia();
				assembler->assembly_damping();
				assembler->assembly_stiffness();
				assembler->assembly_internal_force();
				assembler->assembly_external_force();
				//compute acceleration
				compute_acceleration();
				assembler->apply_acceleration();
				assembler->assembly_inertial_force();
				//compute reaction
				compute_reactions();
				assembler->apply_reactions();
				//record
				assembler->record();
				//system matrix
				compute_system_matrix();
				//time loop
				while(m_step < m_step_max)
				{
					//update time
					m_t += m_dt;
					//update supports and constrains
					assembler->apply_supports();
					assembler->apply_dependencies();
					//predictor
					compute_predictor();
					//apply state
					assembler->apply_state();
					assembler->apply_velocity();
					assembler->apply_configurations();
					//assembly forces
					assembler->assembly_external_force();
					assembler->assembly_internal_force();
					//compute acceleration
					compute_acceleration();
					//compute corector
					compute_corrector();
					//set data
					assembler->apply_state();
					assembler->apply_velocity();
					assembler->apply_acceleration();
					assembler->apply_configurations();
					assembler->assembly_inertial_force();
					assembler->assembly_external_force();
					//reactions
					compute_reactions();
					//record
					assembler->record();
					//print
					printf("step: %04d time: %.2e dof: %+.2e\n", m_step, m_t, dof());
					//increment step
					m_step++;
				}
			}

			void Dynamic_Linear::compute_predictor(void)
			{
				n = assembler->dof_unknow();
				for(unsigned i = 0; i < n; i++)
				{
					m_v[i] += (1 - g) * m_dt * m_a[i];
					m_u[i] += m_dt * m_v[i] + (2 * (g - b) - 1) * m_dt * m_dt * m_a[i] / 2;
				}
			}
			void Dynamic_Linear::compute_corrector(void)
			{
				n = assembler->dof_unknow();
				for(unsigned i = 0; i < n; i++)
				{
					m_v[i] += g * m_dt * m_a[i];
					m_u[i] += b * m_dt * m_dt * m_a[i];
				}
			}
			void Dynamic_Linear::compute_reactions(void)
			{
				n = assembler->dof_know();
				for(unsigned i = 0; i < n; i++)
				{
					m_R[i] = m_Fik[i] + m_Fnk[i] - m_Fek[i];
				}
			}
			void Dynamic_Linear::compute_acceleration(void)
			{
				n = assembler->dof_unknow();
				for(unsigned i = 0; i < n; i++)
				{
					m_Feu[i] -= m_Fiu[i];
				}
				linsolve(m_a, m_M, m_Feu);
			}
			void Dynamic_Linear::compute_system_matrix(void)
			{
				b = m_newmark.beta;
				g = m_newmark.gamma;
				n = assembler->dof_nonzero();
				for(unsigned i = 0; i < n; i++)
				{
					m_M[i] += g * m_dt * m_C[i] + b * m_dt * m_dt * m_K[i];
				}
			}
		}
	}
}
