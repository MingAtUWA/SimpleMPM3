#include "ModelViewer_pcp.h"

#include <QDesktopWidget>

#include "QtApp_Posp_T2D_CHM_s.h"

namespace QtApp_Posp_T2D_CHM_s_Internal
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

QtApp_Posp_T2D_CHM_s::QtApp_Posp_T2D_CHM_s(
	int &argc, char **argv,
	Type tp) :
	app(argc, argv), type(tp),
	scene(window.get_view()),
	pcontroller(nullptr)
{
	switch (tp)
	{
	case SingleFrame:
		pcontroller = new QtController_Posp_Static(window.get_view(), scene);
		break;
	case Animation:
		pcontroller = new QtController_Posp_Animation(window.get_view(), scene);
		break;
	default:
		break;
	}
}

QtApp_Posp_T2D_CHM_s::~QtApp_Posp_T2D_CHM_s()
{
	if (pcontroller)
	{
		delete pcontroller;
		pcontroller = nullptr;
	}
}

int QtApp_Posp_T2D_CHM_s::start()
{
	window.show();
	return app.exec();
}
