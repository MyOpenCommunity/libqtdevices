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


#define MAX_NUM_ICON		42
#define MAX_TEXT 		21
#define MAX_TEXT_2		11


#include "btbutton.h"
#include "genericfunz.h"
#include <qwidget.h>
#include "btlabel.h"
#include "main.h"
#include <qpixmap.h>
#include <qtimer.h>

/*!
  \class banner
  \brief This is an abstract class that describes the general banner that is the general command line of the devices on the system.
  
  This class gives the methods for describing a general object giving the possibility to instantiate differet kind of object, set/get the activation state, open address, background and foreground colors, and so on. This class gives also the base pattern for interaction giving the basig signals/slots related to the buttons, \a open frame gestion etc.
  \author Davide
  \date lug 2005
*/  

class banner : public QWidget
{
    Q_OBJECT
public:
    banner( QWidget *, const char *);
    ~banner();
/*!
  \brief sets the foundamental text one can see on the banner
*/  
    virtual void SetText(const char *);
/*!
  \brief sets a possible additional text one can see on the banner
*/      
    void 	SetSeconaryText( const char * );
/*!
  \brief sets the Icon of a item in the banner
 
 the arguments describe the item as defined in the oggettinoDelBanner, the image is passed thought the file name
*/      
    void 	SetIcons( uchar , const char * );
/*!
  \brief sets the Icon of a item in the banner
*/          
    void 	SetIcons(const char *,char);
/*!
  \brief sets the Icon of left and right button
 
 the arguments describe the images to put on the buttons passing the file names
*/          
    void 	SetIcons(const char *, const char *);
/*!
  \brief sets the Icon of left and right button and the central label
 
 the arguments describe the images to put on the buttons and into the describing label passing the file names
*/              
    void 	SetIcons(const char *, const char *,const char *);
/*!
  \brief sets the Icon of left and right button and the central label when the banner is active or not
  
  the arguments describe the images to put on the buttons and into the describing label passing the file names
*/                  
    void 	SetIcons(const char *, const char *,const char *,const char *);
/*!
  \brief sets the Icon of left and right button and the central label when the banner is active or not
  
  the arguments describe the images to put on the buttons and into the describing label passing the file names. The 5° and 6° argument descrive respectively the period and the number of frame considering that the label is rapresented by a animated image when the banner is in active state
*/         
    void 	SetIcons(const char *, const char *,const char *,const char *,int,int);
/*!
  \brief sets the Icon of left and right button and the central variable in different levels label when the banner is active or not
  
the arguments describe the images to put on the buttons and into the describing label passing the file names. The last argument tells the number of graphical levels (just as in amplifiers and dimmers) the interface has to visualize when the banner is in active state
*/        
    void 	SetIcons(const char *, const char *,const char *,const char *,char);
    
    void 	SetIcons( const char * , const char *,const char*,const char*,const char*);
/*!
  \brief sets the Icon of left and right button and the central variable in different levels label when the banner is active or not. Also an image representing broken state is passed.
  
  the arguments describe the images to put on the buttons and into the describing label passing the file names. The last argument tells the number of graphical levels (just as in amplifiers and dimmers) the interface has to visualize when the banner is in active state. The 5° argument represent the broken state image path.
*/            
    void    SetIcons( const char *,const char* ,const char*,const char*, const char*, char );
    void 	addItem(char ,int ,int ,int , int );
    virtual void  Draw();
    void 	impostaAttivo(char);
    void 	setAddress(char* );
    char*	 getAddress( );
    void 	setChi(char* );
    char*	 getChi( );
    void 	setGroup(bool* );
    bool*	 getGroup( );
    void 	setPul();
    bool 	getPul();
    void 	setSerNum(int);
    int 	getSerNum();
    char 	getId();
    void 	setId(char);    
    virtual void 	setBGColor(int, int , int );
    virtual void 	setFGColor(int , int , int );		
    virtual void 	setBGColor(QColor );
    virtual void 	setFGColor(QColor );	
   
    void 	setValue(char);
    void 	aumValue();
    void 	decValue();
    char 	getValue();
//    char* getChi();
    void 	setMaxValue(char);
    void 	setMinValue(char);
    void 	setRange(char,char);
    unsigned char isActive();
    void 	setAnimationParams(int,int);
    void 	setNumRighe(uchar);
    QTimer *animationTimer;
    uchar 	numRighe;
    unsigned char stato;
    virtual void inizializza();	
    //provvisoriamente per debug li metto public
    char testo[MAX_PATH*2],testoSecondario[MAX_TEXT_2];
    void	nascondi(char);
    void	mostra(char);
    unsigned char getState();
    virtual 	char* 	getManIcon();
    virtual 	char* 	getAutoIcon();
    
public slots:
   virtual void gestFrame(char *);
   void 	animate();
   virtual void  rispStato(char*);
   virtual void hide();
   virtual void show();
signals:
  void 	dxClick(); 
  void 	sxClick(); 
  void 	cdxClick(); 
  void 	csxClick();
  void	sxReleased();
  void 	dxReleased();
  void	sxPressed();
  void 	dxPressed();
  void 	csxReleased();
  void 	cdxReleased();
  void 	csxPressed();
  void 	cdxPressed();
  virtual void freezed(bool);  
  virtual void freeze(bool);
  virtual void svegl(bool);
  virtual void richStato(char*);
  virtual void sendFrame(char*);
 virtual void killMe(banner*);
  
private:
/*   QLabel * BannerIcon;
   QLabel * BannerText;
   QLabel * SecondaryText;*/
   BtLabel * BannerIcon;
   BtLabel * BannerText;
   BtLabel * SecondaryText;
   BtButton * sxButton;
   BtButton * dxButton;
   BtButton * csxButton;
   BtButton * cdxButton;
   QPixmap *  Icon[MAX_NUM_ICON];
   QPixmap * pressIcon[4];
//   char testo[MAX_TEXT],testoSecondario[MAX_TEXT_2];
   char attivo,value,maxValue,minValue,id;
   int periodo, numFrame,contFrame,serNum;
   char address[20];
   char chi[5];
   bool group[9];
   bool pul;

};

/*!
  \enum oggettinoDelBanner
  \brief describes the possible items one can have into a banner
*/  
enum oggettinoDelBanner{
                                                 BUT1,
			 BUT2,
			 TEXT,
			 ICON,
			 BUT3,
			 BUT4,
			 TEXT2,
		     };

#endif //BANNER
