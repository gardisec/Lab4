#include "task1.h"

mutex mtx;

void generateRandomCharsMutex(const int id, const int count, stringstream& stream1) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution distribution(32, 126);
    {
        lock_guard guard(mtx);
        stream1 << "Thread " << id << ": ";
        for (int i = 0; i < count; ++i) {
            const char randomChar = static_cast<char>(distribution(gen));
            stream1 << randomChar << " ";
        }
        stream1 << endl;
    }
}


double mut(bool isDoOutput = false) {
    vector<thread> threads;
    stringstream stream1;
    stream1 << "Mutex: " << endl;
    const auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < THREAD_COUNT; ++i) {
        threads.emplace_back(&generateRandomCharsMutex, i, CHARS_COUNT, ref(stream1));
    }
    for (auto& th : threads) {
        th.join();
    }
    const auto end = chrono::high_resolution_clock::now();
    const chrono::duration<double, std::milli> duration = end - start;
    threads.clear();
    stream1 << endl;
    if (isDoOutput) {
        cout << stream1.str() << endl;
    }
    return duration.count();
}


binary_semaphore smf {1};


void generateRandomCharsSemaphore(const int id, const int count, stringstream& stream1) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution distribution(32, 126);
    smf.acquire();
    stream1 << "Thread " << id << ": ";
    for (int i = 0; i < count; ++i) {
        const char randomChar = static_cast<char>(distribution(gen));
        stream1 << randomChar << " ";
    }
    stream1 << endl;
    smf.release();
}


double semaf(bool isDoOutput = false) {
    vector<thread> threads;
    stringstream stream1;
    stream1 << "Semaphore: " << endl;
    const auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < THREAD_COUNT; ++i) {
        threads.emplace_back(&generateRandomCharsSemaphore, i, CHARS_COUNT, ref(stream1));
    }
    for (auto& th : threads) {
        th.join();
    }
    const auto end = chrono::high_resolution_clock::now();
    const chrono::duration<double, std::milli> duration = end - start;
    threads.clear();
    stream1 << endl;
    if (isDoOutput) {
        cout << stream1.str() << endl;
    }
    return duration.count();
}


class SemaphoreSlim {
public:
    explicit SemaphoreSlim() : count(1) {}

    void wait() {
        while (true) {
            if (int expected = count.load(memory_order_acquire); expected > 0 &&
                count.compare_exchange_strong(expected, expected - 1, memory_order_acquire)) {
                return;
            }
            this_thread::yield(); // ~= wait
        }
    }

    void post() {
        count.fetch_add(1, memory_order_release);
    }

private:
    atomic<int> count;
};


SemaphoreSlim smfS;


void generateRandomCharsSemaphoreSlim(const int id, const int count, stringstream &stream1) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution distribution(32, 126);
    smfS.wait();
    stream1 << "Thread " << id << ": ";
    for (int i = 0; i < count; ++i) {
        const char randomChar = static_cast<char>(distribution(gen));
        stream1 << randomChar << " ";
    }
    stream1 << endl;
    smfS.post();
}


double semafSlim(const bool isDoOutput = false) {
    vector<thread> threads;
    stringstream stream1;
    stream1 << "Semaphore slim: " << endl;
    const auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < THREAD_COUNT; ++i) {
        threads.emplace_back(&generateRandomCharsSemaphoreSlim, i, CHARS_COUNT, ref(stream1));
    }
    for (auto& th : threads) {
        th.join();
    }
    const auto end = chrono::high_resolution_clock::now();
    const chrono::duration<double, std::milli> duration = end - start;
    threads.clear();
    stream1 << endl;
    if (isDoOutput) {
        cout << stream1.str() << endl;
    }
    return duration.count();
}

stringstream barierStream;
barrier whaitPoint(THREAD_COUNT, [](){barierStream << "every thread is ready, START BIG RACE" << endl << endl;});


void generateRandomCharsSemaphoreBarier(const int id, const int count) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution distribution(32, 126);
    {
        lock_guard<mutex> lock(mtx);
        barierStream << "Thread " << id << " is ready" << endl;
    }
    whaitPoint.arrive_and_wait();
    {
        lock_guard<mutex> lock(mtx);
        barierStream << "Thread " << id << ": ";
        for (int i = 0; i < count; ++i) {
            const char randomChar = static_cast<char>(distribution(gen));
            barierStream << randomChar << " ";
        }
        barierStream << endl;
    }
}


double barier(const bool isDoOutput = false) {
    vector<thread> threads;
    barierStream << "Barier: " << endl;
    const auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < THREAD_COUNT; ++i) {
        threads.emplace_back(&generateRandomCharsSemaphoreBarier, i, CHARS_COUNT);
    }
    for (auto& th : threads) {
        th.join();
    }
    const auto end = chrono::high_resolution_clock::now();
    const chrono::duration<double, std::milli> duration = end - start;
    threads.clear();
    barierStream << endl;
    if (isDoOutput) {
        cout << barierStream.str() << endl;
    }
    return duration.count();
}


class SpinLock {
public:
    SpinLock() : isOpen(ATOMIC_FLAG_INIT) {}

