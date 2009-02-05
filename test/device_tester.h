#ifndef DEVICE_TESTER_H
#define DEVICE_TESTER_H

#include <QSignalSpy>

class QString;
class QVariant;
class device;

/**
  * An utility class to test the signal emitted by device after the parsing of
  * a frame from openserver.
  */
class DeviceTester
{
public:
	DeviceTester(device *d, int dim);
	void check(QString frame, const QVariant &v);

private:
	QSignalSpy spy;
	int dim_type;
	device *dev;
};


#endif // DEVICE_TESTER_H
