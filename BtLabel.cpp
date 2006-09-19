
#include "btlabel.h"

#include <qpainter.h>
#include <qdrawutil.h>
#include <qaccel.h>
#include <qmovie.h>
#include <qimage.h>
#include <qbitmap.h>
#include <qpicture.h>
#include <qapplication.h>
#include <qsimplerichtext.h>
#include <qstylesheet.h>
#include <qstyle.h>

class BtLabelPrivate
{
public:
    BtLabelPrivate()
	:img(0), pix(0), valid_hints( -1 )
    {}
    QImage* img; // for scaled contents
    QPixmap* pix; // for scaled contents
    QSize sh;
    QSize msh;
    int valid_hints; // stores the frameWidth() for the stored size hint, -1 otherwise
};



BtLabel::BtLabel( QWidget *parent, const char *name, WFlags f )
    : QFrame( parent, name, f | WMouseNoMask  )
{
    init();
}




BtLabel::BtLabel( const QString &text, QWidget *parent, const char *name,
		WFlags f )
	: QFrame( parent, name, f | WMouseNoMask  )
{
    init();
    setText( text );
}



BtLabel::BtLabel( QWidget *buddy,  const QString &text,
		QWidget *parent, const char *name, WFlags f )
    : QFrame( parent, name, f | WMouseNoMask )
{
    init();
#ifndef QT_NO_ACCEL
    setBuddy( buddy );
#endif
    setText( text );
}



BtLabel::~BtLabel()
{
    clearContents();
    delete d;
}


void BtLabel::init()
{
    lpixmap = 0;
#ifndef QT_NO_MOVIE
    lmovie = 0;
#endif
#ifndef QT_NO_ACCEL
    lbuddy = 0;
    accel = 0;
#endif
    lpixmap = 0;
#ifndef QT_NO_PICTURE
    lpicture = 0;
#endif
    align = AlignAuto | AlignVCenter | ExpandTabs;
    extraMargin = -1;
    autoresize = FALSE;
    scaledcontents = FALSE;
    textformat = Qt::AutoText;
#ifndef QT_NO_RICHTEXT
    doc = 0;
#endif

    setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );
    d = new BtLabelPrivate;
}




void BtLabel::setText( const QString &text )
{
    if ( ltext == text )
	return;
    QSize osh = sizeHint();
#ifndef QT_NO_RICHTEXT
    bool hadRichtext = doc != 0;
#endif
    clearContents();
    ltext = text;
#ifndef QT_NO_RICHTEXT
    bool useRichText = (textformat == RichText ||
      ( ( textformat == AutoText ) && QStyleSheet::mightBeRichText(ltext) ) );
#else
    bool useRichText = TRUE;
#endif
#ifndef QT_NO_ACCEL
    // ### Setting accelerators for rich text labels will not work.
    // Eg. <b>&gt;Hello</b> will return ALT+G which is clearly
    // not intended.
    if ( !useRichText ) {
	int p = QAccel::shortcutKey( ltext );
	if ( p ) {
	    if ( !accel )
		accel = new QAccel( this, "accel label accel" );
	    accel->connectItem( accel->insertItem( p ),
				this, SLOT(acceleratorSlot()) );
	}
    }
#endif
#ifndef QT_NO_RICHTEXT
    if ( useRichText ) {
	if ( !hadRichtext )
	    align |= WordBreak;
	QString t = ltext;
	if ( align & AlignRight )
	    t.prepend( "<div align=\"right\">");
	else if ( align & AlignHCenter )
	    t.prepend( "<div align=\"center\">");
	if ( (align & WordBreak) == 0  )
	    t.prepend( "<nobr>" );
	if(doc)
	  delete doc;
	doc = new QSimpleRichText( t, font() );
    }
#endif

    updateLabel( osh );
}


/*!
    Clears any label contents. Equivalent to setText( "" ).
*/

