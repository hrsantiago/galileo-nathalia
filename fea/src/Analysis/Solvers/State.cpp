//fea
#include "Models/Model.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Solvers/State.h"
#include "Analysis/Assembler/Assembler.h"

namespace fea
{
	namespace analysis
	{
		namespace solvers
		{
			//static data
			static int n;
			static Assembler* assembler;
			
			//constructors
			State::State(void)
			{
				return;
			}

			//destructor
			State::~State(void)
			{
				return;
			}

			//serialization
			void State::load(FILE* file)
			{
				Solver::load(file);
				Nonlinear::load(file);
			}
			void State::save(FILE* file) const
			{
				Solver::save(file);
				Nonlinear::save(file);
			}

			//type
			solvers::type State::type(void) const
			{
				return solvers::type::state;
			}

			//sets
			unsigned State::state_set(void) const
			{
				return (unsigned) solvers::state::u;
			}
			unsigned State::force_set(void) const
			{
				return 
					(unsigned) solvers::force::R | 
					(unsigned) solvers::force::Fi | 
					(unsigned) solvers::force::Fr | 
					(unsigned) solvers::force::Fd;
			}
			unsigned State::tangent_set(void) const
			{
				return 0;
			}

			//analysis
			void State::run(void)
			{
				//assembler
				assembler = m_analysis->assembler();
				//apply configuration
				assembler->apply_supports();
				assembler->apply_dependencies();
				assembler->apply_configurations();
				//record
				assembler->record();
				//assembly forces
				assembler->assembly_dead_force();
				assembler->assembly_reference_force();
				//incremental loop
				while (m_step < m_step_max)
				{
					//increment
					m_t += m_dt;
					//apply configuration
					assembler->apply_supports();
					assembler->apply_dependencies();
					assembler->apply_configurations();
					//compute internal force
					assembler->assembly_internal_force();
					//compute reactions
					compute_reactions();
					assembler->apply_reactions();
					//print
					printf("step: %04d dof: %+.2e\n", m_step, dof());
					//record and update
					assembler->record();
					assembler->update();
					//increment
					m_step++;
				}
			}
			
			void State::record(void)
			{
				char formatter[200];
				sprintf(formatter, "%+.6e\n", m_t);
				m_results[0] += formatter;
			}
			void State::finish(void) const
			{
				const std::string path = m_analysis->model()->folder() + "/Solver/Time.txt";
				FILE* file = fopen(path.c_str(), "w");
				fprintf(file, m_results[0].c_str());
				fclose(file);
			}
			
			void State::compute_reactions(void)
			{
				n = assembler->dof_know();
				for(unsigned i = 0; i < n; i++)
				{
					m_R[i] = m_Fik[i] - m_Fdk[i] - m_Frk[i];
				}
			}
		}
	}
}
