#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>


#include "../Client/TaskProxy.hpp"
#include "../Client/ExecutorProxy.hpp"
#include "../Client/ComponentProxy.hpp"
#include "../Client/DisCODeClient.hpp"

#include "HostSelector.hpp"
#include "WelcomePage.h"

class QTreeWidgetItem;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

    void setClient(DisCODe::Client * c) {
    	client = c;
    }

    void setup(DisCODe::Client * c);

public slots:
	void on_treeWidget_itemClicked(QTreeWidgetItem * item, int column);

	void on_actionConnect_triggered(bool checked);

	void do_connect();
	void do_disconnect();

private:
    Ui::MainWindow *ui;


	DisCODe::Client * client;
	DisCODe::TaskProxy * task;

	QMap<QString, QWidget*> component_props;

	bool m_connected;

	HostSelector hs;
	WelcomePage wp;
};

#endif // MAINWINDOW_H
