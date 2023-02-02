#include "coding.h"

unsigned char encode(unsigned char c)
{
    int N = 4;
    unsigned char cipher_text= (c+N);
    return cipher_text;
}
unsigned char decode(unsigned char c)
{
    int N = 4;
    return c-N;
}
