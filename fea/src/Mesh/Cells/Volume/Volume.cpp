//gl
#include <GL/gl.h>

//mat
#include "misc/util.h"

//fea
#include "Model/Model.h"

#include "Plot/Plot.h"
#include "Plot/Gradient.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Node.h"
#include "Mesh/Cells/Volume/Volume.h"
#include "Mesh/Elements/Element.h"

namespace fea
{
	namespace mesh
	{
		namespace cells
		{
			//constructors
			Volume::Volume(void)
			{
				return;
			}

			//destructor
			Volume::~Volume(void)
			{
				return;
			}

			//serialization
			void Volume::load(FILE*)
			{
				return;
			}
			void Volume::save(FILE*) const
			{
				return;
			}

			//topology
			unsigned Volume::dimension(void) const
			{
				return 3;
			}
			
			//results
			void Volume::plot(const elements::Element* element, const double* color, const double** positions, const double* values, bool flag) const
			{
				//gradient
				double c[4], b[4];
				const double* g[] = {
					m_mesh->model()->plot()->gradient()->color_min(),
					m_mesh->model()->plot()->gradient()->color_mean(),
					m_mesh->model()->plot()->gradient()->color_max()
				};
				glGetDoublev(GL_COLOR_CLEAR_VALUE, b);
				mat::inv_color(b);
				//plot
				for(unsigned i = 0; i < faces(); i++)
				{
					//face
					glColor4dv(color);
					glBegin(GL_POLYGON);
					std::vector<unsigned> fn = face(i);
					for(const unsigned& j : fn)
					{
						if(values)
						{
							glColor4dv(mat::map_color(c, g, values[flag ? j : element->index_node(j)]));
						}
						const double* p = positions ? positions[element->index_node(j)] : element->node(j)->coordinates();
						glVertex3dv(p);
					}
					glEnd();
					//edge
					glColor4dv(b);
					glBegin(GL_LINE_LOOP);
					for(const unsigned& j : fn)
					{
						const double* p = positions ? positions[element->index_node(j)] : element->node(j)->coordinates();
						glVertex3dv(p);
					}
					glEnd();
				}
			}
		}
	}
}
