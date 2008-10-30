#include "btlabel.h"

#include <QPainter>
#include <QStyleOption>
#include <QStyle>

BtLabel::BtLabel(QWidget *parent) : QLabel(parent)
{
	setStyleSheet("QLabel {etch-disabled-text: 0}");

}

void BtLabel::paintEvent(QPaintEvent *e)
{
	QStyle *style = QWidget::style();
	QPainter painter;
	painter.begin(this);
	drawFrame(&painter);
	QRect cr = contentsRect();
	cr.adjust(margin(), margin(), -margin(), -margin());
	int align = QStyle::visualAlignment(layoutDirection(), QFlag(alignment()));

	if (pixmap() && !pixmap()->isNull())
	{
		QPixmap pix;
		if (hasScaledContents())
		{
			/* Removed Qt code below
			if (!d->scaledpixmap || d->scaledpixmap->size() != cr.size()) {
				if (!d->cachedimage)
				d->cachedimage = new QImage(d->pixmap->toImage());
				delete d->scaledpixmap;
				d->scaledpixmap = new QPixmap(QPixmap::fromImage(d->cachedimage->scaled(cr.size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
			}
			*/
			// we may have performance problems here...fortunately, this is not the default path for qlabel
			pix = pixmap()->scaled(cr.size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
		}
		else
			pix = *pixmap();
		QStyleOption opt;
		opt.initFrom(this);
		style->drawItemPixmap(&painter, cr, align, pix);
		// NOTE: In this case, painter.end() is called by QPainter destructor
	}
	else
	{
		painter.end();
		QLabel::paintEvent(e);
	}

	/*
#ifndef QT_NO_MOVIE
    if (movie()) {
        if (hasScaledContents())
            style->drawItemPixmap(&painter, cr, align, movie()->currentPixmap().scaled(cr.size()));
        else
            style->drawItemPixmap(&painter, cr, align, movie()->currentPixmap());
    }
    else
#endif
    if (!text().isEmpty()) {
        QRectF lr = d->layoutRect();
        if (d->control) {
#ifndef QT_NO_SHORTCUT
            const bool underline = (bool)style->styleHint(QStyle::SH_UnderlineShortcut, 0, this, 0);
            if (d->shortcutId != 0
                && underline != d->shortcutCursor.charFormat().fontUnderline()) {
                QTextCharFormat fmt;
                fmt.setFontUnderline(underline);
                d->shortcutCursor.mergeCharFormat(fmt);
            }
#endif
            d->ensureTextLayouted();

            QAbstractTextDocumentLayout::PaintContext context;
            QStyleOption opt(0);
            opt.init(this);

            if (!isEnabled() && style->styleHint(QStyle::SH_EtchDisabledText, &opt, this)) {
                context.palette = palette();
                context.palette.setColor(QPalette::Text, context.palette.light().color());
                painter.save();
                painter.translate(lr.x() + 1, lr.y() + 1);
                painter.setClipRect(lr.translated(-lr.x() - 1, -lr.y() - 1));
                QAbstractTextDocumentLayout *layout = d->control->document()->documentLayout();
                layout->draw(&painter, context);
                painter.restore();
            }

            // Adjust the palette
            context.palette = palette();
            if (foregroundRole() != QPalette::Text && isEnabled())
                context.palette.setColor(QPalette::Text, context.palette.color(foregroundRole()));

            painter.save();
            painter.translate(lr.topLeft());
            painter.setClipRect(lr.translated(-lr.x(), -lr.y()));
            d->control->setPalette(context.palette);
            d->control->drawContents(&painter, QRectF(), this);
            painter.restore();
        } else {
            int flags = align;
            if (d->hasShortcut) {
                flags |= Qt::TextShowMnemonic;
                QStyleOption opt;
                opt.initFrom(this);
                if (!style->styleHint(QStyle::SH_UnderlineShortcut, &opt, this))
                    flags |= Qt::TextHideMnemonic;
            }
            style->drawItemText(&painter, lr.toRect(), flags, palette(), isEnabled(), d->text, foregroundRole());
        }
    } else
#ifndef QT_NO_PICTURE
    if (d->picture) {
        QRect br = d->picture->boundingRect();
        int rw = br.width();
        int rh = br.height();
        if (d->scaledcontents) {
            painter.save();
            painter.translate(cr.x(), cr.y());
            painter.scale((double)cr.width()/rw, (double)cr.height()/rh);
            painter.drawPicture(-br.x(), -br.y(), *d->picture);
            painter.restore();
        } else {
            int xo = 0;
            int yo = 0;
            if (align & Qt::AlignVCenter)
                yo = (cr.height()-rh)/2;
            else if (align & Qt::AlignBottom)
                yo = cr.height()-rh;
            if (align & Qt::AlignRight)
                xo = cr.width()-rw;
            else if (align & Qt::AlignHCenter)
                xo = (cr.width()-rw)/2;
            painter.drawPicture(cr.x()+xo-br.x(), cr.y()+yo-br.y(), *d->picture);
        }
    } else
#endif
	   */
}
