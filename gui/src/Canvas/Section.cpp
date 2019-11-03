//std
#include <cmath>

//gl
#include <GL/glu.h>

//fea
#include "Mesh/Sections/Section.h"

//gui
#include "Canvas/Section.h"

namespace gui
{
	namespace canvas
	{
		//constructors
		Section::Section(QWidget* parent, fea::mesh::sections::Section* section) : QOpenGLWidget(parent), m_section(section)
		{
			return;
		}
	
		//destructor
		Section::~Section(void)
		{
			return;
		}
		
		//data
		fea::mesh::sections::Section* Section::section(void) const
		{
			return m_section;
		}
		fea::mesh::sections::Section* Section::section(fea::mesh::sections::Section* section)
		{
			return m_section = section;
		}
		
		//draw
		void Section::paintGL(void) 
		{
			//clear
			glLoadIdentity();
			glClear(GL_COLOR_BUFFER_BIT);
			//screen
			const int w = width();
			const int h = height();
			//section
			const double a = m_section->width();
			const double b = m_section->height();
			//scale
			const double q = fmax(w, h);
			const double s = fmax(a, b);
			glScaled(2 * h / q / s, 2 * w / s / q, 1);
			//draw
			m_section->draw();
		}
		void Section::resizeGL(int w, int h) 
		{
			glViewport(0, 0, w, h);
		}
	}
}
