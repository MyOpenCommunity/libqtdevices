#ifndef NAVIGATION_BAR_H
#define NAVIGATION_BAR_H

#include <QWidget>

class BtButton;


class NavigationBar : public QWidget
{
Q_OBJECT
public:
	NavigationBar(QString forward_icon=QString(), QString down_icon="scroll_down",
		QString up_icon="scroll_up", QString back_icon="back", QWidget *parent=0);

public slots:
	void displayScrollButtons(bool display);

signals:
	void backClick();
	void upClick();
	void downClick();
	void forwardClick();

protected:
	virtual QSize sizeHint() const;

private:
	BtButton *back_button, *up_button, *down_button, *forward_button;

	BtButton *createButton(QString icon, const char *signal, int pos);
};


#endif // NAVIGATION_BAR_H
