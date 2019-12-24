/** \file asa.h
 * Библиотека для управления аналоговым сигнатурным анализатором. Позволяет подключиться к серверно-аппаратной части, управлять настройками оборудования и получать сигнатуры.
 */

#ifndef LIBASA_H
#define LIBASA_H
#include "stdint.h"
#if defined(_WIN32) || defined (_WIN64) 
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

/*********************************/
/*        Definitions            */
/*********************************/
#define MAX_NUM_POINTS 10000 /**< Максимально возможное количество точек кривой */

#define COMPONENT_MODEL_TYPE_NONE       0 /**< Неопределённый тип */
#define COMPONENT_MODEL_TYPE_RESISTOR   1 /**< Резистор */
#define COMPONENT_MODEL_TYPE_CAPACITOR  2 /**< Конденсатор */

#define COMPONENT_TRIGGER_AUTO 0 /**< Автоматический */
#define COMPONENT_TRIGGER_MANUAL 1 /**< Ручной */

#define status_t int
#define message_t char

#define ASA_OK 0                    /**< Всё работает штатно */
#define SERVER_RESPONSE_ERROR -1    /**< Некорректный ответ сервера */

/*********************************/
/*        Data types             */
/*********************************/

typedef struct
{
    double Voltages[MAX_NUM_POINTS]; /**< Массив точек по напряжению в В. */
    double Currents[MAX_NUM_POINTS]; /**< Массив точек по току в мА. */
} iv_curve_t;

typedef struct
{
	double R1;
	double R2;
	double R3;
	double C1;
	double C2;
	double C3;
	double R_C1;
	double R_C2;
	double R_C3;
	double R_D1;
	double R_D3;
	double D1;
	double D3;
} settings_t;

/*********************************/
/*         Functions             */
/*********************************/

/** 
 * Функция для получения измеренной сигнатуры.
 * @param[in] min_v Mинимальное напряжение
 * @param[in] min_c Mинимальный ток
 */
EXPORT void SetMinVC(double min_v, double min_c);

/**
 * Функция для сравнения двух кривых. Возвращает коэффициент схожести кривых
 * @param[in] size Kоличество точек кривой, обычно равно параметру NumberPoints
 */
EXPORT double CompareIvc(iv_curve_t *a, iv_curve_t *b, size_t size);

EXPORT int CreateIvc(char* name, settings_t* S);

EXPORT char* SimulateIvc(char* name);


#endif // LIBASA_H
