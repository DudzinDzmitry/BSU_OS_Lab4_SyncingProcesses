#include <windows.h>
#include <fstream>
#include <iostream>

int main(int argc, char* argv[]) {
    std::ofstream commFile(argv[0], std::ios::binary);

    HANDLE messageSentEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, "MessageSent");
    HANDLE unreadMessagesSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "UnreadMsg");

    HANDLE senderReadySemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "Ready");
    ReleaseSemaphore(senderReadySemaphore, 1, NULL);

    SetConsoleOutputCP(CP_UTF8);
    bool terminate = false;
    while (!terminate) {
        std::cout << "Введите цифру в соответствии с желаемым действием:\n"
                     "0 - Завершить работу.\n"
                     "1 - Отправить сообщение.\n";
        short action;
        std::cin >> action;
        if (action == 0) terminate = true;
        else if (action == 1) {
            if (WaitForSingleObject(unreadMessagesSemaphore, 0) != WAIT_OBJECT_0) {
                std::cout << "В файле слишком много непрочитанных сообщений. Sender переходит в режим ожидания...\n";
                WaitForSingleObject(unreadMessagesSemaphore, INFINITE);
            } else {
                std::cout << "Введите сообщение:\n";
                char messageString[20];
                std::cin >> messageString;
                commFile.write(messageString, 20);
                ReleaseSemaphore(unreadMessagesSemaphore, -1, NULL);
                PulseEvent(messageSentEvent);
            }
        } else std::cout << "Введен неверный код действия.\n";
    }

    commFile.close();
    return 0;
}