    void lock() {
        bool locker = false;
        while (!isOpen.compare_exchange_weak(locker, true, memory_order_acquire)) {

        }
    }

    void unlock() {
        isOpen.store(false , std::memory_order_release);
    }

private:
    atomic_bool isOpen;
};


SpinLock sLock;


void generateRandomCharsSemaphoreSpinLock(const int id, const int count, stringstream &stream1) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution distribution(32, 126);
    sLock.lock();
    stream1 << "Thread " << id << ": ";
    for (int i = 0; i < count; ++i) {
        const char randomChar = static_cast<char>(distribution(gen));
        stream1 << randomChar << " ";
    }
    stream1 << endl;
    sLock.unlock();
}


double spinLock(const bool isDoOutput = false) {
    vector<thread> threads;
    stringstream stream1;
    stream1 << "SpinLock: " << endl;
    const auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < THREAD_COUNT; ++i) {
        threads.emplace_back(&generateRandomCharsSemaphoreSpinLock, i, CHARS_COUNT, ref(stream1));
    }
    for (auto& th : threads) {
        th.join();
    }
    const auto end = chrono::high_resolution_clock::now();
    const chrono::duration<double, std::milli> duration = end - start;
    threads.clear();
    stream1 << endl;
    if (isDoOutput) {
        cout << stream1.str() << endl;
    }
    return duration.count();
}


class SpinWait {
public:
    SpinWait() : isOpen(ATOMIC_FLAG_INIT) {}

    void lock() {
        while (isOpen.test_and_set(memory_order_acquire)) {
        }
    }

    void unlock() {
        isOpen.clear(memory_order_release);
    }

private:
        atomic_flag isOpen;
};


SpinWait sWait;


void generateRandomCharsSemaphoreSpinWait(const int id, const int count, stringstream &stream1) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution distribution(32, 126);
    sWait.lock();
    stream1 << "Thread " << id << ": ";
    for (int i = 0; i < count; ++i) {
        const char randomChar = static_cast<char>(distribution(gen));
        stream1 << randomChar << " ";
    }
    stream1 << endl;
    sWait.unlock();
}


double spinWait(const bool isDoOutput = false) {
    vector<thread> threads;
    stringstream stream1;
    stream1 << "SpinWait: " << endl;
    const auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < THREAD_COUNT; ++i) {
        threads.emplace_back(&generateRandomCharsSemaphoreSpinWait, i, CHARS_COUNT, ref(stream1));
    }
    for (auto& th : threads) {
        th.join();
    }
    const auto end = chrono::high_resolution_clock::now();
    const chrono::duration<double, std::milli> duration = end - start;
    threads.clear();
    stream1 << endl;
    if (isDoOutput) {
        cout << stream1.str() << endl;
    }
    return duration.count();
}


class Monitor {
public:
    void lock() const {
        monitMutex.lock();
    }

    void unlock() const {
        monitMutex.unlock();
    }

    void notify_one() const noexcept {
        monitCond.notify_one();
    }

private:
    mutable mutex monitMutex;
    mutable condition_variable monitCond;
};


Monitor monik;


void generateRandomCharsSemaphoreSpinMonitor(const int id, const int count, stringstream &stream1) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution distribution(32, 126);
    monik.lock();
    stream1 << "Thread " << id << ": ";
    for (int i = 0; i < count; ++i) {
        const char randomChar = static_cast<char>(distribution(gen));
        stream1 << randomChar << " ";
    }
    stream1 << '\n';
    monik.unlock();
    monik.notify_one();
}


double monitor(const bool isDoOutput = false) {
    vector<thread> threads;
    stringstream stream1;
    stream1 << "SpinWait: " << '\n';
    const auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < THREAD_COUNT; ++i) {
        threads.emplace_back(&generateRandomCharsSemaphoreSpinWait, i, CHARS_COUNT, ref(stream1));
    }
    for (auto& th : threads) {
        th.join();
    }
    const auto end = chrono::high_resolution_clock::now();
    const chrono::duration<double, std::milli> duration = end - start;
    threads.clear();
    stream1 << '\n';
    if (isDoOutput) {
        cout << stream1.str() << endl;
    }
    return duration.count();
}

void taskOne() {
    bool isDoOutput = false;
    double mutexTime = mut(isDoOutput);
    double semaforeTime = semaf(isDoOutput);
    double semaforeSlimTime = semafSlim(isDoOutput);
    double barierTime = barier(isDoOutput);
    double spinLockTime = spinLock(isDoOutput);
    double spinWaitTime = spinWait(isDoOutput);
    double monitorTime = monitor(isDoOutput);
    cout << "Mutex time = " << mutexTime << endl;
    cout << "Semafore time = " << semaforeTime << endl;
    cout << "SemaforeSlim time = " << semaforeSlimTime << endl;
    cout << "Barier time = " << barierTime << endl;
    cout << "SpinLock time = " << spinLockTime << endl;
    cout << "SpinWait time = " << spinWaitTime << endl;
    cout << "monitor Time = " << monitorTime << endl;
}