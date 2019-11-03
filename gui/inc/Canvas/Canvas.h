#pragma once
#undef GL_GLEXT_VERSION

//qt
#include <QOpenGLWidget>

//mat
#include "linear/quat.h"

//gui
#include "Canvas/Bound.h"
#include "Canvas/Click.h"

namespace fea
{
	namespace models
	{
		class Model;
	}
}
namespace gui
{
	class Galileo;
	namespace results
	{
		class Nodal;
		class Deformed;
		class Elements;
	}
}

namespace gui
{
	namespace canvas
	{
		class Canvas : public QOpenGLWidget
		{
			Q_OBJECT
	
			//friends
			friend class gui::Galileo;
			friend class gui::results::Nodal;
			friend class gui::results::Deformed;
			friend class gui::results::Elements;
	
		public:
			//constructors
			Canvas(QWidget* = nullptr);
		
			//destructor
			virtual ~Canvas(void);
			
			//data
			virtual void dof(unsigned);
			virtual void type(unsigned);
			virtual void step(unsigned);
			virtual void state(unsigned);
			
			virtual void node(const double****);
			virtual void element(const double****);
			virtual void position(const double***, unsigned, unsigned);
			
			//model
			virtual void model(const fea::models::Model*);
			
			//update
			virtual void bound(void);
			virtual void redraw(void);
			
		protected:
			//draw
			virtual void paintGL(void) override;
			virtual void resizeGL(int, int) override;
			virtual void initializeGL(void) override;
			
			virtual void setup(void);
			virtual void draw_axis(void);
			virtual void draw_axes(void);
			virtual void draw_scale(void);
			virtual void draw_model(void);
			virtual void draw_plots(void);
			virtual void draw_paths(void);
		
			//events
			virtual void wheelEvent(QWheelEvent*) override;
			virtual void keyPressEvent(QKeyEvent*) override;
			virtual void mouseMoveEvent(QMouseEvent*) override;
			virtual void mousePressEvent(QMouseEvent*) override;
			virtual void mouseReleaseEvent(QMouseEvent*) override;
		
			//arcball
			static mat::vec3 arcball(double, double);

		protected:
			//attributes
			double m_z;
			double m_dp;
			double m_dt;
			double m_dz;
			
			mat::vec3 m_p;
			mat::quat m_q;
			
			Bound m_bound;
			Click m_click;

			unsigned m_dof;		
			unsigned m_type;	
			unsigned m_step;
			unsigned m_list;
			unsigned m_state;
			unsigned m_isometric;
			
			const double*** m_node[3];
			const double*** m_position;
			const double**** m_element;
			
			std::vector<unsigned> m_paths;
			std::vector<unsigned> m_plots;
			
			const fea::models::Model* m_model;
		};
	}
}
