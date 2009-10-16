#ifndef NAVIGATION_BAR_H
#define NAVIGATION_BAR_H

#include <QWidget>

class BtButton;
class QBoxLayout;


class NavigationBar : public QWidget
{
Q_OBJECT
public:
	NavigationBar(QString forward_icon=QString(), QString down_icon="scroll_down",
		QString up_icon="scroll_up", QString back_icon="back", QWidget *parent=0);

signals:
	void backClick();
	void upClick();
	void downClick();
	void forwardClick();

private:
	BtButton *back_button, *up_button, *down_button, *forward_button;
	QBoxLayout *main_layout;

	void addButton(QString icon, const char *signal);
};


#endif // NAVIGATION_BAR_H
