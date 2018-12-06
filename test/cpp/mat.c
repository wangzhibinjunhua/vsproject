#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include "mat.h"

static int width = 15;
static int height = 28;
static void data_conver(POINT *xy, POINT *pxy, int len);
static void data_conver_v2(POINT *xy, POINT *pxy, int len);

static void
swap(POINT *arr, int a, int b)
{
	POINT temp = arr[a];
	arr[a] = arr[b];
	arr[b] = temp;
}

/*
*        Name:  getdist
* Description:  计算两点间的距离
*
*/
static float
getdist(POINT p1, POINT p2)
{
	return sqrt((p2.x - p1.x)*(p2.x - p1.x) + (p2.y - p1.y)*(p2.y - p1.y));
}

/*
*        Name:  getcross
* Description:  计算叉积z方向的值，用于判断两个向量的转向
*
*/
static float
getcross(POINT p0, POINT p1, POINT p2)
{
	return (p1.x - p0.x)*(p2.y - p0.y) - (p2.x - p0.x)*(p1.y - p0.y);
}

/*
*        Name:  getpod
* Description:  计算两个向量的点积
*
*/
static float
getdot(POINT p0, POINT p1, POINT p2)
{
	return (p1.x - p0.x)*(p2.x - p0.x) + (p1.y - p0.y)*(p2.y - p0.y);
}

/*
*        Name:  anglecmp
* Description:  比较两个向量的逆时针转角方向，p1大于p2时返回大于0，等于时返回等于0
*
*/
static float
anglecmp(POINT p0, POINT p1, POINT p2)
{
	float cross = getcross(p0, p1, p2);
	if (cross == 0) {
		return getdist(p0, p2) - getdist(p0, p1);
	}
	return cross;
}

/*
*        Name:  vectorsort
* Description:  根据基点进行向量排序, 快速排序递归实现
*
*/
static void
vectorsort(POINT *arr, int left, int right)
{
	int i, mid, last;

	if (left >= right) {
		return;
	}

	mid = (left + right) / 2;
	swap(arr, left, mid);
	last = left;
	for (i = left + 1; i <= right; i++) {
		if (anglecmp(arr[0], arr[i], arr[left])>0) {
			swap(arr, i, ++last);
		}
	}
	swap(arr, left, last);
	vectorsort(arr, left, last - 1);
	vectorsort(arr, last + 1, right);
}

/*
*        Name:  getconvex
* Description:  计算凸包
*
*/
static void
getconvex(POINT *arr, int len, int *n)
{
	int i, base, top;

	/* 小于4个点的不计算凸包 */
	if (len < 4) {
		*n = len;
		return;
	}
	/* 计算基点，交换到0位置 */
	base = 0;
	for (i = 0; i<len; i++) {
		if (arr[i].y == arr[base].y && arr[i].x < arr[base].x) {
			base = i;
		}
		else if (arr[i].y < arr[base].y) {
			base = i;
		}
	}
	swap(arr, base, 0);

	/* 排序 */
	vectorsort(arr, 1, len - 1);

	/* 计算凸包 */
	top = 1;
	for (i = 2; i<len; i++) {
		while (top>0 && getcross(arr[top - 1], arr[top], arr[i]) <= 0) {
			top--;
		}
		arr[++top] = arr[i];
	}
	*n = top;
}

