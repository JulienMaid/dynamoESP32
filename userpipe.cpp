#include "userpipe.h"

User_Pipe::User_Pipe(uint16_t Taille) : User_Stack(Taille)
{

}

User_Pipe::~User_Pipe()
{

}

uint8_t User_Pipe::Pipe_In(void *Source, uint16_t Taille)
{
    return Push_Stack(Source, Taille);
}

uint8_t User_Pipe::Pipe_Out(void *Destination, uint16_t Taille)
{
    return Pop_Stack_FromBottom(Destination, Taille);
}

uint16_t User_Pipe::Size_Data(void)
{
    return Size_First_Data();
}

uint8_t User_Pipe::Is_Pipe_Empty(void)
{
	return User_Stack::Stack_Is_Empty();
}
