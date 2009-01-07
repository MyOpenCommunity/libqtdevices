#include "btbutton.h"
#include "global.h" // BTouch
#include "generic_functions.h" // beep

#include <QDebug>
#include <QFile>


BtButton::BtButton(QWidget *parent) : QPushButton(parent)
{
	setStyleSheet("border:0px;");
	setFocusPolicy(Qt::NoFocus);
	connect(this, SIGNAL(toggled(bool)), SLOT(toggled(bool)));
}

QSize BtButton::sizeHint() const
{
	if (!pixmap.isNull())
		return pixmap.size();
	return QSize();
}

void BtButton::setImage(const QString &icon_path, IconFlag f)
{
	setPixmap(*BTouch->getIcon(icon_path));

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
	setIcon(pixmap);
	setIconSize(pixmap.size());
}

void BtButton::mousePressEvent(QMouseEvent *event)
{
	QPixmap *p = &pixmap;

	if (!pressed_pixmap.isNull())
		p = &pressed_pixmap;

	setIcon(*p);
	beep();
	QPushButton::mousePressEvent(event);
}

void BtButton::mouseReleaseEvent(QMouseEvent *event)
{
	// Manages only normal buttons. Toggle buttons are managed by toggled slot.
	if (!isCheckable() && !pressed_pixmap.isNull())
		setIcon(pixmap);

	QPushButton::mouseReleaseEvent(event);
}

void BtButton::toggled(bool checked)
{
	if (!checked && !pressed_pixmap.isNull())
		setIcon(pixmap);
}
