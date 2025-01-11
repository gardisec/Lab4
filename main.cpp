#include "task1.h"
#include "task2.h"
#include "task3.h"

int main() {
    cout << "Enter number of task(1-3): ";
    int number;
    cin >> number;
    switch (number) {
        case 1:
            taskOne();
            break;
        case 2:
            taskSecond();
            break;
        case 3:
            taskThird();
            break;
        default:
            cout << "Incorrect number" << endl;
    }
    return 0;
}