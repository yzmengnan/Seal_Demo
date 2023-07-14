//
// Created by 91418 on 2023/7/5.
//
// main.cpp
#include <windows.h>
#include"iostream"
#define buffsize 4096
using namespace std;

int main(int argc, char* argv[])
{
//    SetConsoleOutputCP(CP_UTF8);
    cout<<"\n ** This is a message from the monitor process. ** \n";
    CHAR chBuf[buffsize];
    DWORD len;
    HANDLE handle_read;
    handle_read = GetStdHandle(STD_INPUT_HANDLE);
    if (handle_read == INVALID_HANDLE_VALUE)
        ExitProcess(1);

    for (;;)
    {
        // Read from standard input and stop on error or no data.
        bool ret = ReadFile(handle_read, chBuf, buffsize, &len, NULL);
        if (!ret || len == 0)
        {
            cout << "ReadFile error!" << endl;
            break;
        }
        cout <<chBuf << endl;
        Sleep(10);
    }
    cout << "finish reading!" << endl;
    Sleep(5000);
    freopen("CON", "r", stdin);    // 重定向输入,否则system("pause")会无效
    //CloseHandle(handle_read);
//    system("pause");
    return 0;
}

