/****************************************************************
**
** Definizione della classe della pagina principale
**
****************************************************************/

#ifndef BT_HOMEPAGE
#define BT_HOMEPAGE

#include "main.h"
#include "btbutton.h"

#include <QWidget>
#include <QString>
#include <QList>

class QLCDNumber;
class BtLabel;
class timeScript;

#define MAX_BUT_SOTMEN 9
#define DIM_BUT_HOME 80
#define DIM_BUT_HOME_SMALL 60
#define DIM_SPECBUT_HOME 180
#define H_SCR_TEMP 20


/*!
  \class homePage
  \brief This is the class used to implement the main page from which access to all the subtree (sottoMenu) of the various bticino functions. This class is also used for the special page.

  It can be composed of a date field, a time field, up to three temperatures, a number of BtButton and a special command. 
  \author Davide
  \date lug 2005
*/

class homePage : public QWidget
{
Q_OBJECT
public:
	homePage(QWidget *parent, const char *name);

/*!
  \brief Adds a button to the page.

  The arguments are the position of the button (x,y),  the fileName of the image to put on the button, the function associated to the button as described in pagSecLiv (main.h), the \a Open parameters of the commands of the special command (who, what, where), and the type of the special command as descibed in tipoFunzSpe.
*/
	void addButton(int x=0 , int y=0 , QString iconName=QString(), char function = 0,QString chi=QString(), QString cosa=QString(), QString dove=QString(), char tipo=0);
/*!
  \brief Adds a Clock.

  The arguments are the position of the field (x,y),  the dimensions (w,h), background and foreground color, style and line as for BtLabel.
*/
	void addClock(int, int, int, int, int, int);
/*!
  \brief Same as above. Dimension is assumed 185x35, the colors used are the same of the entire page, with no frame and line.
*/
	void addClock(int , int);
/*!
  \brief Adds a Date.

  The arguments are the position of the field (x,y),  the dimensions (w,h), background and foreground color, style and line as for BtLabel.
*/
	void addDate(int, int, int, int, int, int);
/*!
  \brief Same as above. Dimension is assumed 185x35, the colors used are the same of the entire page, with no frame and line.
*/
	void addDate(int , int);
/*!
  \brief Adds a Temperature.

  The arguments are the zone associated to the temperature measurement, the position of the field (x,y),  the dimensions (w,h), background and foreground color, style and line as for BtLabel and the text describing the zone.
*/
	void addTemp(QString, int, int, int, int, int, int, const QString &, char * Ext="0");

  /*!
  \brief Same as above. Dimension is assumed 185x35, the colors used are the same of the entire page, with no frame, line and text.
*/
	void addTemp(QString, int , int);
/*!
  \brief Adds a Description usually used for special page.

  The arguments are the text, the position of the field (x,y),  the dimensions (w,h), background and foreground color, style and line as for BtLabel.
*/
	void addDescrU(const QString &, int, int, int, int, int, int);
 /*!
  \brief Same as above. Dimension is assumed 160x20, the colors used are the same of the entire page, with no frame, line and text.
*/
	void addDescrU(const QString &, int , int);

/*! \enum tipoFunzSpe
   This enum let decide the behavior of the special button*/
	enum tipoFunzSpe
	{
		NORMALE = 0,  /*!<  Clicking the \a special \a button the device always make the same function.*/
		CICLICO = 1,  /*!< Clicking the \a special \a button the device one time make a \a ON command and then an \a OFF command an so on.*/
		PULSANTE = 2   /*!<  Pushing the \a special \a button the device make a \a ON command while Releasing the button a \a OFF command is made.*/
	};

	void inizializza();

private slots:
	void gestFrame(char*);
	void specFunz();
	void specFunzPress();
	void specFunzRelease();

signals:
/*! \brief Emitted when the \a automation subtree(sottoMenu) is required.*/  
	void Automazione();
/*! \brief Emitted when the \a lighting subtree(sottoMenu) is required.*/  
	void Illuminazione();
/*! \brief Emitted when the \a anti-intrusion subtree(sottoMenu) is required.*/  
	void Antiintrusione();
/*! \brief Emitted when the \a electrict control subtree(sottoMenu) is required.*/  
	void Carichi();
/*! \brief Emitted when the \a thermoregulation subtree(sottoMenu) is required.*/  
	void Termoregolazione();
/*! \brief Emitted when the \a sound \a diffusion subtree(sottoMenu) is required.*/  
	void Difson();
/*! \brief Emitted when the \a multichannel \a diffusione subtree is required. */
	void Difmulti();
/*! \brief Emitted when the \a scenarios subtree(sottoMenu) is required.*/  
	void Scenari();
/*! \brief Emitted when the \a settings subtree(sottoMenu) is required.*/  
	void Settings();
/*! \brief Emitted when the \a special \a function is required.*/
	void Special();
/*! \brief Emitted when the the object is closed.*/
	void Close();
/*! \brief Emitted to send \a Open \a frame on the system.*/
	void sendFrame(char*);
	void sendInit(char*);
/*! \brief Emitted when the \a schedulation subtree(sottoMenu) is required.*/
	void Schedulazioni();
/*! \brief Emitted when the \a advanced scenarios management(sottoMenu) is required */
	void ScenariEvoluti();
/*! \brief Emitted when the \a video door phone management(sottoMenu) is required */
	void Videocitofonia();
/*! \brief Emitted when the \a supervision management is required */
	void Supervisione();

private:
  /**
   * Updates the display when the temperature changes.
   * \param new_bt_temperature The new temperature in BTicino 4-digit format.
   * \param which_display The index of the display to update.
   */
	void updateTemperatureDisplay(unsigned new_bt_temperature, unsigned which_display);

	timeScript* dataOra;
	int xClock, yClock, xTemp, yTemp;
	QList<BtButton*> elencoButtons;
	char zonaTermo1[50], zonaTermo2[50], zonaTermo3[50];
	char *zt[3];
	char ext1[2], ext2[2], ext3[2];
	char *ext[3];
	QLCDNumber *temperatura[3];
	/// The temperature scale set in config file (either Celsius or Fahrenheit)
	TemperatureScale temp_scale;
	char specialFrame[50];
	char chi[10];
	char cosa[10];
	char dove[10];
	char tipoSpecial;
	unsigned int tempCont;
	BtLabel *descrizione, *descrTemp[3];
};


#endif // BT_HOMEPAGE
