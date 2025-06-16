#include <Arduino.h>
#include <Ticker.h>
#include <FastLED.h>
#include "timer_sw.h"
#include "super_timer_sw.h"
#include "RTC_soft.h"
#include "trace_debug.h"
#include "convertAnalogValue.h"
#include "LedBlinkingManagement.h"
#include "userpipe.h"
#include "pinout_definition.h"

#define INTERVALLE_MESURE_PUISSANCE_MS		200
#define INTERVALLE_AFFICHAGE_MESSURE_MS		2000

#define NOMBRE_LEDS_BANDEAU					16

Ticker g_t_blinker; // Composants pour généer une horloge pour TimerSW (objets type TimerEvent_t)

TimerEvent_t g_t_TimerMesures; // Timer cadençant la prise de mesure
TimerEvent_t g_t_TimerAffichage; // Timer cadençant l'affichage des résultats mesurées et calculés

User_Pipe g_t_MeasuresPipe(200); // Tuyau pour pouvoir stocker plusieurs mesures avant traitement

ConvertAnalogValue ConvertVoltage(0, 0, 0.0, 24.0, 0, 944); // Paramètres à ajuster selon composants utilisés

ConvertAnalogValue Convertcurrent(512, 10, 10.0, -10.0, 0, 1024); // Paramètres à ajuster selon composants utilisés

GestionLed g_t_GestionBuiltinLed(BUILTIN_LED); // Object pour gérer le clignotement de la LED du module ESP32

CRGB g_t_BandeauLeds[NOMBRE_LEDS_BANDEAU];

typedef struct
{
    uint32_t m_u32_TensionADC;
    uint32_t m_u32_IntensiteADC;
} s_coupleTensionIntensiteADC_t;

void FonctionMesures(uint32_t p_u32_param, void * p_pv_param);
/* les paramètres de la fonction sont inutiles ici mais nécessaire pour garder
 * l'interface nécessaire pour être appelée par TimerSW */


void setup()
{
	// initialisation du Timer matériel pour le module TimerSW
	g_t_blinker.attach(0.05, Inc_Timer); // Résoltuion du timer 0.05 = 50ms

	Init_Trace_Debug();
    Set_Max_Debug_Level(INFO);

	SEND_VTRACE(INFO, "Démarrage Vélo Dynamo");

	g_t_GestionBuiltinLed.SetSequence3();

	FastLED.addLeds<WS2812B, c_u32_BandeauLeds>(g_t_BandeauLeds, NOMBRE_LEDS_BANDEAU);

	g_t_TimerMesures.Init(FonctionMesures, INTERVALLE_MESURE_PUISSANCE_MS, true);
	g_t_TimerMesures.Start();

    g_t_TimerAffichage.Init(nullptr, INTERVALLE_AFFICHAGE_MESSURE_MS, true);
    g_t_TimerAffichage.Start();
}

void loop()
{
    static double l_dble_ValeurEnergieCumulee = 0.0;
    static double l_dble_ValeurPuissance = 0.0;
    static double l_dble_ValeurTension = 0.0;
    static double l_dble_ValeurIntensite = 0.0;

    // Turn the LED on, then pause
    g_t_BandeauLeds[0] = CRGB::Green;
    g_t_BandeauLeds[1] = CRGB::Blue;
    g_t_BandeauLeds[2] = CRGB::Blue;
    FastLED.show();
    delay(200);
    // Now turn the LED off, then pause
    g_t_BandeauLeds[0] = CRGB::Blue;
    g_t_BandeauLeds[1] = CRGB::Red;
    g_t_BandeauLeds[2] = CRGB::Green;
    FastLED.show();
    delay(200);



    // Affichage des résultats mesurés et calculés
    if(g_t_TimerAffichage.IsTop() == true)
    {
        SEND_VTRACE(INFO,"P: %0.1f W, En Prod: %4.3f Wh, U: %2.1f V, I: %2.2f A",
        		l_dble_ValeurPuissance, l_dble_ValeurEnergieCumulee/3600.0 , l_dble_ValeurTension,
				l_dble_ValeurIntensite);
    }

    // Récupération des grandeurs mesurées et calculs
    while(g_t_MeasuresPipe.Is_Pipe_Empty() == PIPE_NOT_EMPTY)
    {
        uint8_t l_u8_codeRetour = 0;

        s_coupleTensionIntensiteADC_t l_s_MeruresATraiter;
	    double l_dble_ValeurEnergie = 0.0;

		SEND_VTRACE(DBG1, "Reception pipe");


    	uint32_t l_u32_mesureTempsTraitement = 0;

    	l_u32_mesureTempsTraitement = micros();

    	/* pour éviter les accès concurrent à la ressource "userpipe" */
    	portMUX_TYPE myMutex = portMUX_INITIALIZER_UNLOCKED;
    	taskENTER_CRITICAL(&myMutex);
        l_u8_codeRetour = g_t_MeasuresPipe.Pipe_Out(&l_s_MeruresATraiter, sizeof(l_s_MeruresATraiter));
    	taskEXIT_CRITICAL(&myMutex);

        if(l_u8_codeRetour == 0)
        {
	        l_dble_ValeurTension = ConvertVoltage.GetConvertedValue(l_s_MeruresATraiter.m_u32_TensionADC);
	        l_dble_ValeurIntensite = Convertcurrent.GetConvertedValue(l_s_MeruresATraiter.m_u32_IntensiteADC);

	        l_dble_ValeurPuissance = l_dble_ValeurTension * l_dble_ValeurIntensite;
	        l_dble_ValeurEnergie = l_dble_ValeurPuissance * ((double)INTERVALLE_MESURE_PUISSANCE_MS)/1000.0;
	        /* L'énergie est calculée en J => P(W) * T(s) = E(J) */

            l_dble_ValeurEnergieCumulee += l_dble_ValeurEnergie;
        }
        else
        {
            SEND_VTRACE(ERROR, "Erreur sortie Pipe");
        }

        // routine de debug pour mesure les temps de traitement en us
        l_u32_mesureTempsTraitement = micros() - l_u32_mesureTempsTraitement;
        SEND_VTRACE(DBG1, "Temps(us): %d", l_u32_mesureTempsTraitement);
    }
}

void FonctionMesures(uint32_t p_u32_param, void * p_pv_param)
{
    uint8_t l_u8_codeRetour = 0;

    s_coupleTensionIntensiteADC_t l_s_MeruresATrater;

    // Instructions pour éviter des warnings à la compilation sur les paramètres non utilisés
    (void)p_u32_param;
    (void)p_pv_param;

	// !!!!!!!!! Numéro de patte à définir !!!!!!!!!
	l_s_MeruresATrater.m_u32_TensionADC = analogRead(c_u32_MesureTension);
	l_s_MeruresATrater.m_u32_IntensiteADC = analogRead(c_u32_MesureIntensite);

	// pour fixer une valeur mesurée sans avoir de carte ni de vélo !
	l_s_MeruresATrater.m_u32_TensionADC = 200;
	l_s_MeruresATrater.m_u32_IntensiteADC = 200;

	/* pour éviter les accès concurrent à la ressource "userpipe" */
	portMUX_TYPE myMutex = portMUX_INITIALIZER_UNLOCKED;
	taskENTER_CRITICAL(&myMutex);
    l_u8_codeRetour = g_t_MeasuresPipe.Pipe_In(&l_s_MeruresATrater, sizeof(l_s_MeruresATrater));
	taskEXIT_CRITICAL(&myMutex);

    if(l_u8_codeRetour != 0)
    {
        SEND_VTRACE(ERROR, "Erreur entrée Pipe");
    }
}
