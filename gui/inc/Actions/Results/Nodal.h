#pragma once

#include <QDialog>

namespace Ui
{
	class Nodal;
}
namespace fea
{
	namespace models
	{
		class Model;
	}
}

namespace gui
{
	namespace results
	{
		class Nodal : public QDialog
		{
			Q_OBJECT

		public:
			//constructors
			Nodal(fea::models::Model*, QWidget* = nullptr);
	
			//destructor
			virtual ~Nodal(void);
			
			//data
			unsigned dof(void) const;
			unsigned step(void) const;
			
		protected:
			//events
			virtual void keyPressEvent(QKeyEvent*) override;
			
		private slots:
			//slots
			void slot_first(void);
			void slot_previous(void);
			void slot_pause(void);
			void slot_play(void);
			void slot_stop(void);
			void slot_next(void);
			void slot_last(void);
			void slot_print(void);
			void slot_record(void);
			
			void slot_plot(void);
			
			void slot_plots(void);
			void slot_paths(void);
			
			void slot_skip(void);
			void slot_skip_add(void);
			void slot_skip_sub(void);
			
			void slot_push(void);
			void slot_slider(int);
			void slot_limits(void);

		private:
			//attributes
			bool m_play;
			Ui::Nodal* m_ui;
			fea::models::Model* m_model;
		};
	}
}