static void
rotatingcalipers(POINT *arr, int len, POINT *rectangle)
{
	int top, down, right = 1, up = 0, left = 0, downlast, rightlast, uplast, leftlast;
	float area = FLT_MAX, dist, X, Y, k;
	POINT temp;

	getconvex(arr, len, &top);
	//fix bug
	POINT *new_arr;
	new_arr = (POINT*)malloc((len+1)*sizeof(POINT));
	if (new_arr != NULL) {
		memcpy(new_arr, arr, len * sizeof(POINT));
		new_arr[++top] = new_arr[0];
		printf("top=%d\n", top);
		for (int i = 0; i < top; i++) {
			printf("convex:%f,%f\n", new_arr[i].x, new_arr[i].y);
		}
		for (down = 0; down < top; down++) {
			// find right
			while (getdot(new_arr[down], new_arr[down + 1], new_arr[right]) <= getdot(new_arr[down], new_arr[down + 1], new_arr[right + 1])) {
				right = (right + 1) % top;
			}

			// find up
			if (down == 0) {
				up = right;
			}
			while (getcross(new_arr[down], new_arr[down + 1], new_arr[up]) <= getcross(new_arr[down], new_arr[down + 1], new_arr[up + 1])) {
				up = (up + 1) % top;
			}

			// find down
			if (down == 0) {
				left = up;
			}
			while (getdot(new_arr[down], new_arr[down + 1], new_arr[left]) >= getdot(new_arr[down], new_arr[down + 1], new_arr[left + 1])) {
				left = (left + 1) % top;
			}

			dist = getdist(new_arr[down], new_arr[down + 1]);
			X = getcross(new_arr[down], new_arr[down + 1], new_arr[up]) / dist;
			temp.x = new_arr[right].x + new_arr[down].x - new_arr[left].x;
			temp.y = new_arr[right].y + new_arr[down].y - new_arr[left].y;
			Y = getdot(new_arr[down], new_arr[down + 1], temp)/dist;
			printf("x*y=%f,down=%d,right=%d,up=%d,left=%d\n", X*Y, down, right, up, left);
			if (area > X*Y) {
				area = X*Y;
				downlast = down;
				rightlast = right;
				uplast = up;
				leftlast = left;
			}
		}


		printf("last down=%d,right=%d,up=%d,left=%d\n", downlast, rightlast, uplast, leftlast);
		// 计算外接矩形
		if (new_arr[downlast + 1].y == new_arr[downlast].y) {
			rectangle[0].x = new_arr[leftlast].x;
			rectangle[0].y = new_arr[downlast].y;

			rectangle[1].x = new_arr[rightlast].x;
			rectangle[1].y = new_arr[downlast].y;

			rectangle[2].x = new_arr[rightlast].x;
			rectangle[2].y = new_arr[uplast].y;

			rectangle[3].x = new_arr[leftlast].x;
			rectangle[3].y = new_arr[uplast].y;

		}
		else if (new_arr[downlast + 1].x == new_arr[downlast].x) {
			rectangle[0].x = new_arr[downlast].x;
			rectangle[0].y = new_arr[leftlast].y;

			rectangle[1].x = new_arr[downlast].x;
			rectangle[1].y = new_arr[rightlast].y;

			rectangle[2].x = new_arr[uplast].x;
			rectangle[2].y = new_arr[rightlast].y;

			rectangle[3].x = new_arr[uplast].x;
			rectangle[3].y = new_arr[leftlast].y;

		}
		else {
			k = (new_arr[downlast + 1].y - new_arr[downlast].y) / (new_arr[downlast + 1].x - new_arr[downlast].x);

			rectangle[0].x = (k*new_arr[leftlast].y + new_arr[leftlast].x - k*new_arr[downlast].y + k*k*new_arr[downlast].x) / (k*k + 1.0);
			rectangle[0].y = k*rectangle[0].x + new_arr[downlast].y - k*new_arr[downlast].x;

			rectangle[1].x = (k*new_arr[rightlast].y + new_arr[rightlast].x - k*new_arr[downlast].y + k*k*new_arr[downlast].x) / (k*k + 1.0);
			rectangle[1].y = k*rectangle[1].x + new_arr[downlast].y - k*new_arr[downlast].x;

			rectangle[2].x = (k*new_arr[rightlast].y + new_arr[rightlast].x - k*new_arr[uplast].y + k*k*new_arr[uplast].x) / (k*k + 1.0);
			rectangle[2].y = k*rectangle[2].x + new_arr[uplast].y - k*new_arr[uplast].x;

			rectangle[3].x = (k*new_arr[leftlast].y + new_arr[leftlast].x - k*new_arr[uplast].y + k*k*new_arr[uplast].x) / (k*k + 1.0);
			rectangle[3].y = k*rectangle[3].x + new_arr[uplast].y - k*new_arr[uplast].x;
		}
		free(new_arr);
		new_arr = NULL;
		return;
	}
	//end
	arr[++top] = arr[0];

	for (down = 0; down<top; down++) {
		// find right
		while (getdot(arr[down], arr[down + 1], arr[right]) <= getdot(arr[down], arr[down + 1], arr[right + 1])) {
			right = (right + 1) % top;
		}

		// find up
		if (down == 0) {
			up = right;
		}
		while (getcross(arr[down], arr[down + 1], arr[up]) <= getcross(arr[down], arr[down + 1], arr[up + 1])) {
			up = (up + 1) % top;
		}

		// find down
		if (down == 0) {
			left = up;
		}
		while (getdot(arr[down], arr[down + 1], arr[left]) >= getdot(arr[down], arr[down + 1], arr[left + 1])) {
			left = (left + 1) % top;
		}

		dist = getdist(arr[down], arr[down + 1]);
		X = getcross(arr[down], arr[down + 1], arr[up]) / dist;
		temp.x = arr[right].x + arr[down].x - arr[left].x;
		temp.y = arr[right].y + arr[down].y - arr[left].y;
		Y = getdot(arr[down], arr[down + 1], temp)/ dist;
		printf("X*Y=%f,down=%d,right=%d,up=%d,left=%d\n", X*Y, down, right, up, left);
		if (area > X*Y) {
			area = X*Y;
			downlast = down;
			rightlast = right;
			uplast = up;
			leftlast = left;
		}
	}

	// 计算外接矩形
	if (arr[downlast + 1].y == arr[downlast].y) {
		rectangle[0].x = arr[leftlast].x;
		rectangle[0].y = arr[downlast].y;

		rectangle[1].x = arr[rightlast].x;
		rectangle[1].y = arr[downlast].y;

		rectangle[2].x = arr[rightlast].x;
		rectangle[2].y = arr[uplast].y;

		rectangle[3].x = arr[leftlast].x;
		rectangle[3].y = arr[uplast].y;

	}
	else if (arr[downlast + 1].x == arr[downlast].x) {
		rectangle[0].x = arr[downlast].x;
		rectangle[0].y = arr[leftlast].y;

		rectangle[1].x = arr[downlast].x;
		rectangle[1].y = arr[rightlast].y;

		rectangle[2].x = arr[uplast].x;
		rectangle[2].y = arr[rightlast].y;

		rectangle[3].x = arr[uplast].x;
		rectangle[3].y = arr[leftlast].y;

	}
	else {
		k = (arr[downlast + 1].y - arr[downlast].y) / (arr[downlast + 1].x - arr[downlast].x);

		rectangle[0].x = (k*arr[leftlast].y + arr[leftlast].x - k*arr[downlast].y + k*k*arr[downlast].x) / (k*k + 1.0);
		rectangle[0].y = k*rectangle[0].x + arr[downlast].y - k*arr[downlast].x;

		rectangle[1].x = (k*arr[rightlast].y + arr[rightlast].x - k*arr[downlast].y + k*k*arr[downlast].x) / (k*k + 1.0);
		rectangle[1].y = k*rectangle[1].x + arr[downlast].y - k*arr[downlast].x;

		rectangle[2].x = (k*arr[rightlast].y + arr[rightlast].x - k*arr[uplast].y + k*k*arr[uplast].x) / (k*k + 1.0);
		rectangle[2].y = k*rectangle[2].x + arr[uplast].y - k*arr[uplast].x;

		rectangle[3].x = (k*arr[leftlast].y + arr[leftlast].x - k*arr[uplast].y + k*k*arr[uplast].x) / (k*k + 1.0);
		rectangle[3].y = k*rectangle[3].x + arr[uplast].y - k*arr[uplast].x;
	}
}

