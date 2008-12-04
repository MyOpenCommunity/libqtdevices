/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** banner.h
 **
 **Definizioni per la costruzione di un banner generico
 **
 ****************************************************************/

#ifndef BANNER
#define BANNER


#define MAX_NUM_ICON    46
#define MAX_PRESS_ICON  5
#define MAX_TEXT        21
#define MAX_TEXT_2      11

#include <QWidget>


class BtButton;
class sottoMenu;

class QPixmap;
class QTimer;
class QLabel;


/*!
 * \class banner
 * \brief This is an abstract class that describes the general banner that is the general command line of the devices on the system.
 *
 * This class gives the methods for describing a general object giving the possibility to instantiate differet kind of object,
 * set/get the activation state, open address, background and foreground colors, and so on.
 * This class gives also the base pattern for interaction giving the basig signals/slots related to the buttons, openframe gestion etc.
 *
 * \author Davide
 * \date lug 2005
 */

class banner : public QWidget
{
Q_OBJECT
public:
	banner(QWidget *parent);
	virtual ~banner();

	/*!
	 * \brief sets the foundamental text one can see on the banner
	 */
	virtual void setText(const QString &);

	/*!
	 * \brief sets a possible additional text one can see on the banner
	 */
	void setSecondaryText(const QString &);

	/*!
	 *  \brief return the Pressed Icon Name
	 *  for a given IconName return the pressed status IconName
	 *  we added this to wrap GetPressName of genericFunz.cpp
	 */
	QString getPressedIconName(QString);

	/*!
	 * \brief sets the Icon of a item in the banner
	 *
	 * the arguments describe the item as defined in the oggettinoDelBanner, the image is passed thought the file name
	 */
	void SetIcons(int id, QString name);

	/*!
	 * \brief sets the Icon of a item in the banner
	 */
	void SetIcons(QString name, int type);

	/*!
	 * \brief sets the Icon of left and right button
	 *
	 * the arguments describe the images to put on the buttons passing the file names
	 */
	void SetIcons(QString sxIcon, QString dxIcon);

	/*!
	 * \brief sets the Icon of left and right button and the central label
	 *
	 * the arguments describe the images to put on the buttons and into the describing label passing the file names
	 */
	void SetIcons(QString sxIcon, QString dxIcon, QString centerIcon);

	/*!
	 * \brief sets the Icon of left and right button and the central label when the banner is active or not
	 *
	 * the arguments describe the images to put on the buttons and into the describing label passing the file names
	 */
	void SetIcons(QString sxIcon, QString dxIcon, QString centerActiveIcon, QString centerInactiveIcon);

	/*!
	 *  \brief sets the Icon of left and right button and the central variable in different levels label when the banner is active or not
	 *
	 * the arguments describe the images to put on the buttons and into the describing label passing the file names.
	 * The last argument tells the number of graphical levels (just as in amplifiers and dimmers) the interface has
	 * to visualize when the banner is in active state
	 */
	void SetIcons(QString sxIcon, QString dxIcon, QString centerActiveIcon, QString centerInactiveIcon, bool inactiveLevel);

	void SetIcons(QString sxIcon, QString dxIcon, QString centerInactiveIcon, QString centerUpIcon, QString centerDownIcon);
	/*!
	 *  \brief sets the Icon of left and right button and the central variable in different levels label when the banner is active or not. Also an image representing broken state is passed.
	 *
	 * the arguments describe the images to put on the buttons and into the describing label passing the file names. 
	 * The last argument tells the number of graphical levels (just as in amplifiers and dimmers) the interface has 
	 * to visualize when the banner is in active state. The 5° argument represent the broken state image path.
	 */
	void SetIcons(QString sxIcon, QString dxIcon, QString centerActiveIcon, QString centerInactiveIcon, QString breakIcon, bool inactiveLevel);

	/*!
	 * \brief Inserts an object in the banner.
	 *
	 * The arguments describe the object to insert (as described in oggettinoDelBanner), the position (x,y) and the dimension (w,h)
	 */
	void addItem(char, int, int, int, int);

