/*
 * GestionLed.h
 *
 *  Created on: 16 mars 2024
 *      Author: julien
 */

#pragma once

#include <stdint.h>
#include "timer_sw.h"

class GestionLed
{
public:
  //! Retourne la valeur courante de la séquence d'allumage
  //! @param p_u32_PortLed numéro de port de la Led à piloter
  //! @param p_b_AutonomousOperation active ou non le fonctionne autonome de l'object (allumage auto de la led, période séquence 100ms)
  //! @param p_b_AutonomousOperation active ou non le fonctionne autonome de l'object (allumage auto de la led, période séquence 100ms)
  //! @param p_u8_SequenceLength taille de la séquence d'allumage/extinction
  GestionLed(uint32_t p_u32_PortLed = 0, bool p_b_Reverse = false, bool p_b_AutonomousOperation = true,
      uint8_t p_u8_SequenceLength = 20);

  //! Destructeur de la classe
  virtual ~GestionLed();

  //! Retourne la valeur courante de la séquence d'allumage
  virtual uint8_t GetSequence(void);

  //! Retourne le numéro du port Led
  uint32_t GetPortLed(void);

  //! Active la séquence d'extinction (chaque valeur de la séquence est 0)
  virtual void ClearSequence(void);

  //! Active la séquence n°1
  virtual void SetSequence1(void);

  //! Active la séquence n°1
  virtual void SetSequence2(void);

  //! Active la séquence n°1
  virtual void SetSequence3(void);

  //! Active la séquence n°1
  virtual void SetSequence4(void);

  //! methode statique utilisée par le timer logiciel dans le fonctionnement autonome
  static void StaticGetSequence(uint32_t arg1, void *ClassGestionLed);

protected:
  uint32_t m_u32_PortLed;
  uint8_t m_u8_SequenceLength;
  uint8_t m_u8_SequenceIndex;
  bool m_b_Reverse;

  uint8_t *m_ptu8_LedSequence;

  TimerEvent_t m_t_TimerLed;
};

