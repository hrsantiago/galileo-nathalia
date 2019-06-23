#pragma once

namespace mat
{
	namespace solvers
	{
		class runge_kutta
		{
		public:
			//constructors
			runge_kutta(bool = true, bool = true);
			
			//destructor
			virtual ~runge_kutta(void);
		
		private:
			//data
			void update(void);
			void state(double);
			void increment(double);
			
			//solve
			void setup(void);
			void tangent_1(void);
			void tangent_2(void);
			void tangent_3(void);
			void tangent_4(void);
			void corrector(void);
			void serialize(void);
			
		public:
			//solve
			void init(void);
			void step(void);
			void solve(void);
			
			//attributes
			bool m_mem, m_type;
			unsigned m_s, m_nd, m_ns;
			double m_t, m_T, *m_x, *m_v, *m_a;
			double m_dt, *m_xn, *m_vn, *m_dx, *m_dv;
			double* (*m_system_1)(double, const double*, double*);
			double* (*m_system_2)(double, const double*, const double*, double*);
		};
	}
}