static float rectangle_area(POINT p0, POINT p1, POINT p2)
{
	return sqrt((p1.x - p0.x)*(p1.x - p0.x) + (p1.y - p0.y)*(p1.y - p0.y))*sqrt((p2.x - p1.x)*(p2.x - p1.x) + (p2.y - p1.y)*(p2.y - p1.y));
}


static void
rotatingcalipers_new(POINT *arr, int len, POINT *rectangle)
{
	int top, down, right = 1, up = 0, left = 0, downlast, rightlast, uplast, leftlast;
	float area = FLT_MAX, dist, X, Y, k;
	POINT temp;

	getconvex(arr, len, &top);
	arr[++top] = arr[0];
	printf("top=%d\n", top);
	for (int i = 0; i < top; i++) {
		printf("convex:%f,%f\n", arr[i].x, arr[i].y);
	}
	for (down = 0; down < top; down++) {
		// find right
		while (getdot(arr[down], arr[down + 1], arr[right]) <= getdot(arr[down], arr[down + 1], arr[right + 1])) {
			right = (right + 1) % top;
		}

		// find up
		if (down == 0) {
			up = right;
		}
		while (getcross(arr[down], arr[down + 1], arr[up]) <= getcross(arr[down], arr[down + 1], arr[up + 1])) {
			up = (up + 1) % top;
		}

		// find down
		if (down == 0) {
			left = up;
		}
		while (getdot(arr[down], arr[down + 1], arr[left]) >= getdot(arr[down], arr[down + 1], arr[left + 1])) {
			left = (left + 1) % top;
		}

		POINT temp_rectangle[4];
		// 计算外接矩形
		if (arr[down + 1].y == arr[down].y) {
			temp_rectangle[0].x = arr[left].x;
			temp_rectangle[0].y = arr[down].y;

			temp_rectangle[1].x = arr[right].x;
			temp_rectangle[1].y = arr[down].y;

			temp_rectangle[2].x = arr[right].x;
			temp_rectangle[2].y = arr[up].y;

			temp_rectangle[3].x = arr[left].x;
			temp_rectangle[3].y = arr[up].y;

		}
		else if (arr[down + 1].x == arr[down].x) {
			temp_rectangle[0].x = arr[down].x;
			temp_rectangle[0].y = arr[left].y;

			temp_rectangle[1].x = arr[down].x;
			temp_rectangle[1].y = arr[right].y;

			temp_rectangle[2].x = arr[up].x;
			temp_rectangle[2].y = arr[right].y;

			temp_rectangle[3].x = arr[up].x;
			temp_rectangle[3].y = arr[left].y;

		}
		else {
			k = (arr[down + 1].y - arr[down].y) / (arr[down + 1].x - arr[down].x);

			temp_rectangle[0].x = (k*arr[left].y + arr[left].x - k*arr[down].y + k*k*arr[down].x) / (k*k + 1.0);
			temp_rectangle[0].y = k*temp_rectangle[0].x + arr[down].y - k*arr[down].x;

			temp_rectangle[1].x = (k*arr[right].y + arr[right].x - k*arr[down].y + k*k*arr[down].x) / (k*k + 1.0);
			temp_rectangle[1].y = k*temp_rectangle[1].x + arr[down].y - k*arr[down].x;

			temp_rectangle[2].x = (k*arr[right].y + arr[right].x - k*arr[up].y + k*k*arr[up].x) / (k*k + 1.0);
			temp_rectangle[2].y = k*temp_rectangle[2].x + arr[up].y - k*arr[up].x;

			temp_rectangle[3].x = (k*arr[left].y + arr[left].x - k*arr[up].y + k*k*arr[up].x) / (k*k + 1.0);
			temp_rectangle[3].y = k*temp_rectangle[3].x + arr[up].y - k*arr[up].x;
		}
		float rectarea = rectangle_area(temp_rectangle[0], temp_rectangle[1], temp_rectangle[2]);
		printf("rectarea=%f,down=%d,right=%d,up=%d,left=%d\n", rectarea, down, right, up, left);
		if (area > rectarea) {
			area = rectarea;
			downlast = down;
			rightlast = right;
			uplast = up;
			leftlast = left;
			rectangle[0] = temp_rectangle[0];
			rectangle[1] = temp_rectangle[1];
			rectangle[2] = temp_rectangle[2];
			rectangle[3] = temp_rectangle[3];
			printf("last down=%d,right=%d,up=%d,left=%d\n", downlast, rightlast, uplast, leftlast);
		}
	}

	
	
}



