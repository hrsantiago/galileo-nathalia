#pragma once
#undef GL_GLEXT_VERSION

//qt
#include <QOpenGLWidget>

namespace fea
{
	namespace mesh
	{
		namespace sections
		{
			class Section;
		}
	}
}

namespace gui
{
	namespace canvas
	{
		class Section : public QOpenGLWidget
		{
			Q_OBJECT
	
		public:
			//constructors
			Section(QWidget* = nullptr, fea::mesh::sections::Section* = nullptr);
		
			//destructor
			virtual ~Section(void);
			
			//data
			fea::mesh::sections::Section* section(void) const;
			fea::mesh::sections::Section* section(fea::mesh::sections::Section*);
			
		protected:
			//draw
			virtual void paintGL(void) override;
			virtual void resizeGL(int, int) override;

		protected:
			//attributes
			fea::mesh::sections::Section* m_section;
		};
	}
}
