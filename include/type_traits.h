#pragma once

#include <type_traits>
#include <iterator>

namespace nano {

template<typename Iter>
struct is_input_iterator {
    constexpr static bool value = std::is_convertible_v<
        typename std::iterator_traits<Iter>::iterator_category, 
        std::input_iterator_tag>;
};

template<typename Iter>
struct is_output_iterator {
    constexpr static bool value = std::is_convertible_v<
        typename std::iterator_traits<Iter>::iterator_category, 
        std::output_iterator_tag>;
};

template<typename Iter>
struct is_forward_iterator {
    constexpr static bool value = std::is_convertible_v<
        typename std::iterator_traits<Iter>::iterator_category, 
        std::forward_iterator_tag>;
};

template<typename Iter>
struct is_bidirectional_iterator {
    constexpr static bool value = std::is_convertible_v<
        typename std::iterator_traits<Iter>::iterator_category, 
        std::bidirectional_iterator_tag>;
};

template<typename Iter>
struct is_random_access_iterator {
    constexpr static bool value = std::is_convertible_v<
        typename std::iterator_traits<Iter>::iterator_category, 
        std::random_access_iterator_tag>;
};

template<typename Iter>
inline constexpr bool is_input_iterator_v = is_input_iterator<Iter>::value;

template<typename Iter>
inline constexpr bool is_output_iterator_v = is_output_iterator<Iter>::value;

template<typename Iter>
inline constexpr bool is_forward_iterator_v = is_forward_iterator<Iter>::value;

template<typename Iter>
inline constexpr bool is_bidirectional_iterator_v = is_bidirectional_iterator<Iter>::value;

template<typename Iter>
inline constexpr bool is_random_access_iterator_v = is_random_access_iterator<Iter>::value;

} //namespace nano