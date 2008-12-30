#include "btbutton.h"
#include "btmain.h"
#include "main.h"
#include "generic_functions.h" // beep

#include <QDebug>
#include <QFile>


BtButton::BtButton(QWidget *parent) : QPushButton(parent)
{
	setStyleSheet("border:0px;");
	setFocusPolicy(Qt::NoFocus);
}

QSize BtButton::sizeHint() const
{
	if (!pixmap.isNull())
		return pixmap.size();
	return QSize();
}

void BtButton::setImage(const QString &icon_path, IconFlag f)
{
	pixmap = *BTouch->getIcon(icon_path);

	if (f == LOAD_PRESSED_ICON)
	{
		QString pressed_name = getPressName(icon_path);
		if (QFile::exists(pressed_name))
			setPressedImage(pressed_name);
	}
}

void BtButton::setPressedImage(const QString &pressed_icon)
{
	pressed_pixmap = *BTouch->getIcon(pressed_icon);
}

void BtButton::setPressedPixmap(const QPixmap &p)
{
	pressed_pixmap = p;
}

void BtButton::setPixmap(const QPixmap &p)
{
	pixmap = p;
	setFixedSize(p.size());
}

void BtButton::paintEvent(QPaintEvent *event)
{
	QPixmap *p = &pixmap;

	if (!pressed_pixmap.isNull() && (isDown() || isChecked()))
		p = &pressed_pixmap;

	setIcon(*p);
	setIconSize(p->size());
	if (isDown())
		beep();

	QPushButton::paintEvent(event);
}
