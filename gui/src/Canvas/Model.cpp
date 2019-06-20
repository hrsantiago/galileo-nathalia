//std
#include <cmath>

//gl
#include <GL/glu.h>

//qt
#include <QMenu>
#include <QStatusBar>
#include <QWheelEvent>

//mat
#include "misc/defs.h"
#include "misc/util.h"
#include "linear/dense.h"
#include "linear/linear.h"

//fea
#include "Models/Model.h"

#include "Plot/Plot.h"
#include "Plot/What.h"
#include "Plot/Gradient.h"

//gui
#include "Canvas/Model.h"
#include "Galileo/Galileo.h"

namespace gui
{
	namespace canvas
	{
		//constructors
		Model::Model(QWidget* parent) : QOpenGLWidget(parent), m_arc(false), m_pan(false), m_cli_x(0), m_cli_y(0), m_cur_x(0), m_cur_y(0), m_model(nullptr)
		{
			setMouseTracking(true);
		}
	
		//destructor
		Model::~Model(void)
		{
			return;
		}
		
		//bound
		void Model::bound(void)
		{
			m_bound.update(m_model);
		}
		
		//draw
		void Model::paintGL(void) 
		{
			//clear
			glClear(GL_COLOR_BUFFER_BIT);
			//bound
			m_bound.apply();
			//plot model
			resizeGL(width(), height());
			m_model->plot(m_bound.size() / m_bound.zoom());
			//plot axis
			if(m_model->plot()->what()->axes())
			{
				draw_axes();
			}
			//update
			update();
		}
		void Model::resizeGL(int w, int h) 
		{
			if(w < h)
			{
				glViewport(0, (h - w) / 2, w, w);
			}
			else
			{
				glViewport((w - h) / 2, 0, h, h);
			}
		}
		void Model::initializeGL(void)
		{
			glDepthRange(-1, +1);
			glEnable(GL_LINE_SMOOTH);
			glEnable(GL_POINT_SMOOTH);
			glClear(GL_COLOR_BUFFER_BIT);
		}
		void Model::draw_axes(void) const
		{
			//center
			double p[3];
			m_bound.center(p);
			mat::sub(p, m_bound.pan(), 3);
			//arrows
			const double f = 0.30;
			const double s = 0.05 * m_bound.size() / m_bound.zoom();
			//draw axis
			draw_axis_x(p, s, f);
			draw_axis_y(p, s, f);
			draw_axis_z(p, s, f);
		}
		void Model::draw_axis_x(const double* p, double s, double f) const
		{
			//angles
			const double t1 =  30 * M_PI / 180;
			const double t2 = 330 * M_PI / 180;
			//plot
			glBegin(GL_LINES);
			glColor4d(1, 0, 0, 1);
			glVertex3d(p[0], p[1], p[2]);
			glVertex3d(p[0] + s, p[1], p[2]);
			glVertex3d(p[0] + s, p[1], p[2]);
			glVertex3d(p[0] + s - f * s * cos(t1), p[1] - f * s * sin(t1), p[2]);
			glVertex3d(p[0] + s, p[1], p[2]);
			glVertex3d(p[0] + s - f * s * cos(t2), p[1] - f * s * sin(t2), p[2]);
			glVertex3d(p[0] + s, p[1], p[2]);
			glVertex3d(p[0] + s - f * s * cos(t1), p[1], p[2] - f * s * sin(t1));
			glVertex3d(p[0] + s, p[1], p[2]);
			glVertex3d(p[0] + s - f * s * cos(t2), p[1], p[2] - f * s * sin(t2));
			glEnd();
		}
		void Model::draw_axis_y(const double* p, double s, double f) const
		{
			//angles
			const double t1 =  60 * M_PI / 180;
			const double t2 = 120 * M_PI / 180;
			//plot
			glBegin(GL_LINES);
			glColor4d(0, 1, 0, 1);
			glVertex3d(p[0], p[1], p[2]);
			glVertex3d(p[0], p[1] + s, p[2]);
			glVertex3d(p[0], p[1] + s, p[2]);
			glVertex3d(p[0] - f * s * cos(t1), p[1] + s - f * s * sin(t1), p[2]);
			glVertex3d(p[0], p[1] + s, p[2]);
			glVertex3d(p[0] - f * s * cos(t2), p[1] + s - f * s * sin(t2), p[2]);
			glVertex3d(p[0], p[1] + s, p[2]);
			glVertex3d(p[0], p[1] + s - f * s * sin(t1), p[2] - f * s * cos(t1));
			glVertex3d(p[0], p[1] + s, p[2]);
			glVertex3d(p[0], p[1] + s - f * s * sin(t2), p[2] - f * s * cos(t2));
			glEnd();
		}
		void Model::draw_axis_z(const double* p, double s, double f) const
		{
			//angles
			const double t1 =  60 * M_PI / 180;
			const double t2 = 120 * M_PI / 180;
			//plot
			glBegin(GL_LINES);
			glColor4d(0, 0, 1, 1);
			glVertex3d(p[0], p[1], p[2]);
			glVertex3d(p[0], p[1], p[2] + s);
			glVertex3d(p[0], p[1], p[2] + s);
			glVertex3d(p[0] - f * s * cos(t1), p[1], p[2] + s - f * s * sin(t1));
			glVertex3d(p[0], p[1], p[2] + s);
			glVertex3d(p[0] - f * s * cos(t2), p[1], p[2] + s - f * s * sin(t2));
			glVertex3d(p[0], p[1], p[2] + s);
			glVertex3d(p[0], p[1] - f * s * cos(t1), p[2] + s - f * s * sin(t1));
			glVertex3d(p[0], p[1], p[2] + s);
			glVertex3d(p[0], p[1] - f * s * cos(t2), p[2] + s - f * s * sin(t2));
			glEnd();
		}

