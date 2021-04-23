
#include <iostream>
#include "windows.h"
#include "process.h"
#include "stdio.h"

HANDLE evt;
LPCTSTR mailslotName =  L"\\\\.\\mailslot\\$MailslotName$";
HANDLE idMailSlot;
HANDLE mailslotWriter;
HANDLE fileCreationevt;
char str[256] = "12Test3";

DWORD WINAPI firstThr(LPVOID x)
{
    SetEvent(evt);
    WaitForSingleObject(fileCreationevt, INFINITE);
    char buf[256] = {0};
    DWORD bRead;
    if (!ReadFile(idMailSlot, buf, 256, &bRead, NULL))
    {
        printf("mailslot reading error");
        exit(1);
    }
    printf(buf, (int) bRead);
}

DWORD WINAPI secondThr(LPVOID x) 
{
    WaitForSingleObject(evt, INFINITE);
    if(!(mailslotWriter = CreateFile(mailslotName, GENERIC_WRITE, FILE_SHARE_READ,
        NULL, OPEN_EXISTING, 0, NULL)))
    {
        printf("mailslot CreateFile error");
            exit(2);
    }
    DWORD bWritten;
    if (!WriteFile(mailslotWriter, str, strlen(str), &bWritten, NULL))
    {
        printf("mailslot writing error");
        exit(2);
    }
    SetEvent(fileCreationevt);
}


int main()
{
    HANDLE threadsArr[2];
    evt = CreateEvent(NULL, TRUE, FALSE, NULL);
    fileCreationevt = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (! (idMailSlot = CreateMailslot(mailslotName, 0, MAILSLOT_WAIT_FOREVER, NULL))) 
    {
        printf("creating mailslot error");
        exit(3);
    }
    if (!(threadsArr[0] = CreateThread(NULL, 0, firstThr, (LPVOID)1, 0, NULL))) 
    {
        printf("creating first thread error");
        exit(1);
    }
    if (!(threadsArr[1] = CreateThread(NULL, 0, secondThr, (LPVOID)1, 0, NULL)))
    {
        printf("creating second thread error");
        exit(2);
    }
    Sleep(1000);
    return 0;
}




