#ifndef PAGESTACK_H
#define PAGESTACK_H

#include <QList>
#include <QObject>

class Page;
class Window;


// manages the stack of pages/window active on the screen and decides which
// is the next window/page to be shown when the top page is closed/destroyed
class PageStack : public QObject
{
	struct State
	{
		State(Window *window);
		State(Page *page);

		// either window or page are set, not both
		Window *window;
		Page *page;

		// returns either window or page, depending on which is set
		QObject *object() const;
	};

Q_OBJECT
public:
	PageStack();

	// register when different types of window/pages are shown; must be
	// called before Page::showPage() and Window::showWindow()
	void showAlarm(Page *alarm_page);
	void showVCTCall(Page *call_page);
	void showUserWindow(Window *window);
	void showScreensaver(Window *screensaver);
	void showKeypad(Window *keypad);

	// removes the given object from the page stack and shows the
	// topmost stack object; when an object is removed from the middle
	// of the stack, no action is performed
	void closeWindow(Window *window);
	void closePage(Page *page);

public slots:
	// tracks the currently displayed page
	void currentPageChanged(Page *page);

private:
	void showState(const State &state);
	void addState(const State &state);
	void removeFromStack(QObject *obj);

private slots:
	void closed();
	void removeObject(QObject *obj);

private:
	QList<State> states;
};

namespace bt_global { extern PageStack page_stack; }

#endif // PAGESTACK_H
