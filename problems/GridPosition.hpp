#ifndef GRIDPOSITION_HPP
#define GRIDPOSITION_HPP

struct GridPosition {
    long i;
    long j;
    GridPosition() :
        i(0),
        j(0) {
    }
    GridPosition(long i, long j) :
        i(i),
        j(j) {
    }

    double distanceTo(GridPosition const &other) const {
        return std::abs(i - other.i) + std::abs(j - other.j);
    }
};

inline std::ostream &operator<<(std::ostream &os, GridPosition const &obj) {
    os << "(" << obj.i << ", " << obj.j << ")";
    return os;
}

inline bool operator==(GridPosition const &lhs, GridPosition const &rhs) {
    return lhs.i == rhs.i && lhs.j == rhs.j;
}

inline bool operator!=(GridPosition const &lhs, GridPosition const &rhs) {
    return lhs.i != rhs.i || lhs.j != rhs.j;
}

#endif /* GRIDPOSITION_HPP */
