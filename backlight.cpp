#include "backlight.h"
#include "genericfunz.h"

QValueVector< QPair< BrightnessControl::BacklightStatus, unsigned > > BrightnessControl::policy;

bool BrightnessControl::setBrightnessPolicy(const QValueVector< QPair< BacklightStatus, unsigned > > &bp)
{
	if (bp.size() != NUMBER_OF_STATES)
	{
		qWarning("Invalid brightness policy: there are not exactly %u states", NUMBER_OF_STATES);
		return false;
	}
	policy = bp;
	return true;
}

void BrightnessControl::setState(BrightnessControl::State state)
{
	// FIXME: cosi' funziona ma forse e' meglio fare un cast a bool
	setBacklightOn(policy[state].first);

	setBrightnessLevel(policy[state].second);
}
