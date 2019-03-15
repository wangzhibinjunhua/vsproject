#include "config.h"
#ifdef FALL_NEW_V1
#include "fall_new_p.h"
#include <string.h>
#include <math.h>
#include "fall_new_v1.h"

#define LINEAR(Before, Ante, After, Post, Now) (Ante + (Post - Ante) * (double)(Now - Before) / (double)(After - Before))
#define AT(Array, Index, Size) (Array[(Index + Size) % Size])
#define EXPIRE(Timeout) (Timeout > -1 ? Timeout - 1 : -1)

//StateStructure *State = NULL;

double SV(double X, double Y, double Z) {
	return sqrt(X * X + Y * Y + Z * Z);
}

double Min(double *Array, StateStructure *State) {
	int I;
	double Min = AT(Array, State->Position, N);
	for (I = 1; I < SPAN_MAXMIN; I++) {
		double Value = AT(Array, State->Position - I, N);
		if (!isnan(Value) && Value < Min) {
			Min = Value;
		}
	}
	return Min;
}

double Max(double *Array, StateStructure *State) {
	int I;
	double Max = AT(Array, State->Position, N);
	for (I = 1; I < SPAN_MAXMIN; I++) {
		double Value = AT(Array, State->Position - I, N);
		if (!isnan(Value) && Max < Value) {
			Max = Value;
		}
	}
	return Max;
}


// Low-pass Butterworth filter, 2nd order, 50 Hz sampling rate, corner frequency 0.25 Hz
double LPF(double Value, double *XV, double *YV) {
	XV[0] = XV[1];
	XV[1] = XV[2];
	XV[2] = Value / FILTER_LPF_GAIN;
	YV[0] = YV[1];
	YV[1] = YV[2];
	YV[2] = (XV[0] + XV[2]) + 2 * XV[1] + (FILTER_FACTOR_0 * YV[0]) + (FILTER_FACTOR_1 * YV[1]);
	return YV[2];
}

// High-pass Butterworth filter, 2nd order, 50 Hz sampling rate, corner frequency 0.25 Hz
double HPF(double Value, double *XV, double *YV) {
	XV[0] = XV[1];
	XV[1] = XV[2];
	XV[2] = Value / FILTER_HPF_GAIN;
	YV[0] = YV[1];
	YV[1] = YV[2];
	YV[2] = (XV[0] + XV[2]) - 2 * XV[1] + (FILTER_FACTOR_0 * YV[0]) + (FILTER_FACTOR_1 * YV[1]);
	return YV[2];
}

int Count(double *Events) {
	int I, Count = 0;
	for (I = 0; I < N; I++) {
		if (1 == AT(Events, I, N)) {
			Count++;
		}
	}
	return (Count);
}

void Call(StateStructure *State)
{
	printf("################\n");
	printf("FALL   line=%d\n",(State->AnteTime)/20);
	printf("################\n");
	State->fall_count++;
}

void Process(StateStructure *State) {
	//printf("Process State->Position=%d\n", State->Position);
	int At = State->Position;
	State->TimeoutFalling = EXPIRE(State->TimeoutFalling);
	State->TimeoutImpact = EXPIRE(State->TimeoutImpact);
	State->X_LPF[At] = LPF(State->X[At], State->XLPFXV, State->XLPFYV);
	State->Y_LPF[At] = LPF(State->Y[At], State->YLPFXV, State->YLPFYV);
	State->Z_LPF[At] = LPF(State->Z[At], State->ZLPFXV, State->ZLPFYV);
	State->X_HPF[At] = HPF(State->X[At], State->XHPFXV, State->XHPFYV);
	State->Y_HPF[At] = HPF(State->Y[At], State->YHPFXV, State->YHPFYV);
	State->Z_HPF[At] = HPF(State->Z[At], State->ZHPFXV, State->ZHPFYV);
	State->X_MAXMIN[At] = Max(State->X, State) - Min(State->X, State);
	State->Y_MAXMIN[At] = Max(State->Y, State) - Min(State->Y, State);
	State->Z_MAXMIN[At] = Max(State->Z, State) - Min(State->Z, State);
	double SV_TOT = State->SV_TOT[At] = SV(State->X[At], State->Y[At], State->Z[At]);
	double SV_D = State->SV_D[At] = SV(State->X_HPF[At], State->Y_HPF[At], State->Z_HPF[At]);
	State->SV_MAXMIN[At] = SV(State->X_MAXMIN[At], State->Y_MAXMIN[At], State->Z_MAXMIN[At]);
	State->Z_2[At] = (SV_TOT * SV_TOT - SV_D * SV_D - G * G) / (2.0 * G);
	double SV_TOT_BEFORE = AT(State->SV_TOT, At - 1, N);
	State->Falling[At] = 0;
	//printf("SV_TOT_BEFORE=%f,SV_TOT=%f\n", SV_TOT_BEFORE, SV_TOT);
	if (FALLING_WAIST_SV_TOT <= SV_TOT_BEFORE && SV_TOT < FALLING_WAIST_SV_TOT) {
		State->TimeoutFalling = SPAN_FALLING;
		State->Falling[At] = 1;
	}
	State->Impact[At] = 0;
	if (-1 < State->TimeoutFalling) {
		double SV_MAXMIN = State->SV_MAXMIN[At];
		double Z_2 = State->Z_2[At];
		printf("SV_TOT=%f,SV_D=%f,SV_MAXMIN=%f,Z_2=%f\n", SV_TOT, SV_D, SV_MAXMIN, Z_2);
		if (IMPACT_WAIST_SV_TOT <= SV_TOT || IMPACT_WAIST_SV_D <= SV_D ||
			IMPACT_WAIST_SV_MAXMIN <= SV_MAXMIN || IMPACT_WAIST_Z_2 <= Z_2) {
			State->TimeoutImpact = SPAN_IMPACT;
			printf("1111 State->TimeoutImpact=%d\n", State->TimeoutImpact);
			State->Impact[At] = 1;
		}




	}
	State->Lying[At] = 0;
	//printf("State->TimeoutImpact=%d\n", State->TimeoutImpact);
	if (0 == State->TimeoutImpact) {
		int I;
		double Sum = 0, Count = 0;
		for (I = 0; I < SPAN_AVERAGING; I++) {
			double Value = AT(State->Z_LPF, At - I, N);
			if (!isnan(Value)) {
				Sum += Value;
				Count += 1;
			}
		}
		if (LYING_AVERAGE_Z_LPF < (Sum / Count)) {
			State->Lying[At] = 1;
			Call(State);
		}
	}
}

