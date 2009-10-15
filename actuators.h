/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** actuators.h
 **
 **definizioni dei vari attuatori implementati
 **
 ****************************************************************/

#ifndef ACTUATORS_H
#define ACTUATORS_H

#include "bann2_buttons.h" // bannOnOff
#include "bann1_button.h" // bannPuls
#include "main.h" // MAX_PATH
#include "banner.h"

class device;
class device_status;
class LightingDevice;
class BtButton;
class QLabel;


/*
 * This class is a new style banner which creates and has handles to its own buttons
 * and labels.
 * New style banners have business logic and graphics divided in two classes: the graphics class
 * (base class for logic banners) will expose to logic banners a simple interface:
 * - one function to set icons: the graphics banner knows how to load icons;
 * - one to set text on each label that has text;
 * - one function to change state: icon changes are managed by logic banners only through
 * this function.
 * These are only guidelines, each graphics banner can declare its own interface (for example
 * to set the volume level or dimmer level). However you are invited to make the interface
 * similar to this one to maintain a standard coding style.
 * Buttons are created protected so that logic banners can manipulate them directly, thus
 * avoiding BtButton interface duplication. All other elements are created private.
 */
class BannOnOffState : public banner
{
Q_OBJECT
public:
	enum States
	{
		ON,
		OFF,
	};
	BannOnOffState(QWidget *parent);
	void loadIcons(QString l, QString c, QString r);
	void setState(States new_state);
	void setPrimaryText(const QString &text);

protected:
	BtButton *sx_button, *dx_button;

private:
	QLabel *center_icon;
	QLabel *text;
	QString left, center, right;
};


class SingleActuator : public BannOnOffState
{
Q_OBJECT
public:
	SingleActuator(QWidget *parent, const QDomNode &config_node, QString address);
	virtual void inizializza(bool forza = false);

private slots:
	void activate();
	void deactivate();
	void status_changed(const StatusList &status_list);

private:
	LightingDevice *dev;
};


#if 1
/*!
 * \class attuatAutom
 * \brief This is the \a automation \a actuator-banner class.
 * This class is used to represent both lightings and various kind of automation such as fan and irrigation.
 * \author Davide
 * \date lug 2005
 */
class attuatAutom : public bannOnOff
{
Q_OBJECT

public:
	attuatAutom(QWidget *parent, QString where, QString IconaSx, QString IconaDx, QString IconActive,
		QString IconDisactive);
public slots:
	void status_changed(QList<device_status*>);
private:
	char gruppi[4];
	device *dev;
	void inizializza(bool forza=false);
private slots:
	void Attiva();
	void Disattiva();
};


/*!
 * \class attuatPuls
 * \brief This class is made to manage a pulse automation.
 *
 * This object is useful when there's the need to activate something only during the pressure time 
 * (for instance when dealing with a lock).
 * \author Davide
 * \date lug 2005
 */
class attuatPuls : public bannPuls
{
Q_OBJECT
public:
	attuatPuls(QWidget *parent, QString where, QString IconaSx, QString IconActive, char tipo);
private slots:
	void Attiva();
	void Disattiva();
private:
	char type;
};
#endif

#endif
