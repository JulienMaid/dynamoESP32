/*
 * GestionLed.cpp
 *
 *  Created on: 16 mars 2024
 *      Author: julien
 */

#include <string.h>
#include <Arduino.h>
#include "LedBlinkingManagement.h"

GestionLed::GestionLed(uint32_t p_u32_PortLed, bool p_b_AutonomousOperation,
    uint8_t p_u8_SequenceLength) :
    m_u32_PortLed(p_u32_PortLed), m_u8_SequenceLength(p_u8_SequenceLength), m_u8_SequenceIndex(0)
{
  m_ptu8_LedSequence = new uint8_t[m_u8_SequenceLength];
  ClearSequence();

  if (p_b_AutonomousOperation == true)
  {
    pinMode(LED_BUILTIN, OUTPUT);

    m_t_TimerLed.Init(StaticGetSequence, 100, true, this);
    m_t_TimerLed.Start();
  }
}

GestionLed::~GestionLed()
{
  m_t_TimerLed.Stop();
  delete[] m_ptu8_LedSequence;
}

void GestionLed::ClearSequence(void)
{
  memset(m_ptu8_LedSequence, 0, m_u8_SequenceLength);
}

void GestionLed::SetSequence1(void)
{
  uint8_t l_tu8_LedSequence[m_u8_SequenceLength] = { 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0,
      0, 0, 0, 0 };

  memcpy(m_ptu8_LedSequence, l_tu8_LedSequence, m_u8_SequenceLength);
}

void GestionLed::SetSequence2(void)
{
  uint8_t l_tu8_LedSequence[m_u8_SequenceLength] = { 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0 };

  memcpy(m_ptu8_LedSequence, l_tu8_LedSequence, m_u8_SequenceLength);

}

void GestionLed::SetSequence3(void)
{
  uint8_t l_tu8_LedSequence[m_u8_SequenceLength] = { 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0 };

  memcpy(m_ptu8_LedSequence, l_tu8_LedSequence, m_u8_SequenceLength);
}

void GestionLed::SetSequence4(void)
{
  uint8_t l_tu8_LedSequence[m_u8_SequenceLength] = { 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0,
      0, 0, 0, 0 };

  memcpy(m_ptu8_LedSequence, l_tu8_LedSequence, m_u8_SequenceLength);
}

uint8_t GestionLed::GetSequence(bool p_b_Reverse)
{
  uint8_t l_u8_returnedValue;

  if (p_b_Reverse == true)
  {
    l_u8_returnedValue = !m_ptu8_LedSequence[m_u8_SequenceIndex];
  }
  else
  {
    l_u8_returnedValue = m_ptu8_LedSequence[m_u8_SequenceIndex];
  }

  m_u8_SequenceIndex++;
  if (m_u8_SequenceIndex >= m_u8_SequenceLength)
  {
    m_u8_SequenceIndex = 0;
  }

  return l_u8_returnedValue;
}

uint32_t GestionLed::GetPortLed(void)
{
  return m_u32_PortLed;
}

void GestionLed::StaticGetSequence(uint32_t arg1, void *ClassGestionLed)
{
  GestionLed *l_t_GestionLED = (GestionLed*) ClassGestionLed;

  digitalWrite(l_t_GestionLED->GetPortLed(), l_t_GestionLED->GetSequence(false));
}