		void Model::draw_scale(void) const
		{
			//viewport
			glViewport(0, 0, width(), height());
			//state
			int v[4];
			double c[4], m[16], p[16];
			glGetIntegerv(GL_VIEWPORT, v);
			glGetDoublev(GL_MODELVIEW_MATRIX, m);
			glGetDoublev(GL_PROJECTION_MATRIX, p);
			//gradient
			const double* g[] = {
				m_model->plot()->gradient()->color_min(),
				m_model->plot()->gradient()->color_mean(),
				m_model->plot()->gradient()->color_max()
			};
			//window
			const int x = v[0];
			const int y = v[1];
			const int w = v[2];
			const int h = v[3];
			//position
			const double s = 0.01 * w;
			const double t = 0.40 * h;
			const double a = x + 0.95 * w;
			const double b = y + 0.50 * h;
			//world points
			double q[6][3];
			gluUnProject(a + s, b + 0, 0, m, p, v, &q[0][0], &q[0][1], &q[0][2]);
			gluUnProject(a + s, b + t, 0, m, p, v, &q[1][0], &q[1][1], &q[1][2]);
			gluUnProject(a - s, b + t, 0, m, p, v, &q[2][0], &q[2][1], &q[2][2]);
			gluUnProject(a - s, b + 0, 0, m, p, v, &q[3][0], &q[3][1], &q[3][2]);
			gluUnProject(a - s, b - t, 0, m, p, v, &q[4][0], &q[4][1], &q[4][2]);
			gluUnProject(a + s, b - t, 0, m, p, v, &q[5][0], &q[5][1], &q[5][2]);
			//plot face
			glBegin(GL_POLYGON);
			glColor4dv(g[1]);
			glVertex3dv(q[0]);
			glColor4dv(g[2]);
			glVertex3dv(q[1]);
			glColor4dv(g[2]);
			glVertex3dv(q[2]);
			glColor4dv(g[1]);
			glVertex3dv(q[3]);
			glColor4dv(g[0]);
			glVertex3dv(q[4]);
			glColor4dv(g[0]);
			glVertex3dv(q[5]);
			glEnd();
			//plot edge
			glGetDoublev(GL_COLOR_CLEAR_VALUE, c);
			glColor4dv(mat::inv_color(c));
			glBegin(GL_LINE_LOOP);
			glVertex3dv(q[0]);
			glVertex3dv(q[1]);
			glVertex3dv(q[2]);
			glVertex3dv(q[3]);
			glVertex3dv(q[4]);
			glVertex3dv(q[5]);
			glEnd();
		}

		//update
		void Model::update_model(fea::models::Model* model)
		{
			m_model = model;
			m_bound.update(model);
		}

