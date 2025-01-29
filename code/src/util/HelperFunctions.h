#pragma once
#include <string>
#include <random>



//Just a random string generator (https://stackoverflow.com/questions/440133/how-do-i-create-a-random-alpha-numeric-string-in-c)
inline std::string random_string( size_t length )
{
    auto randchar = []() -> char
    {
        const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[ rand() % max_index ];
    };
    std::string str(length,0);
    std::generate_n( str.begin(), length, randchar );
    return str;
}

//Just a random Int generator (https://stackoverflow.com/questions/7560114/random-number-c-in-some-range)
inline int getRandomNumber(int min, int max)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(min, max);
    return distr(gen);
}
