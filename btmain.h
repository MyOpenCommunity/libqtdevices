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
#include <QHash>

class sottoMenu;
class SoundDiffusion;
class MultiSoundDiff;
class Antintrusion;
class ThermalMenu;
class Version;
class homePage;
class Calibrate;
class genPage;
class Client;
class tastiera;
class ScreenSaver;
class SupervisionMenu;
class SpecialPage;
class Page;

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
friend class PageContainer;
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
	void setPwd(bool, QString);

private slots:
	void hom();
	void init();
	void gesScrSav();
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
	SoundDiffusion *difSon;
	MultiSoundDiff *dm;
	Version *datiGen;

private:
	QHash<int, Page*> page_list;

	/// build the page related to id if exists and return it, otherwise return 0
	Page *getPage(int id);

	homePage *Home;
	Page *pagDefault;

	QTimer *tempo1;
	QTimer *tempo2;
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
	bool loadStyleSheet(QString filename);

signals:
	void resettimer();
	void freezed(bool);
};

#endif// BTMAIN_H
