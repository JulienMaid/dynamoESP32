#ifndef USERPIPE_H
#define USERPIPE_H

#include <stdint.h>
#include "userstack.h"

#define PIPE_NOT_EMPTY STACK_NOT_EMPTY
#define PIPE_EMPTY STACK_EMPTY

class User_Pipe : private User_Stack
{
private:
    //User_Stack * Internal_Stack;
public:
    User_Pipe(uint16_t Taille);
    ~User_Pipe();

    uint8_t Pipe_In(void *Source, uint16_t Taille);
    uint8_t Pipe_Out(void *Destination, uint16_t Taille);

    uint16_t Size_Data(void);

    uint8_t Is_Pipe_Empty(void);

};

#endif // USERPIPE_H