void BtLabel::clear()
{
    setText( QString::fromLatin1("") );
}


void BtLabel::setPixmap( const QPixmap &pixmap )
{
    QSize osh = sizeHint();

    if ( !lpixmap || lpixmap->serialNumber() != pixmap.serialNumber() ) {
	clearContents();
	if(lpixmap)
	  delete lpixmap;
	lpixmap = new QPixmap( pixmap );
    }

    if ( lpixmap->depth() == 1 && !lpixmap->mask() )
	lpixmap->setMask( *((QBitmap *)lpixmap) );

    updateLabel( osh );
}

#ifndef QT_NO_PICTURE


void BtLabel::setPicture( const QPicture &picture )
{
    QSize osh = sizeHint();
    clearContents();
    if(lpicture)
      delete lpicture;
    lpicture = new QPicture( picture );

    updateLabel( osh );
}
#endif // QT_NO_PICTURE



void BtLabel::setNum( int num )
{
    QString str;
    str.setNum( num );
	setText( str );
}


void BtLabel::setNum( double num )
{
    QString str;
    str.setNum( num );
	setText( str );
}



void BtLabel::setAlignment( int alignment )
{
    if ( alignment == align )
	return;
    QSize osh = sizeHint();
#ifndef QT_NO_ACCEL
    if ( lbuddy )
	align = alignment | ShowPrefix;
    else
#endif
	align = alignment;

#ifndef QT_NO_RICHTEXT
    QString t = ltext;
    if ( !t.isNull() ) {
	ltext = QString::null;
	setText( t );
    }
#endif

    updateLabel( osh );
}




void BtLabel::setIndent( int indent )
{
    extraMargin = indent;
    updateLabel( QSize( -1, -1 ) );
}




void BtLabel::setAutoResize( bool enable )
{
    if ( (bool)autoresize != enable ) {
	autoresize = enable;
	if ( autoresize )
	    adjustSize();			// calls resize which repaints
    }
}




QSize BtLabel::sizeForWidth( int w ) const
{
    QRect br;
    QPixmap *pix = pixmap();
#ifndef QT_NO_PICTURE
    QPicture *pic = picture();
#else
    const int pic = 0;
#endif
#ifndef QT_NO_MOVIE
    QMovie *mov = movie();
#else
    const int mov = 0;
#endif
    int hextra = 2 * frameWidth();
    int vextra = hextra;
    QFontMetrics fm( fontMetrics() );
    int xw = fm.width( 'x' );
    if ( !mov && !pix && !pic ) {
	int m = indent();
	if ( m < 0 && hextra ) // no indent, but we do have a frame
	    m = xw / 2 - margin();
	if ( m >= 0 ) {
	    int horizAlign = QApplication::horizontalAlignment( align );
	    if ( (horizAlign & AlignLeft) || (horizAlign & AlignRight ) )
		hextra += m;
	    if ( (align & AlignTop) || (align & AlignBottom ) )
		vextra += m;
	}
    }

    if ( pix )
	br = pix->rect();
#ifndef QT_NO_PICTURE
    else if ( pic )
	br = pic->boundingRect();
#endif
#ifndef QT_NO_MOVIE
    else if ( mov )
	br = mov->framePixmap().rect();
#endif
#ifndef QT_NO_RICHTEXT
    else if ( doc ) {
	int oldW = doc->width();
	if ( align & WordBreak ) {
	    if ( w < 0 )
		doc->adjustSize();
	    else
		doc->setWidth( w-hextra );
	}
	br = QRect( 0, 0, doc->widthUsed(), doc->height() );
	doc->setWidth( oldW );
    }
#endif
    else {
	bool tryWidth = (w < 0) && (align & WordBreak);
	if ( tryWidth )
	    w = xw * 80;
	else if ( w < 0 )
	    w = 2000;
	w -= hextra;
	br = fm.boundingRect( 0, 0, w ,2000, alignment(), text() );
	if ( tryWidth && br.height() < 4*fm.lineSpacing() && br.width() > w/2 )
		br = fm.boundingRect( 0, 0, w/2, 2000, alignment(), text() );
	if ( tryWidth && br.height() < 2*fm.lineSpacing() && br.width() > w/4 )
	    br = fm.boundingRect( 0, 0, w/4, 2000, alignment(), text() );
    }
    int wid = br.width() + hextra;
    int hei = br.height() + vextra;

    return QSize( wid, hei );
}


