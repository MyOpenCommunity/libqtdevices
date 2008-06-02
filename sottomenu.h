/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** sottomenu.h
 **
 **definizioni della pagine di sottoMenu
 **
 ****************************************************************/

#ifndef SOTTOMENU_H
#define SOTTOMENU_H

#undef IPHONE_MODE

#include <qwidget.h>
#include <qptrlist.h> 
#include "main.h"
#include "banner.h"

class bannFrecce;
class scenEvo_cond;

/*!
  \class sottoMenu
  \brief This class is the general list of the subtree describing the various system functions.

  Many functions are directily a \a sottoMenu type while other are new class inheriting this one. This class core is made of a dinamical list representing all the devices included in the menu.
  \author Davide
  \date lug 2005
  */  
class sottoMenu : public QWidget
{
	Q_OBJECT
	public:
		sottoMenu( QWidget *parent=0, const char *name=0, uchar withNavBar=3 ,int width=MAX_WIDTH,int  height=MAX_HEIGHT,uchar n=NUM_RIGHE-1);
		/*!
		  \brief Sets the background color for the banner.

		  The arguments are RGB components for the color.
		  */           
		void 	setBGColor(int, int , int );
		/*!
		  \brief Sets the foreground color for the banner.

		  The arguments are RGB components for the color.
		  */       
		void 	setFGColor(int , int , int );		
		/*!
		  \brief Sets the background color for the banner.

		  The argument is the QColor description of the color.
		  */          
		void 	setBGColor(QColor );
		/*!
		  \brief Sets the foreground color for the banner.

		  The argument is the QColor description of the color.
		  */          
		void 	setFGColor(QColor );	
		/*!
		  \brief Sets the background pixmap for the banner.
		  */  
		int 	setBGPixmap(char* );
		/*!
		  \brief Retrieves the number of objects in the list
		  */   
		uint	getCount();
		/*!
		  \brief Adds an item to the list. the arguments are:
tipo:		banner type as described in the bannerType (main.h)
nome:		banner name which is the description showed on the banner
indirizzo:	address pointer which is a char* if the banner controls only a device; QptrList char* if the banner control a device group
IconaSx, IconaDx, IconaAttiva, IconaDisattiv, IcoEx1, IcoEx2, IcoEx3:	Images' name to use in different positions in the banner
periodo:	repetition time of the animated descriptive image on the banner if there's one
numFrame: 	number of images composing the animated descriptive image on the banner if there's one
secondFroreground:	color for a particoular text in the banner
descr1, descr2, descr3, descr4:	additional texts describng the banner
par3:		an other possible parameter
par4:     one more possible parameter
lt : a pointer to a list of QStrings describing the list of times for new timed actuators
lc : a pointer to a list of pointers to scenEvo_cond (advanced scenarios management conditions
action : action string
light : light field (for video door phone items)
key : key field (for video door phone items)
unknown : unknown field (for video door phone items
sstart : soft start values list for dimmer 100 group
sstop : soft stop values list vor dimmer 100 group
*/     
		virtual int addItemU(char tipo, const QString & nome, void *indirizzo,
				QPtrList<QString> &icon_names,
				int periodo = 0, int numFrame = 0, QColor secondFroreground = QColor(0,0,0),
				char *descr1 = NULL, char *descr2 = NULL, char *descr3 = NULL, char *descr4 = NULL,
				int par3=0, int par4=0, 
				QPtrList<QString> *lt = NULL, QPtrList<scenEvo_cond> *lc = NULL, 
				QString action="", QString light="", QString key="", QString unk="", 
				QValueList<int> sstart = QValueList<int>(), 
				QValueList<int> sttop = QValueList<int>(), QString txt1="", QString txt2="", QString txt3="");
		/*!
		  \brief Initializes all the objects in the list calling init() after a certain time
		  */      
		virtual void 	inizializza();
		/*!
		  \brief Draws the subtree starting from the right item.

		  The drawn isn't made if the first item to draw doesn't change
		  */     
		void 	draw();
		/*!
		  \brief Draws the subtree starting from the right item.

		  The drawn is always made
		  */  
		virtual void 	forceDraw();   
		/*!
		  \brief Set the row's number to be drawn in the screen
		  */
		virtual void 	setNumRighe(uchar);
		/*!
		  \brief Retrieves the last banner of the list
		  */  
		banner* getLast();
		/*!
		  \brief Retrieves the current banner in the list
		  */   
		banner* getCurrent();
		/*!
		  \brief Retrieves the next banner in the list
		  */     
		banner* getNext();
		/*!
		  \brief Retrieves the previous banner in the list
		  */     
		banner* getPrevious();
		/*!
		  \brief Set an item in the list to be PUL that means that the device works only with addresse \a Open \a frame.

		  The arguments passed are \a Open \a who and \a Open \a where.
		  */     
		bool 	setPul(char* chi="", char* where="");
		/*!
		  \brief Set an item in the list to be part of some groups.

		  The arguments passed are \a Open \a who and \a Open \a where and a list af bool each of them telling if the object belongs to a certain group or not.
		  */       
		bool	setGroup(char* chi="", char* where="", bool group[9] =NULL);
		/*!
		  \brief Sets the index of the list forcing it to the object having the address passed by argument.
		  */     
		void 	setIndex(char*);
		void mostra_all(char);
		/*!
		  \brief Sets the index of the list forcing which is the first item to draw.
		  */    
		void 	setIndice(char);
		/*!
		  \brief Such as setNumRighe(uchar).
		  */      
		void 	setNumRig(uchar);
		/*!
		  \brief Set the height of the area occupied by the subtree.
		  */        
		void 	setHeight(int);
		/*!
		  \brief Retrieves the number of rows used by the subtree.
		  */ 
		uchar 	getNumRig();
		/*!
		  \brief Retrieves the height of th area occupied by the subtree.
		  */   
		int 	getHeight();  
		/*!
		  \brief Changes the type of navigation bar present at the bsubtree (see bannFrecce)
		  */  
		virtual void 	setNavBarMode(uchar=0, char* IconBut4=ICON_FRECCIA_DX);
		/*!
		  \brief Set the Geometry for the object.
		  */ 
		void 	setGeometry(int, int, int, int );
		/*!
		  \brief reparent implementation
		  */
		virtual void reparent ( QWidget * parent, WFlags f, const QPoint & p, bool showIt = FALSE );
		/*!
		  \brief add amb to all banners
		  */
		void addAmb(char *);
                #ifdef IPHONE_MODE
			void mouseMoveEvent( QMouseEvent *e );
		#endif
signals:
	        void goUP();
        	void goDO();
		/*!
		  \brief Emitted when the object is closed.
		  */  
		void 	Closed();
		/*!
		  \brief Emitted to connect the \a Open \a Frame coming from the openClient to the banners in the list.
		  */
		void 	gestFrame(char*);
		/*!
		  \brief Emitted to connect the \a Open \a Frame coming from to the banners in the list to the openClient.
		  */  
		void 	sendFrame(char*);
		void	sendInit(char*);
		/*!
		  \brief Like sendFrame, but also ack is requested before proceeding
		  */
		void sendFramew(char*);
		/*!
		  \brief Open ack received
		  */
		void openAckRx(void);
		/*!
		  \brief Open nak received
		  */
		void openNakRx(void);