	/*!
	 * \brief Draws all the objects composing the banner.
	 *
	 * This method automatically detects what compose the banner, which is its state and draws it in the right manner.
	 */
	virtual void  Draw();

	/**
	 * Returns a custom button, controlled by the banner, to be shown on the navBar.
	 * This button is completely under control of the banner which may decide to connect to the signal
	 * emitted by the button, change the pixmap and so on.
	 * \return Returns a pointer to a \a BtButton if present, 0 otherwise.
	 */
	virtual BtButton *customButton();

	/*!
	 *  \brief Changes the banner state.
	 *
	 *  If the argument is zero the banner assume that the state of what is controlled is "disabled" otherwise is "active".
	 */
	void impostaAttivo(char);

	/*!
	 *  \brief  Changes the address of what is controlled by the banner.
	 *
	 *  The argument  describe the Open What of the object controlled by the banner.
	 */
	void setAddress(QString addr);

	/*!
	 *  \brief Retrieves the address of what is controlled by the banner.
	 *
	 *  The returned value is the Open What of the object controlled by the banner.
	 */
	QString getAddress();

	/*!
	 *  \brief Sets the serial number of the banner.
	 *
	 *  The \a serial \a number is the progressive number among the total amount of similar banners present in the same subtree.
	 *  It is quite usefull to discriminate, for instance, between different \a wide \a awake in the setting subtree
	 */
	void setSerNum(int);

	/*!
	 *  \brief Retrieves the serial number of the banner.
	 *
	 *  The \a serial \a number is the progressive number among the total amount of similar banners present in the same subtree.
	 *  It is quite usefull to discriminate, for instance, between different \a wide \a awake in the setting subtree
	 */
	int getSerNum();

	/*!
	 *  \brief Retrieves the Id of the object controlled by the banner.
	 */
	char getId();

	/*!
	 *  \brief Sets the Id of the object controlled by the banner.
	 */
	void setId(char);

	/*!
	 *  \brief Sets the Value for the object controlled by the banner.
	 *
	 *  There's need of using this function when controlling a levelled device such as a dimmer or an amplifier.
	 *  The function in used to set the value of such an object.
	 */
	void setValue(char);

	/*!
	 *  \brief Retrieves the Value for the object controlled by the banner.
	 *
	 *  There's need of using this function when controlling a levelled device such as a dimmer or an amplifier.
	 *  The function in used to retrieve the value of such an object.
	 */
	char getValue();

	/*!
	 *  \brief Sets the value range for the object controlled by the banner.
	 *
	 *  There's need of using this function when controlling a levelled device such as a dimmer or an amplifier. 
	 *  The function in used to determine the minimum and maximum (in this order) value for such an object.
	 */
	void setRange(char,char);

	/*!
	 *  \brief Sets the step for the object controlled by the banner.
	 *
	 *  The object's value is decremented/incremented by this quantity
	 */
	void setStep(char);

	/*!
	 *  \brief Retrieves if the object controlled by the banner is \a active or not.
	 */
	unsigned char isActive();

	/*!
	 *  \brief Sets the parameters necessary for the animation of the image describing the image controlled by the banner.
	 *
	 *  The arguments of the method are: the period in ms and the number frame describing the animation.
	 */
	void setAnimationParams(int,int);
	void getAnimationParams(int&, int&);

	virtual void inizializza(bool forza=false);
	/*!
	 *  \brief Force an object of the banner to be hided.
	 *
	 *  The object to be hided is described by the \a oggettinoDelBanner description.
	 */
	void nascondi(char);

	/*!
	 *  \brief Force an object of the banner to be shown.
	 *
	 *  The object to be shown is described by the \a oggettinoDelBanner description.
	 */
	void mostra(char);

	/*!
	 *  \enum oggettinoDelBanner
	 *  \brief describes the possible items one can have into a banner
	 *
	 *  BUT1 - BUT2 - BUT3 - BUT4 are 4 possible button 
	 *  TEXT is the describing text
	 *  TEXT2 is an additional text 
	 *  ICON - ICON2 are possibles image describing the banner or its state
	 */
	enum oggettinoDelBanner
	{
		BUT1,
		BUT2,
		TEXT,
		ICON,
		ICON2,
		BUT3,
		BUT4,
		TEXT2,
	};

