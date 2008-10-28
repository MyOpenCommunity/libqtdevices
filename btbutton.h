#ifndef BTBUTTON_H
#define BTBUTTON_H

#include <QPushButton>


class BtButton : public QPushButton
{
Q_OBJECT
public:
	BtButton(QWidget *parent=0);
	BtButton(const QString &text, QWidget *parent=0);

	void setPressedPixmap(const QPixmap &p);
	void setPixmap(const QPixmap &p);

	void setOn(bool) {}
	void setToggleButton(bool) {}

protected:
	virtual void paintEvent(QPaintEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);

private:
	/// The pixmap to show when the button is down
	QPixmap pressed_pixmap;
	/// The pixmap to show when the button is in normal state
	QPixmap pixmap;

	// A flag used internally to means if the icon is setted or not.
	bool icon_set;
};

#endif // BTBUTTON_H
