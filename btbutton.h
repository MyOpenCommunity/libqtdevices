

#ifndef BTBUTTON_H
#define BTBUTTON_H

#ifndef QT_H
#include <qbutton.h>
#include <qiconset.h>
#endif // QT_H

#ifndef QT_NO_PUSHBUTTON
class BtButtonPrivate;
class QPopupMenu;

/*!
  \class BtButton
  \brief This is the QPushButton given by QT modified for the necessities of this application
  
  In particoular the differencies implemented from QT version of the pushbutton class are:
  no border is drawed on the contour;
  no animation is done during pression;
  a pressed image is added;
  there is no focus;
  a beep is generated when the button is pressed (it's heard only if the beep is enabled).  
  \author Davide
  \date lug 2005
*/   



class Q_EXPORT BtButton : public QButton
{
    Q_OBJECT

//    Q_PROPERTY( bool autoDefault READ autoDefault WRITE setAutoDefault )
    Q_PROPERTY( bool default READ isDefault WRITE setDefault )
  //  Q_PROPERTY( bool menuButton READ isMenuButton DESIGNABLE false )
  //  Q_PROPERTY( QIconSet iconSet READ iconSet WRITE setIconSet )
    Q_OVERRIDE( bool toggleButton WRITE setToggleButton )
    Q_OVERRIDE( bool on WRITE setOn )
   // Q_PROPERTY( bool flat READ isFlat WRITE setFlat )
//    Q_OVERRIDE( bool autoMask DESIGNABLE true SCRIPTABLE true )

public:
    BtButton( QWidget *parent, const char* name=0 );
    BtButton( const QString &text, QWidget *parent, const char* name=0 );

    ~BtButton();

    QSize	sizeHint() const;

    void	move( int x, int y );
    void	move( const QPoint &p );
    void	resize( int w, int h );
    void	resize( const QSize & );
    void	setGeometry( int x, int y, int w, int h );

    void	setGeometry( const QRect & );
    virtual void setPressedPixmap( const QPixmap & );//  BONF
    void setToggleButton( bool );

    bool	autoDefault()	const	{ return autoDefButton; }
    virtual void setAutoDefault( bool autoDef );
    bool	isDefault()	const	{ return defButton; }
    virtual void setDefault( bool def );
    void setPixmap( const QPixmap & );


#ifndef QT_NO_ICONSET
    QIconSet* iconSet() const;
#endif


public slots:
    virtual void setOn( bool );

protected:
    void	drawButton( QPainter * );
    void	drawButtonLabel( QPainter * );
    void	focusInEvent( QFocusEvent * );
    void	focusOutEvent( QFocusEvent * );
    void	resizeEvent( QResizeEvent * );
    void	updateMask();
private slots:

private:
    void	init();

    uint	autoDefButton	: 1;
    uint	defButton	: 1;
    uint	flt		: 1;
    uint	reserved		: 1; // UNUSED
    uint	lastEnabled	: 1; // UNUSED
    uint	hasMenuArrow	: 1;

    BtButtonPrivate* d;
    QPixmap    *prespixmap;
    QPixmap    *rilpixmap;

    friend class QDialog;

#if defined(Q_DISABLE_COPY)
    BtButton( const BtButton & );
    BtButton &operator=( const BtButton & );
#endif
};


#endif // QT_NO_PUSHBUTTON

#endif // BtButton_H
