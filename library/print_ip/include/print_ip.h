#pragma once

#include <iostream>
#include <type_traits>
#include <list>
#include <vector>
#include <tuple>

// Базовые шаблоны
template <typename T>
struct is_string : std::false_type{};

template <typename T, typename = void>
struct is_iterable : std::false_type {};

template <typename T>
struct is_tuple : std::false_type {};

// Частичная специализация
template <typename CharT, typename Traits, typename Allocator>
struct is_string<std::basic_string<CharT, Traits, Allocator>> :std::true_type{};

template <typename T>
struct is_iterable<T, std::void_t<
                          decltype(std::begin(std::declval<T>())),
                          decltype(std::end(std::declval<T>()))
                          >> : std::true_type {};

template <>
struct is_tuple<std::tuple<>> : std::true_type {};

template <typename First, typename... Args>
struct is_tuple<std::tuple<First, Args...>> {
    static constexpr bool value = (std::is_same_v<First, Args> && ...);
};

// целые числа
template <typename T>
std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>, void>
print_ip(const T& value) {
    // Приводим адрес переменной к указателю на беззнаковый байт
    const auto* bytePtr = reinterpret_cast<const unsigned char*>(&value);
    constexpr size_t numBytes = sizeof(T);

    // На Little-Endian системах старшие байты лежат в конце, поэтому идем с конца
    for (size_t i = 0; i < numBytes; ++i) {
        // numBytes - 1 - i для Big-Endian порядка на Little-Endian процессоре
        size_t index = numBytes - 1 - i;

        std::cout << static_cast<unsigned int>(bytePtr[index]);

        if (i < numBytes - 1) {
            std::cout << ".";
        }
    }
    std::cout << std::endl;
}

// std::string
template <typename T>
std::enable_if_t<is_string<T>::value, void>
print_ip(const T& target) {
    std::cout << target << std::endl;
}

// vector, list, ...
template <typename T>
std::enable_if_t<is_iterable<T>::value && !is_string<T>::value, void>
print_ip(const T& ltarget) {
    for(auto l = ltarget.begin(); l != ltarget.end(); ++l){
        std::cout << +(*l);
        if(std::next(l) != ltarget.end())
            std::cout << ".";
    }
    std::cout << std::endl;
}

// свертка
template <typename TupleT, std::size_t... Is>
void print_ip_tuple_impl(const TupleT& tp, std::index_sequence<Is...>) {
    ( (void)(Is > 0 ? (std::cout << ".", std::cout << std::get<Is>(tp))
                    : (std::cout << std::get<Is>(tp))), ... );
    std::cout << std::endl;
}

// std::tuple
template <typename T>
std::enable_if_t<is_tuple<T>::value, void>
print_ip(const T& target) {
    print_ip_tuple_impl(target, std::make_index_sequence<std::tuple_size_v<T>>{});
}
