#pragma once
#ifndef __MAT_H
#define  __MAT_H
typedef struct point_s {
	float x;
	float y;
} POINT;

void
data_analysis(POINT *xy, int len, POINT *pxy);
#endif
