/****************************************************************
***
* BTicino Touch scren Colori art. H4686
**
** homePage.cpp
**
** Finestra principale
**
****************************************************************/

#include "homepage.h"
#include "timescript.h"
#include "genericfunz.h"
#include "openclient.h"
#include "btlabel.h"
#include "fontmanager.h"

#include <qpixmap.h>
#include <qdatetime.h>
#include <qcursor.h>
#include <qlcdnumber.h>
#include <stdlib.h>

homePage::homePage( QWidget *parent, const char *name, WFlags f )
        : QWidget( parent, name )
{
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
    setCursor (QCursor (blankCursor));
//    showFullScreen();
#endif
    setGeometry(0,0,MAX_WIDTH,MAX_HEIGHT);
    setFixedSize(QSize(MAX_WIDTH, MAX_HEIGHT));
    xClock=xTemp=MAX_WIDTH+2;
    yClock=yTemp=MAX_HEIGHT+2;
    freez=FALSE;
    descrizione=NULL;
    tempCont=0;
 }


/*void homePage::showPip()
{
    this->show();
}
*/

void homePage::addButton(int x, int y, char* iconName, char function, char* chix, char* cosax, char* dovex, char tipo)
{
    BtButton *b1;
    QPixmap Icon;

    char nomeFile[MAX_PATH];
    
    
    elencoButtons.append(new BtButton (this,"BelBottone"));
    b1 = elencoButtons.getLast();
    
    if (function==SPECIAL)
	b1->setGeometry(x, y, DIM_SPECBUT_HOME, DIM_BUT_HOME_SMALL );
    else if(function==BACK)
	b1->setGeometry(x, y, DIM_BUT_HOME_SMALL, DIM_BUT_HOME_SMALL );
    else
	b1->setGeometry(x, y, DIM_BUT_HOME, DIM_BUT_HOME );
    
    if (Icon.load(iconName))
    	 b1->setPixmap(Icon);
    b1->setPaletteBackgroundColor(backgroundColor());
    
    
    getPressName((char*)iconName, &nomeFile[0],sizeof(nomeFile));
   
    if (Icon.load(nomeFile))
    	 b1->setPressedPixmap(Icon);     

    switch (function){
//	case USCITA:   connect(b1,SIGNAL(clicked()), qApp, SLOT(quit()) );       break;
	case AUTOMAZIONE:   connect(b1,SIGNAL(clicked()),this, SIGNAL(Automazione() )); break;
	case ILLUMINAZIONE:   connect(b1,SIGNAL(clicked()),this, SIGNAL(Illuminazione() )); break;
	case ANTIINTRUSIONE:   connect(b1,SIGNAL(clicked()),this, SIGNAL(Antiintrusione() )); break;
	case CARICHI:   connect(b1,SIGNAL(clicked()),this, SIGNAL(Carichi() )); break;
	case TERMOREGOLAZIONE:   connect(b1,SIGNAL(clicked()),this, SIGNAL(Termoregolazione()) ); break;		  
	case DIFSON:   connect(b1,SIGNAL(clicked()),this, SIGNAL(Difson() )); break;
    case DIFSON_MULTI:
	connect(b1,SIGNAL(clicked()),this, SIGNAL(Difmulti() )); break;
	case SCENARI:   connect(b1,SIGNAL(clicked()),this, SIGNAL(Scenari() )); break;
	case IMPOSTAZIONI:   connect(b1,SIGNAL(clicked()),this, SIGNAL(Settings() )); break;
	case SPECIAL:
			     tipoSpecial=tipo;
                                               qDebug("tipoSpecial= %d",tipoSpecial);
			     strcpy(&chi[0],chix);
			     strcpy(&cosa[0],cosax);
			     strcpy(&dove[0],dovex);
			     if (tipoSpecial==PULSANTE)
			     {
				 qDebug("tipoSpecial= PULSANTE");
				 connect(b1, SIGNAL(pressed()), this, SLOT(specFunzPress()));
				 connect(b1, SIGNAL(released()), this, SLOT(specFunzRelease()));
			     }
			     else
				 connect(b1, SIGNAL(clicked()), this, SLOT(specFunz()));
			     break;
    case SCENARI_EVOLUTI: connect(b1, SIGNAL(clicked()), this, SIGNAL(ScenariEvoluti())); break;
    case VIDEOCITOFONIA: connect(b1,SIGNAL(clicked()),this, SIGNAL(Videocitofonia() )); break;
    case SUPERVISIONE: connect(b1, SIGNAL(clicked()), this, SIGNAL(Supervisione() )); break;
	case BACK: connect(b1, SIGNAL(clicked()), this, SIGNAL(Close()));break;			
    }
}

void homePage::setBGColor(int r, int g, int b)
{	
    setPaletteBackgroundColor( QColor :: QColor(r,g,b));    
}
void homePage::setFGColor(int r, int g, int b)
{
    setPaletteForegroundColor( QColor :: QColor(r,g,b));
}

int homePage::setBGPixmap(char* backImage)
{
    QPixmap Back; 
     if(Back.load(backImage))
    {
	 setPaletteBackgroundPixmap(Back);      
	 return (0);
    }    
     return (1);
}

