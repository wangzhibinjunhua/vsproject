#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#define  CIRCLE_RADIANS  6.283185307179586476925286766559
double angleOf(double x, double y) {

	double  dist = sqrt(x*x + y*y);

	if (y >= 0.) return acos(x / dist);
	else       return acos(-x / dist) + 0.5*CIRCLE_RADIANS;
}

void findSmallestPolygon(double *x, double *y, long points, double *polyX, double *polyY, long *polyCorners) {

	double  newX = x[0], newY = y[0], xDif, yDif, oldAngle = 0.5*CIRCLE_RADIANS, newAngle, angleDif, minAngleDif;
	long    i;

	//  Find a starting point.
	for (i = 0; i<points; i++) if (y[i]>newY || ((y[i] == newY) && x[i] < newX)) {
		newX = x[i]; newY = y[i];
	}
	printf("newx=%f,newy=%f\n", newX, newY);
	//test
	//newX = x[0];
	//newY = y[0];

	//end
	*polyCorners = 0;

	//  Polygon-construction loop.
	while (!(*polyCorners) || newX != polyX[0] || newY != polyY[0]) {
		polyX[*polyCorners] = newX;
		polyY[*polyCorners] = newY; minAngleDif = CIRCLE_RADIANS;
		for (i = 0; i < points; i++) {
			xDif = x[i] - polyX[*polyCorners];
			yDif = y[i] - polyY[*polyCorners];
			if (xDif || yDif) {
				newAngle = angleOf(xDif, yDif);     angleDif = oldAngle - newAngle;

				while (angleDif < 0.) angleDif += CIRCLE_RADIANS;

				//while (angleDif >= CIRCLE_RADIANS) angleDif -= CIRCLE_RADIANS;
				while (angleDif > CIRCLE_RADIANS) angleDif -= CIRCLE_RADIANS;
				//printf("i=%d,newAngle=%f,oldAngle=%f,angleDif=%f,minAngleDif=%f\n", i, newAngle, oldAngle,
				//	angleDif, minAngleDif);

				if ((angleDif < minAngleDif)
					&& angleDif>0.000001f) {


					minAngleDif = angleDif; newX = x[i]; newY = y[i];
					//printf("i=%d,newX=%f,newY=%f\n", i,newX,newY);
				}
			}
		}
		(*polyCorners)++; oldAngle += 0.5*CIRCLE_RADIANS - minAngleDif;
	}
}