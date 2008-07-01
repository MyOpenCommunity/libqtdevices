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
#include "device_status.h"
#include "btwidgets.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qbuttongroup.h>
#include <qdom.h>
#include <qlcdnumber.h>
#include <qdatetime.h>

class device;
class thermal_regulator;
class thermal_regulator_4z;

/**
 * An interface class for all full screen banners.
 */
class BannFullScreen : public banner
{
Q_OBJECT
public:
	BannFullScreen(QWidget *parent, const char *name);
	virtual void Draw();

	/**
	 * A function called by sottoMenu to request the banner which type of navbar it needs.
	 */
	virtual void postDisplay(sottoMenu *parent) = 0;

	void setSecondForeground(QColor fg2);

	/**
	 * This function is reimplemented from banner because fullscreen banners don't use
	 * the same items as banners (BUT1, BUT2, TEXT and so on).
	 */
	virtual void setBGColor(QColor bg);
	virtual void setFGColor(QColor bg);
public slots:
	/**
	 * Called whenever the status of the device associated with the banner changes, so that
	 * the banner can display the variations.
	 */
	virtual void status_changed(QPtrList<device_status> list) = 0;
protected:
	QColor second_fg;
};

/**
 * A base class for banners that represent a probe. It displays a label with zone name on top
 * and the measured temperature.
 */
class FSBannSimpleProbe : public BannFullScreen
{
Q_OBJECT
public:
	FSBannSimpleProbe(QWidget *parent, QDomNode n, const char *name = 0);
	virtual void Draw();
	virtual void postDisplay(sottoMenu *parent);
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

/**
 * Displays information about a probe controlled by a 4 zones thermal regulator.
 * It has a label with setpoint temperature and local status. In case the status is
 * protection or off, it displays the relative icon.
 */
class FSBann4zProbe : public FSBannSimpleProbe
{
Q_OBJECT
public:
	FSBann4zProbe(QWidget *parent, QDomNode n, const char *name = 0);
	virtual void Draw();
	virtual void postDisplay(sottoMenu *parent);
public slots:
	virtual void status_changed(QPtrList<device_status> list);
protected:
	QString setpoint;
	QLabel  *setpoint_label;

	BtButton *btn_off, *btn_antifreeze;
	// FIXME: setpoint e' la temperatura impostata mentre la rotellina e' `locale'
	// le impostazioni per il locale (rotellina) sono nella specifica del protocollo,
	// ie. 0 = (rotella su) 0, 1 = 1, ... , 11 = -1, 12 = -2, 13 = -3, 4 = Off, 5 = Antigelo
	QString local_temp;
	QLabel *local_temp_label;
private:
	bool isOff, isAntigelo;
};

/**
 * Displays information about the thermal regulator device. Information may be: operation mode, label
 * describing more information about mode (for example, the program name), current season.
 * Operation mode and current season are icons and are always present. The label is optional and depends
 * on the active mode: it is present for manual (setpoint temperature) and week program (program name).
 */
class FSBannTermoReg4z : public BannFullScreen
{
Q_OBJECT
public:
	FSBannTermoReg4z(QWidget *parent, QDomNode n, const char *name = 0);
	virtual void Draw();
	virtual void postDisplay(sottoMenu *parent);
public slots:
	virtual void status_changed(QPtrList<device_status> list);
private:
	/**
	 * Utility function to find in the DOM the program description to be displayed on screen.
	 * \param season The season we are interested into. It must be either "summer" or "winter".
	 * \param program_number The number of the program we are looking the description of.
	 * \return The description of the program as written in DOM.
	 */
	QString lookupProgramDescription(QString season, int program_number);

	QVBoxLayout main_layout;
	/// Label and string that may be visualized
	QLabel *description_label;
	QString description;
	/// Status icon (summer/winter)
	BtButton *season_btn;
	/// Mode icon (off, protection, manual, week program, holiday, weekend)
	BtButton *mode_btn;
	/// A reference to the configuration of the thermal regulator
	QDomNode conf_root;
};

/**
 * Displays information about a controlled probe with fancoil. In addition to FSBann4zProbe, it displays
 * at the bottom of the page 4 buttons to set the speed of fancoil.
 */
class FSBann4zFancoil : public FSBann4zProbe
{
Q_OBJECT
public:
	FSBann4zFancoil(QWidget *parent, QDomNode n, const char *name = 0);
	virtual void Draw();
	virtual void postDisplay(sottoMenu *parent);
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
	fs_date_edit,                         // settings: date edit
	fs_time_edit,                         // settings: time edit
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
	FSBannManual(QWidget *parent, const char *name, thermal_regulator *_dev);
	virtual void Draw();
	void postDisplay(sottoMenu *parent);
public slots:
	void status_changed(QPtrList<device_status> list);
protected:
	QVBoxLayout main_layout;
private:
	QString descr;
	QLabel *descr_label;
	int temp;
	QLabel *temp_label;
	thermal_regulator *dev;
private slots:
	void incSetpoint();
	void decSetpoint();
};

/**
 * A fullscreen banner to edit setpoint temperature and the duration of manual settings
 */
class FSBannManualTimed : public FSBannManual
{
Q_OBJECT
public:
	FSBannManualTimed(QWidget *parent, const char *name, thermal_regulator_4z *_dev);
	virtual void Draw();
	void postDisplay(sottoMenu *parent);
public slots:
	void status_changed(QPtrList<device_status> list);
private:
	thermal_regulator_4z *dev;
	/// TimeEdit widget
	BtTimeEdit *time_edit;
};

class FSBannDate : public BannFullScreen
{
Q_OBJECT
public:
	FSBannDate(QWidget *parent, const char *name = 0);
	virtual void Draw();
	void postDisplay(sottoMenu *parent);
	QDate date();
public slots:
	void status_changed(QPtrList<device_status> list);
private:
	QVBoxLayout main_layout;
	QDate _date;
	BtDateEdit *date_edit;
signals:
	void dateChanged(QDate);
};

class FSBannTime : public BannFullScreen
{
Q_OBJECT
public:
	FSBannTime(QWidget *parent, const char *name = 0);
	virtual void Draw();
	void postDisplay(sottoMenu *parent);
	QTime time();
public slots:
	void status_changed(QPtrList<device_status> list);
	void setTime(int hrs, int mins);
private:
	QVBoxLayout main_layout;
	BtTimeEdit *time_edit;
	int hours, minutes;
signals:
	void timeChanged(QTime);
};
#endif // BANNFULLSCREEN_H
