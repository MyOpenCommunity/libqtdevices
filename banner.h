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


#define MAX_NUM_ICON		46
#define MAX_TEXT 		21
#define MAX_TEXT_2		11

#include "openclient.h"
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
/*!
  \brief Inserts an object in the banner.
  
  The arguments describe the object to insert (as described in oggettinoDelBanner), the position (x,y) and the dimension (w,h)
*/                
    void 	addItem(char ,int ,int ,int , int );
/*!
  \brief Draws all the objects composing the banner.
  
  This method automatically detects what compose the banner, which is its state and draws it in the right manner.
*/      
    virtual void  Draw();
/*!
  \brief Changes the banner state.
  
  If the argument is zero the banner assume that the state of what is controlled is "disabled" otherwise is "active".
*/      
    void 	impostaAttivo(char);
/*!
  \brief  Changes the address of what is controlled by the banner.
  
  The argument  describe the Open What of the object controlled by the banner.
*/      
    void 	setAddress(char* );
/*!
  \brief Retrieves the address of what is controlled by the banner.
  
  The returned value is the Open What of the object controlled by the banner.
*/      
    char*	 getAddress( );
/*!
  \brief Changes the function of what is controlled by the banner.
  
  he argument  describe the Open Who of the object controlled by the banner.
*/          
    void 	setChi(char* );
/*!
  \brief Retrieves the function of what is controlled by the banner.
  
  The returned value is the Open Who of the object controlled by the banner.
*/   
    char*	 getChi( );
/*!
  \brief Changes the group list of the object controlled by the banner.
  
  The group list is an array of 9 bool varibles. The element in position \a n tells if the object controlled is part of the group \a n+1
*/          
    void 	setGroup(bool* );
/*!
  \brief Retrieves the group list of the object controlled by the banner.
  
  The group list is an array of 9 bool varibles. The element in position \a n tells if the object controlled is part of the group \a n+1
*/              
    bool*	 getGroup( );
/*!
  \brief Force the object controlled being PUL.
  
  When a ligthing/automation object is considered PUL it changes its state only with \a point \a to \a point commands.
*/       
    void 	setPul();
/*!
  \brief Retrieves if the object controlled is PUL or not.
  
  When a ligthing/automation object is considered PUL it changes its state only with \a point \a to \a point commands.
*/      
    bool 	getPul();

/*!
  \brief Sets the serial number of the banner.
  
  The \a serial \a number is the progressive number among the total amount of similar banners present in the same subtree. It is quite usefull to discriminate, for instance, between different \a wide \a awake in the setting subtree
*/      
    void 	setSerNum(int);
/*!
  \brief Retrieves the serial number of the banner.
  
  The \a serial \a number is the progressive number among the total amount of similar banners present in the same subtree. It is quite usefull to discriminate, for instance, between different \a wide \a awake in the setting subtree
*/           
    int 	getSerNum();
 /*!
  \brief Retrieves the Id of the object controlled by the banner.
*/              
    char 	getId();
 /*!
  \brief Sets the Id of the object controlled by the banner.
*/                 
    void 	setId(char);    
 /*!
  \brief Sets the background color for the banner.
  
  The arguments are RGB components for the color.
*/      
    virtual void 	setBGColor(int, int , int );
 /*!
  \brief Sets the foreground color for the banner.
  
  The arguments are RGB components for the color.
*/          
    virtual void 	setFGColor(int , int , int );		
 /*!
  \brief Sets the background color for the banner.
  
  The argument is the QColor description of the color.
*/          
    virtual void 	setBGColor(QColor );
 /*!
  \brief Sets the foreground color for the banner.
  
  The argument is the QColor description of the color.
*/          
    virtual void 	setFGColor(QColor );	
 /*!
  \brief Sets the Value for the object controlled by the banner.
  
  There's need of using this function when controlling a levelled device such as a dimmer or an amplifier. The function in used to set the value of such an object.
*/         
    void 	setValue(char);
/*!
  \brief Increments the Value for the object controlled by the banner.
  
  There's need of using this function when controlling a levelled device such as a dimmer or an amplifier. The function in used to increment the value of such an object.
*/         
    void 	aumValue();
/*!
  \brief Derements the Value for the object controlled by the banner.
  
  There's need of using this function when controlling a levelled device such as a dimmer or an amplifier. The function in used to decrement the value of such an object.
*/             
    void 	decValue();
/*!
  \brief Retrieves the Value for the object controlled by the banner.
  
  There's need of using this function when controlling a levelled device such as a dimmer or an amplifier. The function in used to retrieve the value of such an object.
*/             
    char 	getValue();
/*!
  \brief Sets the maximum Value for the object controlled by the banner.
  
  There's need of using this function when controlling a levelled device such as a dimmer or an amplifier. The function in used to determine the maximum value for such an object.
*/         
    void 	setMaxValue(char);
/*!
  \brief Sets the minimum Value for the object controlled by the banner.
  
  There's need of using this function when controlling a levelled device such as a dimmer or an amplifier. The function in used to determine the minimum value for such an object.
*/             
    void 	setMinValue(char);
/*!
  \brief Sets the value range for the object controlled by the banner.
  
  There's need of using this function when controlling a levelled device such as a dimmer or an amplifier. The function in used to determine the minimum and maximum (in this order) value for such an object.
*/                 
    void 	setRange(char,char);
/*!
  \bried Sets the step for the object controlled by the banner.
  
  The object's value is decremented/incremented by this quantity
*/
    void        setStep(char);
/*!
  \brief Retrieves if the object controlled by the banner is \a active or not.
*/                 
    unsigned char isActive();
