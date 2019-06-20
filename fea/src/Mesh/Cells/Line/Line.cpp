//gl
#include <GL/gl.h>
#include <GL/glu.h>

//mat
#include "misc/util.h"
#include "linear/dense.h"

//fea
#include "Models/Model.h"

#include "Plot/Plot.h"
#include "Plot/Gradient.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Node.h"
#include "Mesh/Cells/Line/Line.h"
#include "Mesh/Elements/Element.h"

namespace fea
{
	namespace mesh
	{
		namespace cells
		{
			//constructors
			Line::Line(void) : m_section(0)
			{
				return;
			}

			//destructor
			Line::~Line(void)
			{
				return;
			}

			//serialization
			void Line::load(FILE* file)
			{
				fscanf(file, "%d", &m_section);
			}
			void Line::save(FILE* file) const
			{
				fprintf(file, "%02d ", m_section);
			}

			//data
			sections::Section* Line::section(void) const
			{
				return m_mesh->section(m_section);
			}
			sections::Section* Line::section(unsigned section)
			{
				return m_mesh->section(m_section = section);
			}

			//topology
			unsigned Line::faces(void) const
			{
				return 0;
			}
			unsigned Line::edges(void) const
			{
				return 1;
			}
			unsigned Line::dimension(void) const
			{
				return 1;
			}
			
			std::vector<unsigned> Line::face(unsigned) const
			{
				return std::vector<unsigned>();
			}

			//parametrization
			void Line::edge(double* p, unsigned, double t) const
			{
				p[0] = t;
			}
			void Line::face(double*, unsigned, double, double) const
			{
				return;
			}
			
			void Line::gradient(double* dp, unsigned, double) const
			{
				dp[0] = 1;
			}
			void Line::gradient(double*, unsigned, double, double) const
			{
				return;
			}

			//geometry
			double Line::jacobian(double* J, const elements::Element* element, const double*) const
			{
				const double* xi = element->node(0)->coordinates();
				const double* xj = element->node(1)->coordinates();
				return J[0] = mat::norm(xj, xi, 3) / 2;
			}

			//integration
			unsigned Line::points(void) const
			{
				return m_rule;
			}
			double Line::point(double* x, unsigned i) const
			{
				double w;
				mat::gauss_point(x[0], w, m_rule, i);
				return w;
			}

			//mesh
			void Line::refine(unsigned index, unsigned n)
			{
				//sizes
				const unsigned nn = m_mesh->nodes();
				const unsigned ne = m_mesh->elements();
				//element
				elements::Element* element = m_mesh->element(index);
				//type
				const elements::type type = element->type();
				//coordinates
				const double* xi = element->node(0)->coordinates();
				const double* xj = element->node(1)->coordinates();
				//create nodes
				for(unsigned i = 0; i < n - 1; i++)
				{
					const double x = xi[0] + (i + 1) * (xj[0] - xi[0]) / n;
					const double y = xi[1] + (i + 1) * (xj[1] - xi[1]) / n;
					const double z = xi[2] + (i + 1) * (xj[2] - xi[2]) / n;
					m_mesh->add_node(x, y, z);
				}
				//create elements
				for(unsigned i = 0; i < n - 1; i++)
				{
					//create
					m_mesh->add_element(element);
					//set nodes
					m_mesh->element(ne + i)->node(0, nn + i);
					m_mesh->element(ne + i)->node(1, i != n - 2 ? nn + i + 1 : element->index_node(1));
				}
				//set base element nodes
				if(n > 1)
				{
					element->node(1, nn);
				}
			}
			
			//results
			void Line::plot(const elements::Element* element, const double* color, const double** positions, const double* values, bool flag) const
			{
				//gradient
				double c[4];
				const double* g[] = {
					m_mesh->model()->plot()->gradient()->color_min(),
					m_mesh->model()->plot()->gradient()->color_mean(),
					m_mesh->model()->plot()->gradient()->color_max()
				};
				//plot
				glLineWidth(2);
				glColor4dv(color);
				glBegin(GL_LINE_STRIP);
				for(unsigned i = 0; i < element->nodes(); i++)
				{
					if(values)
					{
						glColor4dv(mat::map_color(c, g, values[flag ? i : element->index_node(i)]));
					}
					const double* p = positions ? positions[element->index_node(i)] : element->node(i)->coordinates();
					glVertex3dv(p);
				}
				glEnd();
			}
		}
	}
}