	virtual void addAmb(QString);
public slots:
	/*!
	 *  \brief Must be reimplemented to analyze the \a Open \a Frame incoming.
	 */
	virtual void gestFrame(char *);
	/*!
	 *  \brief Must be reimplemented to retrieve the state ofthe object controlled by the banner.
	 */
	virtual void  rispStato(char*);
	/*!
	 *  \brief Invoked on open ack reception
	 */
	virtual void openAckRx();
	/*!
	 *  \brief Invoked on open nak reception
	 */
	virtual void openNakRx();
	virtual void ambChanged(const QString &, bool, QString);
	/*!
	 *  \brief Parent of my parent changed
	 */
	virtual void grandadChanged(QWidget *newGrandad);
	/*!
	 *  \brief Parent changed
	 */
	virtual void parentChanged(QWidget *newParent);

protected:
	QLabel *BannerIcon;
	QLabel *BannerIcon2;
	QLabel *BannerText;
	QLabel *SecondaryText;
	BtButton *sxButton;
	BtButton *dxButton;
	BtButton *csxButton;
	BtButton *cdxButton;
	/**
	 * Icons is a vector with pointers to QPixmap.
	 * When an icon is needed, iconsLibrary is asked to create
	 * a QPixmap and return the pointer. This pointer is stored
	 * in Icons, instead to manually create a QPixmap and store the
	 * pointer. In this way iconsLibrary do not waste memory.
	 *
	 * For instance if we have
	 *
	 * if(!Icon[0])
	 * Icon[0] = new QPixmap();
	 * Icon[0]->load(actuallcon);
	 *
	 * we instead write
	 *
	 * if(!Icon[0]) Icon[0] = iconsLibrary.getIcon(actuallcon)
	 *
	 * NOW iconsLibrary has its own destructor for icons
	 * no need to destroy them in banner
	 */
	QPixmap *Icon[MAX_NUM_ICON];
	QPixmap *pressIcon[MAX_PRESS_ICON];

	QString qtesto, qtestoSecondario;

	char attivo,value,maxValue,minValue,id,step;
	int periodo, numFrame,contFrame,serNum;
	QString address;

	/**
	 * Utility function to draw all buttons except the rightmost one
	 */
	void drawAllButRightButton();
private:
	QTimer *animationTimer;
	/**
	 *   Utility functions to get icon name root. For istance
	 *   from "ondimmer.png" we can get "ondimmer" in a generic way
	 */
	QString getNameRoot(QString full_string, QString text_to_strip);


private slots:
	void animate();

signals:
	/*!
	 *  \brief Emitted when the right button is clicked.
	 */
	void dxClick();
	/*!
	 *  \brief Emitted when the left button is clicked.
	 */
	void sxClick();
	/*!
	 *  \brief Emitted when the center-right button is clicked.
	 */
	void cdxClick();
	/*!
	 *  \brief Emitted when the center-left button is clicked.
	 */
	void csxClick();
	/*!
	 *  \brief Emitted when the left button is released.
	 */
	void sxReleased();
	/*!
	 *  \brief Emitted when the right button is released.
	 */
	void dxReleased();
	/*!
	 *  \brief Emitted when the left button is pressed.
	 */
	void sxPressed();
	/*!
	 *  \brief Emitted when the right button is pressed.
	 */
	void dxPressed();
	/*!
	 *  \brief Emitted when the center-left button is released.
	 */
	void csxReleased();
	/*!
	 *  \brief Emitted when the center-right button is released.
	 */
	void cdxReleased();
	/*!
	 *  \brief Emitted when the center-left button is pressed.
	 */
	void csxPressed();
	/*!
	 *  \brief Emitted when the center-right button is pressed.
	 */
	void cdxPressed();
	// TODO: forse sarebbe meglio evitare il rimpallo di segnali per fare una richiesta
	// di stato e usare direttamente BTouch.. ma poi c'e' anche il rischio di abusare
	// troppo di quella variabile globale!
	void richStato(QString);
};



#endif //BANNER
