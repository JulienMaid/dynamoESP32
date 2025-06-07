// @file trace_debug.h
/// <b>Date de creation:</b> 16/12/2015                                                      <br>
///                                                                                          <br>
/// @brief Implementation d'un systeme de trace pour aider au debugage                       <br>
///                                                                                          <br>
///==========================================================================================<br>
/// <b>Donnees SVN:</b>                                                                      <br>
/// $Rev:: 587                                                                              $<br>
/// $Author:: julien                                                                        $<br>
/// $Date:: 2017-01-23 15:03:25 +0100 (lun., 23 janv. 2017)                                  $<br>
///                                                                                          <br>
///==========================================================================================<br>
/// <b>Historique:</b>                                                                       <br>
///                                                                                          <br>
///                                                                                          <br>
///==========================================================================================<br>
/// <b>Description detaillee:</b> (si necessaire)                                            <br>
///                                                                                          <br>
///                                                                                          <br>
///                                                                                          <br>
///==========================================================================================<br>

#define TRACE_DEBUG_C

//************************************
//* Includes lies a l'implementation *
//************************************

#include "trace_debug.h"

#include "RTC_soft.h"

#include <stdint.h>

#include <string.h>
#include <Arduino.h>

//#include <WiFiUdp.h>

#define Send_String_UARTX(a,b) Serial.print(a)

//*********************
//* Variables privees *
//*********************

const uint8_t Txt_None[] = "NONE";
const uint8_t Txt_Error[] = "ERROR";
const uint8_t Txt_Warning[] = "WARNING";
const uint8_t Txt_Info[] = "INFO";
const uint8_t Txt_Tst_Secu_Results[] = "TSR";
const uint8_t Txt_Debug1[] = "DBG1";
const uint8_t Txt_Debug2[] = "DBG2";
const uint8_t Txt_Debug3[] = "DBG3";
const uint8_t Txt_Debug4[] = "DBG4";
const uint8_t Txt_DebugSpecif[] = "DBGX";
const uint8_t Txt_All[] = "ALL";

Txt_Type_Trace_t Table_Type_Trace[11] = { { NONE, Txt_None }, { ERROR, Txt_Error }, { WARNING,
    Txt_Warning }, { INFO, Txt_Info }, { TEST_SECU_RESULTS, Txt_Tst_Secu_Results }, { DBG1,
    Txt_Debug1 }, { DBG2, Txt_Debug2 }, { DBG3, Txt_Debug3 }, { DBG4, Txt_Debug4 }, { DBGX,
    Txt_DebugSpecif }, { ALL, Txt_All } };

std::string g_t_IPDestTracesUDP("192.168.123.255");
uint16_t g_u16_PortDestUdp = 1234;

/// @brief Niveau maximum de trace a remonter
e_type_trace_t g_e_MaxDebugLevel = ALL;

#if defined(ESP32)
QueueHandle_t g_pt_queueTraces;
#endif

bool g_b_TracesUDP = false;
bool g_b_TracesSerie = true;

//********************************
//* Implementation des fonctions *
//********************************

void Init_Trace_Debug(void)
{

  Init_RTC_Soft();

  Serial.begin(115200);

}

const char* Get_Text_Type_Trace(e_type_trace_t Type_Trace)
{
  Txt_Type_Trace_t *p_Table_Type_Trace = &Table_Type_Trace[0];

  // Recherche du texte correpondant au niveau de trace choisi
  while (p_Table_Type_Trace
      < (&Table_Type_Trace[0] + (sizeof(Table_Type_Trace) / sizeof(Txt_Type_Trace_t))))
  {
    if (p_Table_Type_Trace->Type_Trace == Type_Trace)
      return (const char*) p_Table_Type_Trace->p_Text_Data;

    p_Table_Type_Trace++;
  }

  return NULL;
}

void Set_Max_Debug_Level(e_type_trace_t Level)
{
  g_e_MaxDebugLevel = Level;
}

e_type_trace_t Get_Max_Debug_Level(void)
{
  return g_e_MaxDebugLevel;
}

