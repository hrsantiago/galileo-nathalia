//std
#include <cmath>

//gl
#include <GL/glu.h>

//qt
#include <QMenu>
#include <QThread>
#include <QStatusBar>
#include <QWheelEvent>
#include <QPainter>

//mat
#include "misc/util.h"
#include "linear/dense.h"
#include "linear/linear.h"

//fea
#include "Model/Model.h"
#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Node.h"

#include "Plot/Plot.h"
#include "Plot/What.h"
#include "Plot/Colors.h"
#include "Plot/Gradient.h"

//gui
#include "Canvas/Canvas.h"
#include "Galileo/Galileo.h"

namespace gui
{
	namespace canvas
	{
		//constructors
		Canvas::Canvas(QWidget* parent) : QOpenGLWidget(parent), 
			m_z(sqrt(3) / 3), m_dp(0.1), m_dt(M_PI / 12), m_dz(1.1), 
			m_position(nullptr), m_element(nullptr), m_model(nullptr),
			m_dof(0), m_type(0), m_step(0), m_list(0), m_state(0), m_isometric(2) 
		{
			m_node[0] = nullptr;
			m_node[1] = nullptr;
			m_node[2] = nullptr;
		}
	
		//destructor
		Canvas::~Canvas(void)
		{
			return;
		}
		
		//data
		void Canvas::dof(unsigned dof)
		{
			m_dof = dof;
			redraw();
		}
		void Canvas::type(unsigned type)
		{
			m_type = type;
			redraw();
		}
		void Canvas::step(unsigned step)
		{
			m_step = step;
			redraw();
		}
		void Canvas::state(unsigned state)
		{
			m_state = state;
			redraw();
		}
		
		void Canvas::node(const double**** node)
		{
			m_node[0] = node[0];
			m_node[1] = node[1];
			m_node[2] = node[2];
		}
		void Canvas::element(const double**** element)
		{
			m_element = element;
		}
		void Canvas::position(const double*** position, unsigned ns, unsigned nn)
		{
			m_position = position;
			m_bound.update(position, ns, nn);
		}
		
		//model
		void Canvas::model(const fea::models::Model* model)
		{
			//model
			m_model = model;
			m_bound.update(model);
			//draw
			draw_model();
			//update
			update();
		}
		
		//update
		void Canvas::bound(void)
		{
			m_bound.update(m_model);
		}
		void Canvas::redraw(void)
		{
			draw_model();
			draw_paths();
			draw_plots();
			draw_scale();
			update();
		}
		
		//draw
		void Canvas::paintGL(void) 
		{
			//setup
			setup();
			//draw model
			glPushMatrix();
			m_bound.apply(m_z);
			glCallList(m_list + 3);
			glCallList(m_list + 4);
			glCallList(m_list + 5);
			draw_nodes_text();
			glPopMatrix();
			//draw axes
			if(m_model->plot()->what()->axes())
			{
				glCallList(m_list + 1);
				draw_axes_text();
			}
			//draw scale
			if(m_model->plot()->what()->scale() && (m_node[m_type] || m_element))
			{
				glCallList(m_list + 2);
			}
		}
		void Canvas::resizeGL(int w, int h) 
		{
			glViewport(0, 0, w, h);
		}
		void Canvas::initializeGL(void)
		{
			//list
			m_list = glGenLists(6);
			//draw
			draw_axes();
			draw_model();
			draw_scale();
			draw_paths();
			draw_plots();
		}
		
