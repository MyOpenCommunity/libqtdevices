#include "main_window.h"
#include "transitionwidget.h"
#include "sottomenu.h"
#include "page.h"

#include <QDebug>
#include <QLayout>


namespace
{
	void fixVisualization(Page *p, QSize size)
	{
		p->resize(size);

		if (QLayout *layout = p->layout())
		{
			layout->activate();
			layout->update();
		}
		else if (sottoMenu *s = qobject_cast<sottoMenu*>(p))
			s->forceDraw();
	}
}


MainWindow::MainWindow(unsigned int width, unsigned int height) : QStackedWidget(0) // no parent
{
	transition_widget = 0;
	block_transitions = false;
	prev_page = 0;
	dest_page = 0;

	showFullScreen();
	setFixedSize(width, height);
	// TODO: this ugly workaround is needed because the QStackedWidget in some ways
	// invalidate the first widget inserted. FIX it asap!
	addWidget(new QWidget);
}

void MainWindow::installTransitionWidget(TransitionWidget *tr)
{
	if (transition_widget)
	{
		removeWidget(transition_widget);
		transition_widget->disconnect();
		transition_widget->deleteLater();
	}

	transition_widget = tr;
	addWidget(transition_widget);
	connect(transition_widget, SIGNAL(endTransition()), SLOT(endTransition()));
}

void MainWindow::endTransition()
{
	setCurrentPage(dest_page);
	prev_page = 0;
	dest_page = 0;
}

void MainWindow::setCurrentPage(Page *p)
{
	qDebug() << "MainWindow::setCurrentPage on" << p;
	setCurrentWidget(p);
	emit currentPageChanged(p);
}

void MainWindow::showPage(Page *p)
{
	if (transition_widget && !block_transitions)
	{
		prev_page = currentPage();

		// Before grab the screenshot of the next page, we have to ensure that its
		// visualization is correct.
		fixVisualization(p, size());
		startTransition(QPixmap::grabWidget(prev_page), p);
	}
	else
		setCurrentPage(p);
}

void MainWindow::startTransition(const QPixmap &prev_image, Page *p)
{
	if (transition_widget)
	{
		setCurrentWidget(transition_widget);
		dest_page = p;
		transition_widget->startTransition(prev_image, QPixmap::grabWidget(p));
	}
}

Page *MainWindow::currentPage()
{
	// if we are in the middle of a transition, we use the previous page as the current page
	return prev_page ? prev_page : static_cast<Page*>(currentWidget());
}

void MainWindow::blockTransitions(bool block)
{
	block_transitions = block;
	if (block && transition_widget)
	{
		transition_widget->cancelTransition();
		if (prev_page)
			setCurrentWidget(prev_page);
	}
}

void MainWindow::addPage(Page *p)
{
	addWidget(p);
	p->resize(width(), height());
}