static void variance_analysis(POINT *xy,int len)
{
	int i;
	float sumx=0.0f,sumy=0.0f,avex=0.0f,avey=0.0f,varx=0.0f,vary=0.0f,stdx=0.0f,stdy=0.0f;
	for (i = 0; i < len; i++) {
		sumx += xy[i].x;
		sumy += xy[i].y;
	}

	//平均值
	avex = sumx / len;
	avey = sumy / len;

	for (i = 0; i < len; i++) {
		varx += (xy[i].x - avex)*(xy[i].x - avex);
		vary += (xy[i].y - avey)*(xy[i].y - avey);
	}
	//方差
	varx = varx / len;
	vary = vary / len;

	//标准差
	stdx = sqrt(varx);
	stdy = sqrt(vary);

	printf("stdx=%f,stdy=%f,avex=%f,avey=%f\n", stdx, stdy, avex, avey);

	if (xy[0].x>(avex + 2 * stdx)) xy[0].x = avex;
	if (xy[0].x < (avex - 2 * stdx)) xy[0].x = avex;

	if (xy[0].y > (avey + 2 * stdy)) xy[0].y = avey;
	if (xy[0].y < (avey - 2 * stdy)) xy[0].y = avey;

	for (i = 1; i < len; i++) {
		
		if (xy[i].x>(avex + 2 * stdx)) xy[i].x = xy[i-1].x;
		if (xy[i].x < (avex - 2 * stdx)) xy[i].x = xy[i - 1].x;

		if (xy[i].y > (avey + 2 * stdy)) xy[i].y = xy[i-1].y;
		if (xy[i].y < (avey - 2 * stdy)) xy[i].y = xy[i - 1].y;
	}



}

void save_file(char *filename,POINT *data,int len) {
	FILE *file;
	int i;
	fopen_s(&file, filename, "w+");
	for (i = 0; i < len; i++) {
		fprintf(file, "%f", data[i].x);
		fprintf(file, "%s", ",");
		fprintf(file, "%f", data[i].y);
		fprintf(file, "%s", "\n");
	}
	fclose(file);
}


