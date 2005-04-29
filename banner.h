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


#define MAX_NUM_ICON		40
#define MAX_TEXT 		21
#define MAX_TEXT_2		11


#include "btbutton.h"
#include "genericfunz.h"
#include <qwidget.h>
#include "btlabel.h"
#include <qpixmap.h>
#include <qtimer.h>


class banner : public QWidget
{
    Q_OBJECT
public:
    banner( QWidget *, const char *);
    virtual void SetText(const char *);
    void 	SetSeconaryText( const char * );
    void 	SetIcons( uchar , const char * );
    void 	SetIcons(const char *,char);
    void 	SetIcons(const char *, const char *);
    void 	SetIcons(const char *, const char *,const char *);
    void 	SetIcons(const char *, const char *,const char *,const char *);
    void 	SetIcons(const char *, const char *,const char *,const char *,int,int);
    void 	SetIcons(const char *, const char *,const char *,const char *,char);
    void 	SetIcons( const char * , const char *,const char*,const char*,const char*);
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
    bool 	isActive();
    void 	setAnimationParams(int,int);
    void 	setNumRighe(uchar);
    QTimer *animationTimer;
    uchar 	numRighe;
    virtual void inizializza();	
    //provvisoriamente per debug li metto public
    char testo[MAX_TEXT],testoSecondario[MAX_TEXT_2];
    void	nascondi(char);
    void	mostra(char);
	
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
