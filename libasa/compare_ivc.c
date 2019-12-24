/* This is module compares two iv-curves and returnes score (0-100 %)
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "asa.h"

/*********************************/
/*        Definitions            */
/*********************************/
#define min(a, b) (((a<b))?(a):(b))
#define max(a, b) (((a>b))?(a):(b))
#define IV_CURVE_NUM_COMPONENTS 2
#define MIN_VAR_V 0.6
#define MIN_VAR_C 0.0002
static double min_var_v, min_var_c;

/*********************************/
/*    Internal functions         */
/*********************************/

/*
* Returns the difference vector of two vectors
*/
static void Subtract(double * a, double * b, double * v, size_t size)
{
	size_t i;
    for (i = 0; i < size; i++)
        v[i] = a[i] - b[i];
}

/*
* Returns the difference vector of vector and variable
*/
static void Subtract_(double * a, double b, double* v, size_t size)
{
	size_t i;
    for (i = 0; i < size; i++)
        v[i] = a[i] - b;
}

/*
* Returns the vector mean
*/
static double Mean(double * mas, size_t size)
{
    double avg = 0;
	size_t i;
    for (i = 0; i < size; i++)
        avg += mas[i];
    return avg / size;
}

/*
* Returns the scalar product of two vectors
*/
static double Dot(double *a, double *b, size_t size)
{
    double s;
    double sum = 0;
	size_t i;
    for (i = 0; i < size; i++)
    {
        s = a[i] * b[i];
        sum += s;
    }
    return sum;
}

/*
* Returns the vector product of two vectors
*/
static double Cross(double *a, double *b)
{
    return a[0] * b[1] - a[1] * b[0];
}

/*
* Returns the dispersion of vector
*/
static double Disp(double * mas, size_t size)
{
    double avg = 0;
    double *disp = (double *)malloc(size * sizeof(double));
    avg = Mean(mas, size);
	size_t i;
    for (i = 0; i < size; i++)
        disp[i] = (mas[i] - avg) * (mas[i] - avg);
    avg = Mean(disp, size);
    free(disp);
    return avg;
}

/*
* Returns the transpoted matrix
*/
static void Transpose(double ** m, double ** m_t, size_t size_i, size_t size_j)
{
	size_t i;
	size_t j;
    for (i = 0; i < size_i; i++){
        for (j = 0; j < size_j; j++)
            m_t[j][i] = m[i][j];
    }
}

/*
* Returns the distance between 3 points
*/
static double Dist2PtSeg(double * p, double * a, double * b, size_t size)
{
    double *v1 = (double *)malloc(size * sizeof(double));
    double *v2 = (double *)malloc(size * sizeof(double));
    double result;
    Subtract(b, a, v1, size);
    Subtract(p, a, v2, size);
    double seg_len2 = Dot(v1, v1, size);
    double proj = Dot(v1, v2, size) / seg_len2;
    if (proj > 1)
    {
        Subtract(p, b, v1, size);
        result = Dot(v1, v1, size);
    }
    else if (proj < 0)
        result = Dot(v2, v2, size);
    else
        result = pow(Cross(v1, v2), 2) / seg_len2;
    free(v1); free(v2);
    return result;
}

/*
* Updates score value
*/
static double RescaleScore(double x)
{
    return 1 - exp(-8 * x);
}

/*
* Returns all distances of two iv_curves
*/
static double DistCurvePts(double ** curve, double ** pts, size_t size_j)
{
    double res = 0.0;
    size_t min_i; double min_;
    double *v = (double *)malloc(size_j * sizeof(double));
    double *prev_node = NULL;
    double *cur_node = NULL;
    double *next_node = NULL;
    double *pt = NULL;
    double dist1, dist2;

    double ** curve_t = (double **)malloc(size_j * sizeof(double *));
    *curve_t = (double *)malloc(size_j * IV_CURVE_NUM_COMPONENTS * sizeof(double));
	size_t i;
    for (i = 1; i < size_j; i++)
        curve_t[i] = *curve_t + i*IV_CURVE_NUM_COMPONENTS;
    
    double **pts_t = (double **)malloc(size_j * sizeof(double *));
    *pts_t = (double *)malloc(size_j * IV_CURVE_NUM_COMPONENTS * sizeof(double));
    for (i = 1; i < size_j; i++)
        pts_t[i] = *pts_t + i*IV_CURVE_NUM_COMPONENTS;

    Transpose(pts, pts_t, IV_CURVE_NUM_COMPONENTS, size_j);
	size_t j;
    for (j = 0; j < size_j; j++)
    {
        min_ = 100000;
        pt = pts_t[j];
        for (i = 0; i < size_j; i++)
        {
            v[i] = (curve[0][i] - pt[0]) * (curve[0][i] - pt[0]) + (curve[1][i] - pt[1]) * (curve[1][i] - pt[1]);
            if (v[i] < min_)
            {
                min_i = i;
                min_ = v[i];
            }
        }
        Transpose(curve, curve_t, IV_CURVE_NUM_COMPONENTS, size_j);
        prev_node = curve_t[min_i - 1];
        cur_node = curve_t[min_i];
        next_node = curve_t[min_i + 1];
        
        if (min_i > 0)
            dist1 = Dist2PtSeg(pt, prev_node, cur_node, IV_CURVE_NUM_COMPONENTS);
        else
            dist1 = INFINITY;

        if (min_i < size_j - 1)
            dist2 = Dist2PtSeg(pt, cur_node, next_node, IV_CURVE_NUM_COMPONENTS);
        else
            dist2 = INFINITY;
        res += min(dist1, dist2);
    }
    res /= size_j;
    free(*curve_t); free(*pts_t);
    free(curve_t); free(pts_t);
    free(v);
    return res;
}