static int is_full(POINT *xy,int len)
{
	int i;
	POINT min, max;
	variance_analysis(xy, len);
	min.x = xy[0].x;
	min.y = xy[0].y;
	max.x = xy[0].x;
	max.y = xy[0].y;
	int k = len > 75 ? 75 : len;
	int j = len > 75 ? 15 : 0;
	for (i = j; i < k; i++) {
		if (xy[i].x>max.x)max.x = xy[i].x;
		if (xy[i].y>max.y)max.y = xy[i].y;

		if (xy[i].x < min.x)min.x = xy[i].x;
		if (xy[i].y < min.y)min.y = xy[i].y;
	}

	printf("wzb minx=%f,maxx=%f,miny=%f,maxy=%f\n", min.x, max.x, min.y, max.y);
	if (max.x - min.x>23 || max.y - min.y>23) {
		printf("##########full#########\n");
		return 1;
	}
	printf("##########half#########\n");
	return 0;
}

void data_analysis_v2(POINT *xy, int len, POINT *pxy) 
{
	int full = 0;
	if (len > 100) {
		full = is_full(xy, 100);
		if (full == 0) {
			width = 15;
			height = 14;
		}
		else {
			width = 15;
			height = 28;
		}

		int i;
		for (i = 0; i < len / 100; i++) {
			printf("i=%d\n", i);
			if (len - i * 100 > 104) {
				data_conver(xy + i * 100, pxy + i * 100, 100);
				//is_full(xy + i * 100, 100, 1);

			}
			else {
				data_conver(xy + i * 100, pxy + i * 100, 100+len%100);
				//is_full(xy + i * 100, 100, 1);
			}
		}
		if (len % 100 > 4) {
			data_conver(xy + len - len % 100, pxy + len - len % 100, len % 100);
		}
	}
	else {
		full = is_full(xy, len);
		if (full == 0) {
			width = 15;
			height = 14;
		}
		else {
			width = 15;
			height = 28;
		}
		data_conver(xy, pxy, len);
	}

	
}

void data_analysis_v3(POINT *xy, int len, POINT *pxy,int full)
{
	if (full == 0) {
		width = 15;
		height = 14;
	}
	else {
		width = 15;
		height = 28;
	}
	if (len > 100) {
		int i;
		for (i = 0; i < len / 100; i++) {
			printf("i=%d\n", i);
			if (len - i * 100 > 104) {
				data_conver_v2(xy + i * 100, pxy + i * 100, 100);
				//is_full(xy + i * 100, 100, 1);

			}
			else {
				data_conver_v2(xy + i * 100, pxy + i * 100, 100 + len % 100);
				//is_full(xy + i * 100, 100, 1);
			}
		}
		if (len % 100 > 4) {
			data_conver_v2(xy + len - len % 100, pxy + len - len % 100, len % 100);
		}
	}
	else {
		data_conver_v2(xy, pxy, len);
	}


}

