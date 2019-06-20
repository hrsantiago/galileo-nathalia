//fea
#include "Models/Model.h"

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
			//constructors
			Modal::Modal(void) : m_scale(1), m_modes(0)
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
				Solver::load(file);
				fscanf(file, "%lf", &m_scale);
			}
			void Modal::save(FILE* file) const
			{
				Solver::save(file);
				fprintf(file, "%+.6e", m_scale);
			}

			//type
			solvers::type Modal::type(void) const
			{
				return solvers::type::modal;
			}
			
			//data
			double Modal::scale(void) const
			{
				return m_scale;
			}
			double Modal::scale(double scale)
			{
				return m_scale = scale;
			}
			unsigned Modal::modes(void) const
			{
				return m_modes;
			}
			unsigned Modal::modes(unsigned modes)
			{
				return m_modes = modes;
			}

			//sets
			unsigned Modal::state_set(void) const
			{
				return 
					(unsigned) solvers::state::u |
					(unsigned) solvers::state::v |
					(unsigned) solvers::state::a;
			}
			unsigned Modal::force_set(void) const
			{
				return (unsigned) solvers::force::k;
			}
			unsigned Modal::tangent_set(void) const
			{
				return (unsigned) solvers::tangent::K | (unsigned) solvers::tangent::M;
			}

			//analysis
			void Modal::prepare(void)
			{
				return;
			}
			
			void Modal::run(void)
			{
				//assembler
				const Assembler* assembler = m_analysis->assembler();
				//dofs
				const unsigned nu = assembler->dof_unknow();
				//apply
				assembler->apply_supports();
				assembler->apply_initials();
				assembler->apply_dependencies();
				assembler->apply_configurations();
				//inertia and stiffness
				assembler->assembly_inertia();
				assembler->assembly_stiffness();
				//compute natural frequencies and modes
//				const arma::mat M = arma::mat(m_M);
//				const arma::mat K = arma::mat(m_K);
//				const arma::mat S = arma::inv(M) * K;
//				arma::eig_sym(m_k, m_f, S);
				//record
				while(m_step < nu)
				{
//					m_u = m_scale * m_f.col(m_step);
					assembler->apply_state();
					assembler->apply_dependencies();
					assembler->record();
					printf("mode: %04d frequency: %+4.2e dof: %+4.2e\n", m_step, m_k[m_step], dof());
					m_step++;
				}
			}
			
			void Modal::record(void)
			{
				char formatter[200];
				sprintf(formatter, "%+.6e\n", m_k[m_step]);
				m_results[0] += formatter;
			}
			void Modal::finish(void) const
			{
				const std::string path = m_analysis->model()->folder() + "/Solver/Frequencies.txt";
				FILE* file = fopen(path.c_str(), "w");
				fprintf(file, m_results[0].c_str());
				fclose(file);
			}
		}
	}
}
