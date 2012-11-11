#ifndef DATASTREAM_H
#define DATASTREAM_H
#include <QDataStream>

//DataStream - пока что - просто Qt-шный. В будщем может быть заменен на свой.

//для помещения в поток и извлечения из потока использовать операторы >> и <<
typedef QDataStream DataStream;

#endif // DATASTREAM_H
