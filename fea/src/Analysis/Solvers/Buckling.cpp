//mat
#include "linear/dense.h"

//fea
#include "Model/Model.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Solvers/Buckling.h"
#include "Analysis/Assembler/Assembler.h"

namespace fea
{
	namespace analysis
	{
		namespace solvers
		{
			//static data
			static unsigned n;
			static unsigned m;
			static Assembler* assembler;
			
			//constructors
			Buckling::Buckling(void)
			{
				return;
			}

			//destructor
			Buckling::~Buckling(void)
			{
				return;
			}

			//serialization
			void Buckling::load(FILE* file)
			{
				Eigen::load(file);
			}
			void Buckling::save(FILE* file) const
			{
				Eigen::save(file);
			}

			//type
			solvers::type Buckling::type(void) const
			{
				return solvers::type::buckling;
			}
			
			//sets
			unsigned Buckling::state_set(void) const
			{
				return (unsigned) solvers::state::u;
			}
			unsigned Buckling::force_set(void) const
			{
				return (unsigned) solvers::force::k;
			}
			unsigned Buckling::tangent_set(void) const
			{
				return (unsigned) solvers::tangent::f | (unsigned) solvers::tangent::K;
			}

			//analysis
			void Buckling::prepare(void)
			{
				return;
			}
			
			void Buckling::run(void)
			{
				//step
				m_step = 0;
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
				//stiffness
				assembler->assembly_stiffness();
				//solve
				if(!eigen_std())
				{
					printf("\tBuckling decomposition failed!\n");
					return;
				}
				//record
				while(m_step < m)
				{
					for(unsigned i = 0; i < n; i++)
					{
						m_u[i] = m_scale * m_k[n * m_step + i];
					}
					assembler->apply_state();
					assembler->apply_dependencies();
					assembler->apply_configurations();
					assembler->record();
					printf("mode: %04d stiffness: %+.2e dof: %+.2e\n", m_step, m_k[m_step], dof());
					m_step++;
				}
			}
			
			void Buckling::record(void)
			{
				char formatter[200];
				sprintf(formatter, "%+.6e\n", m_k[m_step]);
				m_results[0] += formatter;
			}
			void Buckling::finish(void) const
			{
				const std::string path = m_analysis->model()->folder() + "/Solver/Load.txt";
				FILE* file = fopen(path.c_str(), "w");
				fprintf(file, m_results[0].c_str());
				fclose(file);
			}
		}
	}
}
