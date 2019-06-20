#pragma once

#include <QDialog>

namespace Ui
{
	class Plots;
}

namespace gui
{
	namespace results
	{
		class Plots : public QDialog
		{
			Q_OBJECT

		public:
			//constructors
			Plots(std::vector<unsigned>*, unsigned, QWidget* = nullptr);
	
			//destructor
			virtual ~Plots(void);

		private slots:
			//slots
			void slot_add(void);
			void slot_clear(void);
			void slot_remove(void);

		private:
			//update
			void update_table(void);

		private:
			//attributes
			Ui::Plots* m_ui;
			unsigned m_steps;
			std::vector<unsigned>* m_plots;
		};
	}
}
