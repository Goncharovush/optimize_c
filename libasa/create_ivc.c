#include <stdio.h>
#include "asa.h"


int CreateIvc(char *name, settings_t *S)
{
	FILE* file;
	errno_t err;
	err = fopen_s(&file, "3_win.cir", "w");
	fprintf(file, "* cir file corresponding to the equivalent circuit.\n");
	fprintf(file, "* Circuit 1\n");
	fprintf(file, "R1 _net1 Input %f \n", S->R1);
	fprintf(file, "C1 _net0 _net1 %f \n", S->C1);
	fprintf(file, "R_C1 _net0 _net1 %f \n", S->R_C1);
	fprintf(file, "D1 _net0 0 DMOD_D1 AREA=1.0 Temp=26.85\n");
	fprintf(file, "R_D1 0 _net0 %f \n", S->R_D1);
	fprintf(file, "* Circuit 2\n");
	fprintf(file, "R2 _net4 Input %f \n", S->R2);
	fprintf(file, "C4 0 _net4 %f \n", S->C2);
	fprintf(file, "R_C2 0 _net4 %f \n", S->R_C2);
	fprintf(file, "* Circuit 3\n");
	fprintf(file, "R3 _net3 Input %f \n", S->R3);
	fprintf(file, "C3 _net2 _net3 %f \n", S->C3);
	fprintf(file, "R_C3 _net2 _net3 %f \n", S->R_C3);
	fprintf(file, "D3 0 _net2 DMOD_D3 AREA=1.0 Temp=26.85\n");
	fprintf(file, "R_D3 0 _net2 %f \n", S->R_D3);
	fprintf(file, ".MODEL DMOD_D1 D (Is= %f N=1.65 Cj0=4e-12 M=0.333 Vj=0.7 Fc=0.5 Rs=0.0686 "
		"Tt=5.76e-09 Ikf=0 Kf=0 Af=1 Bv=75 Ibv=1e-06 Xti=3 Eg=1.11 Tcv=0 Trs=0 Ttt1=0 Ttt2=0 Tm1=0 "
		"Tm2=0 Tnom=26.85 )\n", S->D1);
	fprintf(file, ".MODEL DMOD_D3 D (Is= %f N=1.65 Cj0=4e-12 M=0.333 Vj=0.7 Fc=0.5 Rs=0.0686 "
		"Tt=5.76e-09 Ikf=0 Kf=0 Af=1 Bv=75 Ibv=1e-06 Xti=3 Eg=1.11 Tcv=0 Trs=0 Ttt1=0 Ttt2=0 Tm1=0 "
		"Tm2=0 Tnom=26.85 )\n", S->D3);
	fprintf(file, ".END\n");
	fclose(file);
	return 0;
}