void homePage::addClock(int x, int y,int width,int height,QColor bg, QColor fg, int style, int line)
{
     dataOra = new timeScript(this,"scrittaHomePage",1);
     dataOra->setGeometry(x,y,width,height);
     dataOra->setPaletteForegroundColor(fg);
     dataOra->setPaletteBackgroundColor(bg);

     dataOra->setFrameStyle( style );
     dataOra->setLineWidth(line);    
}    
void homePage::addClock(int x, int y)
{  
    addClock(x, y, 180, 35, foregroundColor(), backgroundColor(),QFrame::NoFrame, 0);
}	


void homePage::addDate(int x, int y,int width,int height,QColor bg, QColor fg, int style, int line)
{
     dataOra = new timeScript(this,"scrittaHomePage",uchar (25));
     dataOra->setGeometry(x,y,width,height);
     dataOra->setPaletteForegroundColor(fg);
     dataOra->setPaletteBackgroundColor(bg);

     dataOra->setFrameStyle( style );
     dataOra->setLineWidth(line);    
}    
void homePage::addDate(int x, int y)
{  
    addClock(x, y, 180, 35, foregroundColor(), backgroundColor(),QFrame::NoFrame, 0);
}	

void homePage::addTemp(
	char *z,
	int x,
	int y,
	int width,
	int height,
	QColor bg, 
	QColor fg, 
	int style, 
	int line, 
	const QString & qtext,
	char *Ext)
{
	switch(tempCont)
	{
		case 0: strcpy(zonaTermo1,z);zt[0]=&zonaTermo1[0];strcpy(ext1,Ext);ext[0]=ext1;break;
		case 1: strcpy(zonaTermo2,z);zt[1]=&zonaTermo2[0];strcpy(ext2,Ext);ext[1]=ext2;break;
		case 2: strcpy(zonaTermo3,z);zt[2]=&zonaTermo3[0];strcpy(ext3,Ext);ext[2]=ext3;break;
	}

     temperatura[tempCont] = new QLCDNumber(this,"0.00"TEMP_DEGREES"C");
     temperatura[tempCont] ->setGeometry(x,y,width,height-H_SCR_TEMP);
     temperatura[tempCont] ->setPaletteForegroundColor(fg);
     temperatura[tempCont] ->setPaletteBackgroundColor(bg);

     temperatura[tempCont] ->setFrameStyle( style );
     temperatura[tempCont] ->setLineWidth(line);    
     temperatura[tempCont] ->setNumDigits(6);
     temperatura[tempCont] -> display("0.00"TEMP_DEGREES"C");
     temperatura[tempCont] -> setSegmentStyle(QLCDNumber::Flat);    
     
     if ( ! qtext.isEmpty() )
     {
	QFont aFont;
	FontManager::instance()->getFont( font_homepage_bottoni_label, aFont );
	descrTemp[tempCont] = new BtLabel( this, qtext.ascii() );
	descrTemp[tempCont] ->setFont( aFont );
	descrTemp[tempCont] ->setAlignment(AlignHCenter|AlignVCenter);
	descrTemp[tempCont] ->setText( qtext );
	descrTemp[tempCont] ->setGeometry(x,y+height-H_SCR_TEMP,width,H_SCR_TEMP);
	descrTemp[tempCont] ->setPaletteForegroundColor(fg);
	descrTemp[tempCont] ->setPaletteBackgroundColor(bg);
     }
     tempCont++;
 }    

void homePage::inizializza()
{
   char Frame[50];

   for(unsigned idx=0; idx<tempCont; idx++)
   {
     strcpy(&Frame[0],"*#4*");
     if(!strcmp(ext[idx], "0"))
     {
       strcat(&Frame[0],zt[idx]);
       strcat(&Frame[0],"*0##");
     }
     else
     {
       strcat(&Frame[0],zt[idx]);
       strcat(&Frame[0],"00*15#");
       strcat(&Frame[0],zt[idx]);
       strcat(&Frame[0],"##");
     }
     emit(sendInit(&Frame[0]));
   }
}

void homePage::addTemp(char *z, int x, int y)
{
     addTemp(z,x,y,180, 35, backgroundColor(), foregroundColor(), QFrame::NoFrame, 0,NULL);
}    


void homePage::addDescrU(
	const QString & qz, 
	int x, 
	int y,
	int width,
	int height,
	QColor bg, 
	QColor fg, 
	int style, 
	int line)
{
	QFont aFont;
	FontManager::instance()->getFont( font_homepage_bottoni_descrizione, aFont );
	descrizione = new BtLabel( this, qz.ascii() );
	descrizione->setFont( aFont );
	descrizione->setAlignment(AlignHCenter|AlignVCenter);
	descrizione->setText( qz );
	descrizione->setGeometry(x,y,width,height);
	descrizione->setPaletteForegroundColor(fg);
	descrizione->setPaletteBackgroundColor(bg);
	descrizione->setFrameStyle( style );
	descrizione->setLineWidth(line);
}

