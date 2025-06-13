#include <Arduino.h>
#include <Ticker.h>
#include "timer_sw.h"
#include "super_timer_sw.h"
#include "RTC_soft.h"
#include "trace_debug.h"
#include "convertAnalogValue.h"
#include "LedBlinkingManagement.h"
#include "esp32WS2811.h"


Ticker g_t_blinker;

TimerEvent_t g_t_TimerMesures;

ConvertAnalogValue ConvertVoltage(0, 0, 0.0, 24.0, 0, 944);
ConvertAnalogValue Convertcurrent(512, 10, -10.0, 10.0, 0, 1024);

GestionLed g_t_GestionBuiltinLed(BUILTIN_LED);

WS2811 bandeauLED(2,30);

#define INTERVALLE_MESURE_PUISSANCE_MS		500

void FonctionMesures(uint32_t p_u32_param, void * p_pv_param);

void setup()
{
	// initialisation du Timer matériel pour le module TimerSW
	g_t_blinker.attach(0.05, Inc_Timer);

	Init_Trace_Debug();

	SEND_VTRACE(INFO, "Démarrage Vélo Dynamo");

	g_t_GestionBuiltinLed.SetSequence3();

	g_t_TimerMesures.Init(FonctionMesures, INTERVALLE_MESURE_PUISSANCE_MS, true);
	g_t_TimerMesures.Start();
}

// The loop function is called in an endless loop
void loop()
{
	static uint8_t toto = 0;

	uint32_t l_u32_valeurTest;

	delay(1000);

	SEND_VTRACE(DBG1, "Test ConvertAnalogValue ConvertVoltage");

}

void FonctionMesures(uint32_t p_u32_param, void * p_pv_param)
{
	uint32_t l_u32_LectureTension = 0;
	uint32_t l_u32_LectureIntensite = 0;
	double l_dble_ValeurTension = 0.0;
	double l_dble_ValeurIntensite = 0.0;
	double l_dble_ValeurPuissance = 0.0;
	double l_dble_ValeurEnergie = 0.0;

	uint32_t l_u32_mesureTempsTraitement = 0;

	l_u32_mesureTempsTraitement = millis();

	// !!!!!!!!! Numéro de patte à définir !!!!!!!!!
	l_u32_LectureTension = analogRead(0);
	l_u32_LectureTension = analogRead(0);

	l_dble_ValeurTension = ConvertVoltage.GetConvertedValue(l_u32_LectureTension);
	l_dble_ValeurIntensite = Convertcurrent.GetConvertedValue(l_u32_LectureIntensite);

	l_dble_ValeurPuissance = l_dble_ValeurTension * l_dble_ValeurIntensite;
	l_dble_ValeurEnergie = l_dble_ValeurPuissance * ((double)INTERVALLE_MESURE_PUISSANCE_MS)/1000.0;

	l_u32_mesureTempsTraitement = millis() - l_u32_mesureTempsTraitement;
	SEND_VTRACE(DBG1, "Temps: %d", l_u32_mesureTempsTraitement);
}
