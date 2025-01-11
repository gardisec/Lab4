#ifndef TASK3_H
#define TASK3_H

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>

using namespace std;

class Waiter {
    mutex mtx; //для синхронизации
    condition_variable cv;
    vector<bool> forks; //состояние вилок: true — свободна, false — занята

public:
    Waiter(int totalForks) : forks(totalForks, true) {}

    void requestPermission(int leftFork, int rightFork) {//запрос разрешения у официанта
        unique_lock<mutex> lock(mtx);

        cv.wait(lock, [this, leftFork, rightFork]() {//ждём, пока обе вилки станут свободными
            return forks[leftFork] && forks[rightFork];
        });

        forks[leftFork] = false;//забираем вилки
        forks[rightFork] = false;
    }

    void releaseForks(int leftFork, int rightFork) {//освобождение вилок
        unique_lock<mutex> lock(mtx);

        forks[leftFork] = true;//освобождаем вилки
        forks[rightFork] = true;
        cv.notify_all();//уведомляем ожидающих философов
    }
};

class Philosopher {
    int id;
    Waiter &waiter;
    mutex &leftFork;
    mutex &rightFork;

public:
    Philosopher(int id, Waiter &waiter, mutex &leftFork, mutex &rightFork)
        : id(id), waiter(waiter), leftFork(leftFork), rightFork(rightFork) {}

    void dine() {
        while (true) {
            think();
            waiter.requestPermission(id, (id + 1) % 5); //запрос разрешения у официанта
            eat();
            waiter.releaseForks(id, (id + 1) % 5); //освобождение вилок
        }
    }

    void think() {
        cout << "Философ " << id << " думает...\n";
        this_thread::sleep_for(chrono::milliseconds(1000 + rand() % 1000));
    }

    void eat() {
        lock(leftFork, rightFork); //захватываем мьютексы

        lock_guard<mutex> leftLock(leftFork, adopt_lock);
        lock_guard<mutex> rightLock(rightFork, adopt_lock);
        cout << "Философ " << id << " ест.\n";
        this_thread::sleep_for(chrono::milliseconds(1000 + rand() % 1000));
        cout << "Философ " << id << " закончил есть.\n";
    }
};

void taskThird();

#endif //TASK3_H
