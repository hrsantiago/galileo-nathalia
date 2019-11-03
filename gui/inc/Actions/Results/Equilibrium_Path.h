#pragma once

#include <QDialog>

namespace Ui
{
	class Equilibrium_Path;
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
		class Path;
	}
}

namespace gui
{
	namespace results
	{
		class Equilibrium_Path : public QDialog
		{
			Q_OBJECT

		public:
			//constructors
			Equilibrium_Path(const fea::models::Model*, const Path*, QWidget* = nullptr);
	
			//destructor
			virtual ~Equilibrium_Path(void);
			
			//limits
			void esc_limit_points(void) const;
			void set_limit_points(const std::vector<unsigned>&, const std::vector<unsigned>&) const;
			
		protected:
			//events
			virtual void keyPressEvent(QKeyEvent*) override;

		private slots:
			//data
			void setup(void);
			const QString current_name(unsigned) const;
			const double* current_data(unsigned) const;
		
			//slots
			void slot_fit(void) const;
			void slot_plot(void) const;
			void slot_load(void) const;
			void slot_print(void) const;
			void slot_clean(void) const;
			void slot_points(void) const;
			void slot_limits(void) const;
			void slot_labels(void) const;
			void slot_legend(void) const;
			void slot_slider(void) const;

			void slot_table(void);

			void slot_data(void) const;
			void slot_joint(void) const;
			void slot_element(void) const;
			void slot_support(void) const;
			void slot_reverse(void) const;
			
		private:
			//attributes
			const Path* m_path;
			Ui::Equilibrium_Path* m_ui;
			const fea::models::Model* m_model;
		};
	}
}
