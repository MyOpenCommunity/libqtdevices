#ifndef DEVICE_TESTER_H
#define DEVICE_TESTER_H

#include <QSignalSpy>

class QStringList;
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

	// NOTE: be aware that QVariant::operator== with custom types compare addresses of objects
	void check(const QStringList &frames, const QVariant &result);
	void check(QString frame, const QVariant &result);

	// Verify the number of signals emitted
	void checkSignals(const QStringList &frames, int num_signals);
	void checkSignals(QString frame, int num_signals);

	QVariant getResult(const QStringList& frames);

private:
	QSignalSpy spy;
	int dim_type;
	device *dev;
	void sendFrames(const QStringList& frames);
};


#endif // DEVICE_TESTER_H