/*!
  \reimp
*/

int BtLabel::heightForWidth( int w ) const
{
    if (
#ifndef QT_NO_RICHTEXT
	doc ||
#endif
	(align & WordBreak) )
	return sizeForWidth( w ).height();
    return QWidget::heightForWidth( w );
}



/*!\reimp
*/
QSize BtLabel::sizeHint() const
{
    if ( d->valid_hints != frameWidth() )
	(void) BtLabel::minimumSizeHint();
    return d->sh;
}

/*!
  \reimp
*/

QSize BtLabel::minimumSizeHint() const
{
    if ( d->valid_hints == frameWidth() )
	return d->msh;

    constPolish();
    d->valid_hints = frameWidth();
 //   d->sh = sizeForWidth( -1 );
    QSize sz( -1, -1 );

    if (
#ifndef QT_NO_RICHTEXT
	 !doc &&
#endif
	 (align & WordBreak) == 0 ) {
	sz = d->sh;
    } else {
	// think about caching these for performance
	sz.rwidth() = sizeForWidth( 0 ).width();
	sz.rheight() = sizeForWidth(QWIDGETSIZE_MAX).height();
	if ( d->sh.height() < sz.height() )
	    sz.rheight() = d->sh.height();
    }
    if ( sizePolicy().horData() == QSizePolicy::Ignored )
	sz.rwidth() = -1;
    if ( sizePolicy().verData() == QSizePolicy::Ignored )
	sz.rheight() = -1;
    d->msh = sz;
    return sz;
}

/*!
  \reimp
*/


void BtLabel::resizeEvent( QResizeEvent* e )
{
    QFrame::resizeEvent( e );

#ifdef QT_NO_RICHTEXT
    static const bool doc = FALSE;
#endif

    // optimize for standard labels
    if ( frameShape() == NoFrame && (align & WordBreak) == 0 && !doc &&
	 ( e->oldSize().width() >= e->size().width() && (align & AlignLeft ) == AlignLeft )
	 && ( e->oldSize().height() >= e->size().height() && (align & AlignTop ) == AlignTop ) ) {
	setWFlags( WResizeNoErase );
	return;
    }

    clearWFlags( WResizeNoErase );
    QRect cr = contentsRect();
    if ( !lpixmap ||  !cr.isValid() ||
	 // masked pixmaps can only reduce flicker when being top/left
	 // aligned and when we do not perform scaled contents
	 ( lpixmap->hasAlpha() && ( scaledcontents || ( ( align & (AlignLeft|AlignTop) ) != (AlignLeft|AlignTop) ) ) ) )
	return;

    setWFlags( WResizeNoErase );

    if ( !scaledcontents ) {
	// don't we all love QFrame? Reduce pixmap flicker
	QRegion reg = QRect( QPoint(0, 0), e->size() );
	reg = reg.subtract( cr );
	int x = cr.x();
	int y = cr.y();
	int w = lpixmap->width();
	int h = lpixmap->height();
	if ( (align & Qt::AlignVCenter) == Qt::AlignVCenter )
	    y += cr.height()/2 - h/2;
	else if ( (align & Qt::AlignBottom) == Qt::AlignBottom)
	    y += cr.height() - h;
	if ( (align & Qt::AlignRight) == Qt::AlignRight )
	    x += cr.width() - w;
	else if ( (align & Qt::AlignHCenter) == Qt::AlignHCenter )
	    x += cr.width()/2 - w/2;
	if ( x > cr.x() )
	    reg = reg.unite( QRect( cr.x(), cr.y(), x - cr.x(), cr.height() ) );
	if ( y > cr.y() )
	    reg = reg.unite( QRect( cr.x(), cr.y(), cr.width(), y - cr.y() ) );

	if ( x + w < cr.right() )
	    reg = reg.unite( QRect( x + w, cr.y(),  cr.right() - x - w, cr.height() ) );
	if ( y + h < cr.bottom() )
	    reg = reg.unite( QRect( cr.x(), y +  h, cr.width(), cr.bottom() - y - h ) );

	erase( reg );
    }
}


