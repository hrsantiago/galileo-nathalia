#include "Mesh/Materials/Types.h"
#include "Mesh/Materials/Heat/Heat.h"

namespace fea
{
	namespace mesh
	{
		namespace materials
		{
			//constructors
			Heat::Heat(void) : m_capacity(1), m_condutivity(1)
			{
				return;
			}

			//destructor
			Heat::~Heat(void)
			{
				return;
			}

			//serialization
			void Heat::load(FILE* file)
			{
				Material::load(file);
				fscanf(file, "%lf %lf", &m_condutivity, &m_capacity);
			}
			void Heat::save(FILE* file) const
			{
				Material::save(file);
				fprintf(file, "%+.6e %+.6e ", m_condutivity, m_capacity);
			}

			//type
			materials::type Heat::type(void) const
			{
				return materials::type::heat;
			}
			
			//data
			double Heat::capacity(void) const
			{
				return m_capacity;
			}
			double Heat::capacity(double capacity)
			{
				return m_capacity = capacity;
			}
			double Heat::condutivity(void) const
			{
				return m_condutivity;
			}
			double Heat::condutivity(double condutivity)
			{
				return m_condutivity = condutivity;
			}
			
		}
	}
}
