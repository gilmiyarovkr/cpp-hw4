#include <gtest/gtest.h>
#include <sstream>
#include <cstdint>
#include "print_ip.h"

// Вспомогательный класс-фикстура для перехвата std::cout
class PrintIpTest : public ::testing::Test {
protected:
    std::stringstream buffer;
    std::streambuf* old_cout;

    void SetUp() override {
        // Перенаправляем std::cout в наш буфер перед каждым тестом
        old_cout = std::cout.rdbuf(buffer.rdbuf());
    }

    void TearDown() override {
        // Возвращаем std::cout в исходное состояние после каждого теста
        std::cout.rdbuf(old_cout);
    }

    // Вспомогательный метод для получения строки без финального переноса строки \n
    std::string getOutput() {
        std::string res = buffer.str();
        if (!res.empty() && res.back() == '\n') {
            res.pop_back();
        }
        return res;
    }
};

// =================================================================
// 1. ТЕСТЫ СТАТИЧЕСКОЙ ВАЛИДАЦИИ (SFINAE / compile-time)
// =================================================================

TEST(StaticFiltersTest, StringValidation) {
    EXPECT_TRUE(is_string<std::string>::value);
    EXPECT_FALSE(is_string<int>::value);
    EXPECT_FALSE(is_string<std::vector<char>>::value);
}

TEST(StaticFiltersTest, IterableValidation) {
    EXPECT_TRUE(is_iterable<std::vector<int>>::value);
    EXPECT_TRUE(is_iterable<std::list<uint8_t>>::value);
    EXPECT_FALSE(is_iterable<int32_t>::value);
}

TEST(StaticFiltersTest, TupleValidation) {
    // Гомогенные кортежи разрешены
    EXPECT_TRUE((is_tuple<std::tuple<int, int, int>>::value));
    EXPECT_TRUE((is_tuple<std::tuple<std::string, std::string>>::value));

    // Гетерогенные кортежи строго запрещены
    EXPECT_FALSE((is_tuple<std::tuple<int, double, int>>::value));
    EXPECT_FALSE((is_tuple<std::tuple<int, std::string>>::value));
}

// =================================================================
// 2. ДИНАМИЧЕСКИЕ ТЕСТЫ ВЫВОДА (print_ip)
// =================================================================

// Тест 1: Целочисленные типы (от старшего байта к младшему)
TEST_F(PrintIpTest, IntegralTypes) {
    int32_t ip_int = 2130706433; // 0x7F000001
    print_ip(ip_int);
    EXPECT_EQ(getOutput(), "127.0.0.1");
    buffer.str(""); buffer.clear();

    int16_t small_int = 0; // 0x0000
    print_ip(small_int);
    EXPECT_EQ(getOutput(), "0.0");
    buffer.str(""); buffer.clear();

    // Тест на корректность знаковых/беззнаковых (число -1 во всех байтах содержит 255)
    int8_t negative_one = -1;
    print_ip(negative_one);
    EXPECT_EQ(getOutput(), "255");
}

// Тест 2: Строковые типы (вывод без изменений)
TEST_F(PrintIpTest, StringType) {
    std::string ip_str = "192.168.50.11";
    print_ip(ip_str);
    EXPECT_EQ(getOutput(), "192.168.50.11");
}

// Тест 3: Контейнеры (vector и list)
TEST_F(PrintIpTest, ContainerTypes) {
    std::vector<int> ip_vec = {10, 0, 0, 1};
    print_ip(ip_vec);
    EXPECT_EQ(getOutput(), "10.0.0.1");
    buffer.str(""); buffer.clear();

    // Проверяем, что uint8_t (char) выводится как числа, а не как ASCII-символы
    std::list<uint8_t> ip_list = {172, 16, 254, 1};
    print_ip(ip_list);
    EXPECT_EQ(getOutput(), "172.16.254.1");
}

// Тест 4: Гомогенные кортежи
TEST_F(PrintIpTest, TupleTypes) {
    auto ip_tuple = std::make_tuple(192, 168, 0, 254);
    print_ip(ip_tuple);
    EXPECT_EQ(getOutput(), "192.168.0.254");
}
