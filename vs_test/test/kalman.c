#include <stdio.h>  
#include <stdlib.h>  
#include <math.h> 
typedef int uint8_t;
float dtTimer = 0.008;
float xk[9] = { 0,0,0,0,0,0,0,0,0 };
float pk[9] = { 1,0,0,0,1,0,0,0,0 };
float I[9] = { 1,0,0,0,1,0,0,0,1 };
float R[9] = { 0.5,0,0,0,0.5,0,0,0,0.01 };
float Q[9] = { 0.005,0,0,0,0.005,0,0,0,0.001 };

void matrix_add(float* mata, float* matb, float* matc) {
	uint8_t i, j;
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			matc[i * 3 + j] = mata[i * 3 + j] + matb[i * 3 + j];
		}
	}
}

void matrix_sub(float* mata, float* matb, float* matc) {
	uint8_t i, j;
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			matc[i * 3 + j] = mata[i * 3 + j] - matb[i * 3 + j];
		}
	}
}

void matrix_multi(float* mata, float* matb, float* matc) {
	uint8_t i, j, m;
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			matc[i * 3 + j] = 0.0;
			for (m = 0; m < 3; m++)
			{
				matc[i * 3 + j] += mata[i * 3 + m] * matb[m * 3 + j];
			}
		}
	}
}

void KalmanFilter(float* am_angle_mat, float* gyro_angle_mat) {
	uint8_t i, j;
	float yk[9];
	float pk_new[9];
	float K[9];
	float KxYk[9];
	float I_K[9];
	float S[9];
	float S_invert[9];
	float sdet;

	//xk = xk + uk
	matrix_add(xk, gyro_angle_mat, xk);
	//pk = pk + Q
	matrix_add(pk, Q, pk);
	//yk =  xnew - xk
	matrix_sub(am_angle_mat, xk, yk);
	//S=Pk + R
	matrix_add(pk, R, S);
	//SÇóÄæinvert
	sdet = S[0] * S[4] * S[8]
		+ S[1] * S[5] * S[6]
		+ S[2] * S[3] * S[7]
		- S[2] * S[4] * S[6]
		- S[5] * S[7] * S[0]
		- S[8] * S[1] * S[3];

	S_invert[0] = (S[4] * S[8] - S[5] * S[7]) / sdet;
	S_invert[1] = (S[2] * S[7] - S[1] * S[8]) / sdet;
	S_invert[2] = (S[1] * S[7] - S[4] * S[6]) / sdet;

	S_invert[3] = (S[5] * S[6] - S[3] * S[8]) / sdet;
	S_invert[4] = (S[0] * S[8] - S[2] * S[6]) / sdet;
	S_invert[5] = (S[2] * S[3] - S[0] * S[5]) / sdet;

	S_invert[6] = (S[3] * S[7] - S[4] * S[6]) / sdet;
	S_invert[7] = (S[1] * S[6] - S[0] * S[7]) / sdet;
	S_invert[8] = (S[0] * S[4] - S[1] * S[3]) / sdet;
	//K = Pk * S_invert
	matrix_multi(pk, S_invert, K);
	//KxYk = K * Yk
	matrix_multi(K, yk, KxYk);
	//xk = xk + K * yk
	matrix_add(xk, KxYk, xk);
	//pk = (I - K)*(pk)
	matrix_sub(I, K, I_K);
	matrix_multi(I_K, pk, pk_new);
	//update pk
	//pk = pk_new;
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			pk[i * 3 + j] = pk_new[i * 3 + j];
		}
	}
}