void updateAT()
{

}
// Android sampling is irregular, thus the signal is (linearly) resampled at 50 Hz
void Resample(long PostTime, double PostX, double PostY, double PostZ, StateStructure *State) {
	//printf("State->AnteTime=%lld,State->Regular=%lld,PostTime=%lld\n", State->AnteTime, State->Regular,PostTime);
	if (0 == State->AnteTime) {
		State->Regular = PostTime+20;
		return;
	}
	if (State->Regular < PostTime) {
		int At = State->Position;
		State->X[At] = LINEAR(State->AnteTime, State->AnteX, PostTime, PostX, State->Regular);
		State->Y[At] = LINEAR(State->AnteTime, State->AnteY, PostTime, PostY, State->Regular);
		State->Z[At] = LINEAR(State->AnteTime, State->AnteZ, PostTime, PostZ, State->Regular);
		Process(State);
		State->Position = (State->Position + 1) % N;

		State->Regular += 20;
	}
}

void Protect(long PostTime, double PostX, double PostY, double PostZ, StateStructure *State) {
	//pthread_mutex_lock(&State->Lock);
	Resample(PostTime, PostX, PostY, PostZ, State);
	//pthread_mutex_unlock(&State->Lock);
}


void Sampled(double accx,double accy,double accz,int line, StateStructure *State) {
	long PostTime = (line+1) * 20;
	double PostX = accx / (9.80665f);
	double PostY = accy / (9.80665f);
	double PostZ = accz / (9.80665f);
	
	Protect(PostTime, PostX, PostY, PostZ, State);
	State->AnteTime = PostTime;
	State->AnteX = PostX;
	State->AnteY = PostY;
	State->AnteZ = PostZ;
}
/*
int Sampled(int FD, int Events, void *Data) {
	ASensorEvent Event;
	while (ASensorEventQueue_getEvents(State->Queue, &Event, 1) > 0) {
		jlong PostTime = Event.timestamp / 1000000;
		if (ASENSOR_TYPE_ACCELEROMETER == Event.type) {
			double PostX = Event.acceleration.x / ASENSOR_STANDARD_GRAVITY;
			double PostY = Event.acceleration.y / ASENSOR_STANDARD_GRAVITY;
			double PostZ = Event.acceleration.z / ASENSOR_STANDARD_GRAVITY;
			Protect(PostTime, PostX, PostY, PostZ);
			State->AnteTime = PostTime;
			State->AnteX = PostX;
			State->AnteY = PostY;
			State->AnteZ = PostZ;
		}
	}
	return (1);
}*/

void Fill(double *Array, int Offset, int Length, double Value) {
	int I;
	for (I = Offset; I < Offset + Length; I++) {
		Array[I] = Value;
	}
}

void InitiateBuffer(StateStructure *State) {
	//JNIEnv *JNI = State->JNI;
	//State->BufferArray = (*JNI)->NewGlobalRef(JNI, (*JNI)->NewDoubleArray(JNI, SIZE_BUFFER));
	//State->Buffer = (*JNI)->GetDoubleArrayElements(JNI, State->BufferArray, NULL);
	printf("InitiateBuffer buffer addr:%x\n", State);
	
	Fill(State->Buffer, 0, SIZE_BUFFER, FP_NAN);
	//(*JNI)->ReleaseDoubleArrayElements(JNI, State->BufferArray, State->Buffer, JNI_COMMIT);
	State->TimeoutImpact = -1;
	State->TimeoutFalling = -1;
	State->Position = 0;
	//add by wzb 
	State->fall_count = 0;
}

