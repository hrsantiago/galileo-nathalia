//std
#include <cstdio>
#include <cstring>

//mat
#include "linear/dense.h"
#include "linear/linear.h"
#include "solvers/newton_raphson.h"

namespace mat
{
	namespace solvers
	{
		//constructors
		newton_raphson::newton_raphson(unsigned nd, bool mem) : m_mem(mem), m_dl(0.01), m_lm(1.00), m_tol(1e-5), m_nd(nd), m_ns(100), m_ni(10)
		{
			m_x = m_mem ? new double[m_nd] : nullptr;
			m_r = m_mem ? new double[m_nd] : nullptr;
			m_dt = m_mem ? new double[m_nd] : nullptr;
			m_dr = m_mem ? new double[m_nd] : nullptr;
			m_dx = m_mem ? new double[m_nd] : nullptr;
			m_fi = m_mem ? new double[m_nd] : nullptr;
			m_fe = m_mem ? new double[m_nd] : nullptr;
			m_pt = m_mem ? new unsigned[m_nd] : nullptr;
			m_kt = m_mem ? new double[m_nd * m_nd] : nullptr;
		}

		//destructor
		newton_raphson::~newton_raphson(void)
		{
			if(m_mem)
			{
				delete[] m_x;
				delete[] m_r;
				delete[] m_dt;
				delete[] m_dr;
				delete[] m_dx;
				delete[] m_fi;
				delete[] m_fe;
				delete[] m_pt;
				delete[] m_kt;
			}
		}
		
		//data
		void newton_raphson::residue(void)
		{
			for(unsigned i = 0; i < m_nd; i++)
			{
				m_r[i] = m_fi[i] - m_l * m_fe[i];
			}
		}

		//solve
		void newton_raphson::predictor(void)
		{
			load_predictor();
			mat::solve(m_dt, m_kt, m_fe, m_nd);
			for(unsigned i = 0; i < m_nd; i++)
			{
				m_x[i] += m_dx[i] = m_dl * m_dt[i];
			}
			m_internal_force(m_fi, m_x);
			m_external_force(m_fe, m_x);
			m_stiffness(m_kt, m_x, m_l);
		}
		void newton_raphson::corrector(void)
		{
			m_i = 0;
			while(true)
			{
				residue();
				if(mat::norm(m_r, m_nd) < m_tol * m_fr || m_i == m_ni)
				{
					printf("step: %04d iteration: %02d\n", m_s, m_i);
					break;
				}
				mat::solve(m_dr, m_kt, m_r, m_nd);
				mat::solve(m_dt, m_kt, m_fe, m_nd);
				load_corrector();
				for(unsigned i = 0; i < m_nd; i++)
				{
					m_x[i] += m_dr[i] + m_ddl * m_dt[i];
				}
				m_internal_force(m_fi, m_x);
				m_external_force(m_fe, m_x);
				m_stiffness(m_kt, m_x, m_l);
			}
		}
		
		//load
		void newton_raphson::load_predictor(void)
		{
			
		}
		void newton_raphson::load_corrector(void)
		{
			
		}
		
		//solve
		void newton_raphson::save(void)
		{
			printf("%04d ", m_s);
			printf("%+.6e ", m_l);
			for(unsigned i = 0; i < m_nd; i++)
			{
				printf("%+.6e ", m_x[i]);
			}
			printf("\n");
			m_s++;
		}
		void newton_raphson::setup(void)
		{
			m_l = 0;
			m_s = 0;
			m_i = 0;
			m_fr = mat::norm(m_external_force(m_fe, m_x), m_nd);
		}
		
		//solve
		void newton_raphson::step(void)
		{
			predictor();
			corrector();
		}
		void newton_raphson::solve(void)
		{
			save();
			setup();
			while(m_s < m_ns) 
			{
				step();
				save();
			}
		}
	}
}
