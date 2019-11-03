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
#include "Mesh/Cells/Plane/Plane.h"
#include "Mesh/Elements/Element.h"

namespace fea
{
	namespace mesh
	{
		namespace cells
		{
			//constructors
			Plane::Plane(void) : m_thickness(1)
			{
				return;
			}

			//destructor
			Plane::~Plane(void)
			{
				return;
			}

			//serialization
			void Plane::load(FILE* file)
			{
				fscanf(file, "%lf", &m_thickness);
			}
			void Plane::save(FILE* file) const
			{
				fprintf(file, "%+.6e", m_thickness);
			}

			//data
			double Plane::thickness(void) const
			{
				return m_thickness;
			}
			double Plane::thickness(double thickness)
			{
				return m_thickness = thickness;
			}

			//topology
			unsigned Plane::faces(void) const
			{
				return 1;
			}
			unsigned Plane::dimension(void) const
			{
				return 2;
			}
			
			//parametrization
			void Plane::face(double* p, unsigned, double u, double v) const
			{
				p[0] = u;
				p[1] = v;
			}
			void Plane::gradient(double* dp, unsigned, double, double) const
			{
				dp[0] = 1;
				dp[1] = 0;
				dp[2] = 0;
				dp[3] = 1;
			}
			
			//results
			void Plane::plot(const elements::Element* element, const double* color, const double** positions, const double* values, bool flag) const
			{
				//gradient
				double c[4];
				const double* g[] = {
					m_mesh->model()->plot()->gradient()->color_min(),
					m_mesh->model()->plot()->gradient()->color_mean(),
					m_mesh->model()->plot()->gradient()->color_max()
				};
				//face
				glColor4dv(color);
				glBegin(GL_POLYGON);
				for(unsigned i = 0; i < element->nodes().size(); i++)
				{
					if(values)
					{
						glColor4dv(mat::map_color(c, g, values[flag ? i : element->index_node(i)]));
					}
					const double* p = positions ? positions[element->index_node(i)] : element->node(i)->coordinates();
					glVertex3dv(p);
				}
				glEnd();
				//color
				glGetDoublev(GL_COLOR_CLEAR_VALUE, c);
				//edge
				glBegin(GL_LINE_LOOP);
				glColor4dv(mat::inv_color(c));
				for(unsigned i = 0; i < element->nodes().size(); i++)
				{
					const double* p = positions ? positions[element->index_node(i)] : element->node(i)->coordinates();
					glVertex3dv(p);
				}
				glEnd();
			}
		}
	}
}