/*!
    Draws the label contents using the painter \a p.
*/

void BtLabel::drawContents( QPainter *p )
{
    QRect cr = contentsRect();

    QPixmap *pix = pixmap();
#ifndef QT_NO_PICTURE
    QPicture *pic = picture();
#else
    const int pic = 0;
#endif
#ifndef QT_NO_MOVIE
    QMovie *mov = movie();
#else
    const int mov = 0;
#endif

    if ( !mov && !pix && !pic ) {
	int m = indent();
	if ( m < 0 && frameWidth() ) // no indent, but we do have a frame
	    m = fontMetrics().width('x') / 2 - margin();
	if ( m > 0 ) {
	    int hAlign = QApplication::horizontalAlignment( align );
	    if ( hAlign & AlignLeft )
		cr.setLeft( cr.left() + m );
	    if ( hAlign & AlignRight )
		cr.setRight( cr.right() - m );
	    if ( align & AlignTop )
		cr.setTop( cr.top() + m );
	    if ( align & AlignBottom )
		cr.setBottom( cr.bottom() - m );
	}
    }

#ifndef QT_NO_MOVIE
    if ( mov ) {
	// ### should add movie to qDrawItem
	QRect r = style().itemRect( p, cr, align, isEnabled(), &(mov->framePixmap()),
				    QString::null );
	// ### could resize movie frame at this point
	p->drawPixmap(r.x(), r.y(), mov->framePixmap() );
    }
    else
#endif
#ifndef QT_NO_RICHTEXT
    if ( doc ) {
	doc->setWidth(p, cr.width() );
	int rh = doc->height();
	int yo = 0;
	if ( align & AlignVCenter )
	    yo = (cr.height()-rh)/2;
	else if ( align & AlignBottom )
	    yo = cr.height()-rh;
	/*if (! isEnabled() &&
	    style().styleHint(QStyle::SH_EtchDisabledText, this)) {
	    QColorGroup cg = colorGroup();
	    cg.setColor( QColorGroup::Text, cg.light() );
	    doc->draw(p, cr.x()+1, cr.y()+yo+1, cr, cg, 0);
	}*/

	// QSimpleRichText always draws with QColorGroup::Text as with
	// background mode PaletteBase. BtLabel typically has
	// background mode PaletteBackground, so we create a temporary
	// color group with the text color adjusted.
	QColorGroup cg = colorGroup();
	if ( backgroundMode() != PaletteBase && isEnabled() )
	    cg.setColor( QColorGroup::Text, paletteForegroundColor() );

	doc->draw(p, cr.x(), cr.y()+yo, cr, cg, 0);
    } else
#endif
#ifndef QT_NO_PICTURE
    if ( pic ) {
	QRect br = pic->boundingRect();
	int rw = br.width();
	int rh = br.height();
	if ( scaledcontents ) {
	    p->save();
	    p->translate( cr.x(), cr.y() );
#ifndef QT_NO_TRANSFORMATIONS
	    p->scale( (double)cr.width()/rw, (double)cr.height()/rh );
#endif
	    p->drawPicture( -br.x(), -br.y(), *pic );
	    p->restore();
	} else {
	    int xo = 0;
	    int yo = 0;
	    if ( align & AlignVCenter )
		yo = (cr.height()-rh)/2;
	    else if ( align & AlignBottom )
		yo = cr.height()-rh;
	    if ( align & AlignRight )
		xo = cr.width()-rw;
	    else if ( align & AlignHCenter )
		xo = (cr.width()-rw)/2;
	    p->drawPicture( cr.x()+xo-br.x(), cr.y()+yo-br.y(), *pic );
	}
    } else
#endif
    {
#ifndef QT_NO_IMAGE_SMOOTHSCALE
	if ( scaledcontents && pix ) {
	    if ( !d->img )
		d->img = new QImage( lpixmap->convertToImage() );

	    if ( !d->pix )
		d->pix = new QPixmap;
	    if ( d->pix->size() != cr.size() )
		d->pix->convertFromImage( d->img->smoothScale( cr.width(), cr.height() ) );
	    pix = d->pix;
	}
#endif
	int alignment = align;
	//if ((align & ShowPrefix) && !style().styleHint(QStyle::SH_UnderlineAccelerator, this))
	 //   alignment |= NoAccel;
	
	
	// ordinary text or pixmap label
	style().drawItem( p, cr, alignment, colorGroup(),TRUE/* isEnabled()*/,
			  pix, ltext );
    }
}