static void data_conver_v2(POINT *xy,POINT *pxy,int len)
{
	variance_analysis(xy, len);
	if (len == 5) {
		for (int i = 0; i < len; i++)
			printf("x:%f,y:%f\n", xy[i].x, xy[i].y);
	}
	POINT b[4];
	memset(b, 0, sizeof(b));
	POINT *xy_temp;
	xy_temp = (POINT*)malloc(len*sizeof(POINT));
	memset(xy_temp, 0, len * sizeof(POINT));
	memcpy(xy_temp, xy, sizeof(POINT)*len);
	rotatingcalipers(xy_temp, len, b);
	free(xy_temp);
	xy_temp = NULL;
	int i;
	for (i = 0; i < 4; i++) {
		printf("b:[%f, %f] ", b[i].x, b[i].y);
	}
	POINT max_y, min_x, min_y,max_x;
	max_y.y = b[0].y;
	max_y.x = b[0].x;
	max_x.y = b[0].y;
	max_x.x = b[0].x;
	min_x.x = b[0].x;
	min_x.y = b[0].y;
	min_y.y = b[0].y;
	min_y.x = b[0].x;
	for (i = 0; i < 4; i++) {
		if (max_y.y < b[i].y) {
			max_y.y = b[i].y;
			max_y.x = b[i].x;
		}
		if (max_y.y == b[i].y && max_y.x>b[i].x) {
			max_y.y = b[i].y;
			max_y.x = b[i].x;
		}

		if (max_x.x < b[i].x) {
			max_x.x = b[i].x;
			max_x.y = b[i].y;
		}
		if (max_x.x == b[i].x && max_x.y < b[i].y) {
			max_x.x = b[i].x;
			max_x.y = b[i].y;
		}

		if (min_x.x > b[i].x) {
			min_x.x = b[i].x;
			min_x.y = b[i].y;
		}
		if (min_x.x == b[i].x && min_x.y>b[i].y) {
			min_x.x = b[i].x;
			min_x.y = b[i].y;
		}

		if (min_y.y >b[i].y) {
			min_y.y = b[i].y;
			min_y.x = b[i].x;
		}

		if (min_y.y == b[i].y && min_y.x < b[i].x) {
			min_y.y = b[i].y;
			min_y.x = b[i].x;
		}

	}

	float sin_yaw, cos_yaw;
	if (min_x.y == min_y.y) {
		sin_yaw = 0;
		cos_yaw = 1;
	}
	else {
		sin_yaw = (max_y.x - min_x.x) / getdist(min_x, max_y);
		cos_yaw = (max_y.y - min_x.y) / getdist(min_x, max_y);
	}


	for (i = 0; i < len; i++) {
		pxy[i].x = xy[i].x*cos_yaw - xy[i].y*sin_yaw;
		pxy[i].y = xy[i].y*cos_yaw + xy[i].x*sin_yaw;

	}

	POINT p_max_y, p_min_x, p_min_y,p_max_x;
	p_max_y.x = max_y.x*cos_yaw - max_y.y*sin_yaw;
	p_max_y.y = max_y.y*cos_yaw + max_y.x*sin_yaw;

	p_max_x.x = max_x.x*cos_yaw - max_x.y*sin_yaw;
	p_max_x.y = max_x.y*cos_yaw + max_x.x*sin_yaw;

	p_min_x.x = min_x.x*cos_yaw - min_x.y*sin_yaw;
	p_min_x.y = min_x.y*cos_yaw + min_x.x*sin_yaw;


	p_min_y.x = min_y.x*cos_yaw - min_y.y*sin_yaw;
	p_min_y.y = min_y.y*cos_yaw + min_y.x*sin_yaw;

	POINT zero;
	zero.x = 0;
	zero.y = 0;
	float s_p_min_x = getdist(p_min_x, zero);
	float s_p_min_y = getdist(p_min_y, zero);
	float s_p_max_x = getdist(p_max_x, zero);
	float s_p_max_y = getdist(p_max_y, zero);

	POINT new_p_max_y, new_p_min_x, new_p_min_y, new_p_max_x;
	if (s_p_min_y<s_p_min_x && s_p_min_y <= s_p_max_x && s_p_min_y <= s_p_max_y) {
		for (i = 0; i < len; i++) {
			pxy[i].x = 2 * p_min_y.x - pxy[i].x;
		}
		new_p_min_x = p_min_y;
		new_p_max_y = p_max_x;
		new_p_min_y.y = p_min_x.y;
		new_p_min_y.x = 2 * p_min_y.x - p_min_x.x;
	}
	else if (s_p_max_x<s_p_min_x && s_p_max_x <= s_p_min_y && s_p_max_x <= s_p_max_y) {
		for (i = 0; i < len; i++) {
			pxy[i].x = 2 * p_min_y.x - pxy[i].x;
			pxy[i].y = 2 * p_max_x.y - pxy[i].y;
		}
		new_p_min_x = p_max_x;
		new_p_max_y.x = p_min_y.y;
		new_p_max_y.y = 2 * p_max_x.y - p_min_y.y;
		new_p_min_y.y = p_max_x.y;
		new_p_min_y.x = 2 * p_max_x.x - p_max_y.x;
	}
	else if (s_p_max_y<s_p_min_x && s_p_max_y <= s_p_min_y && s_p_max_y <= s_p_max_x) {
		for (i = 0; i<len; i++) {
			pxy[i].y = 2 * p_max_y.y - pxy[i].y;
		}
		new_p_min_x = p_max_y;
		new_p_min_y = p_max_x;
		new_p_max_y.x = p_max_y.x;
		new_p_max_y.y = 2 * p_max_y.y - p_min_x.y;
	}
	else {
		new_p_max_y = p_max_y;
		new_p_min_x = p_min_x;
		new_p_min_y = p_min_y;
	}

	if ((new_p_max_y.y - new_p_min_x.y) > (new_p_min_y.x - new_p_min_x.x) && (new_p_max_y.y - new_p_min_x.y)>15) {
		for (i = 0; i < len; i++) {


			//if (p_min_x.x < 0)pxy[i].x -= p_min_x.x - 0.1f;
			//if (p_min_x.y < 0)pxy[i].y -= p_min_x.y - 0.1f;
			pxy[i].x -= new_p_min_x.x - 0.3f;
			pxy[i].y -= new_p_min_x.y - 0.3f;
			float temp;
			temp = pxy[i].x;
			pxy[i].x = pxy[i].y;
			pxy[i].y = temp;

			if (new_p_max_y.y - new_p_min_x.y>height) {
				pxy[i].x = pxy[i].x*height / (new_p_max_y.y - new_p_min_x.y);
			}

			if (new_p_min_y.x - new_p_min_x.x>width) {
				pxy[i].y = pxy[i].y*width / (new_p_min_y.x - new_p_min_x.x);
			}

		}
	}
	else {
		printf("1111\n");
		for (i = 0; i < len; i++) {
			//if (p_min_x.x < 0)pxy[i].x -= p_min_x.x - 0.1f;
			//if (p_min_x.y < 0)pxy[i].y -= p_min_x.y - 0.1f;
			pxy[i].x -= new_p_min_x.x - 0.3f;
			pxy[i].y -= new_p_min_x.y - 0.3f;
			if (new_p_min_y.x - new_p_min_x.x>height) {
				pxy[i].x = pxy[i].x*height / (new_p_min_y.x - new_p_min_x.x);
			}

			if (new_p_max_y.y - new_p_min_x.y>width) {
				pxy[i].y = pxy[i].y*width / (new_p_max_y.y - new_p_min_x.y);
			}
		}
	}

}

