#pragma once

namespace mat
{
	namespace solvers
	{
		class newton_raphson
		{
		public:
			//constructors
			newton_raphson(unsigned = 1, bool = true);
			
			//destructor
			virtual ~newton_raphson(void);
		
		private:
			//data
			void residue(void);
			
			//solve
			void save(void);
			void setup(void);
			void predictor(void);
			void corrector(void);
			
			//load
			void load_predictor(void);
			void load_corrector(void);
			
		public:
			//solve
			void step(void);
			void solve(void);
			
			//attributes
			bool m_mem;
			double *m_x, *m_dx, *m_dt, *m_dr;
			double *m_r, *m_fi, *m_fe, *m_kt;
			double m_l, m_dl, m_ddl, m_lm, m_tol, m_fr;
			unsigned m_s, m_i, m_nd, m_ns, m_ni, *m_pt;
			double* (*m_internal_force)(double*, const double*);
			double* (*m_external_force)(double*, const double*);
			double* (*m_stiffness)(double*, const double*, double);
		};
	}
}
