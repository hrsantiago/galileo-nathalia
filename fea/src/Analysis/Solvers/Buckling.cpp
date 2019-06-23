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
			//constructors
			Buckling::Buckling(void) : m_scale(1)
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
				Solver::load(file);
				fscanf(file, "%lf", &m_scale);
			}
			void Buckling::save(FILE* file) const
			{
				Solver::save(file);
				fprintf(file, "%+.6e", m_scale);
			}

			//type
			solvers::type Buckling::type(void) const
			{
				return solvers::type::buckling;
			}
			
			//data
			double Buckling::scale(void) const
			{
				return m_scale;
			}
			double Buckling::scale(double scale)
			{
				return m_scale = scale;
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
				const Assembler* assembler = m_analysis->assembler();
				//apply
				assembler->apply_supports();
				assembler->apply_initials();
				assembler->apply_dependencies();
				assembler->apply_configurations();
				//stiffness
				assembler->assembly_stiffness();
				//record
				assembler->record();
				//decomposition
//				arma::eig_sym(m_k, m_f, arma::mat(m_K));
				//dofs
				const unsigned nu = assembler->dof_unknow();
				//record
				while(m_step < nu)
				{
					mat::mul(m_u, m_f + m_step * nu, nu, m_scale);
					assembler->apply_state();
					assembler->apply_dependencies();
					assembler->record();
					printf("mode: %04d stiffness: %+4.2e dof: %+4.2e\n", m_step, m_k[m_step], dof());
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
				const std::string path = m_analysis->model()->folder() + "/Solver/Stiffness.txt";
				FILE* file = fopen(path.c_str(), "w");
				fprintf(file, m_results[0].c_str());
				fclose(file);
			}
		}
	}
}
