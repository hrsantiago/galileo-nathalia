#pragma once

#include <QDialog>

namespace Ui
{
	class Paths;
}

namespace gui
{
	namespace results
	{
		class Paths : public QDialog
		{
			Q_OBJECT

		public:
			//constructors
			Paths(std::vector<unsigned>&, unsigned, QWidget* = nullptr);
	
			//destructor
			virtual ~Paths(void);

		private slots:
			//slots
			void slot_add(void);
			void slot_clear(void);
			void slot_remove(void);

		private:
			//update
			void update_table(void);

			//attributes
			Ui::Paths* m_ui;
			unsigned m_nodes;
			std::vector<unsigned>& m_paths;
		};
	}
}
