#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QStackedWidget>

class TransitionWidget;
class Page;


class MainWindow : public QStackedWidget
{
Q_OBJECT
public:
	MainWindow(unsigned int width, unsigned int height);

	// Install a widget for the transition. The transition widget must have the public functions:
	//
	// void startTransition(const QPixmap &prev, const QPixmap &dest);
	// void cancelTransition();
	// and the signal:
	// void endTransition();
	//
	// NOTE: MainWindow take the ownership of the TransitionWidget
	void installTransitionWidget(TransitionWidget *tr);

	// Set the argument page as the current page, without transition
	void setCurrentPage(Page *p);
	// Set the argument page as the current page, with transition if active
	void showPage(Page *p);

	// add the page at the stack of the pages
	void addPage(Page *p);
	// return the current page (or the previous if during a transition)
	Page *currentPage();

	// stop the transition effect implicity called by the showPage method.
	void blockTransitions(bool block);

	void startTransition(const QPixmap &prev_image, Page *p);

signals:
	void currentPageChanged(Page *p);

private:
	bool block_transitions;
	TransitionWidget *transition_widget;

	// The previous page if during the transition, 0 otherwise
	Page *prev_page;
	// The destination page if during the transition, 0 otherwise
	Page *dest_page;

private slots:
	void endTransition();
};

#endif // MAIN_WINDOW_H
