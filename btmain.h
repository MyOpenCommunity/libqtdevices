/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** BtMain.cpp
**
**Apertura pagina iniziale e associazione tasti-sottomenù
**
****************************************************************/

#ifndef BTMAIN_H
#define BTMAIN_H

#include "icondispatcher.h"

#include <QWidget>

class sottoMenu;
class diffSonora;
class diffmulti;
class Antintrusion;
class ThermalMenu;
class versio;
class homePage;
class Calibrate;
class genPage;
class Client;
class tastiera;
class ScreenSaver;
class SupervisionMenu;
class QPixmap;
class QString;

/*!
  \class BtMain
  \brief This is a class used to manage the starting sequence, call the xml handler that builds the objects configured, shunt signals and controlling screen-saver and special page displaying (such as configuration page, colored pages and so on).

  This is the widget used for screen saver but has the main aim to connect the various class passing signal from one to the others. This class supervise the freezing (baclight off and uman inteface disabled) and the starting sequence also deciding if there's need of a new calibration.
  \author Davide
  \date lug 2005
*/

class BtMain : public QWidget
{
Q_OBJECT
public:
	BtMain(QWidget *parent=0);
	~BtMain();
	Client *client_richieste;
	Client *client_comandi;
	Client *client_monitor;

	void resetTimer();
	/// Freeze or unfreeze the application
	void freeze(bool);
	/// Set on/off the sveglia status
	void svegl(bool);

	// Return a pixmap* to the icon with 'name', using the cache of icons.
	QPixmap* getIcon(QString name, const char *format=0, Qt::ImageConversionFlags flags=Qt::AutoColor);

	// A global way to send frames/init requests. Do not use these directly, prefer using
	// devices specific methods, unless you have to send frames without reading responses.
	void sendFrame(QString frame);
	void sendInit(QString frame);

private slots:
	void hom();
	void init();
	void gesScrSav();
	void setPwd(bool, QString);
	void testPwd(char*);
	void testFiles();
	void waitBeforeInit();
	void monitorReady();

public slots:
	void startCalib();
	void endCalib();

protected:
	virtual bool eventFilter(QObject *obj, QEvent *ev);

public:
	// TODO: vedere se ci puo' evitare di rendere questi membri pubblici!
	diffSonora *difSon;
	diffmulti *dm;
	versio *datiGen;

private:
	homePage *Home, *specPage;
	sottoMenu *illumino,*scenari,*videocitofonia,*carichi,*imposta,*automazioni,*scenari_evoluti;
	ThermalMenu *termo;
	Antintrusion *antintr;
	SupervisionMenu *supervisione;
	QTimer *tempo1;
	QTimer *tempo2;
	QWidget *pagDefault;
	QString pwd;
	bool pwdOn,svegliaIsOn,alreadyCalibrated;
	tastiera *tasti;
	bool event_unfreeze;
	bool firstTime, bloccato;
	bool pd_shown;
	genPage *screen;
	unsigned char tiposcreen;
	unsigned long tiempo_ev;
	unsigned long tiempo_last_ev;
	bool calibrating;
	Calibrate *calib;
	ScreenSaver *screensaver;

	// A flag that is set when the client monitor socket is ready
	bool monitor_ready;
	// A flag that is set when the configuration has loaded
	bool config_loaded;
	// The manager of icons
	IconDispatcher icons_library;

	void myMain();
	/// Load the main configuration
	bool loadConfiguration(QString cfg_file);
	/// Load the skin configuration
	bool loadSkin(QString xml_file);

signals:
	void resettimer();
	void freezed(bool);
};

#endif// BTMAIN_H
