#ifndef SHUFFLER_H
#define SHUFFLER_H

#include <vector>

//Класс-перемешиватель чисел.
class Shuffler
{
public:
    Shuffler(int count);
    ~Shuffler();
    int getRealIndex(int virtIndex);
    int getVirtIndex(int realIndex);
    void shuffle();// перемешать числа
    void ascending();//выстроить в порядке возрастания

    int count()
    {
        return _count;
    }
private:
    int *_virtIndex;
    int _count;
};

#endif // SHUFFLER_H
