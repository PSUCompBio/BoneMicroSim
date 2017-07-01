#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

int main(){
FILE *yieldpoints;
yieldpoints = fopen("YieldPoints_calculated.csv", "r");

char xstrain[500], ystrain[500], zstrain[500];
char xystrain[100], yzstrain[100], zxstrain[100];
char xstress[500], ystress[500], zstress[500];
char xystress[100], yzstress[100], zxstress[100];
double strainx, strainy, strainz, strainxy, strainyz, strainzx;
double stressx, stressy, stressz, stressxy, stressyz, stresszx;

fscanf(yieldpoints, "%s", &xstrain);
fscanf(yieldpoints, "%s", &xstress);
fscanf(yieldpoints, "%s", &ystrain);
fscanf(yieldpoints, "%s", &ystress);
fscanf(yieldpoints, "%s", &zstrain);
fscanf(yieldpoints, "%s", &zstress);

fscanf(yieldpoints, "%s", &xystrain);
fscanf(yieldpoints, "%s", &xystress);
fscanf(yieldpoints, "%s", &yzstrain);
fscanf(yieldpoints, "%s", &yzstress);
fscanf(yieldpoints, "%s", &zxstrain);
fscanf(yieldpoints, "%s", &zxstress);


strainx = atof(xstrain);
strainy = atof(ystrain);
strainz = atof(zstrain);
strainxy = atof(xystrain);
strainyz = atof(yzstrain);
strainzx = atof(zxstrain);
stressx = atof(xstress);
stressy = atof(ystress);
stressz = atof(zstress);
stressxy = atof(xystress);
stressyz = atof(yzstress);
stresszx = atof(zxstress);


FILE *normalstress;
normalstress = fopen("/storage/home/anr5202/scratch/NewYieldSurface/1MICRO/Points/NormalStress.csv", "a");

FILE *shearstress;
shearstress = fopen("/storage/home/anr5202/scratch/NewYieldSurface/1MICRO/Points/ShearStress.csv", "a");

FILE *normalstrains;
normalstrains = fopen("/storage/home/anr5202/scratch/NewYieldSurface/1MICRO/Points/NormalStrain.csv", "a");

FILE *shearstrains;
shearstrains = fopen("/storage/home/anr5202/scratch/NewYieldSurface/1MICRO/Points/ShearStrain.csv", "a");

fprintf(normalstrains, "%e, %e, %e\n", strainx, strainy, strainz);
fprintf(normalstress, "%e, %e, %e\n", stressx, stressy, stressz);

fprintf(shearstrains, "%e %e\n", strainxy, strainyz, strainzx);
fprintf(shearstress, "%e %e\n", stressxy, stressyz, stresszx);

}
