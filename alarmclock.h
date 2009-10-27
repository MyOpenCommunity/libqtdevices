/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** alarmclock.h
**
** the page to set the alarm clock
**
****************************************************************/

#ifndef ALARMCLOCK_H
#define ALARMCLOCK_H

#include "frame_receiver.h"
#include "hardware_functions.h"
#include "page.h"

class BtButton;
class bannFrecce;
class timeScript;

class QDateTime;
class QWidget;
class QLabel;
class QTimer;
class AlarmClockTime;
class AlarmClockFreq;
class AlarmClockSoundDiff;


/*!
  \class AlarmClock
  \brief This class is the implementation af the alarm set.

  \author Davide
  \date lug 2005
*/
class AlarmClock : public Page, FrameReceiver
{
	friend class AlarmClockTime;
	friend class AlarmClockFreq;
	friend class AlarmClockSoundDiff;

Q_OBJECT
public:
/*! \enum  Freq
  Differentiate the alarm set frequencies of operation
*/
	enum Freq
	{
		SEMPRE = 1,  /*!< Always -> every day*/
		ONCE = 0,  /*!< Once -> only at the first occurrence of the time selected after the alarm set was setted*/
		FERIALI = 2,  /*!< Week days -> only from monday to friday*/
		FESTIVI = 3,  /*!< Festive days -> only on Sunday and Saturday*/
		NESSUNO = 50  /*!< Never*/
	};

/*! \enum Type
  Differentiate the alarm set type
*/
	enum Type
	{
		BUZZER = 0,  /*!< The buzzer sounds and backlight flashes*/
		DI_SON = 1  /*!< The sound diffusion system is used*/
	};

	AlarmClock(Type t, Freq f, int hour, int minute);

/*!
  \brief Reads from the eeprom the alarm set state.
*/
	void inizializza();

	virtual void manageFrame(OpenMsg &msg);

/*!
  \brief Sets the number of the actual instance of this class among all the alarm set present in the project.
*/
	void setSerNum(int);

/*!
  \brief Sets the alarm set (dis)active.
*/
	void setActive(bool);

	/**
	 * Sets the alarm state according to the parameter. It doesn't update the configuration file.
	 */
	void _setActive(bool a);

	/**
	 * Returns the active state of the alarm clock. When alarm clock type is ONCE, this method
	 * returns false as soon as the alarm clock fires.
	 * \return True if the alarm clock is set, false otherwise.
	 */
	bool isActive();

public slots:
/*!
  \brief Show the frequency (once-always-mon/fri-sat-sun).
*/
	void showTypePage();

/*!
  \brief Show the sound diffusion page.
*/
	void showSoundDiffPage();

protected:
	virtual bool eventFilter(QObject *obj, QEvent *ev);

private slots:
	void freezed(bool b);

/*!
  \brief Draws the first page for alarm set setting and initializes some connections.
*/
	virtual void showPage();

/*!
  \brief Executed when the alarm set sequency is closed to save the data and adjust sound diffusion page if necessary.
*/
	void handleClose();

/*!
  \brief Executed every minute when alarm set is active to detect if it's time to make the alarm ser start.
*/
	void verificaSveglia();

/*!
  \brief Executed every three seconds to increase the soud volume during \a sound \a diffusion \a alarm \a set starting up.
*/
	void aumVol();

/*!
  \brief Executed every 100 ms to manage the \a buzzer  \a alarm \a set.
*/
	void buzzerAlarm();

/*!
  \brief Stops the alarm set.
*/
	void spegniSveglia(bool);

/*!
  \brief Sets the alarm frequency value
  */
	void setFreq(AlarmClock::Freq f);

private:
	Type type;
	Freq freq;
	uchar conta2min,sorgente,stazione, aggiornaDatiEEprom;
	int serNum;
	bool buzAbilOld;
	unsigned int contaBuzzer;
	QDateTime *oraSveglia;
	int volSveglia[AMPLI_NUM];
	bool gesFrameAbil, active;
	QTimer *minuTimer,*aumVolTimer;
	AlarmClockTime *alarm_time;
	AlarmClockFreq *alarm_type;
	AlarmClockSoundDiff *alarm_sound_diff;

signals:
	void alarmClockFired();
};

/*!
  \class AlarmClockTime
  \brief Used to set the alarm time.

  \author Davide
  \date lug 2005
*/
class AlarmClockTime : public Page
{
Q_OBJECT
public:
	AlarmClockTime(AlarmClock *alarm_page);

	QDateTime getDataOra() const;

private:
	BtButton *but[4];
	QLabel *Immagine;
	timeScript *dataOra;
	bannFrecce *bannNavigazione;
};

/*!
  \class AlarmClockFreq
  \brief Used to set the alarm frequency.

  \author Davide
  \date lug 2005
*/
class AlarmClockFreq : public Page
{
Q_OBJECT
public:
	AlarmClockFreq(AlarmClock *alarm_page);

private slots:
/*!
  \brief Executed when "once" frequency is selected.
*/
	void sel1(bool);

/*!
  \brief Executed when "always" frequency is selected.
*/
	void sel2(bool);

/*!
  \brief Executed when "mon-fri" frequency is selected.
*/
	void sel3(bool);

/*!
  \brief Executed when "sat-sun" frequency is selected.
*/
	void sel4(bool);

signals:
	void selectionChanged(AlarmClock::Freq freq);

private:
	BtButton *choice[4];
	QLabel *testiChoice[4];
	void setSelection(AlarmClock::Freq freq);
	bannFrecce *bannNavigazione;
};

class AlarmClockSoundDiff : public Page
{
Q_OBJECT
public:
	AlarmClockSoundDiff(AlarmClock *alarm_page);

/*!
  \brief Draws the first page for alarm set setting and initializes some connections.
*/
	virtual void showPage();

private slots:
/*!
  \brief Executed when the alarm set sequency is closed to save the data and adjust sound diffusion page if necessary.
*/
	void handleClose();

private:
	Page *difson;
};

#endif // ALARMCLOCK_H
