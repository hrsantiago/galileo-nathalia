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
			glClear(GL_COLOR_BUFFER_BIT);
			//data
			int v[4];
			glGetIntegerv(GL_VIEWPORT, v);
			const double w = m_section->width();
			const double h = m_section->height();
			const double fx = 1.05 * (v[2] * h > v[3] * w ? (v[2] * h) / (v[3] * w) : 1);
			const double fy = 1.05 * (v[2] * h < v[3] * w ? (v[3] * w) / (v[2] * h) : 1);
			//model matrix
			glLoadIdentity();
			glOrtho(-fx * w / 2, fx * w / 2, -fy * h / 2, fy * h / 2, -1, 1);
			//draw
			m_section->draw();
		}
		void Section::resizeGL(int w, int h) 
		{
			glViewport(0, 0, w, h);
		}
		void Section::initializeGL(void)
		{
			glDepthRange(-1, +1);
			glClearColor(0, 0, 0, 1);
			glEnable(GL_LINE_SMOOTH);
			glEnable(GL_POINT_SMOOTH);
			glClear(GL_COLOR_BUFFER_BIT);
		}
	}
}
