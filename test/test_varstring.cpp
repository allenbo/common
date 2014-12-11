#include "common/varstring.hpp"
#include <cassert>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

static const char* alphabeta_number = "abcdefghijklmnopqrstuvwxyzABCDEFGHILKLMNOPQRSTUVWXYZ0123456789";

using namespace common;

std::string random_string(int size) {
    char* tmp = new char[size];
    int len = strlen(alphabeta_number);
    int ind;
    for (int i = 0; i < size; i ++ ) {
        ind = rand() % len;
        tmp[i] = alphabeta_number[ind];
    }
    std::string rst(tmp, size);
    delete [] tmp;
    return rst;
}

int main() {
    // test for small append
    std::string str1 = "abc";
    std::string str2 = "def";
    const char* str3 = "efg";
    char c4 = 'h';
    const char* str5 = "ijklmn";

    VarString vs1;
    vs1.append(str1).append(str2).append(str3).append(c4).append(str5, 2);
    assert(vs1.toString() == str1 + str2 + str3 + c4 + "ij");

    // test for massive append
    std::string answer = "";
    int round = 100;
    int min = 200;
    int max = 2000;

    int round_id = 0;
    VarString vs2;
    while(round_id < round) {
        std::string str;
        if (round_id % 2) {
            str = random_string(rand() % (max - min) + min); 
        } else {
            str = random_string(rand() % (min - 10) + 10);
        }
        vs2.append(str); 
        answer += str;
        round_id ++;
    }

    assert(vs2.toString() == answer);


    // test for other function
    {
        std::string str1 = random_string(10);
        std::string str2 = str1.substr(0, str1.size() - 1);
        VarString vs;
        vs.append(str1);
        vs.drop();
        assert(vs.toString() == str2);
        vs.clear();

        vs.append(str1);
        int len = 5;
        std::string str3 = str1.substr(0, str1.size() - len);
        vs.drop(len);
        assert(vs.toString() == str3);
    }

    std::cout << VarString::format("This is an integer %d", 2) << std::endl;

}
