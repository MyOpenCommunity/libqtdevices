#include "pagestack.h"
#include "page.h"
#include "window.h"
#include "btmain.h"

#include <QtDebug>


PageStack::State::State(Window *_window)
{
	window = _window;
	page = NULL;
}

PageStack::State::State(Page *_page)
{
	window = NULL;
	page = _page;
}

QObject *PageStack::State::object() const
{
	return page ? static_cast<QObject*>(page) : static_cast<QObject*>(window);
}


PageStack::PageStack()
{
	states.append(State(static_cast<Window*>(NULL)));
}

void PageStack::showAlarm(Page *alarm_page)
{
	addState(State(alarm_page));

	connect(alarm_page, SIGNAL(Delete()), SLOT(closed()));

	bt_global::btmain->makeActiveAndFreeze();
}

void PageStack::showVCTCall(Page *call_page)
{
	addState(State(call_page));

	connect(call_page, SIGNAL(Closed()), SLOT(closed()));

	bt_global::btmain->makeActiveAndFreeze();
}

void PageStack::showScreensaver(Window *window)
{
	addState(State(window));

	connect(window, SIGNAL(Closed()), SLOT(closed()));
}

void PageStack::showKeypad(Window *window)
{
	addState(State(window));
}

void PageStack::showUserWindow(Window *window)
{
	addState(State(window));

	connect(window, SIGNAL(Closed()), SLOT(closed()));
}

void PageStack::addState(const State &state)
{
	removeFromStack(state.object());
	states.append(state);

	connect(state.object(), SIGNAL(destroyed(QObject*)), SLOT(removeObject(QObject*)));
}

void PageStack::showState(const State &state)
{
	if (state.page)
		state.page->showPage();
	else if (state.window)
		state.window->showWindow();
}

void PageStack::currentPageChanged(Page *page)
{
	qDebug() << "PageStack::currentPageChanged on" << page;

	for (int i = 0; i < states.size(); ++i)
		if (states[i].page == page)
			return;

	states[0].page = page;
}

void PageStack::closed()
{
	removeObject(sender());
}

void PageStack::closeWindow(Window *window)
{
	removeObject(window);
}

void PageStack::closePage(Page *page)
{
	removeObject(page);
}

void PageStack::removeObject(QObject *obj)
{
	disconnect(obj, 0, this, 0);

	State top = states.back();

	if (top.object() == obj)
	{
		states.pop_back();
		showState(states.back());
	}
	else
		removeFromStack(obj);
}

void PageStack::removeFromStack(QObject *obj)
{
	for (int i = 0; i < states.size(); ++i)
	{
		if (states[i].object() == obj)
		{
			states.removeAt(i);
			break;
		}
	}
}

PageStack bt_global::page_stack;
