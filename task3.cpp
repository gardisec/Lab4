#include "task3.h"

void taskThird() {
    const int numPhilosophers = 5;

    vector<mutex> forks(numPhilosophers); //создаём мьютексы для вилок
    Waiter waiter(numPhilosophers); //создаём официанта

    vector<thread> philosopherThreads; //потоки для философов

    for (int i = 0; i < numPhilosophers; ++i) {//создаём философов и потоки
        philosopherThreads.emplace_back(
            [i, &waiter, &forks]() {
                Philosopher philosopher(i, waiter, forks[i], forks[(i + 1) % numPhilosophers]);
                philosopher.dine(); //запускаем метод dine() в потоке
            }
        );
    }
    for (auto &thread : philosopherThreads) {//ожидаем завершение потоков
        thread.join();
    }
}
