//fea
#include "Model/Model.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Solvers/Modal.h"
#include "Analysis/Assembler/Assembler.h"

namespace fea
{
	namespace analysis
	{
		namespace solvers
		{
			//static data
			static unsigned n, m;
			static Assembler* assembler;
			
			//constructors
			Modal::Modal(void)
			{
				return;
			}

			//destructor
			Modal::~Modal(void)
			{
				return;
			}
			
			//serialization
			void Modal::load(FILE* file)
			{
				Eigen::load(file);
			}
			void Modal::save(FILE* file) const
			{
				Eigen::save(file);
			}

			//type
			solvers::type Modal::type(void) const
			{
				return solvers::type::modal;
			}
			
			//sets
			unsigned Modal::state_set(void) const
			{
				return (unsigned) solvers::state::u;
			}
			unsigned Modal::force_set(void) const
			{
				return (unsigned) solvers::force::k;
			}
			unsigned Modal::tangent_set(void) const
			{
				return 
					(unsigned) solvers::tangent::f | 
					(unsigned) solvers::tangent::K | 
					(unsigned) solvers::tangent::M;
			}

			//analysis
			void Modal::prepare(void)
			{
				return;
			}
			
			void Modal::run(void)
			{
				//assembler
				assembler = m_analysis->assembler();
				//dofs
				m = modes();
				n = assembler->dof_unknow();
				//apply
				assembler->apply_supports();
				assembler->apply_initials();
				assembler->apply_dependencies();
				assembler->apply_configurations();
				//inertia and stiffness
				assembler->assembly_inertia();
				assembler->assembly_stiffness();
				//solve
				if(!eigen_gen())
				{
					printf("\tModal decomposition failed!\n");
					return;
				}
				//record
				while(m_step < m)
				{
					for(unsigned i = 0; i <  n; i++)
					{
						m_u[i] = m_scale * m_k[n * m_step + i];
					}
					assembler->apply_state();
					assembler->apply_dependencies();
					assembler->apply_configurations();
					assembler->record();
					printf("mode: %04d frequency: %+.2e dof: %+.2e\n", m_step, m_e[m_step], dof());
					m_step++;
				}
			}
			
			void Modal::record(void)
			{
				char formatter[200];
				sprintf(formatter, "%+.6e\n", m_e[m_step]);
				m_results[0] += formatter;
			}
			void Modal::finish(void) const
			{
				const std::string path = m_analysis->model()->folder() + "/Solver/Load.txt";
				FILE* file = fopen(path.c_str(), "w");
				fprintf(file, m_results[0].c_str());
				fclose(file);
			}
		}
	}
}
