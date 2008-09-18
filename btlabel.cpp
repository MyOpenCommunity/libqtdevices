#include "btlabel.h"

BtLabel::BtLabel(QWidget *parent, const char *name, Qt::WindowFlags f) : QLabel(name, parent, f)
{
}

void BtLabel::setEnabled(bool)
{
}

void BtLabel::setPaletteBackgroundColor(const QColor &c)
{
	setStyleSheet(QString("color: %1;").arg(c.name()));
}

void BtLabel::setPaletteForegroundColor(const QColor &c)
{
	setStyleSheet(QString("background-color: %1;").arg(c.name()));
}
