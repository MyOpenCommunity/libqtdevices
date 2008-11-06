/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** sveglia.h
**
**definizioni della pagine di impostazione sveglia
**
****************************************************************/

#ifndef SVEGLIA_H
#define SVEGLIA_H

#include <QFrame>

#define AMPLI_NUM 100
#define BASE_EEPROM 11360
#define KEY_LENGTH 5
#define AL_KEY "\125\252\125\252\125"
#define SORG_PAR 2

class contdiff;
class BtLabel;
class BtButton;
class bannFrecce;
class timeScript;
class QDateTime;


/*!
  \class sveglia
  \brief This class is the implementation af the alarm set.

  \author Davide
  \date lug 2005
*/
class  sveglia : public QFrame
{
Q_OBJECT
public:
    sveglia(QWidget *parent=0, const char *name=0,uchar freq=1, uchar t=0,contdiff* diso=NULL, char* frame=NULL, char*h="12", char* m="0");

/*!
  \brief Sets the number of the actual instance of this class among all the alarm set present in the project.
*/
	void setSerNum(int);

	BtButton *but[4];
	BtLabel *Immagine;
	BtButton *choice[4];
	BtLabel *testiChoice[4];

/*!
  \brief Sets the alarm set (dis)active.
*/
	void activateSveglia(bool);

/*!
  \brief Retrieves the activation state of the alarm set.
*/
	bool getActivation();

/*!
  \brief Reads from the eeprom the alarm set state.
*/
	void inizializza();

/*! \enum  sveFreq
  Differentiate the alarm set frequencies of operation
*/
	enum sveFreq
	{
		SEMPRE = 1,  /*!< Always -> every day*/
		ONCE = 0,  /*!< Once -> only at the first occurrence of the time selected after the alarm set was setted*/
		FERIALI = 2,  /*!< Week days -> only from monday to friday*/
		FESTIVI = 3,  /*!< Festive days -> only on Sunday and Saturday*/
		NESSUNO = 50  /*!< Never*/
	};

/*! \enum sveType
  Differentiate the alarm set type
*/
	enum sveType
	{
		BUZZER = 0,  /*!< The buzzer sounds and backlight flashes*/
		DI_SON = 1,  /*!< The sound diffusion system is used*/
		FRAME = 2   /*!< An \a Open \a frame is emitted (used in schedulations)*/
	};

signals:
/*!
  \brief Emitted to send \a Open \a Frame to the system.
*/
void sendFrame(char*);
void sendInit(char*);

/*!
  \brief Emitted when the object is closed.
*/
	void ImClosed();

/*!
  \brief Emitted to freeze the device.
*/
// TODO: rimuovere appena possibile!
	void freeze(bool);

/*!
  \brief Emitted to comunicate weather the alarm set is running or not.
*/
	void svegl(bool);

public slots:
/*!
  \brief Execute when the time for the alarm set is chosen to show the frequency (once-always-mon/fri-sat-sun).
*/
	void okTime();

/*!
  \brief Draws the first page for alarm set setting and initializes some connections.
*/
	void mostra();

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

/*!
  \brief Executed when the alarm set sequency is closed to save the data and adjust sound diffusion page if necessary.
*/
	void Closed();

/*!
  \brief Execute when the frequency for the alarm set is chosen to show the sound diffusion page if necessary.
*/
	void okTipo();

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
  \brief Analyzes the \a Open \a Frame incoming to understand how the end-user want his a sound \a diffusion \a alarm \a set to work.
*/
	void gestFrame(char*);

/*!
  \brief Stops the alarm set.
*/
	void spegniSveglia(bool);

private:
	void drawSelectPage();
	timeScript *dataOra;
	bannFrecce *bannNavigazione;
	uchar tipoSveglia,conta2min,sorgente,stazione,tipo,aggiornaDatiEEprom;
	int serNum;
	bool buzAbilOld;
	unsigned int contaBuzzer;
	QDateTime *oraSveglia;
	contdiff *difson;
	int volSveglia[AMPLI_NUM];
	bool gesFrameAbil,svegliaAbil,onceToGest;
	QTimer *minuTimer,*aumVolTimer;
	char *frame;
};

#endif // SVEGLIA_H
