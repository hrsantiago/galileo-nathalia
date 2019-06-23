#pragma once

namespace mat
{
	namespace solvers
	{
		class newmark
		{
		public:
			//constructors
			newmark(unsigned = 1, unsigned = 100, double = 10, bool = true);
			
			//destructor
			virtual ~newmark(void);
		
		private:
			//data
			void update(void);
			void system(void);
			void residue(void);
			void inertia(void);
			void damping(void);
			void stifness(void);
			
			//solve
			void setup(void);
			void predictor(void);
			void corrector(void);
			void serialize(void);
			
		public:
			//solve
			void step(void);
			void solve(void);
			
			//attributes
			bool m_mem;
			unsigned m_s, m_nd, m_ns;
			double m_t, m_T, m_g, m_b, *m_x, *m_v, *m_a, *m_r;
			double m_dt, *m_dx, *m_dv, *m_f, *m_K, *m_C, *m_M;
			double* (*m_system)(double, const double*, const double*, double*);
			double* (*m_inertia)(double, const double*, const double*, double*);
			double* (*m_damping)(double, const double*, const double*, double*);
			double* (*m_stiffness)(double, const double*, const double*, double*);
		};
	}
}
