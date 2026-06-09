#pragma once

#include <iostream>
#include <type_traits>
#include <list>
#include <vector>
#include <tuple>

/**
 * @file
 * @brief Набор шаблонов функций print_ip для вывода IP-адресов из различных типов данных.
 */

/**
 * @brief Шаблонный тип для проверки, является ли тип строкой.
 * @details Базовый шаблон определяет, что тип не является строкой.
 * @tparam T Проверяемый тип.
 */
template <typename T>
struct is_string : std::false_type{};

/**
 * @brief Шаблонный тип для проверки, является ли тип итерируемым (контейнером).
 * @details Базовый шаблон использует SFINAE с std::void_t.
 * @tparam T Проверяемый тип.
 */
template <typename T, typename = void>
struct is_iterable : std::false_type {};

/**
 * @brief Шаблонный тип для проверки, является ли тип std::tuple с одинаковыми типами элементов.
 * @details Базовый шаблон определяет, что тип не является кортежем.
 * @tparam T Проверяемый тип.
 */
template <typename T>
struct is_tuple : std::false_type {};

/**
 * @brief Частичная специализация is_string для std::basic_string.
 */
template <typename CharT, typename Traits, typename Allocator>
struct is_string<std::basic_string<CharT, Traits, Allocator>> :std::true_type{};

/**
 * @brief Частичная специализация is_iterable для типов, поддерживающих std::begin и std::end.
 */
template <typename T>
struct is_iterable<T, std::void_t<
                          decltype(std::begin(std::declval<T>())),
                          decltype(std::end(std::declval<T>()))
                          >> : std::true_type {};

/**
 * @brief Полная специализация is_tuple для пустого кортежа std::tuple<>.
 */
template <>
struct is_tuple<std::tuple<>> : std::true_type {};

/**
 * @brief Частичная специализация is_tuple для непустого кортежа.
 * @details Проверяет, что все элементы кортежа имеют строго одинаковый тип.
 * @tparam First Тип первого элемента кортежа.
 * @tparam Args Типы остальных элементов кортежа.
 */
template <typename First, typename... Args>
struct is_tuple<std::tuple<First, Args...>> {
    /** Флаг, истинный только если типы всех элементов совпадают. */
    static constexpr bool value = (std::is_same_v<First, Args> && ...);
};

/**
 * @brief Выводит IP-адрес, представленный целочисленным типом.
 * @details Разбивает число на байты и выводит их через точку в порядке Big-Endian
 *          (старшие байты первыми, адаптировано под Little-Endian архитектуру). Исключает тип bool.
 * @tparam T Целочисленный тип (int, char, short, long long и т.д.).
 * @param value Значение IP-адреса.
 */
template <typename T>
std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>, void>
print_ip(const T& value) {
    const auto* bytePtr = reinterpret_cast<const unsigned char*>(&value);
    constexpr size_t numBytes = sizeof(T);

    for (size_t i = 0; i < numBytes; ++i) {
        size_t index = numBytes - 1 - i;

        std::cout << static_cast<unsigned int>(bytePtr[index]);

        if (i < numBytes - 1) {
            std::cout << ".";
        }
    }
    std::cout << std::endl;
}

/**
 * @brief Выводит IP-адрес, представленный в виде строки.
 * @details Выводит строку в стандартный поток вывода «как есть».
 * @tparam T Тип строки (std::string).
 * @param target Строковое представление IP-адреса.
 */
template <typename T>
std::enable_if_t<is_string<T>::value, void>
print_ip(const T& target) {
    std::cout << target << std::endl;
}

/**
 * @brief Выводит IP-адрес, представленный в виде контейнера (vector, list и др.).
 * @details Итерируется по элементам контейнера, выводя их через точку.
 *          Использует унарный плюс для корректного вывода символьных типов как чисел.
 * @tparam T Тип контейнера.
 * @param ltarget Контейнер с байтами IP-адреса.
 */
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

/**
 * @brief Вспомогательная функция для раскрытия элементов std::tuple.
 * @details Использует свёртку параметров (fold expression) и std::index_sequence для обхода кортежа.
 * @tparam TupleT Тип кортежа.
 * @tparam Is Сгенерированные индексы элементов.
 * @param tp Ссылка на кортеж.
 */
template <typename TupleT, std::size_t... Is>
void print_ip_tuple_impl(const TupleT& tp, std::index_sequence<Is...>) {
    ( (void)(Is > 0 ? (std::cout << ".", std::cout << std::get<Is>(tp))
                    : (std::cout << std::get<Is>(tp))), ... );
    std::cout << std::endl;
}

/**
 * @brief Выводит IP-адрес, представленный в виде std::tuple.
 * @details Вызывает функцию вывода только в том случае, если все элементы кортежа однородны.
 * @tparam T Тип кортежа.
 * @param target Кортеж с компонентами IP-адреса.
 */
template <typename T>
std::enable_if_t<is_tuple<T>::value, void>
print_ip(const T& target) {
    print_ip_tuple_impl(target, std::make_index_sequence<std::tuple_size_v<T>>{});
}
