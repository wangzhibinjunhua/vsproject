#include "config.h"
#ifdef  FALL_NEW_V1
#ifndef FALL_DETECTOR_H
#define FALL_DETECTOR_H


#define INTERVAL_MS altermarkive_guardian_Detector_INTERVAL_MS
#define N altermarkive_guardian_Detector_N
#define SIZE_BUFFER altermarkive_guardian_Detector_SIZE
#define SPAN_MAXMIN (100 / INTERVAL_MS)
#define SPAN_FALLING (1000 / INTERVAL_MS)
#define SPAN_IMPACT (2000 / INTERVAL_MS)
#define SPAN_AVERAGING (400 / INTERVAL_MS)

#define FILTER_NZEROS 2
#define FILTER_NPOLES 2
#define FILTER_LPF_GAIN 4.143204922e+03
#define FILTER_HPF_GAIN 1.022463023e+00
#define FILTER_FACTOR_0 -0.9565436765
#define FILTER_FACTOR_1 +1.9555782403

#define G 1.0

#define FALLING_WAIST_SV_TOT altermarkive_guardian_Detector_FALLING_WAIST_SV_TOT
#define IMPACT_WAIST_SV_TOT altermarkive_guardian_Detector_IMPACT_WAIST_SV_TOT
#define IMPACT_WAIST_SV_D altermarkive_guardian_Detector_IMPACT_WAIST_SV_D
#define IMPACT_WAIST_SV_MAXMIN altermarkive_guardian_Detector_IMPACT_WAIST_SV_MAXMIN
#define IMPACT_WAIST_Z_2 altermarkive_guardian_Detector_IMPACT_WAIST_Z_2
#define LYING_AVERAGE_Z_LPF 0.5

typedef struct {
	double *Buffer;
	int TimeoutFalling;
	int TimeoutImpact;
	int Position;
	double *X;
	double *Y;
	double *Z;
	double *X_LPF;
	double *Y_LPF;
	double *Z_LPF;
	double *X_HPF;
	double *Y_HPF;
	double *Z_HPF;
	double *X_MAXMIN;
	double *Y_MAXMIN;
	double *Z_MAXMIN;
	double *SV_TOT;
	double *SV_D;
	double *SV_MAXMIN;
	double *Z_2;
	double *Falling;
	double *Impact;
	double *Lying;
	double XLPFXV[FILTER_NZEROS + 1];
	double XLPFYV[FILTER_NPOLES + 1];
	double YLPFXV[FILTER_NZEROS + 1];
	double YLPFYV[FILTER_NPOLES + 1];
	double ZLPFXV[FILTER_NZEROS + 1];
	double ZLPFYV[FILTER_NPOLES + 1];
	double XHPFXV[FILTER_NZEROS + 1];
	double XHPFYV[FILTER_NPOLES + 1];
	double YHPFXV[FILTER_NZEROS + 1];
	double YHPFYV[FILTER_NPOLES + 1];
	double ZHPFXV[FILTER_NZEROS + 1];
	double ZHPFYV[FILTER_NPOLES + 1];
	double AnteX;
	double AnteY;
	double AnteZ;
	long AnteTime;
	long Regular;
	//pthread_mutex_t Lock;
	//ASensorEventQueue *Queue;
	int fall_count;
} StateStructure;
void Sampled(double accx, double accy, double accz, int line);
void detector_init(StateStructure *S);
void print_fall_new_result();
#endif 
#endif