		void Canvas::setup(void)
		{
			//data
			double m[16];
			const unsigned w = width();
			const unsigned h = height();
			const double a = w < h ? 1 : double(h) / w;
			const double b = w > h ? 1 : double(w) / h;
			const double* c = m_model->plot()->colors()->back();
			//back
			glClearColor(c[0], c[1], c[2], c[3]);
			//clear
			glLoadIdentity();
			glClear(GL_COLOR_BUFFER_BIT);
			//scale
			glScaled(a, b, 0);
			//shift
			glTranslated(m_p[0], m_p[1], m_p[2]);
			//rotate
			glMultMatrixd(m_q.rotation(m));
		}
		void Canvas::draw_axis(void)
		{
			//data
			double t;
			const unsigned n = 8;
			const double s = 0.2;
			const double h = 0.3;
			const double a = M_PI / 12;
			//list
			glNewList(m_list, GL_COMPILE);
				glBegin(GL_LINES);
					glVertex3d(0, 0, 0);
					glVertex3d(s, 0, 0);
				for(unsigned i = 0; i < n; i++)
				{
					glVertex3d(s, 0, 0);
					t = 2 * i * M_PI / n;
					glVertex3d(s - s * h * cos(a), s * h * sin(a) * cos(t), s * h * sin(a) * sin(t));
				}
				glEnd();
				glBegin(GL_LINE_LOOP);
				for(unsigned i = 0; i < n; i++)
				{
					t = 2 * i * M_PI / n;
					glVertex3d(s - s * h * cos(a), s * h * sin(a) * cos(t), s * h * sin(a) * sin(t));
				}
				glEnd();
			glEndList();
		}
		void Canvas::draw_axes(void)
		{
			draw_axis();
			glNewList(m_list + 1, GL_COMPILE);
				glColor3d(1, 0, 0);
				glCallList(m_list);
				glPushMatrix();
				glColor3d(0, 1, 0);
				glRotated(+90, 0, 0, 1);
				glCallList(m_list);
				glPopMatrix();
				glPushMatrix();
				glColor3d(0, 0, 1);
				glRotated(-90, 0, 1, 0);
				glCallList(m_list);
				glPopMatrix();
			glEndList();
		}
		void Canvas::draw_axes_text(void)
		{
			const double s = 0.2;
			draw_text(0 + s, 0, 0, "X", Qt::red);
			draw_text(0, 0 + s, 0, "Y", Qt::green);
			draw_text(0, 0, 0 + s, "Z", Qt::blue);
		}
		void Canvas::draw_text(double x, double y, double z, const QString& text, Qt::GlobalColor color)
		{
			//https://stackoverflow.com/questions/28216001/how-to-render-text-with-qopenglwidget
			int height = this->height();

			GLdouble model[4][4], proj[4][4];
			GLint view[4];
			glGetDoublev(GL_MODELVIEW_MATRIX, &model[0][0]);
			glGetDoublev(GL_PROJECTION_MATRIX, &proj[0][0]);
			glGetIntegerv(GL_VIEWPORT, &view[0]);

			GLdouble textPosX = 0, textPosY = 0, textPosZ = 0;
			project(x, y, z, &model[0][0], &proj[0][0], &view[0], &textPosX, &textPosY, &textPosZ);

			textPosY = height - textPosY; // y is inverted

			QPainter painter(this);
			painter.setPen(color);
			painter.setFont(QFont("Helvetica", 10));
			painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
			painter.drawText(static_cast<int>(textPosX), static_cast<int>(textPosY), text);
			painter.end();

			resizeGL(this->width(), this->height());
		}
		GLint Canvas::project(GLdouble objx, GLdouble objy, GLdouble objz, const GLdouble model[16], const GLdouble proj[16], const GLint viewport[4], GLdouble * winx, GLdouble * winy, GLdouble * winz)
		{
			GLdouble in[4], out[4];

			in[0] = objx;
			in[1] = objy;
			in[2] = objz;
			in[3] = 1.0;
			transformPoint(out, model, in);
			transformPoint(in, proj, out);

			if(in[3] == 0.0)
				return GL_FALSE;

			in[0] /= in[3];
			in[1] /= in[3];
			in[2] /= in[3];

			*winx = viewport[0] + (1 + in[0]) * viewport[2] / 2;
			*winy = viewport[1] + (1 + in[1]) * viewport[3] / 2;

			*winz = (1 + in[2]) / 2;
			return GL_TRUE;
		}

