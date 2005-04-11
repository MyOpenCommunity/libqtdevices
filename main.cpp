/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** main.cpp
**
**Gestione apertura applicazione
**
****************************************************************/

#include	"btmain.h"
#include <qapplication.h>


int main( int argc, char **argv )
{
    QApplication a( argc, argv );
        
    QTextOStream (stdout)<<"\nQTouch Vivo\n";   
    
    BtMain mainprogr(NULL,"MAIN PROGRAM",&a);

    return a.exec();
}






