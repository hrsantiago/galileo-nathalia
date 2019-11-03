#pragma once

#include <QDialog>

namespace Ui
{
	class Path;
}

namespace gui
{
	namespace results
	{
		class Limits : public QDialog
		{
			Q_OBJECT

		public:
			//constructors
			Limits(const QVector<double>[], const std::vector<unsigned>[], const std::vector<unsigned>[], const QString[], QWidget* = nullptr);
	
			//destructor
			virtual ~Limits(void);
			
		private:
			//attributes
			Ui::Path* m_ui;
		};
	}
}
