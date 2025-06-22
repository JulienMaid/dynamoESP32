#include <Arduino.h>
#include <Ticker.h>
#include <FastLED.h>
#include "timer_sw.h"
#include "trace_debug.h"
#include "convertAnalogValue.h"
#include "LedBlinkingManagement.h"
#include "userpipe.h"
#include "pinout_definition.h"

#define INTERVALLE_MESURE_PUISSANCE_MS		200
#define INTERVALLE_ENVOI_MESSURES_MS		2000
#define INTERVALLE_AFFICHAGE_MESSURES_MS	100

#define NOMBRE_LEDS_BANDEAU					16

Ticker g_t_blinker; // Composants pour généer une horloge pour TimerSW (objets type TimerEvent_t)

TimerEvent_t g_t_TimerMesures; // Timer cadençant la prise de mesure
TimerEvent_t g_t_TimerEnvoiMesures; // Timer cadençant l'envoi des résultats mesurées et calculés
TimerEvent_t g_t_TimerMAJLeds; // Timer cadençant l'affichage des résultats mesurées et calculés

User_Pipe g_t_MeasuresPipe(200); // Tuyau pour pouvoir stocker plusieurs mesures avant traitement


ConvertAnalogValue ConvertVoltage(0, 0, 0.0, 24.0, 0, 3470); // Objet pour calculer la tension correspondant à la meure
// => Paramètres à ajuster selon composants utilisés

ConvertAnalogValue Convertcurrent(2990, 15, 10.0, -10.0, 0, 4095); // Objet pour calculer la tension correspondant à la meure
// => Paramètres à ajuster selon composants utilisés

GestionLed g_t_GestionBuiltinLed(BUILTIN_LED); // Object pour gérer le clignotement de la LED du module ESP32

CRGB g_t_BandeauLeds[NOMBRE_LEDS_BANDEAU]; // Tableau représentant chaque led du bandeau (objet FastLED)

// Défintiion d'un struct pour stocker des couples tension - intensité en attente d'être traités
typedef struct
{
    uint32_t m_u32_TensionADC;
    uint32_t m_u32_IntensiteADC;
} s_coupleTensionIntensiteADC_t;

// Fonction appelée par g_t_TimerMesures pour mesurer la tension et l'intensité
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

	// initialisation de FastLED pour gérer le bandeau de LEDs
	FastLED.addLeds<WS2812B, c_u8_BandeauLeds, GRB>(g_t_BandeauLeds, NOMBRE_LEDS_BANDEAU);

	// Initialisation puis démarrage du Timer gérant l'intervalle entre 2 mesures
	g_t_TimerMesures.Init(FonctionMesures, INTERVALLE_MESURE_PUISSANCE_MS, Periodic_Timer);
	g_t_TimerMesures.Start();

	// Initialisation puis démarrage du Timer gérant l'intervalle entre 2 envois de mesures
    g_t_TimerEnvoiMesures.Init(nullptr, INTERVALLE_ENVOI_MESSURES_MS, Periodic_Timer);
    g_t_TimerEnvoiMesures.Start();

	// Initialisation puis démarrage du Timer gérant l'intervalle entre 2 affichages de mesures sur le bandeau
    g_t_TimerMAJLeds.Init(nullptr, INTERVALLE_AFFICHAGE_MESSURES_MS, Periodic_Timer);
    g_t_TimerMAJLeds.Start();
}

void loop()
{
    static double l_dble_ValeurEnergieCumulee = 0.0;
    static double l_dble_ValeurPuissance = 0.0;
    static double l_dble_ValeurTension = 0.0;
    static double l_dble_ValeurIntensite = 0.0;

    // Affichage des résultats calculés sur le bandeau de LEDs
    if(g_t_TimerMAJLeds.IsTop() == true)
    {
    	static uint8_t l_u8_ValeurTestPuissance = 0;
    	uint8_t l_u8_NbreLeds = 0;
    	uint8_t l_u8_IndexLed = 0;

        l_u8_NbreLeds = ((uint16_t)l_u8_ValeurTestPuissance)*((uint16_t)NOMBRE_LEDS_BANDEAU)/200;

        l_u8_ValeurTestPuissance += 12;

        if(l_u8_ValeurTestPuissance > 200)
        {
        	l_u8_ValeurTestPuissance = 0;
        }

        for(l_u8_IndexLed=0; l_u8_IndexLed<NOMBRE_LEDS_BANDEAU; l_u8_IndexLed++)
        {
        	if((l_u8_IndexLed == l_u8_NbreLeds) && (l_u8_NbreLeds != 0))
        	{
        		g_t_BandeauLeds[l_u8_IndexLed] = CRGB::Blue;
        	}
        	else if((l_u8_IndexLed < l_u8_NbreLeds) && (l_u8_NbreLeds != 0))
        	{
        		g_t_BandeauLeds[l_u8_IndexLed] = CRGB::Green;
        	}
        	else
        	{
        		g_t_BandeauLeds[l_u8_IndexLed] = CRGB::Red;
        	}
        }

        FastLED.show();
    }

    // Envoi des résultats mesurés et calculés
    if(g_t_TimerEnvoiMesures.IsTop() == true)
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
	l_s_MeruresATrater.m_u32_TensionADC = analogRead(c_u8_MesureTension);
	l_s_MeruresATrater.m_u32_IntensiteADC = analogRead(c_u8_MesureIntensite);

	SEND_VTRACE(DBG1, "Tension(E/S: %d): %d, Intensite(E/S: %d): %d",
			c_u8_MesureTension, l_s_MeruresATrater.m_u32_TensionADC,
			c_u8_MesureIntensite, l_s_MeruresATrater.m_u32_IntensiteADC);

	// pour fixer une valeur mesurée sans avoir de carte ni de vélo !
//	l_s_MeruresATrater.m_u32_TensionADC = 200;
//	l_s_MeruresATrater.m_u32_IntensiteADC = 200;

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
