//std
#include <cstring>

//mat
#include "misc/util.h"

//fea
#include "Mesh/Materials/Material.h"

#include "Mesh/Elements/Mechanic/Mechanic.h"

#include "Mesh/Points/Types.h"
#include "Mesh/Points/Mechanic/States.h"
#include "Mesh/Points/Mechanic/Mechanic.h"

namespace fea
{
	namespace mesh
	{
		namespace points
		{
			//constructors
			Mechanic::Mechanic(void) :
				m_state_old(state::elastic), m_state_new(state::elastic),
				m_hardening_old(0), m_hardening_new(0), m_stress_types(0), 
				m_plastic_strain_old(nullptr), m_plastic_strain_new(nullptr)
			{
				return;
			}

			//destructor
			Mechanic::~Mechanic(void)
			{
				delete[] m_plastic_strain_old;
				delete[] m_plastic_strain_new;
			}

			//type
			points::type Mechanic::type(void) const
			{
				return points::type::mechanic;
			}

			//analysis
			void Mechanic::prepare(unsigned t)
			{
				//types
				m_stress_types = t;
				const unsigned n = mat::bit_count(t);
				//state
				m_hardening_old = 0;
				m_hardening_new = 0;
				m_state_old = state::elastic;
				m_state_new = state::elastic;
				m_plastic_strain_old = new double[n];
				m_plastic_strain_new = new double[n];
				memset(m_plastic_strain_old, 0, n * sizeof(double));
				memset(m_plastic_strain_new, 0, n * sizeof(double));
			}
			void Mechanic::prepare(const elements::Element* element)
			{
				prepare(((elements::Mechanic*) element)->stresses());
			}
			
			
			void Mechanic::update(void)
			{
				//components
				const unsigned n = mat::bit_count(m_stress_types);
				//update
				m_state_old = m_state_new;
				m_hardening_old = m_hardening_new;
				memcpy(m_plastic_strain_old, m_plastic_strain_new, n * sizeof (double));
			}
			void Mechanic::restore(void)
			{
				//components
				const unsigned n = mat::bit_count(m_stress_types);
				//update
				m_state_new = m_state_old;
				m_hardening_new = m_hardening_old;
				memcpy(m_plastic_strain_new, m_plastic_strain_old, n * sizeof (double));
			}
		}
	}
}
