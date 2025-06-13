/*
 * class_user_stack.cpp
 *
 *  Created on: 26 juin 2015
 *      Author: julien
 */

#include "userstack.h"
#include <string.h>
#include <stdint.h>

User_Stack::User_Stack(uint16_t Size)
{
    Tab_User_Stack = new uint8_t [Size];
    Taille_User_Stack = Size;
    memset(Tab_User_Stack, 0 , Size);
    P_User_Stack = Tab_User_Stack;
}

User_Stack::User_Stack(User_Stack const& Stack) : Taille_User_Stack(Stack.Taille_User_Stack)
{
    Tab_User_Stack = new uint8_t [Taille_User_Stack];
    memcpy(Tab_User_Stack, Stack.Tab_User_Stack, Taille_User_Stack);
    P_User_Stack = Tab_User_Stack + (Stack.P_User_Stack - Stack.Tab_User_Stack);
}


User_Stack::~User_Stack()
{
    // @TODO Auto-generated destructor stub
    delete [] Tab_User_Stack;
}

User_Stack& User_Stack::operator=(User_Stack const& Stack)
{
    if(this != &Stack)
    {
        Taille_User_Stack = Stack.Taille_User_Stack;
        delete [] Tab_User_Stack;
        Tab_User_Stack = new uint8_t [Taille_User_Stack];
        memcpy(Tab_User_Stack, Stack.Tab_User_Stack, Taille_User_Stack);
        P_User_Stack = Tab_User_Stack + (Stack.P_User_Stack - Stack.Tab_User_Stack);
    }

    return *this;
}


uint16_t User_Stack::Used_Size(void)
{
    return P_User_Stack - Tab_User_Stack;
}

uint16_t User_Stack::Remained_Size(void)
{
    return Taille_User_Stack - Used_Size() - 2*sizeof(uint16_t);
}


void User_Stack::Clean(void)
{
    memset(Tab_User_Stack, 0, Taille_User_Stack);
    P_User_Stack = Tab_User_Stack;
}


uint8_t User_Stack::Push_Stack(void *Source, uint16_t Taille)
{

    if(Remained_Size() < Taille)
        return 1;

    *(uint16_t *)P_User_Stack =  Taille;
    P_User_Stack += 2;
    memcpy(P_User_Stack,Source,Taille);
    P_User_Stack += Taille;
    *(uint16_t *)P_User_Stack =  Taille;
    P_User_Stack += 2;
    return 0;
}

uint8_t User_Stack::Push_Stack_ToBottom(void * Source, uint16_t Taille)
{

    if(Source == NULL)
        return 1;

    if(Taille == 0)
        return 1;

    if(Remained_Size() < Taille)
        return 1;

    if(P_User_Stack != Tab_User_Stack)
    {
        uint8_t *p1, *p2;
        uint16_t temp;

        p1 = P_User_Stack - 1 + Taille + 4;
        p2 = P_User_Stack - 1;
        temp = P_User_Stack - Tab_User_Stack;

        while(temp != 0)
        {
            *p1 = *p2;
            p1--;
            p2--;
            temp--;
        }
    }

    P_User_Stack += Taille + 4;
    *((uint16_t *)Tab_User_Stack) = Taille;
    *(uint16_t *)(Tab_User_Stack + Taille + 2) = Taille;
    memcpy(Tab_User_Stack + 2, Source, Taille);

    return 0;
}


uint8_t User_Stack::Pop_Stack(void * Destination, uint16_t Taille)
{
    uint16_t Longueur;

    if(P_User_Stack == &Tab_User_Stack[0])
        return 1;

    P_User_Stack -= 2;
    Longueur = *(uint16_t *)P_User_Stack;

    if(Taille != Longueur)
    {
        P_User_Stack += 2;
        return 1;
    }

    P_User_Stack -= Longueur;
    memcpy(Destination, P_User_Stack, Longueur);
    P_User_Stack -= 2;

    return 0;
}

uint8_t User_Stack::Pop_Stack_FromBottom(void * Destination, uint16_t Taille)
{
    uint16_t Longueur, Longueur2;
    uint8_t * P_Fond_De_Pile;

    if(P_User_Stack == &Tab_User_Stack[0])
        return 1;

    if(Taille != *(uint16_t *)Tab_User_Stack)
        return 1;

    Longueur2 = *(uint16_t *)Tab_User_Stack + 4;

    memcpy(Destination, &Tab_User_Stack[2], *(uint16_t *)Tab_User_Stack);

    P_Fond_De_Pile = &Tab_User_Stack[2] + *(uint16_t *)Tab_User_Stack + 2;
    Longueur = P_User_Stack - P_Fond_De_Pile + 1;

    memcpy(&Tab_User_Stack[0], P_Fond_De_Pile, Longueur);

    P_User_Stack -= (Longueur2);

    return 0;
}

uint8_t User_Stack::Stack_Is_Empty(void)
{
    if(P_User_Stack == Tab_User_Stack)
        return STACK_EMPTY;
    else
        return STACK_NOT_EMPTY;
}

uint16_t User_Stack::Size_Last_Data(void)
{
    if(P_User_Stack == Tab_User_Stack)
        return 0;

    uint8_t * Ptemp;
    Ptemp = P_User_Stack - 2;

    return *((uint16_t *)Ptemp);
}

uint16_t User_Stack::Size_First_Data(void)
{
    return *(uint16_t *)(Tab_User_Stack);
}

uint8_t User_Stack::View_First_Data(void * Destination, uint16_t Taille)
{
    uint16_t Temp_Longueur;


    if(Destination == NULL)
        return 1;

    if(Stack_Is_Empty() == STACK_EMPTY)
        return 1;

    Temp_Longueur = *(uint16_t *)(Tab_User_Stack);

    if(Temp_Longueur != Taille)
        return 1;

    memcpy(Destination, Tab_User_Stack + sizeof(uint16_t), Temp_Longueur);

    return 0;
}

uint8_t User_Stack::View_Last_Data(void * Destination, uint16_t Taille)
{
    uint16_t Temp_Longueur;
    uint8_t * P_User_Stack_Temp = P_User_Stack;

    if(Destination == NULL)
        return 1;

    if(Stack_Is_Empty() == STACK_EMPTY)
        return 1;

    P_User_Stack_Temp -= 2;
    Temp_Longueur = *(uint16_t *)(P_User_Stack_Temp);

    if(Temp_Longueur != Taille)
    {
        return 1;
    }

    memcpy(Destination, P_User_Stack_Temp - Temp_Longueur, Temp_Longueur);

    return 0;
}
