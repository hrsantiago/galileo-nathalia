#pragma once

#include <QDialog>

namespace Ui
{
	class Model;
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
	namespace canvas
	{
		class Canvas;
	}
}

namespace gui
{
	namespace model
	{
		class Model : public QDialog
		{
			Q_OBJECT

		public:
			//constructors
			Model(fea::models::Model*, gui::canvas::Canvas*, QWidget* = nullptr, bool = true);
	
			//destructor
			virtual ~Model(void);

		private slots:
			//slots
			void slot_edit(void);
			void slot_push(void);
			void slot_grad(void);
			void slot_sizes(void);
			void slot_check(void);
			void slot_combo(void);
			void slot_reset(void);
			void slot_print(void);

		private:
			//attributes
			Ui::Model* m_ui;
			fea::models::Model* m_model;
			gui::canvas::Canvas* m_canvas;
		};
	}
}
