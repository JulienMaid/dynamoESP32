/*
 * AnalogFilter.h
 *
 *  Created on: 14 mars 2024
 *      Author: julien
 */

#pragma once

#include <stdint.h>
#include <math.h>

template<typename T>
class NumericFilter
{
public:
  //! Contructeur de la classe
  //! @param valeur du coeff du membre "entrée" tel que Outn = p_d_InputCoeff * Inn + (1 - p_d_InputCoeff) * Outn-1
  NumericFilter(double p_d_InputCoeff = 0.5);

  virtual ~NumericFilter();

  //! Modifier le coeff du membre "entrée"
  //! @param p_d_InputCoeff valeur du coefficient
  //! @return true si valeur acceptée, false sinon
  bool SetInputCoeff(double p_d_InputCoeff);

  //! Enregistrer nouvelle valeur
  //! @param p_T_NewValue nouvelle valeur
  //! @return nouvelle sortie filtrée
  T SetNewValue(T p_T_NewValue);

  //! Reseter le filtrage (n'efface pas les coefficients)
  void Reset(void);

private:

  double m_d_InputCoeff;
  double m_d_OutputCoeff;

  double m_d_PreviousOutput;
  bool m_b_FilterInitialized;

};

template<typename T>
inline NumericFilter<T>::NumericFilter(double p_d_InputCoeff) :
    m_b_FilterInitialized(false), m_d_PreviousOutput(0.0)
{
  if (p_d_InputCoeff <= 1.0)
  {
    m_d_InputCoeff = p_d_InputCoeff;
    m_d_OutputCoeff = 1.0 - m_d_InputCoeff;
  }
  else
  {
    m_d_InputCoeff = 1.0;
    m_d_OutputCoeff = 1.0 - m_d_InputCoeff;
  }
}

template<typename T>
inline NumericFilter<T>::~NumericFilter()
{
}

template<typename T>
inline bool NumericFilter<T>::SetInputCoeff(double p_d_InputCoeff)
{
  bool l_b_Return = false;

  if (p_d_InputCoeff <= 1.0)
  {
    m_d_InputCoeff = p_d_InputCoeff;
    m_d_OutputCoeff = 1.0 - m_d_InputCoeff;
    l_b_Return = true;
  }

  return l_b_Return;
}

template<typename T>
inline T NumericFilter<T>::SetNewValue(T p_T_NewValue)
{
  T l_T_ReturnValue = 0;

  if (m_b_FilterInitialized == true)
  {
    m_d_PreviousOutput = m_d_InputCoeff * p_T_NewValue + m_d_OutputCoeff * m_d_PreviousOutput;
    l_T_ReturnValue = round(m_d_PreviousOutput);
  }
  else
  {
    l_T_ReturnValue = p_T_NewValue;
    m_d_PreviousOutput = p_T_NewValue;
    m_b_FilterInitialized = true;
  }

  return l_T_ReturnValue;
}

template<typename T>
inline void NumericFilter<T>::Reset(void)
{
  m_b_FilterInitialized = false;
}

