/*
 * class_user_stack.h
 *
 *  Created on: 26 juin 2015
 *      Author: julien
 */

#ifndef CLASS_USER_STACK_H_
#define CLASS_USER_STACK_H_

#include <stdint.h>

#define STACK_EMPTY		0
#define STACK_NOT_EMPTY 1

class User_Stack
{
private:
    /// \brief Pile utilisateur
    uint8_t * Tab_User_Stack;
    uint16_t Taille_User_Stack;
    /// \brief Pointeur dans la pile utilisateur
    uint8_t * P_User_Stack;


public:
    User_Stack(uint16_t Size);
    User_Stack(User_Stack const&);

    virtual ~User_Stack();

    User_Stack& operator=(User_Stack const&);

    uint16_t Used_Size(void);
    uint16_t Remained_Size(void);

    /** \brief Reinitialiser la pile utilisateur (effacement donnees)
     *
     * \param void
     * \return void
     *
     */
    void Clean(void);

    /** \brief Rentrer une donnee dans la pile
     *
     * \param Source unsigned char* Pointeur sur la donnee a empiler
     * \param Taille unsigned char Taille de la donnee a empiler
     * \return unsigned char 0 si OK, 1 sinon
     */
     uint8_t Push_Stack(void *Source, uint16_t Taille);

     uint8_t Push_Stack_ToBottom(void * Source, uint16_t Taille);

    /** \brief Sortir une donnee de la pile
     *
     * \param Destination unsigned char* Pointeur sur l'emplacement ou depiler
     * \param Taille unsigned char Taille de la destination (juste pour verifier qu'on ne depile pas n'importe quoi)
     * \return unsigned char 0 si OK, 1 sinon
     */
     uint8_t Pop_Stack(void * Destination, uint16_t Taille);

    /** \brief Sortir une donnee de la pile par le bas de la pile (emulation d'un pipe)
     *
     * \param Destination unsigned char* Pointeur sur l'emplacement ou depiler
     * \param Taille unsigned char Taille de la destination (juste pour verifier qu'on ne depile pas n'importe quoi)
     * \return unsigned char 0 si OK, 1 sinon
     */
     uint8_t Pop_Stack_FromBottom(void *Destination, uint16_t Taille);

    /** \brief Savoir si la pile est vide ou non
     *
     * \param void
     * \return 1 si la pile est vide, 0 sinon
     *
     */
    uint8_t Stack_Is_Empty(void);

    /** \brief Donner la taille de la derniere donnee rentree
     *
     * \param void
     * \return Donne la valeur de la taille de la derniere donnee (0 si pile vide)
     *
     */
    uint16_t Size_Last_Data(void);

    uint16_t Size_First_Data(void);

    uint8_t View_Last_Data(void * Destination, uint16_t Taille);
    uint8_t View_First_Data(void * Destination, uint16_t Taille);

};

#endif /* CLASS_USER_STACK_H_ */