		void Canvas::transformPoint(GLdouble out[4], const GLdouble m[16], const GLdouble in[4])
		{
			out[0] = m[0*4+0] * in[0] + m[1*4+0] * in[1] + m[2*4+0] * in[2] + m[3*4+0] * in[3];
			out[1] = m[0*4+1] * in[0] + m[1*4+1] * in[1] + m[2*4+1] * in[2] + m[3*4+1] * in[3];
			out[2] = m[0*4+2] * in[0] + m[1*4+2] * in[1] + m[2*4+2] * in[2] + m[3*4+2] * in[3];
			out[3] = m[0*4+3] * in[0] + m[1*4+3] * in[1] + m[2*4+3] * in[2] + m[3*4+3] * in[3];
		}
		void Canvas::draw_scale(void)
		{
			//gradient
			const double* g[] = {
				m_model->plot()->gradient()->color_min(),
				m_model->plot()->gradient()->color_max(),
				m_model->plot()->gradient()->color_mean()
			};
			//points
			const double q[6][2] = {
				{+0.95, -0.95},
				{+0.95, +0.00},
				{+0.95, +0.95},
				{+0.90, +0.95},
				{+0.90, +0.00},
				{+0.90, -0.95}
			};
			//draw
			glNewList(m_list + 2, GL_COMPILE);
				//face
				glLoadIdentity();
				glBegin(GL_QUAD_STRIP);
					glColor4dv(g[0]);
					glVertex2dv(q[0]);
					glVertex2dv(q[5]);
					glColor4dv(g[2]);
					glVertex2dv(q[1]);
					glVertex2dv(q[4]);
					glColor4dv(g[1]);
					glVertex2dv(q[2]);
					glVertex2dv(q[3]);
				glEnd();
				//edge
				double c[4];
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
			glEndList();
		}
		void Canvas::draw_nodes_text(void)
		{
			if(m_model->plot()->what()->nodes())
			{
				fea::mesh::Mesh *mesh = m_model->mesh();
				for(unsigned int i = 0; i < mesh->nodes().size(); ++i)
				{
					fea::mesh::nodes::Node *node = mesh->node(i);
					const double *p = node->coordinates();
					draw_text(p[0], p[1], p[2], QString::number(i), Qt::white);
				}
			}
		}
		void Canvas::draw_model(void)
		{
			//data
			const double** vp0 = m_position ? m_position[0] : nullptr;
			const double** ve0 = m_element ? m_element[0][m_state] : nullptr;
			const double* vn0 = m_node[m_type] ? m_node[m_type][0][m_dof] : nullptr;

			const double** vp = m_position ? m_position[m_step] : nullptr;
			const double** ve = m_element ? m_element[m_step][m_state] : nullptr;
			const double* vn = m_node[m_type] ? m_node[m_type][m_step][m_dof] : nullptr;
			//draw
			glNewList(m_list + 3, GL_COMPILE);
			m_model->plot(m_bound.size(), nullptr, nullptr, nullptr);

			m_model->plot(m_bound.size(), vp, vn, ve);
			glEndList();
		}
		void Canvas::draw_plots(void)
		{
			//list
			glNewList(m_list + 4, GL_COMPILE);
			glColor4dv(m_model->plot()->colors()->paths());
			//draw
			for(const unsigned i : m_paths)
			{
				glBegin(GL_LINE_STRIP);
				for(unsigned j = 0; j <= m_step; j++)
				{
					glVertex3dv(m_position[j][i]);
				}
				glEnd();
			}
			glEndList();
		}
		void Canvas::draw_paths(void)
		{
			glNewList(m_list + 5, GL_COMPILE);
			for(const unsigned i : m_plots)
			{
				const double** ve = m_element ? m_element[i][m_state] : nullptr;
				const double* vn = m_node[m_type] ? m_node[m_type][i][m_dof] : nullptr;
				m_model->plot(m_bound.size(), m_position[i], vn, ve);
			}
			glEndList();
		}