		//events
		void Model::wheelEvent(QWheelEvent* event)
		{
			m_bound.zoom(1 + (double) event->delta() / 1200, true);
			paintGL();
		}
		void Model::keyPressEvent(QKeyEvent* event)
		{
			//key and modifiers
			const unsigned key = event->key();
			const unsigned mod = event->modifiers();
			const unsigned con = mod & Qt::ControlModifier || mod & Qt::ShiftModifier;
			//fit
			if(key == Qt::Key_F)
			{
				slot_fit();
			}
			if(key == Qt::Key_C)
			{
				slot_center();
			}
			//pan +x
			if(key == Qt::Key_Right)
			{
				slot_pan_x(true);
			}
			//pan -x
			if(key == Qt::Key_Left)
			{
				slot_pan_x(false);
			}
			//pan +y
			if(key == Qt::Key_Up)
			{
				slot_pan_y(true);
			}
			//pan -y
			if(key == Qt::Key_Down)
			{
				slot_pan_y(false);
			}
			//zoom +
			if(key == Qt::Key_Plus)
			{
				m_bound.zoom(1.10, true);
				paintGL();
			}
			//zoom -
			if(key == Qt::Key_Minus)
			{
				m_bound.zoom(0.90, true);
				paintGL();
			}
			//isoview
			if(key == Qt::Key_I)
			{
				slot_view_i();
			}
			//rotate x
			if(key == Qt::Key_X)
			{
				con ? slot_rot_x(mod & Qt::ControlModifier) : slot_view_x();
			}
			//rotate y
			if(key == Qt::Key_Y)
			{
				con ? slot_rot_y(mod & Qt::ControlModifier) : slot_view_y();
			}
			//rotate z
			if(key == Qt::Key_Z)
			{
				con ? slot_rot_z(mod & Qt::ControlModifier) : slot_view_z();
			}
		}
		void Model::mouseMoveEvent(QMouseEvent* event) 
		{
			//set current position
			m_cur_x = event->x();
			m_cur_y = event->y();
			//check click position
			if(m_cur_x == m_cli_x && m_cur_y == m_cli_y)
			{
				return;
			}
			//pan
			if(m_pan)
			{
				//data
				int v[4];
				double d[2], dp[3];
				double p1[3], p2[3];
				double x1[3], x2[3];
				double p[16], m[16];
				//viewport
				glGetIntegerv(GL_VIEWPORT, v);
				//depth
				glGetDoublev(GL_DEPTH_RANGE, d);
				//matrices
				glGetDoublev(GL_MODELVIEW_MATRIX, m);
				glGetDoublev(GL_PROJECTION_MATRIX, p);
				//inverse
				mat::inv(m, 4);
				mat::inv(p, 4);
				//unproject
				p1[2] = p2[2] = -(d[1] + d[0]) / (d[1] - d[0]);
				p1[0] = 2 * (double) (m_cli_x - v[0]) / v[2] - 1;
				p1[1] = 2 * (double) (m_cli_y - v[1]) / v[3] - 1;
				p2[0] = 2 * (double) (m_cur_x - v[0]) / v[2] - 1;
				p2[1] = 2 * (double) (m_cur_y - v[1]) / v[3] - 1;
				mat::multiply(p1, m, mat::multiply(x1, p, p1, 4, 4), 4, 4);
				mat::multiply(p2, m, mat::multiply(x2, p, p2, 4, 4), 4, 4);
				//pan
				m_bound.pan(mat::sub(dp, p2, p1, 3), true);
			}
			//rotate
			else if(m_arc)
			{
				double v1[3], v2[3];
				double va[3], rm[16];
				arc_ball(v1, m_cli_x, m_cli_y);
				arc_ball(v2, m_cur_x, m_cur_y);
				mat::normalize(mat::cross(va, v1, v2), 3);
				const double f = acos(mat::bound(mat::dot(v1, v2, 3)));
				m_bound.rotation(m_cli_r);
				m_bound.rotation(Bound::rotation(rm, va, f), true);
			}
			else
			{
				if(dynamic_cast<QMainWindow*>(parent()))
				{
					int v[4];
					char msg[200];
					double x, y, z, p[16], m[16];
					glGetIntegerv(GL_VIEWPORT, v);
					glGetDoublev(GL_MODELVIEW_MATRIX, m);
					glGetDoublev(GL_PROJECTION_MATRIX, p);
					gluUnProject(m_cur_x, m_cur_y, 0, m, p, v, &x, &y, &z);
					sprintf(msg, "x: %+.2e y: %+.2e z: %+.2e", x, y, z);
					((QMainWindow*) parent())->statusBar()->showMessage(msg, 5000);
				}
			}
			//draw
			paintGL();
		}
		void Model::mousePressEvent(QMouseEvent* event) 
		{
			m_cur_x = m_cli_x = event->x();
			m_cur_y = m_cli_y = event->y();
			if(m_pan = event->button() & Qt::MidButton)
			{
				mat::set(m_cli_p, m_bound.pan(), 3);
				setCursor(QCursor(Qt::ClosedHandCursor));
			}
			if(m_arc = event->button() & Qt::LeftButton)
			{
				setCursor(QCursor(Qt::CrossCursor));
				mat::set(m_cli_r, m_bound.rotation(), 16);
			}
			if(event->button() & Qt::RightButton)
			{
				//menu
				QMenu menu(this);
				//actions
				QObject::connect(menu.addAction("Fit"), SIGNAL(triggered(bool)), this, SLOT(slot_fit(void)));
				QObject::connect(menu.addAction("View X"), SIGNAL(triggered(bool)), this, SLOT(slot_view_x(void)));
				QObject::connect(menu.addAction("View Y"), SIGNAL(triggered(bool)), this, SLOT(slot_view_y(void)));
				QObject::connect(menu.addAction("View Z"), SIGNAL(triggered(bool)), this, SLOT(slot_view_z(void)));
				QObject::connect(menu.addAction("View I"), SIGNAL(triggered(bool)), this, SLOT(slot_view_i(void)));
				//execute
				menu.exec(event->globalPos());
			}
		}
		void Model::mouseReleaseEvent(QMouseEvent* event)
		{
			m_pan = m_arc = false;
			setCursor(QCursor(Qt::ArrowCursor));
		}
		void Model::mouseDoubleClickEvent(QMouseEvent*)
		{
			return;
		}
	