static void data_conver(POINT *xy, POINT *pxy, int len)
{
	variance_analysis(xy, len);
	POINT b[4];
	memset(b, 0, sizeof(b));

	POINT *xy_temp;
	xy_temp = (POINT*)malloc(len*sizeof(POINT));
	memset(xy_temp, 0, len * sizeof(POINT));
	memcpy(xy_temp,xy,sizeof(POINT)*len);
	rotatingcalipers(xy_temp, len, b);
	free(xy_temp);
	xy_temp = NULL;
	int i;
	for (i = 0; i < 4; i++) {
		printf("b:[%f, %f] ", b[i].x, b[i].y);
	}
	POINT max_y, min_x,min_y;
	max_y.y = b[0].y;
	max_y.x = b[0].x;
	min_x.x = b[0].x;
	min_x.y = b[0].y;
	min_y.y = b[0].y;
	min_y.x = b[0].x;
	for (i = 0; i < 4; i++) {
		if (max_y.y < b[i].y) {
			max_y.y = b[i].y;
			max_y.x = b[i].x;
		}

		if (min_x.x > b[i].x) {
			min_x.x = b[i].x;
			min_x.y = b[i].y;
		}
		if (min_y.y >= b[i].y) {
			min_y.y = b[i].y;
			min_y.x = b[i].x;
		}

	}

	float sin_yaw, cos_yaw;
	if (min_x.y == min_y.y) {
		sin_yaw = 0;
		cos_yaw = 1;
	}
	else {
		sin_yaw = (max_y.x - min_x.x) / getdist(min_x, max_y);
		cos_yaw = (max_y.y - min_x.y) / getdist(min_x, max_y);
	}


	for (i = 0; i < len; i++) {
		pxy[i].x = xy[i].x*cos_yaw - xy[i].y*sin_yaw;
		pxy[i].y = xy[i].y*cos_yaw + xy[i].x*sin_yaw;

	}

	POINT p_max_y, p_min_x, p_min_y;
	p_max_y.x = max_y.x*cos_yaw - max_y.y*sin_yaw;
	p_max_y.y= max_y.y*cos_yaw + max_y.x*sin_yaw;

	p_min_x.x = min_x.x*cos_yaw - min_x.y*sin_yaw;
	p_min_x.y = min_x.y*cos_yaw + min_x.x*sin_yaw;

	p_min_y.x = min_y.x*cos_yaw - min_y.y*sin_yaw;
	p_min_y.y = min_y.y*cos_yaw + min_y.x*sin_yaw;

	if ((p_max_y.y - p_min_x.y) > (p_min_y.x - p_min_x.x)) {
		for (i = 0; i < len; i++) {


			//if (p_min_x.x < 0)pxy[i].x -= p_min_x.x - 0.1f;
			//if (p_min_x.y < 0)pxy[i].y -= p_min_x.y - 0.1f;
			pxy[i].x -= p_min_x.x - 0.3f;
			pxy[i].y -= p_min_x.y - 0.3f;
			float temp;
			temp = pxy[i].x;
			pxy[i].x = pxy[i].y;
			pxy[i].y = temp;

			if (p_max_y.y - p_min_x.y>height) {
				pxy[i].x = pxy[i].x*height / (p_max_y.y - p_min_x.y);
			}

			if (p_min_y.x - p_min_x.x>width) {
				pxy[i].y = pxy[i].y*width / (p_min_y.x - p_min_x.x);
			}

		}
	}
	else {
		printf("1111\n");
		for (i = 0; i < len; i++) {
			//if (p_min_x.x < 0)pxy[i].x -= p_min_x.x - 0.1f;
			//if (p_min_x.y < 0)pxy[i].y -= p_min_x.y - 0.1f;
			pxy[i].x -= p_min_x.x - 0.3f;
			pxy[i].y -= p_min_x.y - 0.3f;
			if (p_min_y.x - p_min_x.x>height) {
				pxy[i].x = pxy[i].x*height / (p_min_y.x - p_min_x.x);
			}

			if (p_max_y.y - p_min_x.y>width) {
				pxy[i].y = pxy[i].y*width / (p_max_y.y - p_min_x.y);
			}
		}
	}

}

