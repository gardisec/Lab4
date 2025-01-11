#include "task2.h"

string generateRandomString(size_t length) {
    const string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, chars.size() - 1);
    string result;
    for (size_t i = 0; i < length; ++i) {
        result += chars[dis(gen)];
    }
    return result;
}

// Функция для генерации случайной посылки
Parcel generateRandomParcel() {
    Parcel p;
    // Генерация кода товара в формате AB123
    p.productCode = "AB" + to_string(rand() % 1000); // от AB000 до AB999
    while (p.productCode.length() < 5) {
        p.productCode.insert(2, "0"); // Добавляем ведущие нули для длины 5
    }
    p.city = generateRandomString(8); // Генерация случайного города
    p.recipientName = generateRandomString(6) + " " + generateRandomString(8); // ФИО
    return p;
}


// Функция для обработки данных без многопоточности
void processWithoutThreads(const vector<Parcel>& parcels, const regex& pattern, vector<string>& result) {
    for (const auto& parcel : parcels) {
        if (regex_match(parcel.productCode, pattern)) {
            stringstream ss(parcel.recipientName);
            string firstName, lastName;
            ss >> firstName >> lastName;  // Извлекаем фамилию
            result.push_back(lastName);
        }
    }
}

// Функция для обработки данных с многопоточностью
void processWithThreads(const vector<Parcel>& parcels, const regex& pattern, vector<string>& result, size_t start, size_t end, mutex& resultMutex) {
    vector<string> localResults;
    for (size_t i = start; i < end; ++i) {
        if (regex_match(parcels[i].productCode, pattern)) {
            stringstream ss(parcels[i].recipientName);
            string firstName, lastName;
            ss >> firstName >> lastName;
            localResults.push_back(lastName);
        }
    }

    // Защищаем доступ к результатам с помощью мьютекса
    lock_guard<mutex> guard(resultMutex);
    result.insert(result.end(), localResults.begin(), localResults.end());
}

void taskSecond() {
    size_t numParcels = 100000;  // количество посылок
    size_t numThreads = 8;       // количество потоков

    vector<Parcel> parcels(numParcels);
    for (auto& parcel : parcels) {
        parcel = generateRandomParcel();
    }

    regex pattern("AB[0-9]{3}"); // шаблон для поиска по коду товара

    // Обработка без потоков
    vector<string> resultWithoutThreads;
    auto start = chrono::high_resolution_clock::now();
    processWithoutThreads(parcels, pattern, resultWithoutThreads);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> durationWithoutThreads = end - start;
    cout << "Time without threads: " << durationWithoutThreads.count() << " seconds" << endl;

    // Обработка с потоками
    vector<string> resultWithThreads;
    mutex resultMutex;
    start = chrono::high_resolution_clock::now();
    vector<thread> threads;
    size_t chunkSize = numParcels / numThreads;

    for (size_t i = 0; i < numThreads; ++i) {
        size_t startIdx = i * chunkSize;
        size_t endIdx = (i == numThreads - 1) ? numParcels : (i + 1) * chunkSize; // обрабатываем оставшиеся данные в последнем потоке
        threads.emplace_back(processWithThreads, cref(parcels), cref(pattern), ref(resultWithThreads), startIdx, endIdx, ref(resultMutex));
    }

    for (auto& t : threads) { // окончание работы потоков
        t.join();
    }
    end = chrono::high_resolution_clock::now();
    chrono::duration<double> durationWithThreads = end - start;
    cout << "Time with threads: " << durationWithThreads.count() << " seconds" << endl;

    // Выводим количество результатов, чтобы проверить, сколько элементов было найдено
    cout << "Results found (without threads): " << resultWithoutThreads.size() << endl;
    cout << "Results found (with threads): " << resultWithThreads.size() << endl;

    // Вывод первых 10 фамилий (чтобы убедиться в правильности работы)
    cout << "First 10 results (without threads):" << endl;
    for (size_t i = 0; i < min(resultWithoutThreads.size(), size_t(10)); ++i) {
        cout << resultWithoutThreads[i] << endl;
    }

    cout << "First 10 results (with threads):" << endl;
    for (size_t i = 0; i < min(resultWithThreads.size(), size_t(10)); ++i) {
        cout << resultWithThreads[i] << endl;
    }
}
