//std
#include <cstdio>
#include <cstring>

//mat
#include "linear/dense.h"
#include "linear/linear.h"
#include "solvers/newmark.h"

namespace mat
{
	namespace solvers
	{
		namespace dynamic
		{
			//constructors
			newmark::newmark(unsigned m_nd, unsigned m_ns, double T, bool mem) : m_nd(m_nd), m_ns(m_ns), m_T(T), m_mem(mem), m_g(0.50), m_b(0.25)
			{
				m_x = m_mem ? new double[m_nd] : nullptr;
				m_v = m_mem ? new double[m_nd] : nullptr;
				m_a = m_mem ? new double[m_nd] : nullptr;
				m_f = m_mem ? new double[m_nd] : nullptr;
				m_r = m_mem ? new double[m_nd] : nullptr;
				m_dx = m_mem ? new double[m_nd] : nullptr;
				m_dv = m_mem ? new double[m_nd] : nullptr;
				m_K = m_mem ? new double[m_nd * m_nd] : nullptr;
				m_C = m_mem ? new double[m_nd * m_nd] : nullptr;
				m_M = m_mem ? new double[m_nd * m_nd] : nullptr;
			}

			//destructor
			newmark::~newmark(void)
			{
				if(m_mem)
				{
					delete[] m_x;
					delete[] m_v;
					delete[] m_a;
					delete[] m_f;
					delete[] m_r;
					delete[] m_K;
					delete[] m_C;
					delete[] m_M;
					delete[] m_dx;
					delete[] m_dv;
				}
			}
			
			//data
			void newmark::update(void)
			{
				system();
				inertia();
				mat::mul(mat::solve(m_a, m_M, m_f, m_nd), -1, m_nd);
			}
			void newmark::system(void)
			{
				m_system(m_t, m_x, m_v, m_f);
			}
			void newmark::residue(void)
			{
				for(unsigned i = 0; i < m_nd; i++)
				{
					m_r[i] = m_f[i];
					for(unsigned j = 0; j < m_nd; j++)
					{
						m_r[i] += m_M[i + m_nd * j] * m_a[j];
					}
				}
			}
			void newmark::inertia(void)
			{
				m_inertia(m_t, m_x, m_v, m_M);
			}
			void newmark::damping(void)
			{
				m_damping(m_t, m_x, m_v, m_C);
			}
			void newmark::stifness(void)
			{
				m_stiffness(m_t, m_x, m_v, m_K);
			}

			//solve
			void newmark::setup(void)
			{
				m_t = 0;
				m_s = 0;
				m_dt = m_T / m_ns;
			}
			void newmark::predictor(void)
			{
				m_t += m_dt;
				for(unsigned i = 0; i < m_nd; i++)
				{
					m_v[i] += m_dt * m_a[i];
					m_x[i] += m_dt * m_v[i] - m_dt * m_dt / 2 * m_a[i];
				}
			}
			void newmark::corrector(void)
			{
				while(true)
				{
					system();
					inertia();
					residue();
					const double f = mat::norm(m_f, m_nd);
					if(mat::norm(m_r, m_nd) < 1e-5 * (f == 0 ? 1 : f))
					{
						break;
					}
					damping();
					stifness();
					for(unsigned i = 0; i < m_nd * m_nd; i++)
					{
						m_K[i] += (m_g * m_dt * m_C[i] + m_M[i]) / (m_b * m_dt * m_dt);
					}
					mat::solve(m_dx, m_K, m_r, m_nd);
					for(unsigned i = 0; i < m_nd; i++)
					{
						m_x[i] -= m_dx[i];
						m_v[i] -= m_dx[i] * m_g / (m_b * m_dt);
						m_a[i] -= m_dx[i] / (m_b * m_dt * m_dt);
					}
				}
			}
			void newmark::serialize(void)
			{
				printf("%04d ", m_s);
				printf("%+.6e ", m_t);
				for(unsigned i = 0; i < m_nd; i++)
				{
					printf("%+.6e %+.6e %+.6e ", m_x[i], m_v[i], m_a[i]);
				}
				printf("\n");
				m_s++;
			}
			
			//solve
			bool newmark::step(void)
			{
				predictor();
				corrector();
			}
			bool newmark::solve(void)
			{
				setup();
				update();
				serialize();
				while(m_s < m_ns) 
				{
					step();
					serialize();
				}
			}
		}
	}
}
