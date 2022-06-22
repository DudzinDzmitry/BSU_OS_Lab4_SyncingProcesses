#include <windows.h>
#include <iostream>
#include <fstream>

bool startProcess(const char *appName, const char *cmdLine) {
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);

    PROCESS_INFORMATION pi;

    if (!CreateProcess(appName, const_cast<char *>(cmdLine), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si,
                       &pi))
        return false;

    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return true;
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    std::cout << "Данная программа позволяет запись и чтение сообщений в/из бинарного файла различными потоками.\n";

    std::cout << "Введите имя бинарного файла, который будет использоваться потоками для обмена сообщениями:\n";
    std::string commFileName;
    std::cin >> commFileName;

    std::cout
            << "Введите максимальное количество записей в бинарном файле, который будет использоваться потоками для обмена сообщениями:";
    int recordCount;
    std::cin >> recordCount;

    std::ifstream commFile(commFileName.c_str(), std::ios::binary);

    std::cout << "Введите количество экземпляров процесса Sender, которые будут запущены:";
    int processCount;
    std::cin >> processCount;

    HANDLE messageSentEvent = CreateEvent(NULL, TRUE, FALSE, "MessageSent");
    HANDLE unreadMessagesSemaphore = CreateSemaphore(NULL, recordCount, recordCount, "UnreadMsg");

    HANDLE senderReadySemaphore = CreateSemaphore(NULL, -processCount, 0, "Ready");
    WaitForSingleObject(senderReadySemaphore, INFINITE);

    int i = 0;
    while (i < processCount) {
        startProcess("sender.exe", commFileName.c_str());
        ++i;
    }

    WaitForSingleObject(senderReadySemaphore, INFINITE);

    bool terminate = false;
    while (!terminate) {
        std::cout << "Введите цифру в соответствии с желаемым действием:\n"
                     "0 - Завершить работу.\n"
                     "1 - Прочитать сообщение.\n";
        byte action;
        std::cin >> action;
        if (action == 0) terminate = true;
        else if (action == 1) {
            if (commFile.eof()) {
                std::cout << "В файле нет сообщений. Receiver переходит в режим ожидания сообщений...\n";
                WaitForSingleObject(messageSentEvent, INFINITE);
            } else {
                char message[20];
                commFile.read(message, 20);
                std::cout << "Запрошенное сообщение:\n" << message << "\n";
                ReleaseSemaphore(unreadMessagesSemaphore, 1, NULL);
            }
        } else std::cout << "Введен неверный код действия.\n";
    }

    commFile.close();

    system("pause");
    return 0;
}