#include <windows.h>
#include <fstream>

int main(int argc, char* argv[]) {
    std::ofstream commFile(argv[0], std::ios::binary);

    HANDLE senderReadySemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "Ready");

    ReleaseSemaphore(senderReadySemaphore, 1, NULL);

    return 0;
}