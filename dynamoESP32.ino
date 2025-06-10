#include <Arduino.h>
#include <Ticker.h>
#include "timer_sw.h"
#include "super_timer_sw.h"
#include "RTC_soft.h"
#include "trace_debug.h"
#include "convertAnalogValue.h"

Ticker g_t_blinker;
TimerEvent_t TimerTOP;
ConvertAnalogValue ConvertVoltage(0, 0, 24.0, 0.0, 0, 1024);
ConvertAnalogValue Convertcurrent(512, 10, 10.0, -10.0, 0, 1024);


void setup()
{
	// initialisation du Timer matériel pour le module TimerSW
	g_t_blinker.attach(0.05, Inc_Timer);

	Init_Trace_Debug();

	SEND_VTRACE(INFO, "Démarrage Vélo Dynamo");

	pinMode(BUILTIN_LED, OUTPUT);

	TimerTOP.Init(nullptr, 5000, true);
	TimerTOP.Start();
}

// The loop function is called in an endless loop
void loop()
{
	static uint8_t toto = 0;
	SEND_VTRACE(DBG1, "Test pour François");
	digitalWrite(BUILTIN_LED, 1);
	delay(1000);
	digitalWrite(BUILTIN_LED, 0);
	delay(1000);

	SEND_VTRACE(INFO, "Coucou: %d", toto);

	toto++;

	if(TimerTOP.IsTop() == true)
	{
		SEND_VTRACE(INFO, "Top Mesure !");
	}
}
