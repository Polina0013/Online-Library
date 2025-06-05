// Copyright 2025 Pozdnova Polina

#include <iostream>
#include <vector>
#include <windows.h>

#include "TVector.h"

void set_color(int text_color, int bg_color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, (bg_color << 4) | text_color);
}

namespace TestSystem {
    int count_success = 0, count_failed = 0;

    void start_test(bool(*test)(), const char* name_of_test) {
        set_color(2, 0);
        std::cout << "[ RUN      ]";
        set_color(7, 0);
        std::cout << name_of_test << std::endl;

        bool status = test();

        if (status == true) {
            set_color(2, 0);
            std::cout << "[       OK ]\n";
            count_success++;
        }
        else {
            set_color(4, 0);
            std::cout << "[  FAILED  ]\n";
            count_failed++;
        }
        set_color(7, 0);
    }

    template <class T>
    bool check(const T& expected, const T& actual) {
        if (expected == actual) {
            return true;
        }
        else {
            std::cerr << "Expected result is " << expected
                << ", but actual is " << actual << "." << std::endl;
            return false;
        }
    }

    void print_init_info() {
        set_color(2, 0);
        std::cout << "[==========] " << std::endl;
        set_color(7, 0);
    }

    void print_final_info() {
        set_color(2, 0);
        std::cout << "[==========] ";
        set_color(7, 0);
        std::cout << count_success + count_failed << " test"
            << (count_success + count_failed > 1 ? "s" : "") << " ran." << std::endl;
        set_color(2, 0);
        std::cout << "[  PASSED  ] ";
        set_color(7, 0);
        std::cout << count_success << " test"
            << (count_success > 1 ? "s" : "") << std::endl;
        if (count_failed > 0) {
            set_color(4, 0);
            std::cout << "[  FAILED  ] ";
            set_color(7, 0);
            std::cout << count_failed << " test"
                << (count_failed > 1 ? "s." : ".") << std::endl;
        }
    }
};  // namespace TestSystem

int main() {
    //std::vector<char> vec{ 1, 2, 3 };
    //vec.resize(5); // Увеличивает размер вектора до 5 элементов.  
    //for (int n : vec) {
    //    std::cout << n << ' '; // Вывод: 1, 2, 3, 0, 0
    //}
    //std::vector<int> vec2{ 1, 2, 3, 4, 5 };
    //vec2.resize(3); // Уменьшает размер вектора до 3 элементов, удаляя последние два элемента.  
    //for (int n : vec2) {
    //    std::cout << n << ' '; // Вывод: 1, 2, 3
    //}
    //std::vector<int> vec3{ 1, 2, 3 };
    //vec3.resize(3, 9); // Новые элементы инициализируются значением 9.  
    //for (int n : vec3) {
    //    std::cout << n << ' '; // Вывод: 1, 2, 3, 9, 9
    //}
    //std::vector<int> vec1{ 1, 2, 3 };
    //vec1.insert(vec1.begin(), 10); // вставляет 10 на позицию 1
    //for (int n : vec1) {
    //    std::cout << n << ' '; // Вывод: 1, 10, 2, 3
    //}
    //std::cout << ' ' << std::endl;

    //std::vector<int> vec{ 1, 2, 3 };
    //vec.erase(vec.begin());
    //for (int n : vec) {
    //    std::cout << n << ' '; // Вывод: 2, 3
    //}




    TestSystem::print_final_info();
    system("pause");
    return 0;
}


