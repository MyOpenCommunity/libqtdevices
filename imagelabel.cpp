#include "imagelabel.h"

#include <QSize>
#include <QSizePolicy>
#include <QMouseEvent>
#include <QPixmap>


ImageLabel::ImageLabel()
{
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	setAlignment(Qt::AlignCenter);
}

void ImageLabel::setPixmap(const QPixmap &pixmap)
{
	QSize screen_size = size(), pixmap_size = pixmap.size();
	QPixmap scaled = pixmap;

	// resize the pixmap if it's too big for the screen
	if (pixmap_size.width() > screen_size.width() ||
		pixmap_size.height() > screen_size.height())
		scaled = pixmap.scaled(screen_size, Qt::KeepAspectRatio);

	QLabel::setPixmap(scaled);
}

void ImageLabel::mouseReleaseEvent(QMouseEvent *e)
{
	QLabel::mouseReleaseEvent(e);

	emit clicked();
}

