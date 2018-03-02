#ifndef MATHS_H
#define MATHS_H

#include <algorithm>

class Maths{
public:
    template <class T>
    static T clamp(T value, T limit1, T limit2);
    template <class T>
    static T clampBetween(T value, T limit1, T limit2, T diff);
};

template<class T>
T Maths::clamp(T value, T limit1, T limit2) {
    T minLimit = std::min(limit1, limit2);
    T maxLimit = std::max(limit1, limit2);
    return std::min(std::max(value, minLimit),maxLimit);
}

template<class T>
T Maths::clampBetween(T value, T limit1, T limit2, T diff) {
    T minLimit = std::min(limit1, limit2) + diff;
    T maxLimit = std::max(limit1, limit2) - diff;
    return  std::min(std::max(value, minLimit),maxLimit);
}


#endif //MATHS_H