void 
data_analysis(POINT *xy, int len, POINT *pxy)
{
	int i;
	
	variance_analysis(xy, len);
	for (i = 0; i < len; i++) {
		printf("var ana: %f,%f\n", xy[i].x, xy[i].y);
	}
	save_file("newxy1.txt",xy,len);

	POINT b[4];
	memset(b, 0, sizeof(b));

	POINT *xy_temp;
	xy_temp = (POINT*)malloc(len*sizeof(POINT));
	memset(xy_temp, 0, len * sizeof(POINT));
	memcpy(xy_temp,xy,sizeof(POINT)*len);
	rotatingcalipers(xy_temp, len, b);
	free(xy_temp);
	xy_temp = NULL;
	for (i = 0; i < 4; i++) {
		printf("b:[%f, %f] ", b[i].x, b[i].y);
	}
	POINT max_y, min_x,min_y;
	max_y.y = b[0].y;
	max_y.x = b[0].x;
	min_x.x = b[0].x;
	min_x.y = b[0].y;
	min_y.y = b[0].y;
	min_y.x = b[0].x;
	for (i = 0; i < 4; i++) {
		if (max_y.y < b[i].y) {
			max_y.y = b[i].y;
			max_y.x = b[i].x;
		}

		if (min_x.x > b[i].x) {
			min_x.x = b[i].x;
			min_x.y = b[i].y;
		}
		if (min_y.y >= b[i].y) {
			min_y.y = b[i].y;
			min_y.x = b[i].x;
		}

	}

	float sin_yaw, cos_yaw;
	if (min_x.y == min_y.y) {
		sin_yaw = 0;
		cos_yaw = 1;
	}
	else {
		sin_yaw = (max_y.x - min_x.x) / getdist(min_x, max_y);
		cos_yaw = (max_y.y - min_x.y) / getdist(min_x, max_y);
	}


	for (i = 0; i < len; i++) {
		pxy[i].x = xy[i].x*cos_yaw - xy[i].y*sin_yaw;
		pxy[i].y = xy[i].y*cos_yaw + xy[i].x*sin_yaw;

	}

	POINT p_max_y, p_min_x, p_min_y;
	p_max_y.x = max_y.x*cos_yaw - max_y.y*sin_yaw;
	p_max_y.y= max_y.y*cos_yaw + max_y.x*sin_yaw;

	p_min_x.x = min_x.x*cos_yaw - min_x.y*sin_yaw;
	p_min_x.y = min_x.y*cos_yaw + min_x.x*sin_yaw;


	p_min_y.x = min_y.x*cos_yaw - min_y.y*sin_yaw;
	p_min_y.y = min_y.y*cos_yaw + min_y.x*sin_yaw;

	if ((p_max_y.y - p_min_x.y) > (p_min_y.x - p_min_x.x)) {
		for (i = 0; i < len; i++) {


			//if (p_min_x.x < 0)pxy[i].x -= p_min_x.x - 0.1f;
			//if (p_min_x.y < 0)pxy[i].y -= p_min_x.y - 0.1f;
			pxy[i].x -= p_min_x.x - 0.3f;
			pxy[i].y -= p_min_x.y - 0.3f;

			float temp;
			temp = pxy[i].x;
			pxy[i].x = pxy[i].y;
			pxy[i].y = temp;

		}
	}
	else {
		printf("1111\n");
		for (i = 0; i < len; i++) {
			//if (p_min_x.x < 0)pxy[i].x -= p_min_x.x-0.1f;
			//if (p_min_x.y < 0)pxy[i].y -= p_min_x.y-0.1f;
			pxy[i].x -= p_min_x.x - 0.3f;
			pxy[i].y -= p_min_x.y - 0.3f;
		}
	}

}

/*int
main(int argc, char *argv[])
{
	int n, i, length;
	POINT a[] = { { 1.0, 2.0 },{ 2.0, 0.5 },{ 2.5, 1.0 },{ 2.0, 0.0 },{ 4.0, 2.0 } };
	POINT b[4];
	length = sizeof(a) / sizeof(POINT);
	rotatingcalipers(a, length, b);
	for (i = 0; i<4; i++) {
		printf("[%f, %f] ", b[i].x, b[i].y);
	}
	printf("\n");
}*/