uint8_t Send_Trace_Buffer(e_type_trace_t Type_Trace, const char Txt_Donnees[], uint8_t *Buffer,
    uint8_t Size, bool Horodatage)
{
  (void) Horodatage;
  char Buffer_Temp[128];
  char *P_Buffer_Temp;
  uint8_t lenText_Donnnes;
  uint8_t Partie_Haute, Partie_Basse;

  // Test pour savoir si la trace a un niveau "remontable" ou non
  if (Test_Trace_Level(Type_Trace) != 0)
    return 2;

  //*****************
  //* Sanity Checks *
  //*****************
  if (Txt_Donnees == NULL)
    return 1;

  if (Buffer == NULL)
    return 1;

  lenText_Donnnes = strlen(Txt_Donnees);

  if ((lenText_Donnnes + Size + 3) >= 128)
    return 1;

  strcpy(Buffer_Temp, Txt_Donnees);
  P_Buffer_Temp = &Buffer_Temp[0] + lenText_Donnnes;

  *P_Buffer_Temp = ' ';
  P_Buffer_Temp++;
  *P_Buffer_Temp = ':';
  P_Buffer_Temp++;
  *P_Buffer_Temp = ' ';
  P_Buffer_Temp++;

  while (Size != 0)
  {
    Partie_Haute = *Buffer >> 4;

    if (Partie_Haute >= 0x0a)
      Partie_Haute += 0x57;
    else
      Partie_Haute += 0x30;

    *P_Buffer_Temp = Partie_Haute;

    P_Buffer_Temp++;

    Partie_Basse = *Buffer & 0xf;

    if (Partie_Basse >= 0x0a)
      Partie_Basse += 0x57;
    else
      Partie_Basse += 0x30;

    *P_Buffer_Temp = Partie_Basse;

    P_Buffer_Temp++;

    if (Size != 1)
    {
      *P_Buffer_Temp = ' ';
      P_Buffer_Temp++;
    }

    Size--;
    Buffer++;
  }

  *P_Buffer_Temp = 0;

  return 0;  //Send_Trace(Type_Trace, Buffer_Temp, Horodatage);
}

uint8_t Test_Trace_Level(e_type_trace_t Level)
{
  // Pas de trace voulue, on sort !
  if (g_e_MaxDebugLevel == NONE)
    return 1;

  if (Level == NONE)
    return 1;

  // Niveau de trace superieur au niveau max defini, on sort !
  if (Level > g_e_MaxDebugLevel)
  {
    // Sauf s'il s'agit du niveau de "debug specifique" permettant d'extraire
    // une trace particuliere
    if (Level != DBGX)
      return 1;
  }

  return 0;
}

uint8_t Send_VTrace(e_type_trace_t Type_Trace, bool Horodatage, const char *i_ps8_nomFichier,
    const char *i_ps8_nomFonction, uint16_t i_u16_numeroLigne, const char *Txt_Donnees, ...)
{
  char ts8_BufferTx[200];
  char ts8_BufferTxString[300];
  va_list argp;
  uint32_t u32_Temps;
  uint32_t u32_TimeInt;
  uint8_t u8_TimeFrac;

  // Test pour savoir si la trace a un niveau "remontable" ou non
  if (Test_Trace_Level(Type_Trace) != 0)
    return 2;

  if (Horodatage == true)
  {
    u32_Temps = Get_Time();

    u32_TimeInt = u32_Temps / 10;
    u8_TimeFrac = u32_Temps - (10 * u32_TimeInt);
  }

  va_start(argp, Txt_Donnees);
  vsprintf(ts8_BufferTx, Txt_Donnees, argp);
  va_end(argp);

  if (Horodatage == true)
  {
    sprintf(ts8_BufferTxString, "%u.%us: %s - %s    @@ (%s -> %s(l.%u))", u32_TimeInt, u8_TimeFrac,
        Get_Text_Type_Trace(Type_Trace), ts8_BufferTx, i_ps8_nomFonction, i_ps8_nomFichier,
        i_u16_numeroLigne);
  }
  else
  {
    sprintf(ts8_BufferTxString, "%s - %s    @@ (%s -> %s(l.%u))", Get_Text_Type_Trace(Type_Trace),
        ts8_BufferTx, i_ps8_nomFonction, i_ps8_nomFichier, i_u16_numeroLigne);
  }

  Serial.println(ts8_BufferTxString);

  return 0;

}
