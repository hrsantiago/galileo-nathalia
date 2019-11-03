#pragma once

#include <QMainWindow>

//forward
namespace Ui
{
	class Galileo;
}
namespace fea
{
	namespace plot
	{
		class Plot;
	}
	namespace models
	{
		class Model;
	}
}
namespace gui
{
	namespace util
	{
		class Recent;
	}
	namespace results
	{
		class Results;
	}
}

namespace gui
{
	class Galileo : public QMainWindow
	{
		Q_OBJECT

	public:
		//constructors
		Galileo(QWidget* = nullptr);
	
		//destructor
		~Galileo(void);
	
	protected slots:
		//slots
		virtual bool slot_saved(void);
		virtual void slot_recent(void);
		
		virtual void on_action_model_new_triggered(void);
		virtual void on_action_model_save_triggered(void);
		virtual void on_action_model_open_triggered(void);
		virtual void on_action_model_plot_triggered(void);
		virtual void on_action_model_print_triggered(void);
		virtual void on_action_model_close_triggered(void);
		
		virtual void on_action_mesh_nodes_triggered(void);
		virtual void on_action_mesh_cells_triggered(void);
		virtual void on_action_mesh_sections_triggered(void);
		virtual void on_action_mesh_elements_triggered(void);
		virtual void on_action_mesh_materials_triggered(void);
		
		virtual void on_action_results_nodes_triggered(void);
		virtual void on_action_results_elements_triggered(void);
		virtual void on_action_results_deformed_triggered(void);
		virtual void on_action_results_equilibrium_path_triggered(void);
		
		virtual void on_action_help_index_triggered(void);
		virtual void on_action_help_about_triggered(void);

	protected:
		//events
		virtual void keyPressEvent(QKeyEvent*) override;
		
		//saved
		virtual void plot(bool);
		virtual void current(char*, bool);

	private:
		//attributes
		Ui::Galileo* m_ui;
		fea::plot::Plot* m_plot;
		fea::models::Model* m_model;
		gui::util::Recent* m_recent; 
		gui::results::Results* m_results;
	};
}
