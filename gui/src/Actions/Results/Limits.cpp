//gui
#include "Actions/Results/Limits.h"

//ui
#include "uic/Results/Limits.h"

namespace gui
{
	namespace results
	{
		//constructors
		Limits::Limits(const QVector<double> d[], const std::vector<unsigned> p[], const std::vector<unsigned> t[], const QString l[], QWidget* parent) : 
		QDialog(parent), m_ui(new Ui::Limits)
		{
			//set ui
			m_ui->setupUi(this);
			//groups
			m_ui->group_x->setTitle(l[0]);
			m_ui->group_y->setTitle(l[1]);
			//tables
			m_ui->table_x->setColumnCount(4);
			m_ui->table_y->setColumnCount(4);
			m_ui->table_x->setRowCount(p[0].size());
			m_ui->table_y->setRowCount(p[1].size());
			m_ui->table_x->setHorizontalHeaderItem(1, new QTableWidgetItem("X"));
			m_ui->table_y->setHorizontalHeaderItem(1, new QTableWidgetItem("X"));
			m_ui->table_x->setHorizontalHeaderItem(2, new QTableWidgetItem("Y"));
			m_ui->table_y->setHorizontalHeaderItem(2, new QTableWidgetItem("Y"));
			m_ui->table_x->setHorizontalHeaderItem(0, new QTableWidgetItem("Step"));
			m_ui->table_y->setHorizontalHeaderItem(0, new QTableWidgetItem("Step"));
			m_ui->table_x->setHorizontalHeaderItem(3, new QTableWidgetItem("Type"));
			m_ui->table_y->setHorizontalHeaderItem(3, new QTableWidgetItem("Type"));
			m_ui->table_x->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
			m_ui->table_y->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
			//data
			char fs[20], fx[20], fy[20];
			m_ui->table_x->setEnabled(!p[0].empty());
			m_ui->table_y->setEnabled(!p[1].empty());
			const char* names[] = {"Min", "Max", "Zero"};
			const QColor colors[] = {QColor(0, 0, 255, 100), QColor(255, 0, 0, 100), QColor(0, 255, 0, 100)};
			for(unsigned i = 0; i < p[0].size(); i++)
			{
				//items
				sprintf(fs, "%05d", p[0][i]);
				sprintf(fx, "%+.2e", d[0][i]);
				sprintf(fy, "%+.2e", d[1][i]);
				QTableWidgetItem* is = new QTableWidgetItem(fs);
				QTableWidgetItem* ix = new QTableWidgetItem(fx);
				QTableWidgetItem* iy = new QTableWidgetItem(fy);
				QTableWidgetItem* it = new QTableWidgetItem(names[t[0][i]]);
				//properties
				is->setTextAlignment(Qt::AlignCenter);
				ix->setTextAlignment(Qt::AlignCenter);
				iy->setTextAlignment(Qt::AlignCenter);
				it->setTextAlignment(Qt::AlignCenter);
				is->setBackground(QBrush(colors[t[0][i]]));
				ix->setBackground(QBrush(colors[t[0][i]]));
				iy->setBackground(QBrush(colors[t[0][i]]));
				it->setBackground(QBrush(colors[t[0][i]]));
				is->setFlags(is->flags() ^ Qt::ItemIsEditable);
				ix->setFlags(ix->flags() ^ Qt::ItemIsEditable);
				iy->setFlags(iy->flags() ^ Qt::ItemIsEditable);
				it->setFlags(it->flags() ^ Qt::ItemIsEditable);
				//add
				m_ui->table_x->setItem(i, 0, is);
				m_ui->table_x->setItem(i, 1, ix);
				m_ui->table_x->setItem(i, 2, iy);
				m_ui->table_x->setItem(i, 3, it);
			}
			for(unsigned i = 0; i < p[1].size(); i++)
			{
				//items
				sprintf(fs, "%05d", p[1][i]);
				sprintf(fx, "%+.2e", d[2][i]);
				sprintf(fy, "%+.2e", d[3][i]);
				QTableWidgetItem* is = new QTableWidgetItem(fs);
				QTableWidgetItem* ix = new QTableWidgetItem(fx);
				QTableWidgetItem* iy = new QTableWidgetItem(fy);
				QTableWidgetItem* it = new QTableWidgetItem(names[t[1][i]]);
				//properties
				is->setTextAlignment(Qt::AlignCenter);
				ix->setTextAlignment(Qt::AlignCenter);
				iy->setTextAlignment(Qt::AlignCenter);
				it->setTextAlignment(Qt::AlignCenter);
				is->setBackground(QBrush(colors[t[1][i]]));
				ix->setBackground(QBrush(colors[t[1][i]]));
				iy->setBackground(QBrush(colors[t[1][i]]));
				it->setBackground(QBrush(colors[t[1][i]]));
				is->setFlags(is->flags() ^ Qt::ItemIsEditable);
				ix->setFlags(ix->flags() ^ Qt::ItemIsEditable);
				iy->setFlags(iy->flags() ^ Qt::ItemIsEditable);
				it->setFlags(it->flags() ^ Qt::ItemIsEditable);
				//add
				m_ui->table_y->setItem(i, 0, is);
				m_ui->table_y->setItem(i, 1, ix);
				m_ui->table_y->setItem(i, 2, iy);
				m_ui->table_y->setItem(i, 3, it);
			}
		}
		
		//destructor
		Limits::~Limits(void)
		{
			delete m_ui;
		}
	}
}
