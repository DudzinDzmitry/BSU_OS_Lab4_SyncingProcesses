#include <windows.h>
#include <fstream>
#include <iostream>

int main(int argc, char* argv[]) {
    std::ofstream commFile(argv[0], std::ios::binary);

    HANDLE messageSentEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, "MessageSent");

    HANDLE senderReadySemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "Ready");
    ReleaseSemaphore(senderReadySemaphore, 1, NULL);

    SetConsoleOutputCP(CP_UTF8);
    bool terminate = false;
    while (!terminate) {
        std::cout << "Введите цифру в соответствии с желаемым действием:\n"
                     "0 - Завершить работу.\n"
                     "1 - Отправить сообщение.\n";
        byte action;
        std::cin >> action;
        if (action == 0) terminate = true;
        else if (action == 1) {
            std::cout << "Введите сообщение:\n";
            std::string messageString;
            std::cin >> messageString;
            commFile.write((char *) &messageString, sizeof(std::string));
        } else std::cout << "Введен неверный код действия.\n";
    }

    commFile.close();
    return 0;
}