/*
* Removes repeates data in curve
*/
static void RemoveRepeatsIvc(double ** a, double eps, size_t size_j)
{
    int *rep_ind = (int *)malloc(size_j * sizeof(int)); 
    int kol = 0;
    int ii = 0;
	size_t j;
	size_t k;
    for (j = 0; j < size_j; j++)
    {
        for (k = j; k < size_j; k++)
        {
            if (a[0][k] == a[0][j] && a[1][k] == a[1][j] && k != j)
            {
                rep_ind[j] = 1;
                kol++;
            }
            else
                rep_ind[j] = 0;
        }
    }
    for (j = 0; j < size_j; j++)
    {
        if (rep_ind[j] != 1)
        {
            a[0][ii] = a[0][j];
            a[1][ii] = a[1][j];
            ii++;
        }
    }
    free(rep_ind);
}

/*
* Subroutine to generate a B-spline open knot vector with multiplicity
    equal to the order at the ends.
    
    c            = order of the basis function
    n            = the number of defining polygon vertices
    nplus2       = index of x() for the first occurence of the maximum knot vector value
    nplusc       = maximum value of the knot vector -- $n + c$
    x()          = array containing the knot vector
*/
static void Knot(size_t n, int c, double *x)
{
    int nplusc, nplus2, i;
    nplusc = n + c;
    nplus2 = n + 2;
    x[1] = 0;
    for (i = 2; i <= nplusc; i++)
    {
        if ((i > c) && (i < nplus2))
            x[i] = x[i - 1] + 1;
        else
            x[i] = x[i - 1];
    }
}

/*
* Subroutine to generate B-spline basis functions for open knot vectors
c        = order of the B-spline basis function
d        = first term of the basis function recursion relation
e        = second term of the basis function recursion relation
npts     = number of defining polygon vertices
n[]      = array containing the basis functions
n[1] contains the basis function associated with B1 etc.
nplusc   = constant -- npts + c -- maximum number of knot values
t        = parameter value
temp[]   = temporary array
x[]      = knot vector
*/
static void Basis(size_t c, double t, size_t npts, double *x, double *n)
{
    size_t nplusc;
    size_t k; size_t i;
    double d, e;
    double *temp = (double *)malloc(4 * npts * sizeof(double));
    nplusc = npts + c;
    for (i = 1; i <= nplusc-1; i++)
    {
        if ((t >= x[i]) && (t < x[i + 1]))
            temp[i] = 1.000;
        else
            temp[i] = 0;
    }
    for (k = 2; k <= c; k++)
    {
        for (i = 1; i <= nplusc - k; i++)
        {
            if (temp[i] != 0)
                d = ((t - x[i])*temp[i]) / (x[i + k - 1] - x[i]);
            else
                d = 0;

            if (temp[i + 1] != 0)
                e = ((x[i + k] - t)*temp[i + 1]) / (x[i + k] - x[i + 1]);
            else
                e = 0;
            temp[i] = d + e;
        }
    }
    if (t == x[nplusc])
        temp[npts] = 1;

    for (i = 0; i <= npts; i++)
        n[i] = temp[i];
    free(temp);
}

/*  
* Subroutine to generate a B-spline curve using an uniform open knot vector
b[]        = array containing the defining polygon vertices
b[1] contains the x-component of the vertex
b[2] contains the y-component of the vertex
b[3] contains the z-component of the vertex
k           = order of the \bsp basis function
nbasis      = array containing the basis functions for a single value of t
nplusc      = number of knot values
npts        = number of defining polygon vertices
p[,]        = array containing the curve points
p[1] contains the x-component of the point
p[2] contains the y-component of the point
p[3] contains the z-component of the point
p1          = number of points to be calculated on the curve
t           = parameter value 0 <= t <= 1
x[]         = array containing the knot vector
*/
static void Bspline(size_t npts, size_t k, size_t p1, double *b, double *p)
{
    size_t i, j, icount, jcount;
    size_t i1;
    double *x = (double *)malloc(IV_CURVE_NUM_COMPONENTS * npts * sizeof(double));
    size_t nplusc;
    double step;
    double t;
    double *nbasis = (double *)malloc(IV_CURVE_NUM_COMPONENTS * npts * sizeof(double));
    double temp;
    nplusc = npts + k;

    for (i = 1; i <= npts; i++)
        nbasis[i] = 0.;

    for (i = 1; i <= nplusc; i++)
        x[i] = 0.;

    Knot(npts, k, x);

    icount = 0;

    t = 0;
    step = (x[nplusc]) / (p1 - 1);
    for (i1 = 1; i1 <= p1; i1++)
    {
        if ((double)x[nplusc] - t < 5e-16)
            t = (double)x[nplusc];
        Basis(k, t, npts, x, nbasis);
        for (j = 1; j <= 2; j++)
        {
            jcount = j;
            p[icount + j] = 0.;
            for (i = 1; i <= npts; i++)
            {
                temp = nbasis[i] * b[jcount];
                p[icount + j] = p[icount + j] + temp;
                jcount = jcount + 2;
            }
        }
        icount = icount + 2;
        t = t + step;
    }
    free(x);
    free(nbasis);
}

