#ifndef TASK2_H
#define TASK2_H

#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <regex>
#include <atomic>
#include <random>
#include <string>
#include <sstream>
#include <mutex>

using namespace std;

struct Parcel {
    string productCode;
    string city;
    string recipientName; // ФИО
};

void taskSecond();

#endif //TASK2_H
