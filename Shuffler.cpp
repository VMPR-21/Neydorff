#include "Shuffler.h"
//#include <vector>
#include <stdlib.h>
#include <time.h>
#include <assert.h>


int rand_int(int n);

Shuffler::Shuffler(int count) : _count(count)
{
    assert(count >= 1);
    _virtIndex = new int[count];

    for(int i = 0; i < _count; i++)
        _virtIndex[i] = i;
}

Shuffler::~Shuffler()
{
    delete [] _virtIndex;
}

int Shuffler::getRealIndex(int virtIndex)
{
    assert(virtIndex >= 0 && virtIndex < _count);

    for(int i = 0; i < _count; i++)
        if(_virtIndex[i] == virtIndex)
            return i;

    assert(0 != 0); // exception
}

int Shuffler::getVirtIndex(int realIndex)
{
    assert(realIndex >= 0 && realIndex < _count);
    return _virtIndex[realIndex];
}

void Shuffler::ascending()
{
    for(int i = 0; i < _count; i++)
        _virtIndex[i] = i;
}

void Shuffler::shuffle()
{
    int i, j, tmp;

    for (i = _count - 1; i > 0; i--)
    {
        j = rand_int(i + 1);
        tmp = _virtIndex[j];
        _virtIndex[j] = _virtIndex[i];
        _virtIndex[i] = tmp;
    }
}


int rand_int(int n)
{
    int limit = RAND_MAX - RAND_MAX % n;
    int rnd;

    do
    {
        rnd = rand();
    }
    while (rnd >= limit);

    return rnd % n;
}
