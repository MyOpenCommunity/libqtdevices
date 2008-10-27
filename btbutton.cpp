#include "btbutton.h"
#include "genericfunz.h"

/*
#include "main.h"
#include "genericfunz.h"

#include <qdialog.h>
#include <qfontmetrics.h>
#include <qpainter.h>
#include <qdrawutil.h>
#include <qpixmap.h>
#include <qbitmap.h>
#include <qpopupmenu.h>
#include <qguardedptr.h>
#include <qapplication.h>
#include <qtoolbar.h>
#include <qstyle.h>
#if defined(QT_ACCESSIBILITY_SUPPORT)
#include <qaccessible.h>
#endif
#include <unistd.h>
*/


BtButton::BtButton(QWidget *parent) : QPushButton(parent), icon_set(false)
{
	setStyleSheet(QString("border:0px;"));
}

BtButton::BtButton(const QString &text, QWidget *parent) : QPushButton(text, parent), icon_set(false)
{
	setStyleSheet(QString("border:0px;"));
}

void BtButton::setPressedPixmap(const QPixmap &p)
{
	pressed_pixmap = p;
}

void BtButton::setPixmap(const QPixmap &p)
{
	pixmap = p;
}

void BtButton::mousePressEvent(QMouseEvent *event)
{
	if (!pressed_pixmap.isNull())
	{
		setIcon(pressed_pixmap);
		setIconSize(pressed_pixmap.size());
		icon_set = true;
	}
	QPushButton::mousePressEvent(event);
}

void BtButton::mouseReleaseEvent(QMouseEvent *event)
{
	setIcon(pixmap);
	setIconSize(pixmap.size());
	icon_set = true;
	QPushButton::mouseReleaseEvent(event);
}

void BtButton::paintEvent(QPaintEvent *event)
{
	if (!icon_set)
	{
		setIcon(pixmap);
		setIconSize(pixmap.size());
		icon_set = true;
	}

#ifdef BEEP
	if (isDown())
		beep();
#endif

	QPushButton::paintEvent(event);
}




#if 0
static const int autoRepeatDelay  = 2300;
static const int autoRepeatPeriod = 100;


class BtButtonPrivate
{
public:
	BtButtonPrivate()
		:iconset( 0 )
	{}
	~BtButtonPrivate()
	{}
	QIconSet* iconset;
};


/*!
 *  Constructs a push button with no text.
 *
 *  The \a parent and \a name arguments are sent on to the QWidget
 *  constructor.
 */

BtButton::BtButton( QWidget *parent, const char *name ) : QButton( parent, name )
{
	init();
}

/*!
 * Constructs a push button called \a name with the parent \a parent
 * and the text \a text.
 */

BtButton::BtButton( const QString &text, QWidget *parent, const char *name ) : QButton( parent, name )
{
	init();
	setText( text );
}


/*!
 * Constructs a push button with an \a icon and a \a text.
 *
 * Note that you can also pass a QPixmap object as an icon (thanks to
 * the implicit type conversion provided by C++).
 *
 * The \a parent and \a name arguments are sent to the QWidget
 * constructor.
 */
/*#ifndef QT_NO_ICONSET
  BtButton::BtButton( const QIconSet& icon, const QString &text,
  QWidget *parent, const char *name )
  : QButton( parent, name )
  {
  init();
  setText( text );
  setIconSet( icon );
  }
#endif
*/

/*!
 * Destroys the push button.
 */
BtButton::~BtButton()
{
	delete d;
	if (prespixmap)
		delete (prespixmap);
	if (rilpixmap)
		delete(rilpixmap);
}

void BtButton::init()
{
	d = 0;
	defButton = FALSE;
	lastEnabled = FALSE;
	hasMenuArrow = FALSE;
	flt = TRUE;//FALSE;
#ifndef QT_NO_DIALOG
	//    autoDefButton = ::qt_cast<QDialog*>(topLevelWidget()) != 0;   ***BONF
#else
	autoDefButton = FALSE;
#endif
	setBackgroundMode( PaletteButton );
	setSizePolicy( QSizePolicy( QSizePolicy::Fixed/*QSizePolicy::Minimum*/, QSizePolicy::Fixed ) );
	prespixmap=NULL;
	rilpixmap=NULL;
}


/*
   Makes the push button a toggle button if \a enable is TRUE or a normal
   push button if \a enable is FALSE.

   Toggle buttons have an on/off state similar to \link QCheckBox check
   boxes. \endlink A push button is initially not a toggle button.

   \sa setOn(), toggle(), isToggleButton() toggled()
   */

void BtButton::setToggleButton( bool enable )
{
	QButton::setToggleButton( enable );
}



void BtButton::setOn( bool enable )
{
	if ( !isToggleButton() )
		return;
	QButton::setOn( enable );
}

void BtButton::setAutoDefault( bool enable )
{
	if ( (bool)autoDefButton == enable )
		return;
	autoDefButton = enable;
	update();
	updateGeometry();
}


void BtButton::setDefault( bool enable )
{
	if ( (bool)defButton == enable )
		return;  // no change
	defButton = enable;

	update();
#if defined(QT_ACCESSIBILITY_SUPPORT)
	QAccessible::updateAccessibility( this, 0, QAccessible::StateChanged );
#endif
}


/*!
 * \reimp
 */
