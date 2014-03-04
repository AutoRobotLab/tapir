#ifndef GLOBAL_HPP_
#define GLOBAL_HPP_

#include <memory>                       // for unique_ptr
#include <random>                       // for default_random_engine
#include <sstream>
#include <utility>                      // for forward

#define _NO_COPY_OR_MOVE(ClassName) \
    ClassName(ClassName const &) = delete; \
    ClassName(ClassName &&) = delete; \
    ClassName &operator=(ClassName const &) = delete; \
    ClassName &operator=(ClassName &&) = delete

typedef std::default_random_engine RandomGenerator;

namespace std {
    template<class T> struct _Unique_if {
        typedef unique_ptr<T> _Single_object;
    };

    template<class T> struct _Unique_if<T[]> {
        typedef unique_ptr<T[]> _Unknown_bound;
    };

    template<class T, size_t N> struct _Unique_if<T[N]> {
        typedef void _Known_bound;
    };

    template<class T, class... Args>
        typename _Unique_if<T>::_Single_object
        make_unique(Args&&... args) {
            return unique_ptr<T>(new T(std::forward<Args>(args)...));
        }

    template<class T>
        typename _Unique_if<T>::_Unknown_bound
        make_unique(size_t n) {
            typedef typename remove_extent<T>::type U;
            return unique_ptr<T>(new U[n]());
        }

    template<class T, class... Args>
        typename _Unique_if<T>::_Known_bound
        make_unique(Args&&...) = delete;
}

namespace abt {
template<class T>
void hash_combine(std::size_t &seed, T const &v) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}
}

namespace debug {
    void show_message(std::string message, bool print = true,
            bool bp_branch = true);

    template <typename T>
    std::string to_string(T t) {
        std::ostringstream sstr;
        sstr << t;
        return sstr.str();
    }
} /* namespace debug */

#endif /* GLOBAL_HPP_ */
