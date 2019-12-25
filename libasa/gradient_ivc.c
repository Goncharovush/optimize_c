#include "asa.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define min(a, b) (((a<b))?(a):(b))
#define max(a, b) (((a>b))?(a):(b))

char* concat(char* s1, char* s2) {

    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);

    char* result = malloc(len1 + len2 + 1);

    if (!result) {
        printf("malloc() failed: insufficient memory!\n");
        return 0;
    }

    memcpy(result, s1, len1);
    memcpy(result + len1, s2, len2 + 1);

    return result;
}

settings_t arr_to_settings(long double* arr)
{
    settings_t t;
    t.C1 = arr[0];
    t.C2 = arr[1];
    t.C3 = arr[2];
    t.D1 = arr[3];
    t.D3 = arr[4];
    t.R1 = arr[5];
    t.R2 = arr[6];
    t.R3 = arr[7];
    t.R_C1 = arr[8];
    t.R_C2 = arr[9];
    t.R_C3 = arr[10];
    t.R_D1 = arr[11];
    t.R_D3 = arr[12];
    return t;
}

double score_func(long double* ss, char *name, iv_curve_t *Ivc_origin)
{
    settings_t S = arr_to_settings(ss);
    //char* _name = concat(name, ".cir");
	int status = CreateIvc(name, &S);
    printf("name: %f\n", Ivc_origin->Currents[0]);
    iv_curve_t Ivc;
    Ivc = SimulateIvc(name);
    printf("kk");
    SetMinVC(0, 0);
    double score = CompareIvc(&Ivc, Ivc_origin, 3);
    return score;
}


long double* deriv_func(long double *S, char *name, iv_curve_t *Ivc_origin)
{
    long double d_s[13];
    long double* S_left = S;
    long double* S_right = S;
    for (int i = 0; i < 13; i++) {
        S_left[i] *= (1 - 1e-7);
        S_right[i] *= (1 + 1e-7);
        d_s[i] = (score_func(S_right, name, Ivc_origin) - score_func(S_left, name, Ivc_origin)) / (2 * 1e-7);
        S_left[i] = S[i];
        S_right[i] = S[i];
        
    }

    return d_s;
}

settings_t GradientMethod(settings_t *x0, char *name, iv_curve_t *Ivc_origin, double err)
{
    int k = 0;
    long double alfa[] = { 1e-7, 1e-7, 1e-7, 1e-7, 1e-7, 1e5, 1e5, 1e5, 1e5, 1e5, 1e5, 1e5, 1e5 };
    long double xk[] = { x0->C1, x0->C2, x0->C3, x0->D1, x0->D3, x0->R1, x0->R2, x0->R3, x0->R_C1, x0->R_C2, x0->R_C3, x0->R_D1, x0->R_D3 };
    while (score_func(xk, name, Ivc_origin) > err)
    {
        k++;
        long double* d_x = deriv_func(xk, name, Ivc_origin);
        for (int i = 0; i < 13; i++) {
            xk[i] -= alfa[i] * d_x[i];
            if (i < 5) {
                xk[i] = max(xk[i], 1e-12);
                xk[i] = min(xk[i], 1e-6);
            }
            else
            {
                xk[i] = max(xk[i], 1e-12);
                xk[i] = min(xk[i], 1e6);
            }
        }
        if (score_func(xk, name, Ivc_origin) < err || k > 10) {
            return arr_to_settings(xk);
        }
            
    }
    return *x0;
}