/*!
  \brief Sets the parameters necessary for the animation of the image describing the image controlled by the banner.
  
  The arguments of the method are: the period in ms and the number frame describing the animation.
*/                 
    void 	setAnimationParams(int,int);
    void        getAnimationParams(int&, int&);
    void 	setNumRighe(uchar);
    QTimer *animationTimer;
    uchar 	numRighe;
    unsigned char stato;
    virtual void inizializza(bool forza=false);	
    //provvisoriamente per debug li metto public
    char testo[MAX_PATH*2],testoSecondario[MAX_TEXT_2];
/*!
  \brief Force an object of the banner to be hided.
  
  The object to be hided is described by the \a oggettinoDelBanner description.
*/                 
    void	nascondi(char);
/*!
  \brief Force an object of the banner to be shown.
  
  The object to be shown is described by the \a oggettinoDelBanner description.
*/        
    void	mostra(char);
/*!
  \brief Retrieves the state of the object controlled by the banner.
*/        
    unsigned char getState();
/*!
  \brief Must be reimplemented to retrieve the filereference of the manual icon for the object controlled by the banner.
*/        
    virtual 	char* 	getManIcon();
/*!
  \brief Must be reimplemented to retrieve the filereference of the automatic icon for the object controlled by the banner.
*/     
    virtual 	char* 	getAutoIcon();
    
    /*!
  \enum oggettinoDelBanner
  \brief describes the possible items one can have into a banner
  
  BUT1 - BUT2 - BUT3 - BUT4 are 4 possible button 
  TEXT is the describing text
  TEXT2 is an additional text 
  ICON - ICON2 are possibles image describing the banner or its state
*/  
enum oggettinoDelBanner{
                         BUT1,
			 BUT2,
			 TEXT,
			 ICON,
			 ICON2,
			 BUT3,
			 BUT4,
			 TEXT2,
		     };
 virtual void addAmb(char *); 
public slots:
/*!
  \brief Must be reimplemented to analyze the \a Open \a Frame incoming.
*/        
   virtual void gestFrame(char *);   
/*!
  \brief Must be reimplemented to retrieve the state ofthe object controlled by the banner.
*/     
   virtual void  rispStato(char*);
/*!
  \brief Must be reimplemented to do something when hiding the banner.
*/     
   virtual void hide();
/*!
  \brief Must be reimplemented to do something when showing the banner.
*/        
   virtual void show();
/*!
  \brief Invoked on open ack reception
*/
   virtual void openAckRx();
/*!
  \brief Invoked on open nak reception
*/
   virtual void openNakRx();
   virtual void ambChanged(char *, bool, void *);
/*!
  \brief Parent of my parent changed
*/
 virtual void grandadChanged(QWidget *newGrandad);
/*!
  \brief Parent changed
*/
 virtual void parentChanged(QWidget *newParent);
private slots:
    void 	animate();
signals:
/*!
  \brief Emitted when the right button is clicked.
*/  
  void 	dxClick(); 
/*!
  \brief Emitted when the left button is clicked.
*/    
  void 	sxClick(); 
/*!
  \brief Emitted when the center-right button is clicked.
*/    
  void 	cdxClick(); 
/*!
  \brief Emitted when the center-left button is clicked.
*/    
  void 	csxClick();
/*!
  \brief Emitted when the left button is released.
*/    
  void	sxReleased();
/*!
  \brief Emitted when the right button is released.
*/      
  void 	dxReleased();
/*!
  \brief Emitted when the left button is pressed.
*/      
  void	sxPressed();
/*!
  \brief Emitted when the right button is pressed.
*/        
  void 	dxPressed();
/*!
  \brief Emitted when the center-left button is released.
*/      
  void 	csxReleased();
/*!
  \brief Emitted when the center-right button is released.
*/    
  void 	cdxReleased();
/*!
  \brief Emitted when the center-left button is pressed.
*/        
  void 	csxPressed();
/*!
  \brief Emitted when the center-right button is pressed.
*/        
  void 	cdxPressed();
  /*!
  \brief Emitted to (de)freeze the objects of the banner
*/      
  virtual void freezed(bool);  
/*!
  \brief Emitted when the banner decide to freeze the device (i.e. when the \a alarm \a clock turns on).
*/        
  virtual void freeze(bool);
  /*!
  \brief Emitted to communicate when the \a alarm \a clock begins and ends.
*/  
  virtual void svegl(bool);
  virtual void richStato(char*);
/*!
  \brief Emitted when the banner decide send an \a Open \a frame to the system.
*/    
  virtual void sendFrame(char*);
/*!
  \brief As above, but we want to wait for the relevant open ack message
*/
  virtual void sendFramew(char*);
  virtual void sendInit(char*);
/*!
  \brief Emitted when the banner decide to die and to be removed from the list containig it.
*/    
 virtual void killMe(banner*);
 /*// FIXMEEEEEEEE !!!!
 virtual void active(int, int);*/
  
protected:
/*   QLabel * BannerIcon;
   QLabel * BannerText;
   QLabel * SecondaryText;*/
   BtLabel * BannerIcon;
   BtLabel * BannerIcon2;
   BtLabel * BannerText;
   BtLabel * SecondaryText;
   BtButton * sxButton;
   BtButton * dxButton;
   BtButton * csxButton;
   BtButton * cdxButton;
   QPixmap *  Icon[MAX_NUM_ICON];
   QPixmap * pressIcon[4];
//   char testo[MAX_TEXT],testoSecondario[MAX_TEXT_2];
   char attivo,value,maxValue,minValue,id,step;
   int periodo, numFrame,contFrame,serNum;
   char address[20];
   char chi[5];
   bool group[9];
   bool pul;
   /*
      \brief Returns true if the object is a target for message
    */
   bool isForMe(openwebnet& message);
};



#endif //BANNER
