#include "stdio.h"
#include "asa.h"
#include <math.h>
int main(int const argc, const char ** const argv) 
{
	settings_t S;
	int status;
	char* g;
	S.C1 = 1.0;
	S.C2 = 1.0;
	S.C3 = 1.0;
	S.R1 = 1.0;
	S.R2 = 1.0;
	S.R3 = 1.0;
	S.R_C1 = 1.0;
	S.R_C2 = 1.0;
	S.R_C3 = 1.0;
	S.D1 = 1.0;
	S.D3 = 1.0;
	S.R_D1 = 1.0;
	S.R_D3 = 1.0;
	char name[20] = "ivc";
    printf("-- Get measured IV curve --\n");
	SetMinVC(0, 0);
	status = CreateIvc(name, &S);
	printf("status %d", status);
	g = SimulateIvc(name);
	//g = _PyLong_AsInt(g)
	//double score = CompareIvc(&IVC, &IVC, SettingsOut.NumberPoints);
	//printf("Score =  %lf; ", score);
    return 0;
}