#ifndef VECTOR_UTILITY_H
#define VECTOR_UTILITY_H

#include <vector>

class CollectionUtility {
public:
    template <class T>
    static std::vector<T> combine(std::vector<T> a, std::vector<T> b);
};

template<class T>
std::vector<T> CollectionUtility::combine(std::vector<T> a, std::vector<T> b) {
    std::vector<T> ab;
    ab.reserve(a.size() + b.size());
    ab.insert(ab.end(), a.begin(), a.end());
    ab.insert(ab.end(), b.begin(), b.end());
    return ab;
}

#endif //VECTOR_UTILITY_H
