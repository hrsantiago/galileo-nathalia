//fea
#include "Mesh/Nodes/Dofs.h"

#include "Analysis/Solvers/Watch_Dof.h"

namespace fea
{
	namespace analysis
	{
		namespace solvers
		{
			//constructors
			Watch_Dof::Watch_Dof(void) : m_node(0), m_dof(mesh::nodes::dof::translation_x)
			{
				return;
			}

			//destructor
			Watch_Dof::~Watch_Dof(void)
			{
				return;
			}

			//serialization
			void Watch_Dof::load(FILE* file)
			{
				fscanf(file, "%d %d", &m_node, &m_dof);
			}
			void Watch_Dof::save(FILE* file) const
			{
				fprintf(file, "%04d %02d ", m_node, m_dof);
			}
		}
	}
}
