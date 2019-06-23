//fea
#include "Model/Model.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Assembler/Assembler.h"
#include "Analysis/Solvers/Static_Linear.h"

namespace fea
{
	namespace analysis
	{
		namespace solvers
		{
			//static data
			static unsigned n;
			static Assembler* assembler;
			
			//constructors
			Static_Linear::Static_Linear(void)
			{
				return;
			}

			//destructor
			Static_Linear::~Static_Linear(void)
			{
				return;
			}

			//type
			solvers::type Static_Linear::type(void) const
			{
				return solvers::type::static_linear;
			}

			//sets
			unsigned Static_Linear::state_set(void) const
			{
				return (unsigned) solvers::state::u;
			}
			unsigned Static_Linear::force_set(void) const
			{
				return 
					(unsigned) solvers::force::R | 
					(unsigned) solvers::force::Fi | 
					(unsigned) solvers::force::Fr |
					(unsigned) solvers::force::Fd ;
			}
			unsigned Static_Linear::tangent_set(void) const
			{
				return (unsigned) solvers::tangent::K;
			}

			//analysis
			void Static_Linear::prepare(void)
			{
				return;
			}
			
			void Static_Linear::run(void)
			{
				//assembler
				assembler = m_analysis->assembler();
				//apply state
				assembler->apply_supports();
				assembler->apply_dependencies();
				assembler->apply_configurations();
				//assembly data
				assembler->assembly_stiffness();
				assembler->assembly_dead_force();
				assembler->assembly_internal_force();
				assembler->assembly_reference_force();
				//compute increment
				compute_increment();
				assembler->increment_state();
				assembler->apply_dependencies();
				assembler->apply_configurations();
				assembler->assembly_internal_force();
				//increment loop
				while (m_step <= m_step_max)
				{
					//compute
					compute_state();
					compute_reactions();
					//apply
					assembler->apply_state();
					assembler->apply_reactions();
					assembler->apply_dependencies();
					assembler->apply_configurations();
					assembler->apply_supports();
					//record
					assembler->record();
					//print
					printf("step: %04d dof: %+4.2e\n", m_step, dof());
					//increment
					m_step++;
				}
			}
			
			void Static_Linear::record(void)
			{
				char formatter[200];
				sprintf(formatter, "%+.6e\n", m_l);
				m_results[0] += formatter;
			}
			void Static_Linear::finish(void) const
			{
				const std::string path = m_analysis->model()->folder() + "/Solver/Load.txt";
				FILE* file = fopen(path.c_str(), "w");
				fprintf(file, m_results[0].c_str());
				fclose(file);
			}
			
			void Static_Linear::compute_state(void)
			{
				n = assembler->dof_unknow();
				m_l = (double) m_step / m_step_max;
				for(unsigned i = 0; i < n; i++)
				{
					m_u[i] = m_l * m_du[i];
				}
			}
			void Static_Linear::compute_increment(void)
			{
				n = assembler->dof_unknow();
				for(unsigned i = 0; i < n; i++)
				{
					m_Fru[i] += m_Fdu[i] - m_Fiu[i];
				}
				linsolve(m_du, m_K, m_Fru);
			}
			void Static_Linear::compute_reactions(void)
			{
				n = assembler->dof_know();
				for(unsigned i = 0; i < n; i++)
				{
					m_R[i] = m_l * (m_Fik[i] - m_Frk[i] - m_Fdk[i]);
				}
			}
		}
	}
}
