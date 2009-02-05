#ifndef TEST_DEVICE_H
#define TEST_DEVICE_H

#include <QSignalSpy>

class QString;
class QVariant;
class device;


class TestDevice
{
public:
	TestDevice(device *d, int dim);
	void check(QString frame, const QVariant &v);

private:
	QSignalSpy spy;
	int dim_type;
	device *dev;
};


#endif // TEST_DEVICE_H