void InitiateSamples(StateStructure *State) {
	State->X = State->Buffer + altermarkive_guardian_Detector_OFFSET_X;
	State->Y = State->Buffer + altermarkive_guardian_Detector_OFFSET_Y;
	State->Z = State->Buffer + altermarkive_guardian_Detector_OFFSET_Z;
}

void InitiateResampling(StateStructure *State) {
	State->AnteX = State->AnteY = State->AnteZ = FP_NAN;
	State->AnteTime = 0;
}

void InitiateFiltering(StateStructure *State) {
	State->X_LPF = State->Buffer + altermarkive_guardian_Detector_OFFSET_X_LPF;
	State->Y_LPF = State->Buffer + altermarkive_guardian_Detector_OFFSET_Y_LPF;
	State->Z_LPF = State->Buffer + altermarkive_guardian_Detector_OFFSET_Z_LPF;
	State->X_HPF = State->Buffer + altermarkive_guardian_Detector_OFFSET_X_HPF;
	State->Y_HPF = State->Buffer + altermarkive_guardian_Detector_OFFSET_Y_HPF;
	State->Z_HPF = State->Buffer + altermarkive_guardian_Detector_OFFSET_Z_HPF;
	Fill(State->XLPFXV, 0, FILTER_NZEROS + 1, 0);
	Fill(State->XLPFYV, 0, FILTER_NPOLES + 1, 0);
	Fill(State->YLPFXV, 0, FILTER_NZEROS + 1, 0);
	Fill(State->YLPFYV, 0, FILTER_NPOLES + 1, 0);
	Fill(State->ZLPFXV, 0, FILTER_NZEROS + 1, 0);
	Fill(State->ZLPFYV, 0, FILTER_NPOLES + 1, 0);
	Fill(State->XHPFXV, 0, FILTER_NZEROS + 1, 0);
	Fill(State->XHPFYV, 0, FILTER_NPOLES + 1, 0);
	Fill(State->YHPFXV, 0, FILTER_NZEROS + 1, 0);
	Fill(State->YHPFYV, 0, FILTER_NPOLES + 1, 0);
	Fill(State->ZHPFXV, 0, FILTER_NZEROS + 1, 0);
	Fill(State->ZHPFYV, 0, FILTER_NPOLES + 1, 0);
}

void InitiateDeltas(StateStructure *State) {
	State->X_MAXMIN = State->Buffer + altermarkive_guardian_Detector_OFFSET_X_D;
	State->Y_MAXMIN = State->Buffer + altermarkive_guardian_Detector_OFFSET_Y_D;
	State->Z_MAXMIN = State->Buffer + altermarkive_guardian_Detector_OFFSET_Z_D;
}

void InitiateSV(StateStructure *State) {
	State->SV_TOT = State->Buffer + altermarkive_guardian_Detector_OFFSET_SV_TOT;
	State->SV_D = State->Buffer + altermarkive_guardian_Detector_OFFSET_SV_D;
	State->SV_MAXMIN = State->Buffer + altermarkive_guardian_Detector_OFFSET_SV_MAXMIN;
	State->Z_2 = State->Buffer + altermarkive_guardian_Detector_OFFSET_Z_2;
}

void InitiateEvents(StateStructure *State) {
	State->Falling = State->Buffer + altermarkive_guardian_Detector_OFFSET_FALLING;
	State->Impact = State->Buffer + altermarkive_guardian_Detector_OFFSET_IMPACT;
	State->Lying = State->Buffer + altermarkive_guardian_Detector_OFFSET_LYING;
}

void InitiateProtection(StateStructure *State) {
	//pthread_mutex_init(&State->Lock, NULL);
}

void InitiateSensor(StateStructure *State) {

}

void print_fall_new_result(StateStructure *State) {
	printf("fall count:%d\n", State->fall_count);
}

void detector_init(StateStructure *Blank) {
	
	

		//StateStructure *Blank = (StateStructure *)malloc(sizeof(StateStructure));
		//Blank = (StateStructure *)malloc(sizeof(StateStructure));
		printf("Blank:%x\n", Blank);
		printf("Blank->Buffer:%x\n", Blank->Buffer);
	
		InitiateBuffer(Blank);
		InitiateSamples(Blank);
		InitiateResampling(Blank);
		InitiateFiltering(Blank);
		InitiateDeltas(Blank);
		InitiateSV(Blank);
		InitiateEvents(Blank);
		InitiateProtection(Blank);
		InitiateSensor(Blank);
		
	

}

#endif