/*********************************/
/*      Public functions         */
/*********************************/

void SetMinVC(double new_min_v, double new_min_c)
{
    if (new_min_v > 0 && new_min_c > 0)
    {
        min_var_v = new_min_v;
        min_var_c = new_min_c;
    }
    else
    {
        min_var_v = MIN_VAR_V;
        min_var_c = MIN_VAR_C;
    }
}

double CompareIvc(iv_curve_t *a, iv_curve_t *b, size_t size)
{
    
    double* a_[IV_CURVE_NUM_COMPONENTS];
    double* b_[IV_CURVE_NUM_COMPONENTS];

    if (!a->Voltages | !a->Currents)
        return 0;
    a_[0] = a->Voltages;
    a_[1] = a->Currents;
    if (b->Voltages)
    {
        b_[0] = b->Voltages;
        b_[1] = b->Currents;
    }
    double var_v, var_c;
    double score;
    double _v = max(sqrt(Disp(a_[0], size)), sqrt(Disp(b_[0], size)));
    double _c = max(sqrt(Disp(a_[1], size)), sqrt(Disp(b_[1], size)));
    var_v = max(_v, min_var_v);
    var_c = max(_c, min_var_c);
    Subtract_(a_[0], Mean(a_[0], size), a_[0], size);
    Subtract_(a_[1], Mean(a_[1], size), a_[1], size);
	size_t i;
    for (i = 0; i < size; i++)
    { 
        a_[0][i] = a_[0][i] / var_v;
        a_[1][i] = a_[1][i] / var_c;

    }

    RemoveRepeatsIvc(a_, 0.002, size);

    double *in_curve = (double *)malloc((size * IV_CURVE_NUM_COMPONENTS + 1) * sizeof(double));
    double *out_curve = (double *)malloc((size * IV_CURVE_NUM_COMPONENTS + 1) * sizeof(double));
    for (i = 0; i < size; i++)
    {
        in_curve[i * IV_CURVE_NUM_COMPONENTS + 1] = a_[0][i];
        in_curve[i * IV_CURVE_NUM_COMPONENTS + 2] = a_[1][i];

    }
    for (i = 1; i <= IV_CURVE_NUM_COMPONENTS * size; i++)
        out_curve[i] = 0.;
    Bspline(size, 2, size, in_curve, out_curve);
    for (i = 0; i < size; i++)
    {
        a_[0][i] = out_curve[i * IV_CURVE_NUM_COMPONENTS + 1];
        a_[1][i] = out_curve[i * IV_CURVE_NUM_COMPONENTS + 2];
    }
    if (!b->Voltages)
    {
        double x = Mean(a_[1], size);
        score = RescaleScore(x*x);
    }
    else
    {
        Subtract_(b_[0], Mean(b_[0], size), b_[0], size);
        Subtract_(b_[1], Mean(b_[1], size), b_[1], size);
        for (i = 0; i < size; i++)
        {
            b_[0][i] = b_[0][i] / var_v;
            b_[1][i] = b_[1][i] / var_c;
        }

        RemoveRepeatsIvc(b_, 0.002, size);

        for (i = 0; i < size; i++)
        {
            in_curve[i * IV_CURVE_NUM_COMPONENTS + 1] = b_[0][i];
            in_curve[i * IV_CURVE_NUM_COMPONENTS + 2] = b_[1][i];
        }

        for (i = 1; i <= IV_CURVE_NUM_COMPONENTS * size; i++)
        {
            out_curve[i] = 0.;
        }

        Bspline(size, 2, size, in_curve, out_curve);

        for (i = 0; i < size; i++)
        {
            b_[0][i] = out_curve[i * IV_CURVE_NUM_COMPONENTS + 1];
            b_[1][i] = out_curve[i * IV_CURVE_NUM_COMPONENTS + 2];
        }

        score = RescaleScore((DistCurvePts(a_, b_, size) + DistCurvePts(b_, a_, size)) / 2.);
    }
    free(in_curve);
    free(out_curve);
    return score;
}

//int main() {
  //  return 0;
//}
