# G10_Ashutosh Maharana_2241013027_System Monitor

C++ Win32 System Monitor
A lightweight, real-time system monitor for Windows, built from scratch using the native Win32 API and C++. This application provides a view of system performance and running processes, similar to a simplified Windows Task Manager.

🚀 Features
Real-Time Stats: Displays global CPU and Memory usage, updated every 2 seconds.

Process List: Lists all currently running processes with their:

Process Name

Process ID (PID)

Memory Usage (Working Set Size in KB)

Kill Process: Includes a button to terminate any process selected from the list.

Native Performance: Built directly on the Win32 API for minimal overhead.

🛠️ Tech Stack
Language: C++

Core API: Win32 API (User32, Gdi32)

UI Controls: Windows Common Controls (Comctl32.lib) for the WC_LISTVIEW table.

System Data APIs:

Performance Data Helper (Pdh.lib): Used to get reliable global CPU load.

Toolhelp Library (TlHelp32.h): Used to enumerate all running processes.

PSAPI (Psapi.lib): Used to get global memory stats and per-process memory info.

⚙️ Building the Project
Prerequisites
Windows 10 or 11

Visual Studio 2022 (or newer)

The "Desktop development with C++" workload installed in Visual Studio.

Build Steps
Open the .sln (Solution) file in Visual Studio.

Set the build configuration to Release and x64.

From the top menu, select Build > Build Solution (or press F7).

The executable will be created in the x64/Release folder.

🖥️ How to Use
To use all features, you must run the application as an administrator. This is required to terminate system-level processes.

Navigate to your build output folder (e.g., x64/Release/).

Find the executable (e.g., G10_Ashutosh Maharana_2241013027_SystemMonitor.exe).

Right-click the file and select "Run as administrator".

Once open, the application will automatically load and refresh system data.

Click on any process in the list to select it.

Click the "Kill Selected Process" button to terminate it.

🚧 Known Issues & Future Improvements
Per-Process CPU: The "CPU %" column is currently stubbed as "N/A". Calculating this accurately requires storing previous process times and calculating the delta, which is not yet implemented.

Column Sorting: The process list is not sortable by clicking the column headers.

Static UI: The UI elements are at fixed positions and do not resize when the window is resized.
