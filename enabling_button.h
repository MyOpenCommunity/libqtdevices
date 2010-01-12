#ifndef ENABLING_BUTTON_H
#define ENABLING_BUTTON_H

#include "btbutton.h"

#include <QPixmap>

class QString;



class EnablingButton : public BtButton
{
Q_OBJECT
public:
using BtButton::setImage;
using BtButton::setPressedImage;

	enum Status
	{
		ON,
		OFF,
		DISABLED
	};

	EnablingButton(QWidget *parent = 0);
	void setDisabledImage(const QString &path);
	void setStatus(Status st);
	Status getStatus() { return status; }

private:
	QPixmap disabled_pixmap;
	Status status;
};


#endif // ENABLING_BUTTON_H
