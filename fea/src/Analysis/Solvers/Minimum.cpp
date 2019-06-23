//std
#include <cmath>

//fea
#include "Model/Model.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Solvers/Minimum.h"
#include "Analysis/Assembler/Assembler.h"

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
			Minimum::Minimum(void) : m_penalty(1e3), m_increment(1e-3)
			{
				return;
			}

			//destructor
			Minimum::~Minimum(void)
			{
				return;
			}

			//serialization
			void Minimum::load(FILE* file)
			{
				Solver::load(file);
				Nonlinear::load(file);
				fscanf(file, "%lf", &m_penalty);
				fscanf(file, "%lf", &m_increment);
			}
			void Minimum::save(FILE* file) const
			{
				Solver::save(file);
				Nonlinear::save(file);
				fprintf(file, "%+.2e ", m_penalty);
				fprintf(file, "%+.2e ", m_increment);
			}

			//type
			solvers::type Minimum::type(void) const
			{
				return solvers::type::minimum;
			}

			//sets
			unsigned Minimum::state_set(void) const
			{
				return (unsigned) solvers::state::u;
			}
			unsigned Minimum::force_set(void) const
			{
				return 
					(unsigned) solvers::force::r | 
					(unsigned) solvers::force::R | 
					(unsigned) solvers::force::Fi;
			}
			unsigned Minimum::tangent_set(void) const
			{
				return (unsigned) solvers::tangent::K;
			}

			void Minimum::run(void)
			{
				//assembler
				assembler = m_analysis->assembler();
				//apply state
				assembler->apply_initials();
				assembler->apply_supports();
				assembler->apply_dependencies();
				//record
				assembler->record();
				//time loop
				while(m_iteration < m_iteration_max)
				{
					//update configuration
					assembler->apply_dependencies();
					assembler->apply_configurations();
					//assembly forces
					assembler->assembly_internal_force();
					//check solution
					compute_residue();
					if(equilibrium())
					{
						//record
						assembler->record();
						//break iteration loop
						break;
					}
					//compute corrector
					compute_corrector();
					//increment iteration
					m_iteration++;
				}
				//record
				assembler->record();
			}
			
			void Minimum::record(void)
			{
				return;
			}
			void Minimum::finish(void) const
			{
				return;
			}
			
			void Minimum::compute_residue(void)
			{
//				double f, df;
				n = assembler->dof_unknow();
//				for(unsigned i = 0; i < n; i++)
//				{
//					m_r[i] = m_Fiu[i] + 2 * m_penalty * f * df;
//				}
			}
			void Minimum::compute_corrector(void)
			{
//				double f, df;
//				n = assembler->dof_unknow();
//				for(unsigned i = 0; i < n; i++)
//				{
//					m_r[i] = m_Fiu[i] + 2 * m_penalty * f * df;
//				}
			}
		}
	}
}
