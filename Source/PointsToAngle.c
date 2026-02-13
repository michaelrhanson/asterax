/*
 * PointsToAngle conversion for Flare
 *
 * begun 3/24/94 mrh
 *
 */

#include <math.h>
#include "PointsToAngle.h"

int PointsToAngle(Point from, Point to) {
	int dh,dv,intres;
	double res;
	dh = to.h - from.h;
	dv = to.v - from.v;

	if (!dh) {
		if (dv < 0) res = 0;
		else res = 180;
	} else {
		res = (atan((double)dv/dh) * 180 / 3.14);
		res += 90;
		if (dh < 0) res += 180;
	}
	intres = res;
	intres %= 360;
	return intres;
}