#ifndef GATHER_H
#define GATHER_H

#include <vector.h>

/**
  gather copies in to result according to map. Map points the order of elements that will appear in the in vector
  */
template<typename scalar>
void gather(const int map_first, const int map_last, const Vector<scalar, CPU>& in, const Vector<int, CPU>& map, Vector<scalar, CPU>& out);


#endif // GATHER_H
