//fea
#include "linear/dense.h"

//fea
#include "Models/Model.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Time.h"
#include "Analysis/Solvers/Nonlinear.h"
#include "Analysis/Assembler/Assembler.h"

namespace fea
{
	namespace analysis
	{
		namespace solvers
		{
			//constructors
			Nonlinear::Nonlinear(void) :
				m_mode(solvers::mode::full), m_equilibrium(false), m_tolerance(1e-5),
				m_attempt(0), m_attempt_max(5), m_iteration(0), m_iteration_max(10), m_iteration_desired(5), m_criticals(0),
				m_convergence((unsigned) solvers::convergence::force)
			{
				return;
			}

			//destructor
			Nonlinear::~Nonlinear(void)
			{
				return;
			}

			//serialization
			void Nonlinear::load(FILE* file)
			{
				//load
				fscanf(file, "%d %d %d %d %d %lf",
					&m_mode,
					&m_attempt_max,
					&m_iteration_max,
					&m_iteration_desired,
					&m_convergence,
					&m_tolerance);
			}
			void Nonlinear::save(FILE* file) const
			{
				//save
				fprintf(file, "%01d %02d %02d %02d %02d %+.6e ",
					m_mode,
					m_attempt_max,
					m_iteration_max,
					m_iteration_desired,
					m_convergence,
					m_tolerance);
			}

			//data
			double Nonlinear::tolerance(void) const
			{
				return m_tolerance;
			}
			double Nonlinear::tolerance(double tolerance)
			{
				return m_tolerance = tolerance;
			}
			
			unsigned Nonlinear::convergence(void) const
			{
				return m_convergence;
			}
			unsigned Nonlinear::convergence(unsigned convergence)
			{
				return m_convergence = convergence;
			}
			unsigned Nonlinear::attempt_max(void) const
			{
				return m_attempt_max;
			}
			unsigned Nonlinear::attempt_max(unsigned attempt_max)
			{
				return m_attempt_max = attempt_max;
			}
			unsigned Nonlinear::iteration_max(void) const
			{
				return m_iteration_max;
			}
			unsigned Nonlinear::iteration_max(unsigned iteration_max)
			{
				return m_iteration_max = iteration_max;
			}
			unsigned Nonlinear::iteration_desired(void) const
			{
				return m_iteration_desired;
			}
			unsigned Nonlinear::iteration_desired(unsigned iterations_desired)
			{
				return m_iteration_desired = iterations_desired;
			}
			
			solvers::mode Nonlinear::mode(void) const
			{
				return m_mode;
			}
			solvers::mode Nonlinear::mode(solvers::mode mode)
			{
				return m_mode = mode;
			}

			//convergence
			bool Nonlinear::equilibrium(void)
			{
				//tolerance
				const double t = m_tolerance;
				//force
				const double* Fe = dynamic_cast<const Time*> (this) ? m_Feu : m_Fru;
				//number of dof
				const unsigned nu = m_analysis->assembler()->dof_unknow();
				//parameters
				const double r = mat::norm(m_r, nu);
				const double D = mat::norm(m_du, nu);
				const double d = mat::norm(m_ddu, nu);
				const double F = mat::norm(Fe, nu) != 0 ? mat::norm(Fe, nu) : 1;
				//convergence
				const bool a = !(m_convergence & (unsigned) solvers::convergence::fixed) || r < t;
				const bool f = !(m_convergence & (unsigned) solvers::convergence::force) || r < t * F;
				const bool s = !(m_convergence & (unsigned) solvers::convergence::state) || d < t * D;
				//return
				return m_equilibrium = a && f && s;
			}

			//loops
			unsigned Nonlinear::attempt(void) const
			{
				return m_attempt;
			}
			unsigned Nonlinear::iteration(void) const
			{
				return m_iteration;
			}
			
			//analysis
			bool Nonlinear::stop(bool log) const
			{
				if(m_attempt > m_attempt_max)
				{
					if(log)
					{
						printf("\tSolution not found in max attempts!\n");
					}
					return true;
				}
				return false;
			}
		}
	}
}
