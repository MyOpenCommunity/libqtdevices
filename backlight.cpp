#include "backlight.h"
#include "genericfunz.h"

brightness_policy_t BrightnessControl::policy;

bool BrightnessControl::setBrightnessPolicy(const brightness_policy_t &bp)
{
	if (bp.size() != NUMBER_OF_STATES)
	{
		qWarning("Invalid brightness policy: there are not exactly %u states", NUMBER_OF_STATES);
		return false;
	}
	policy = bp;
	return true;
}

void BrightnessControl::setState(BrightnessState state)
{
	// FIXME: cosi' funziona ma forse e' meglio fare un cast a bool
	setBacklightOn(policy[state].first);

	setBrightnessLevel(policy[state].second);
}
