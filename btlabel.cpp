#include "btlabel.h"
#include "btstyle.h"

BtLabel::BtLabel(QWidget *parent) : QLabel(parent)
{
	setStyle(new BtStyle());
}

