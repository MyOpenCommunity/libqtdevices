#ifndef NAVIGATION_BAR_H
#define NAVIGATION_BAR_H

#include <QWidget>

class BtButton;
class QLayout;


class NavigationBar : public QWidget
{
Q_OBJECT
public:
	NavigationBar(QString forward_icon=QString(), QString down_icon="scroll_down",
		QString up_icon="scroll_up", QString back_icon="back", QWidget *parent=0);

	// the forward button is a common customization of the NavigationBar, so we
	// do it public
	BtButton *forward_button;

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
	BtButton *back_button, *up_button, *down_button;
	QLayout *main_layout;
	BtButton *createButton(QString icon, const char *signal);
};


#endif // NAVIGATION_BAR_H