/*!
    Updates the label, but not the frame.
*/

void BtLabel::updateLabel( QSize oldSizeHint )
{
    d->valid_hints = -1;
    QSizePolicy policy = sizePolicy();
    bool wordBreak = align & WordBreak;
    policy.setHeightForWidth( wordBreak );
    if ( policy != sizePolicy() )
	setSizePolicy( policy );
    if ( sizeHint() != oldSizeHint )
	updateGeometry();
    if ( autoresize ) {
	adjustSize();
	update( contentsRect() );
    } else {
	update( contentsRect() );
    }
}


/*!
  \internal

  Internal slot, used to set focus for accelerator labels.
*/
#ifndef QT_NO_ACCEL
void BtLabel::acceleratorSlot()
{
    if ( !lbuddy )
	return;
    QWidget * w = lbuddy;
    while ( w->focusProxy() )
	w = w->focusProxy();
    if ( !w->hasFocus() &&
	 w->isEnabled() &&
	 w->isVisible() &&
	 w->focusPolicy() != NoFocus ) {
	QFocusEvent::setReason( QFocusEvent::Shortcut );
	w->setFocus();
	QFocusEvent::resetReason();
    }
}
#endif

/*!
  \internal

  Internal slot, used to clean up if the buddy widget dies.
*/
#ifndef QT_NO_ACCEL
void BtLabel::buddyDied() // I can't remember if I cried.
{
    lbuddy = 0;
}



void BtLabel::setBuddy( QWidget *buddy )
{
    if ( buddy )
	setAlignment( alignment() | ShowPrefix );
    else
	setAlignment( alignment() & ~ShowPrefix );

    if ( lbuddy )
	disconnect( lbuddy, SIGNAL(destroyed()), this, SLOT(buddyDied()) );

    lbuddy = buddy;

    if ( !lbuddy )
	return;
#ifndef QT_NO_RICHTEXT
    if ( !( textformat == RichText || (textformat == AutoText &&
				       QStyleSheet::mightBeRichText(ltext) ) ) )
#endif
    {
	int p = QAccel::shortcutKey( ltext );
	if ( p ) {
	    if ( !accel )
		accel = new QAccel( this, "accel label accel" );
	    accel->connectItem( accel->insertItem( p ),
				this, SLOT(acceleratorSlot()) );
	}
    }

    connect( lbuddy, SIGNAL(destroyed()), this, SLOT(buddyDied()) );
}




QWidget * BtLabel::buddy() const
{
    return lbuddy;
}
#endif //QT_NO_ACCEL


