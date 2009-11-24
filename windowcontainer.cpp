#include "windowcontainer.h"
#include "homewindow.h"
#include "page.h" // Page::setPageContainer

#include <QStackedLayout>


WindowContainer::WindowContainer(int width, int height)
{
	main = new HomeWindow;
	addWidget(main);
	QStackedLayout *root_layout = static_cast<QStackedLayout *>(layout());
	root_layout->setSpacing(0);
	root_layout->setContentsMargins(0, 0, 0, 0);

	showFullScreen();
	setFixedSize(width, height);

	Page::setPageContainer(main->centralLayout());
}

PageContainer *WindowContainer::centralLayout()
{
	return main->centralLayout();
}

HomeWindow *WindowContainer::homeWindow()
{
	return main;
}
