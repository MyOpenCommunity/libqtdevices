#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <QLabel>

class QPixmap;
class QMouseEvent;


// common code to display an image
class ImageLabel : public QLabel
{
Q_OBJECT
public:
	ImageLabel();

	// if the pixmap is too big for the label, it is scaled down, respecting
	// proportions, otherwise it is displayed as it is
	void setPixmap(const QPixmap &pixmap);

signals:
	void clicked();

protected:
	void mouseReleaseEvent(QMouseEvent *e);
};


#endif // IMAGELABEL_H