QSize BtButton::sizeHint() const
{
	constPolish();

	int w = 0, h = 0;

	// calculate contents size...
	/*#ifndef QT_NO_ICONSET
	  if ( iconSet() && !iconSet()->isNull() ) {
	  int iw = iconSet()->pixmap( QIconSet::Small, QIconSet::Normal ).width() + 4;
	  int ih = iconSet()->pixmap( QIconSet::Small, QIconSet::Normal ).height();
	  w += iw;
	  h = QMAX( h, ih );
	  }
#endif*/
	/*   if ( isMenuButton() )
	     w += style().pixelMetric(QStyle::PM_MenuButtonIndicator, this);
	     */
	if ( pixmap() )
	{
		QPixmap *pm = (QPixmap *)pixmap();
		w += pm->width();
		h += pm->height();
	} 
	else 
	{
		/*	
		QString s( text() );
		bool empty = s.isEmpty();
		if ( empty )
		s = QString::fromLatin1("XXXX");
		QFontMetrics fm = fontMetrics();
		QSize sz = fm.size( ShowPrefix, s );
		if(!empty || !w)
		w += sz.width();
		if(!empty || !h)
		h = QMAX(h, sz.height());
		*/
	}

	return (style().sizeFromContents(QStyle::CT_CustomBase/*CT_PushButton*/, this, QSize(w, h)).
			expandedTo(QApplication::globalStrut()));
}


/*!
 * \reimp
 */
void BtButton::move( int x, int y )
{
	//   QWidget::move( x, y );
}

/*!
 * \reimp
 */
void BtButton::move( const QPoint &p )
{
	//  move( p.x(), p.y() );
}

/*!
 * \reimp
 */
void BtButton::resize( int w, int h )
{
	QWidget::resize( w, h );
}

/*!
 * \reimp
 */
void BtButton::resize( const QSize &s )
{
	resize( s.width(), s.height() );
}

/*!
 * \reimp
 */
void BtButton::setGeometry( int x, int y, int w, int h )
{
	QWidget::setGeometry( x, y, w, h );
}

/*!
 * \reimp
 */
void BtButton::setGeometry( const QRect &r )
{
	QWidget::setGeometry( r );
}

/*!
 * \reimp
 */
void BtButton::resizeEvent( QResizeEvent * )
{
	if ( autoMask() )
		updateMask();
}

/*!
 * \reimp
 */
void BtButton::drawButton( QPainter *paint )
{
	// int diw = 0;

	// QStyle::SFlags flags = QStyle::Style_Enabled;//QStyle::Style_Default;
	//  if (isEnabled())
	//    flags |= QStyle::Style_Enabled;

	if  ( ( (isDown()) && (prespixmap) ) || ( (isOn()) && (prespixmap) ) )
	{

		QButton::setPixmap(*prespixmap);
		paint->drawPixmap( 0,0, *prespixmap );
	}
	else if (rilpixmap)
	{
		QButton::setPixmap(*rilpixmap); 
		paint->drawPixmap( 0,0, *rilpixmap );
	}

	//  style().drawControl(QStyle::CE_PushButton, paint, this, rect(), colorGroup(), flags);

#if 0   
	//drawButtonLabel( paint );
#else
	drawButtonLabel( paint );
#endif





#ifdef  BEEP
	if (isDown())
		//	qApp->beep();
		beep();    
#endif

	lastEnabled = isEnabled();
}


/*!
 * \reimp
 */
void BtButton::drawButtonLabel( QPainter *paint )
{

	QStyle::SFlags flags =QStyle::Style_Enabled;// QStyle::Style_Default;
	//    if (isEnabled())
	//    flags |= QStyle::Style_Enabled;
	//    flags && ~QStyle::Style_Down;

	// update();


	style().drawControl(QStyle::CE_PushButtonLabel, paint, this, style().subRect(QStyle::SR_CustomBase,this),
			colorGroup(),flags);
	/*   style().drawControl(QStyle::CE_PushButtonLabel, paint, this,
	     style().subRect(QStyle::SR_PushButtonContents, this),
	     colorGroup(), flags);*/

}


/*!
 * \reimp
 */
void BtButton::updateMask()
{
	/* QBitmap bm( size() );
	   bm.fill( color0 );

	   {
	   QPainter p( &bm, this );
	   style().drawControlMask(QStyle::CE_PushButton, &p, this, rect());
	   }

	   setMask( bm );*/
}

/*!
 * \reimp
 */
void BtButton::focusInEvent( QFocusEvent *e )
{
	/* if (autoDefButton && !defButton) {
	   defButton = TRUE;
	   }
	   QButton::focusInEvent( e );*/
}

/*!
 * \reimp
 */
void BtButton::focusOutEvent( QFocusEvent *e )
{

	//   QButton::focusOutEvent( e );
	/*#ifndef QT_NO_POPUPMENU
	  if ( popup() && popup()->isVisible() )	// restore pressed status
	  setDown( TRUE );
#endif*/
}




void BtButton::setPressedPixmap( const QPixmap &Icon)
{
	if(prespixmap)
		delete prespixmap;
	prespixmap = new QPixmap( Icon );
}

void BtButton::setPixmap( const QPixmap &pixmap )
{
	if(rilpixmap)
		delete rilpixmap;
	rilpixmap = new QPixmap(pixmap);
	QButton::setPixmap( pixmap );
}

#endif
//#endif


