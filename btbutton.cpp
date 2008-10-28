#include "btbutton.h"
#include "genericfunz.h"

#include <QStyleOption>
#include <QStylePainter>


BtButton::BtButton(QWidget *parent) : QPushButton(parent), icon_set(false)
{
	setStyleSheet(QString("border:0px;"));
	setFocusPolicy(Qt::NoFocus);
}

BtButton::BtButton(const QString &text, QWidget *parent) : QPushButton(text, parent), icon_set(false)
{
	setStyleSheet(QString("border:0px;"));
	setFocusPolicy(Qt::NoFocus);
}

void BtButton::setPressedPixmap(const QPixmap &p)
{
	pressed_pixmap = p;
}

void BtButton::setPixmap(const QPixmap &p)
{
	pixmap = p;
}

void BtButton::mousePressEvent(QMouseEvent *event)
{
	if (!pressed_pixmap.isNull())
	{
		setIcon(pressed_pixmap);
		setIconSize(pressed_pixmap.size());
		icon_set = true;
	}
	QPushButton::mousePressEvent(event);
}

void BtButton::mouseReleaseEvent(QMouseEvent *event)
{
	setIcon(pixmap);
	setIconSize(pixmap.size());
	icon_set = true;
	QPushButton::mouseReleaseEvent(event);
}

void BtButton::paintEvent(QPaintEvent *event)
{
	if (!icon_set)
	{
		setIcon(pixmap);
		setIconSize(pixmap.size());
		icon_set = true;
	}

#ifdef BEEP
	if (isDown())
		beep();
#endif

	// The drawing of BtButton is reimplemented here to prevent the "graying"
	// of the image when the button is in the disabled state.
	QStylePainter p(this);
	QStyleOptionButton option;
	initStyleOption(&option);

	option.state |= QStyle::State_Enabled; // Prevent the "graying"
	p.drawControl(QStyle::CE_PushButton, option);
}
