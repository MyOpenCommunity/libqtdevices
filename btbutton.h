#ifndef BTBUTTON_H
#define BTBUTTON_H

#include <QPushButton>


class BtButton : public QPushButton
{
Q_OBJECT
public:
	BtButton(QWidget *parent=0);

	void setPressedPixmap(const QPixmap &p);
	void setPixmap(const QPixmap &p);

	void setToggleButton(bool) {}

protected:
	virtual void paintEvent(QPaintEvent *event);

private:
	/// The pixmap to show when the button is down
	QPixmap pressed_pixmap;
	/// The pixmap to show when the button is in normal state
	QPixmap pixmap;
};

#endif // BTBUTTON_H