#ifndef QT_NO_MOVIE
void BtLabel::movieUpdated(const QRect& rect)
{
    QMovie *mov = movie();
    if ( mov && !mov->isNull() ) {
	QRect r = contentsRect();
	r = style().itemRect( 0, r, align, TRUE/*isEnabled()*/, &(mov->framePixmap()),
			      QString::null );
	r.moveBy(rect.x(), rect.y());
	r.setWidth(QMIN(r.width(), rect.width()));
	r.setHeight(QMIN(r.height(), rect.height()));
	repaint( r, mov->framePixmap().mask() != 0 );
    }
}

void BtLabel::movieResized( const QSize& size )
{
    d->valid_hints = -1;
    if ( autoresize )
	adjustSize();
    movieUpdated( QRect( QPoint(0,0), size ) );
    updateGeometry();
}



void BtLabel::setMovie( const QMovie& movie )
{
    QSize osh = sizeHint();
    clearContents();
    
    if(lmovie)
      delete lmovie;
    lmovie = new QMovie( movie );
	lmovie->connectResize(this, SLOT(movieResized(const QSize&)));
	lmovie->connectUpdate(this, SLOT(movieUpdated(const QRect&)));

    if ( !lmovie->running() )	// Assume that if the movie is running,
	updateLabel( osh );	// resize/update signals will come soon enough
}

#endif // QT_NO_MOVIE



void BtLabel::clearContents()
{
#ifndef QT_NO_RICHTEXT
    delete doc;
    doc = 0;
#endif

    delete lpixmap;
    lpixmap = 0;
#ifndef QT_NO_PICTURE
    delete lpicture;
    lpicture = 0;
#endif
    delete d->img;
    d->img = 0;
    delete d->pix;
    d->pix = 0;

    ltext = QString::null;
#ifndef QT_NO_ACCEL
    if ( accel )
	accel->clear();
#endif
#ifndef QT_NO_MOVIE
    if ( lmovie ) {
	lmovie->disconnectResize(this, SLOT(movieResized(const QSize&)));
	lmovie->disconnectUpdate(this, SLOT(movieUpdated(const QRect&)));
	delete lmovie;
	lmovie = 0;
    }
#endif
}


#ifndef QT_NO_MOVIE



QMovie* BtLabel::movie() const
{
    return lmovie;
}

#endif  // QT_NO_MOVIE



Qt::TextFormat BtLabel::textFormat() const
{
    return textformat;
}

void BtLabel::setTextFormat( Qt::TextFormat format )
{
    if ( format != textformat ) {
	textformat = format;
	QString t = ltext;
	if ( !t.isNull() ) {
	    ltext = QString::null;
	    setText( t );
	}
    }
}

/*!
  \reimp
*/

void BtLabel::fontChange( const QFont & )
{
    if ( !ltext.isEmpty() ) {
#ifndef QT_NO_RICHTEXT
	if ( doc )
	    doc->setDefaultFont( font() );
#endif
	updateLabel( QSize( -1, -1 ) );
    }
}

#ifndef QT_NO_IMAGE_SMOOTHSCALE
/*!
    \property BtLabel::scaledContents
    \brief whether the label will scale its contents to fill all
    available space.

    When enabled and the label shows a pixmap, it will scale the
    pixmap to fill the available space.

    This property's default is FALSE.

    \sa setScaledContents()
*/

bool BtLabel::hasScaledContents() const
{
    return scaledcontents;
}

void BtLabel::setScaledContents( bool enable )
{
    if ( (bool)scaledcontents == enable )
	return;
    scaledcontents = enable;
    if ( !enable ) {
	delete d->img;
	d->img = 0;
	delete d->pix;
	d->pix = 0;
    }
    update( contentsRect() );
}

#endif // QT_NO_IMAGE_SMOOTHSCALE

/*!
    Sets the font used on the BtLabel to font \a f.
*/

void BtLabel::setFont( const QFont &f )
{
    QFrame::setFont( f );
}


