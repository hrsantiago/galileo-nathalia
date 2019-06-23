#pragma once
#undef GL_GLEXT_VERSION

//qt
#include <QOpenGLWidget>

//gui
#include "Bound.h"

namespace fea
{
	namespace models
	{
		class Model;
	}
}

namespace gui
{
	//forward declarations
	class Galileo;

	namespace canvas
	{
		class Model : public QOpenGLWidget
		{
			Q_OBJECT
	
			//friends
			friend class gui::Galileo;
	
		public:
			//constructors
			Model(QWidget* = nullptr);
		
			//destructor
			virtual ~Model(void);
			
			//bound
			virtual void bound(void);
			
		protected:
			//draw
			virtual void paintGL(void) override;
			virtual void resizeGL(int, int) override;
			virtual void initializeGL(void) override;
			
			virtual void draw_axes(void) const;
			virtual void draw_axis_x(const double*, double, double) const;
			virtual void draw_axis_y(const double*, double, double) const;
			virtual void draw_axis_z(const double*, double, double) const;

			virtual void draw_scale(void) const;

			//update
			virtual void update_model(fea::models::Model*);
		
			//events
			virtual void wheelEvent(QWheelEvent*) override;
			virtual void keyPressEvent(QKeyEvent*) override;
			virtual void mouseMoveEvent(QMouseEvent*) override;
			virtual void mousePressEvent(QMouseEvent*) override;
			virtual void mouseReleaseEvent(QMouseEvent*) override;
			virtual void mouseDoubleClickEvent(QMouseEvent*) override;			
		
			//arc ball
			virtual void arc_ball(double*, unsigned, unsigned);

		protected slots:
			//slots
			virtual void slot_fit(void);
			virtual void slot_pan_x(bool);
			virtual void slot_pan_y(bool);
			virtual void slot_rot_x(bool);
			virtual void slot_rot_y(bool);
			virtual void slot_rot_z(bool);
			virtual void slot_view_x(void);
			virtual void slot_view_y(void);
			virtual void slot_view_z(void);
			virtual void slot_view_i(void);
			virtual void slot_center(void);

		protected:
			//attributes
			Bound m_bound;
		
			bool m_pan;
			bool m_arc;
			
			unsigned m_cli_x;
			unsigned m_cli_y;
			unsigned m_cur_x;
			unsigned m_cur_y;
		
			double m_cli_p[3];
			double m_cli_r[16];
		
			fea::models::Model* m_model;
		};
	}
}