		//arc ball
		void Model::arc_ball(double* p, unsigned x, unsigned y)
		{
			//canvas size
			const unsigned w = width();
			const unsigned h = height();
			//arcball point
			const double a = 2 * (double) x / w - 1;
			const double b = 1 - 2 * (double) y / h;
			if(a * a + b * b < 1)
			{
				p[0] = a;
				p[1] = b;
				p[2] = sqrt(1 - a * a - b * b);
			}
			else
			{
				p[2] = 0;
				p[0] = a / (a * a + b * b);
				p[1] = b / (a * a + b * b);
			}
		}
		
		//slots
		void Model::slot_fit(void)
		{
			m_bound.reset();
			paintGL();
		}
		void Model::slot_pan_x(bool inc)
		{
			//factor
			const double f = 0.05;
			const double z = m_bound.zoom();
			const double s = m_bound.size();
			const double d = (inc ? +1 : -1) * f * s / z;
			//rotation
			const double* R = m_bound.rotation();
			//pan increment
			const double p[] = {
				R[0 + 4 * 0] * d, 
				R[0 + 4 * 1] * d, 
				R[0 + 4 * 2] * d
			};
			m_bound.pan(p, true);
			paintGL();
		}
		void Model::slot_pan_y(bool inc)
		{
			//factor
			const double f = 0.05;
			const double z = m_bound.zoom();
			const double s = m_bound.size();
			const double d = (inc ? +1 : -1) * f * s / z;
			//rotation
			const double* R = m_bound.rotation();
			//pan increment
			const double p[] = {
				R[1 + 4 * 0] * d, 
				R[1 + 4 * 1] * d, 
				R[1 + 4 * 2] * d
			};
			m_bound.pan(p, true);
			paintGL();
		}
		void Model::slot_rot_x(bool inc)
		{
			double r[16];
			const double a[] = {1, 0, 0};
			m_bound.rotation(Bound::rotation(r, a, (inc ? +5 : -5) * M_PI / 180), true);
			paintGL();
		}
		void Model::slot_rot_y(bool inc)
		{
			double r[16];
			const double a[] = {0, 1, 0};
			m_bound.rotation(Bound::rotation(r, a, (inc ? +5 : -5) * M_PI / 180), true);
			paintGL();
		}
		void Model::slot_rot_z(bool inc)
		{
			double r[16];
			const double a[] = {0, 0, 1};
			m_bound.rotation(Bound::rotation(r, a, (inc ? +5 : -5) * M_PI / 180), true);
			paintGL();
		}
		void Model::slot_view_x(void)
		{
			double r[16];
			const double e1[] = {1, 0, 0};
			const double e3[] = {0, 0, 1};
			m_bound.rotation(Bound::rotation(r, e1, -M_PI / 2));
			m_bound.rotation(Bound::rotation(r, e3, -M_PI / 2), true);
			paintGL();
		}
		void Model::slot_view_y(void)
		{
			double r[16];
			const double e1[] = {1, 0, 0};
			m_bound.rotation(Bound::rotation(r, e1, -M_PI / 2));
			paintGL();
		}
		void Model::slot_view_z(void)
		{
			double r[16];
			const double e1[] = {1, 0, 0};
			m_bound.rotation(Bound::rotation(r, e1, 0));
			paintGL();
		}
		void Model::slot_view_i(void)
		{
			double r[16];
			const double ti = 35.264;
			const double e1[] = {1, 0, 0};
			const double e2[] = {0, 1, 0};
			const double e3[] = {0, 0, 1};
			m_bound.rotation(Bound::rotation(r, e1, +ti * M_PI / 180));
			m_bound.rotation(Bound::rotation(r, e2, -45 * M_PI / 180), true);
			m_bound.rotation(Bound::rotation(r, e1, -90 * M_PI / 180), true);
			m_bound.rotation(Bound::rotation(r, e3, -90 * M_PI / 180), true);
			paintGL();
		}
		void Model::slot_center(void)
		{
			slot_fit();
		}
	}
}
