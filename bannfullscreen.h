/*!
 * \bannfullscreen.h
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief A set of full screen banners
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */
#ifndef BANNFULLSCREEN_H
#define BANNFULLSCREEN_H

#include "banner.h"
#include "device.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qbuttongroup.h>
#include <qdom.h>
#include <qlcdnumber.h>
#include <qdatetime.h>

class device_status;

class BannFullScreen : public banner
{
Q_OBJECT
public:
	BannFullScreen(QWidget *parent, const char *name);
	virtual void Draw();
	virtual void postDisplay() = 0;
	void setSecondForeground(QColor fg2);
	virtual void setBGColor(QColor bg);
	virtual void setFGColor(QColor bg);
public slots:
	virtual void status_changed(QPtrList<device_status> list) = 0;
protected:
	QColor second_fg;
};

class FSBannSimpleProbe : public BannFullScreen
{
Q_OBJECT
public:
	// mi manca di sicuro la descrizione della zona e il device da cui prendo le modifiche
	// forse posso spostare il set dei vari *ground in alcuni membri?
	FSBannSimpleProbe(QWidget *parent, QDomNode n, const char *name = 0);
	virtual void Draw();
	// void callmeBack() call this from sottoMenu to setNavBarMode with the correct icon
	virtual void postDisplay();
	void setSecondForeground(QColor fg2);
public slots:
	virtual void status_changed(QPtrList<device_status> list);

protected:
	/// Global layout for the banner
	QVBoxLayout main_layout;
	/// Measured temperature label and string
	QLabel *temp_label;
	QString temp;
	/// Zone description label and string
	QLabel *descr_label;
	QString descr;
};


class FSBann4zProbe : public FSBannSimpleProbe
{
Q_OBJECT
public:
	FSBann4zProbe(QWidget *parent, QDomNode n, const char *name = 0);
	virtual void Draw();
	virtual void postDisplay();
public slots:
	virtual void status_changed(QPtrList<device_status> list);
protected:
	QString setpoint;
	QLabel  *setpoint_label;
	// FIXME: setpoint e' la temperatura impostata mentre la rotellina e' `locale'
	// le impostazioni per il locale (rotellina) sono nella specifica del protocollo,
	// ie. 0 = (rotella su) 0, 1 = 1, ... , 11 = -1, 12 = -2, 13 = -3, 4 = Off, 5 = Antigelo
	QString local_temp;
	QLabel *local_temp_label;
private:
	bool isOff, isAntigelo;
};

class FSBannTermoReg4z : public FSBann4zProbe
{
Q_OBJECT
public:
	FSBannTermoReg4z(QWidget *parent, QDomNode n, const char *name = 0);
	virtual void Draw();
	virtual void postDisplay();
protected:
};

class FSBann4zFancoil : public FSBann4zProbe
{
Q_OBJECT
public:
	FSBann4zFancoil(QWidget *parent, QDomNode n, const char *name = 0);
	virtual void Draw();
	virtual void postDisplay();
	virtual void status_changed(QPtrList<device_status> list);
private:
	/**
	 * Creates fancoil buttons and loads icons
	 */
	void createFancoilButtons();
	QButtonGroup fancoil_buttons;
	int fancoil_status;
};

enum BannID
{
	fs_nc_probe = 1,                      // not controlled probe
	fs_4z_thermal_regulator,              // 4 zones thermal regulator device
	fs_4z_probe,                          // 4 zones controlled probe
	fs_4z_fancoil,                        // 4 zones controlled probe with fancoil
	fs_99z_thermal_regulator,             // 99 zones thermal regulator device
	fs_99z_probe,                         // 99 zones controlled probe
	fs_99z_fancoil,                       // 99 zones controlled probe with fancoil
	fs_manual,                            // settings: manual operation
	fs_manual_timed,                      // settings: timed manual operation
	fs_date,                              // settings: date edit
};

class FSBannFactory
{
public:
	static FSBannFactory *getInstance();
	BannFullScreen *getBanner(BannID id, QWidget *parent, QDomNode n);
private:
	static FSBannFactory *instance;
	FSBannFactory();
	FSBannFactory(const FSBannFactory &);
	FSBannFactory &operator= (const FSBannFactory&);
};


class FSBannManual : public BannFullScreen
{
Q_OBJECT
public:
	FSBannManual(QWidget *parent, const char *name);
	virtual void Draw();
	void postDisplay();
public slots:
	void setThermalRegulator();
	void status_changed(QPtrList<device_status> list);
protected:
	QVBoxLayout main_layout;
private:
	int temp;
	QLabel *temp_label;
private slots:
	void incSetpoint();
	void decSetpoint();
};

class FSBannManualTimed : public FSBannManual
{
Q_OBJECT
public:
	FSBannManualTimed(QWidget *parent, const char *name);
	virtual void Draw();
	void postDisplay();
public slots:
	void setThermalRegulator();
	void status_changed(QPtrList<device_status> list);
private slots:
	void incHours();
	void decHours();
	void incMin();
	void decMin();
private:
	int hours, minutes;
	QLCDNumber *num;
};

class FSBannDate : public BannFullScreen
{
Q_OBJECT
public:
	FSBannDate(QWidget *parent, const char *name);
	virtual void Draw();
	void postDisplay();
	QDate getDate();
public slots:
	//void setThermalRegulator();
	void status_changed(QPtrList<device_status> list);
private:
	/// display date set
	QLCDNumber *date_display;
	QVBoxLayout main_layout;
	QDate date;
private slots:
	void incDay();
	void incMonth();
	void incYear();
	void decDay();
	void decMonth();
	void decYear();
};
#endif // BANNFULLSCREEN_H