void homePage::addDescrU(const QString & qz, int x, int y)
{
     addDescrU(qz,x,y,160, 20, backgroundColor(), foregroundColor(), QFrame::NoFrame, 0);
}    



void homePage::mouseReleaseEvent ( QMouseEvent * e )	
{
     qDebug("Released");   
     if (freez)
     {
	 freez=FALSE;
	 emit(freeze(freez));    
     }
     QWidget::mouseReleaseEvent ( e );
}

void homePage::freezed(bool f)
{
    freez=f;
//        	qDebug("%s freezed %d",name(),freez);			
    if (freez)
    {
	for(uchar idx=0;idx<elencoButtons.count();idx++)
	    elencoButtons.at(idx)->setEnabled(FALSE);
	if (descrizione)
	    descrizione->setEnabled(FALSE);
    }
    else
    {
	for(uchar idx=0;idx<elencoButtons.count();idx++)
	    elencoButtons.at(idx)->setEnabled(TRUE);
	if (descrizione)
	    descrizione->setEnabled(TRUE);
    }
 
}

void homePage::updateTemperatureDisplay(float new_bt_temperature, unsigned which_display)
{
	char tmp[10], temp[10];
	qDebug("vedo temperatura per Temp in Homepage: %d", (int)new_bt_temperature);
	memset(temp,'\000',sizeof(temp));
	if (new_bt_temperature>=1000)
	{
		strcat(temp,"-");
		new_bt_temperature=new_bt_temperature-1000;
	}
	new_bt_temperature/=10;
	sprintf(tmp,"%.1f",new_bt_temperature);
	strcat(temp,tmp);
	strcat(temp,TEMP_DEGREES"C");
	temperatura[which_display]->display(&temp[0]);
}

void homePage::gestFrame(char* frame)
 {    
    openwebnet msg_open;
//    char aggiorna;
    char dovex[30];
    float icx;
    bool my_frame;


    
    msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);
//qDebug("vedo frame in Homepage: ");
    
    if (!strcmp(msg_open.Extract_chi(),"4"))
    {
	qDebug("vedo temperatura per Temp: %s",msg_open.frame_open);
	strcpy(&dovex[0], msg_open.Extract_dove());
	if (dovex[0]=='#')
	    strcpy(&dovex[0], &dovex[1]);
	
	for(unsigned idx=0; idx<tempCont; idx++)
	{
        my_frame = false;
	qDebug("dove frame %s dove oggetto %s",dovex, zt[idx]);
	if(!strcmp(ext[idx], "0") && !strcmp(dovex,zt[idx]) && !strcmp(msg_open.Extract_grandezza(),"0")) 
	{
          //Temperatura misurata
	  icx=atoi(msg_open.Extract_valori(0));
          my_frame = true;
	 }
         else if(!strcmp(ext[idx], "1") && !strcmp(msg_open.Extract_grandezza(),"15") && !strncmp(dovex,zt[idx], 1))
         {
           icx=atoi(msg_open.Extract_valori(1));
           my_frame = true;
         }
         if(my_frame)
         {
	   updateTemperatureDisplay(icx, idx);
         }
    }
    }
     if ( (!strcmp(msg_open.Extract_chi(),&chi[0])) && (tipoSpecial==CICLICO) )
    {
	if ( (! strcmp(&dove[0],msg_open.Extract_dove())) )
	{
	     strcpy(&cosa[0], msg_open.Extract_cosa());
	 }
    }
 }

void homePage::specFunz()
{    
      char specialFrame[50];
      
      if (tipoSpecial==CICLICO)
      {
	  if (strcmp(&cosa[0], "0"))
	      strcpy(&cosa[0],"0");
	  else
	      strcpy(&cosa[0],"1");
      }
	    
      strcpy(&specialFrame[0],"*");
      strcat(&specialFrame[0],&chi[0]);
      strcat(&specialFrame[0],"*");      
      strcat(&specialFrame[0],&cosa[0]);
      strcat(&specialFrame[0],"*");      
      strcat(&specialFrame[0],&dove[0]);      
      strcat(&specialFrame[0],"##");                  

      emit(sendFrame(&specialFrame[0]));      
}

void homePage::specFunzPress()
{  
    	 qDebug("tipoSpecial= PRESSED");
      char specialFrame[50];
      
      strcpy(&specialFrame[0],"*");
      strcat(&specialFrame[0],&chi[0]);
      strcat(&specialFrame[0],"*1*");      
      strcat(&specialFrame[0],&dove[0]);      
      strcat(&specialFrame[0],"##");          
      emit(sendFrame(&specialFrame[0]));
  }

void homePage::specFunzRelease()
{    
    	 qDebug("tipoSpecial= RELEASED");
      char specialFrame[50];
      
      strcpy(&specialFrame[0],"*");
      strcat(&specialFrame[0],&chi[0]);
      strcat(&specialFrame[0],"*0*");      
      strcat(&specialFrame[0],&dove[0]);      
      strcat(&specialFrame[0],"##");                  
      emit(sendFrame(&specialFrame[0]));
  }
