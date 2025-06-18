#pragma once

#include <stdint.h>
#include <stdlib.h>

enum Timer_Type
{
  Periodic_Timer = true, One_Shot_Timer = false
};

#ifndef TimerTime_t
typedef uint64_t TimerTime_t;
#endif

#define DEC_TIMESTAMP 50

class TimerEvent_t
{
public:
  /**
   * @brief Contructeur complet
   * @param Callback_Fct Fonction à appeler quand le timer arrive à échéance (peut-être mis à null_ptr si pas besoin)
   * @param remainingTime Temps d'attente du timer en ms
   * @param Perodic Booléen pour que le timer se relance automatiquement ou non une fois arrivé à échéance
   * @param ArgCallBack Arguement à faire passer à la fonction Callback_Fct (peut-être mis à null_ptr si pas besoin)
   * @param Label Chaine de caractères pour nommer le timer dans les traces par exemple (peut-être mis à null_ptr si pas besoin)
   */
  TimerEvent_t(void (*Callback_Fct)(uint32_t, void*), TimerTime_t remainingTime,
      bool Perodic = true, void *ArgCallBack = NULL, const char Label[] = NULL);
  TimerEvent_t(void);
  TimerEvent_t(TimerEvent_t&);
  virtual ~TimerEvent_t(void);

  /** Redefinition des opération = et == */
  TimerEvent_t& operator=(TimerEvent_t const&);
  bool operator==(TimerEvent_t const&);

  /**
   * @brief Intialise l'objet Timer
   * @param Callback_Fct Fonction à appeler quand le timer arrive à échéance (peut-être mis à null_ptr si pas besoin)
   * @param remainingTime Temps d'attente du timer en ms
   * @param Perodic Booléen pour que le timer se relance automatiquement ou non une fois arrivé à échéance
   * @param ArgCallBack Arguement à faire passer à la fonction Callback_Fct (peut-être mis à null_ptr si pas besoin)
   * @param Label Chaine de caractères pour nommer le timer dans les traces par exemple (peut-être mis à null_ptr si pas besoin)
   */
  virtual void Init(void (*Callback_Fct)(uint32_t, void*), TimerTime_t remainingTime, bool Perodic =
      true, void *ArgCallBack = NULL, const char Label[] = NULL);
  // @brief Démarre le timer
  virtual void Start(void);
  // @brief Arrêt le timer
  virtual void Stop(void);
  /**
   * @brief Fixe une  nouvelle valeur au timer
   * @param Time Valeur en ms
   */
  virtual void SetValue(TimerTime_t Time);
  // @brief Réinitialise le timer
  virtual void Reload(void);

  /**
   * @brief Indique sur le timer est inserée dans la liste chainée des timer en cours de traitement
   * @return true si dedans, false sinon
   */
  bool Exists(void);

  /**
   * @brief Efface le timer de la liste chainée des timer en cours de traitement
   * @return 0 si effacé, 1 sinon (non effacé, non trouvé ?)
   */
  uint8_t Delete(void);

  /**
   * @brief Indique si l'on se trouve ou non dans la fonction de callback du timer
   * @return true si oui, false sinon
   */
  bool DoLaunchMe(void);

  /**
   * @brief Inique le timer est arrivé à écheance (utilie uniquement si Callback_Fct != nullptr)
   * @return true si oui, false sinon
   */
  bool IsTop(void);

  /**
   * @brief Donne le temps restant avant écheance
   * @return Temps restant en ms
   */
  TimerTime_t Time_Remaining(void);

  /**
   * @brief Donne le temps écoulé depuis démarrage du timer
   * @return Temps écoulé en ms
   */
  TimerTime_t Time_Elapsed(void);

  // @brief la valeur utilisateur passée à la fonction de callback utilisateur du timer
  void setUserValeur(uint32_t value);
  uint32_t getUserValue(void);

  // fonction à appeler périodiquement depuis un timer HW pour traiter la liste chainée des timer en cours
  friend void Inc_Timer(void);

protected:
  static TimerEvent_t *TimerListHead;
  static bool TimerExists(const TimerEvent_t *obj);
  static uint8_t TimerDelete(TimerEvent_t *obj);
  static bool TimerClean(void);

  static bool IRQ_Timer_SW_En_Cours;

  uint32_t u32_userValue;
  TimerTime_t Timestamp;      //! Valeur courant du timer
  TimerTime_t ReloadValue;    //! Valeur initiale du timer
  bool IsRunning;             //! est-ce que le timer est en marche
  bool Periodic;              //! est-ce que le timer est en mode périodique (relance automatique)
  bool CallBack_Launched;
  bool Top;
  bool Registered;
  void (*Callback)(uint32_t value, void *ArgCallBack); //! Timer IRQ callback function
  void *ArgCallBack;
  const char *Label;
  class TimerEvent_t *Next;  //! Pointer to the next Timer object.

  unsigned char InsertTimer(void);

};

void Inc_Timer(void);
