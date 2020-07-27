#include "ModelViewer_pcp.h"

#include <QDesktopWidget>

#include "QtApp_Prep_T2D_ME_p.h"

namespace QtApp_Prep_T2D_ME_p_Internal
{

MainWindow::MainWindow()
{
	setObjectName(QString::fromUtf8("model_view"));

	model_view = new QtGLView(this);
	model_view->setObjectName(QString::fromUtf8("model_view"));
	setCentralWidget(model_view);

	status_bar = new QStatusBar(this);
	status_bar->setObjectName(QString::fromUtf8("status_bar"));
	setStatusBar(status_bar);
}

MainWindow::~MainWindow() {}

};


QtApp_Prep_T2D_ME_p::QtApp_Prep_T2D_ME_p(int &argc, char **argv) :
	app(argc, argv),
	scene(window.get_view()),
	controller(window.get_view(), scene) {}

QtApp_Prep_T2D_ME_p::~QtApp_Prep_T2D_ME_p() {}

int QtApp_Prep_T2D_ME_p::start()
{
	window.show();
	return app.exec();
}
