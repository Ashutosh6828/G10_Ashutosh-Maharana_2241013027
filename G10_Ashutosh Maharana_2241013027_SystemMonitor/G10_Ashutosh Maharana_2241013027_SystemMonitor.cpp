#include "framework.h"
#include "G10_Ashutosh Maharana_2241013027_SystemMonitor.h"
#include <windows.h>
#include <Pdh.h>
#include <psapi.h>
#include <commctrl.h>
#include <TlHelp32.h>
#include <stdlib.h>

#pragma comment(lib, "pdh.lib")
#pragma comment(lib, "Comctl32.lib")

#define MAX_LOADSTRING 100
#define IDT_TIMER1 1001

static HWND hCpuLabel;
static HWND hMemLabel;
static HWND hProcessList;
static HWND hKillButton;

static PDH_HQUERY cpuQuery;
static PDH_HCOUNTER cpuTotal;

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_G10ASHUTOSHMAHARANA2241013027SYSTEMMONITOR, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_G10ASHUTOSHMAHARANA2241013027SYSTEMMONITOR));

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_G10ASHUTOSHMAHARANA2241013027SYSTEMMONITOR));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_G10ASHUTOSHMAHARANA2241013027SYSTEMMONITOR);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 600, 550, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

void UpdateProcessList()
{
    ListView_DeleteAllItems(hProcessList);

    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        return;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hProcessSnap, &pe32)) {
        CloseHandle(hProcessSnap);
        return;
    }

    wchar_t pidString[20];
    wchar_t memString[50];

    do
    {
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
            FALSE, pe32.th32ProcessID);
        PROCESS_MEMORY_COUNTERS_EX pmc;
        wcscpy_s(memString, L"N/A");

        if (NULL != hProcess)
        {
            if (GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
            {
                swprintf(memString, 50, L"%llu KB", pmc.WorkingSetSize / 1024);
            }
            CloseHandle(hProcess);
        }

        swprintf(pidString, 20, L"%d", pe32.th32ProcessID);

        int rowIndex = ListView_GetItemCount(hProcessList);

        LVITEM lvi;
        lvi.mask = LVIF_TEXT;
        lvi.iItem = rowIndex;
        lvi.iSubItem = 0;
        lvi.pszText = pe32.szExeFile;
        ListView_InsertItem(hProcessList, &lvi);

        ListView_SetItemText(hProcessList, rowIndex, 1, pidString);

        ListView_SetItemText(hProcessList, rowIndex, 2, memString);

        ListView_SetItemText(hProcessList, rowIndex, 3, (LPWSTR)L"N/A");

    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
}

void UpdateSystemData()
{
    UpdateProcessList();

    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);

    int memUsagePercentage = memInfo.dwMemoryLoad;

    PDH_FMT_COUNTERVALUE counterVal;
    PdhCollectQueryData(cpuQuery);
    PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);

    double cpuLoad = counterVal.doubleValue;

    wchar_t memText[100];
    swprintf(memText, 100, L"Memory Usage: %d%%", memUsagePercentage);
    SetWindowText(hMemLabel, memText);

    wchar_t cpuText[100];
    swprintf(cpuText, 100, L"CPU Load: %.1f%%", cpuLoad);
    SetWindowText(hCpuLabel, cpuText);
}

void HandleKillProcess()
{
    int selectedIndex = ListView_GetNextItem(hProcessList, -1, LVNI_SELECTED);

    if (selectedIndex == -1)
    {
        MessageBox(NULL, L"Please select a process from the list first.", L"Error", MB_OK);
        return;
    }

    wchar_t pidString[20];
    ListView_GetItemText(hProcessList, selectedIndex, 1, pidString, 20);

    int pid = _wtoi(pidString);

    if (pid == 0)
    {
        MessageBox(NULL, L"Could not read Process ID.", L"Error", MB_OK);
        return;
    }

    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (hProcess == NULL)
    {
        MessageBox(NULL, L"Could not open process. Try running as Administrator.", L"Access Denied", MB_OK);
        return;
    }

    if (TerminateProcess(hProcess, 1))
    {
        UpdateSystemData();
    }
    else
    {
        MessageBox(NULL, L"Failed to terminate process.", L"Error", MB_OK);
    }

    CloseHandle(hProcess);
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        hCpuLabel = CreateWindowEx(
            0, L"STATIC",
            L"CPU Load: 0%",
            WS_CHILD | WS_VISIBLE,
            10, 10, 150, 20,
            hWnd, (HMENU)1, NULL, NULL);

        hMemLabel = CreateWindowEx(
            0, L"STATIC",
            L"Memory Usage: 0%",
            WS_CHILD | WS_VISIBLE,
            170, 10, 150, 20,
            hWnd, (HMENU)2, NULL, NULL);

        hProcessList = CreateWindowEx(
            0, WC_LISTVIEW,
            L"",
            WS_CHILD | WS_VISIBLE | LVS_REPORT | WS_BORDER | LVS_SINGLESEL,
            10, 40, 560, 400,
            hWnd, (HMENU)3, NULL, NULL);

        LVCOLUMN lvc;
        lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

        lvc.iSubItem = 0;
        lvc.pszText = (LPWSTR)L"Process Name";
        lvc.cx = 180;
        ListView_InsertColumn(hProcessList, 0, &lvc);

        lvc.iSubItem = 1;
        lvc.pszText = (LPWSTR)L"PID";
        lvc.cx = 80;
        ListView_InsertColumn(hProcessList, 1, &lvc);

        lvc.iSubItem = 2;
        lvc.pszText = (LPWSTR)L"Memory Usage";
        lvc.cx = 120;
        ListView_InsertColumn(hProcessList, 2, &lvc);

        lvc.iSubItem = 3;
        lvc.pszText = (LPWSTR)L"CPU %";
        lvc.cx = 80;
        ListView_InsertColumn(hProcessList, 3, &lvc);

        hKillButton = CreateWindowEx(
            0, L"BUTTON",
            L"Kill Selected Process",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            10, 450, 180, 30,
            hWnd, (HMENU)4, NULL, NULL);

        PdhOpenQuery(NULL, NULL, &cpuQuery);
        PdhAddEnglishCounter(cpuQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
        PdhCollectQueryData(cpuQuery);

        SetTimer(hWnd, IDT_TIMER1, 2000, NULL);
        UpdateSystemData();
    }
    break;

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case 4:
            HandleKillProcess();
            break;

        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;

        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;

    case WM_TIMER:
        if (wParam == IDT_TIMER1)
        {
            UpdateSystemData();
        }
        break;

    case WM_DESTROY:
        KillTimer(hWnd, IDT_TIMER1);
        PdhCloseQuery(cpuQuery);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}