		void 	richStato(char*);
		void 	rispStato(char*);
		/*!
		  \brief Emitted to signal to BtMain that a banner requires to freeze the device.
		  */    
		void	freeze(bool);
		/*!
		  \brief Emitted to tell to banners in the list that the device was frozen.
		  */  
		void 	frez(bool);
		/*!
		  \brief Emitted to tell BtMain the state af the alarmset.
		  */    
		virtual void svegl(bool);
		/*!
		  \brief Emitted to go to a further page.
		  */  
		void	goDx();  
		/*!
		  \brief Emitted to tell that an item in the list was removed.
		  */  
		void	itemKilled();
		/*!
		  \brief Emitted to tell to BtMain tha state of the password (value and abilitation).
		  */   
		void 	setPwd(bool,char*);
		/*!
		  \brief Parent changed
		  */
		void parentChanged(QWidget *newParent);
		/*!
		  \brief amb description changed (for diffmulti)
		  */
		void ambChanged(char *newdescr, bool multiwhere, char *where);
		/*!
		  \brief hide all children
		  */
		void hideChildren();
		/*!
		  \brief emitted on calibration start
		  */
		void startCalib();
		/*!
		  \brief emitted on calibration end
		  */
		void endCalib();
		public slots:
			/*!
			  \brief Slides the list upward.
			  */      
			virtual void 	goUp();	
		/*!
		  \brief Slides the list downward.
		  */ 
		virtual void 	goDown();
		/*!
		  \brief  See inizializza().
		  */   
		void	init();    
		/*!
		  \brief  See inizializza().
		  */   
		void	init_dimmer();    
		/*!
		  \brief  Sets all the banners disabled.
		  */     
		void 	freezed(bool);
		/*!
		  \brief  Reimplements QWidget::Hide() and hides the page.
		  */      
		virtual void 	hide(bool index = true);
		/*!
		  \brief  Reimplements QWidget::mouseReleaseEvent( QMouseEvent * ), used to wake up from frozen state.
		  */       
		void 	mouseReleaseEvent ( QMouseEvent * );  
		/*!
		  \brief  Removes a banner from the list.
		  */    
		void 	killBanner(banner*);
		/*!
		  \brief Adds a banner
		  */
		void addItem(banner *);
		/*!
		  \brief  Empties the banner list.
		  */    
		void 	svuota();
		void 	show();
	protected:
		QPtrList<banner> elencoBanner;
		QTimer* iniTim;
		int	indice, indicold;
		unsigned int height,width;  
		uchar numRighe, hasNavBar;	
		bannFrecce * bannNavigazione;
		bool 	freez;
		char	iconName[MAX_PATH];
	private:
		void setModeIcon(char*);
};


#endif // SOTTOMENU_H