		//events
		void Canvas::wheelEvent(QWheelEvent* event)
		{
			if(event->angleDelta().y() > 0)
			{
				m_z *= m_dz;
			}
			else
			{
				m_z /= m_dz;
			}
			update();
		}
		void Canvas::keyPressEvent(QKeyEvent* event)
		{
			//animation
			const unsigned ns = 10;
			//key and modifiers
			const unsigned key = event->key();
			const unsigned mod = event->modifiers();
			//fit
			if(key == Qt::Key_F)
			{
				m_p.clear();
				m_q.clear();
				m_isometric = 2;
				m_z = sqrt(3) / 3;
			}
			//center
			if(key == Qt::Key_C)
			{
				m_p.clear();
			}
			//views
			if(key == Qt::Key_X)
			{
				m_q.view_x();
			}
			if(key == Qt::Key_Y)
			{
				m_q.view_y();
			}
			if(key == Qt::Key_Z)
			{
				m_q.view_z();
			}
			if(key == Qt::Key_I)
			{
				m_q.isometric(m_isometric);
				m_isometric = (m_isometric + 1) % 3;
			}
			//pan and rotate
			if(key == Qt::Key_Up)
			{
				if(mod & Qt::ShiftModifier)
				{
					m_p += mat::vec3(0, m_dp, 0);
				}
				if(mod & Qt::ControlModifier)
				{
					m_q *= mat::quat(-m_dt, 0);
				}
			}
			if(key == Qt::Key_Down)
			{
				if(mod & Qt::ShiftModifier)
				{
					m_p -= mat::vec3(0, m_dp, 0);
				}
				if(mod & Qt::ControlModifier)
				{
					m_q *= mat::quat(+m_dt, 0);
				}
			}
			if(key == Qt::Key_Left)
			{
				if(mod & Qt::ShiftModifier)
				{
					m_p -= mat::vec3(m_dp, 0, 0);
				}
				if(mod & Qt::ControlModifier)
				{
					m_q *= mat::quat(-m_dt, 1);
				}
			}
			if(key == Qt::Key_Right)
			{
				if(mod & Qt::ShiftModifier)
				{
					m_p += mat::vec3(m_dp, 0, 0);
				}
				if(mod & Qt::ControlModifier)
				{
					m_q *= mat::quat(+m_dt, 1);
				}
			}
			//zoom and rotate
			if(key == Qt::Key_Plus)
			{
				if(mod & Qt::ControlModifier)
				{
					m_q *= mat::quat(+m_dt, 2);
				}
				else
				{
					m_z *= m_dz;
				}
			}
			if(key == Qt::Key_Minus)
			{
				if(mod & Qt::ControlModifier)
				{
					m_q *= mat::quat(-m_dt, 2);
				}
				else
				{
					m_z /= m_dz;
				}
			}
			//update
			update();
		}
		void Canvas::mouseMoveEvent(QMouseEvent* event) 
		{
			//mouse
			const int x = event->x();
			const int y = event->y();
			//click
			const int i = m_click.m_x;
			const int j = m_click.m_y;
			//screen
			const unsigned w = width();
			const unsigned h = height();
			const double a = w > h ? double(w) / h : 1;
			const double b = w < h ? double(h) / w : 1;
			//shift
			if(m_click.m_pan)
			{
				m_p[0] = m_click.m_p[0] + 2 * a * double(x - i) / w;
				m_p[1] = m_click.m_p[1] - 2 * b * double(y - j) / h;
			}
			//rotate
			if(m_click.m_rot)
			{
				const mat::vec3 v1 = arcball(2 * a * double(i) / w - a, b - 2 * b * double(j) / h);
				const mat::vec3 v2 = arcball(2 * a * double(x) / w - a, b - 2 * b * double(y) / h);
				m_q = mat::quat(acos(v1.dot(v2)), v1.cross(v2).normalize()) * m_click.m_q;
			}
			//update
			update();
		}
		void Canvas::mousePressEvent(QMouseEvent* event) 
		{
			m_click.m_x = event->x();
			m_click.m_y = event->y();
			if(event->button() & Qt::LeftButton)
			{
				m_click.m_q = m_q;
				m_click.m_rot = true;
				setCursor(QCursor(Qt::CrossCursor));
			}
			if(event->button() & Qt::MiddleButton)
			{
				m_click.m_p = m_p;
				m_click.m_pan = true;
				setCursor(QCursor(Qt::ClosedHandCursor));
			}
		}
		void Canvas::mouseReleaseEvent(QMouseEvent* event)
		{
			m_click.m_pan = false;
			m_click.m_rot = false;
			setCursor(QCursor(Qt::ArrowCursor));
		}
	
		//arc ball
		mat::vec3 Canvas::arcball(double x, double y)
		{
			const double s = sqrt(x * x + y * y);
			return mat::vec3(x / fmax(s, 1), y / fmax(s, 1), sqrt(1 - fmin(s * s, 1)));
		}
	}
}
