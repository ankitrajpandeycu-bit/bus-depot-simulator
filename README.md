Bus Depot Operation Simulator

A graphical simulator of a bus depot, written in C with raylib. Register buses and drivers, park buses in a limited number of bays, schedule trips, dispatch buses after rule checks, and watch the depot state change live. Records are saved to files and loaded again at startup.

Features
Bus and driver management: register, delete, and mark buses as under maintenance.
Parking bays: 5 bays; two buses can never share a bay.
Waiting queue: when every bay is full, buses wait in a first-in-first-out queue and are parked automatically when a bay frees up.
Trips: schedule a trip, assign a driver, dispatch, and return.
Dispatch rules: a bus is dispatched only if the trip is scheduled, the bus is in service, the bus is parked in a bay, and a driver is assigned. Every failed check is shown.
Live dashboard: totals, bays used and vacant, waiting buses, trip counts, driver availability, and an event log.
File handling: buses, drivers and trips are saved to binary files and restored on restart.
C concepts demonstrated

Structures, enums, arrays, pointers, a circular queue, conditional statements, loops, binary file handling, and modular programming with header files.

Project structure
src/
├── models.h        Bus, Driver, Trip structures, status enums, limits
│  ---- depot logic (plain C, no graphics) ----
├── state.c/.h      Global arrays + find helpers
├── queue.c/.h      Circular waiting queue
├── depot.c/.h      All rules: park, release, schedule, assign, dispatch, return, delete
├── eventlog.c/.h   Event messages + bay flash effect
├── storage.c/.h    saveData() / loadData()
│  ---- user interface (raylib) ----
├── ui_layout.h     Positions of panels, shared UI state
├── ui_draw.c/.h    Colours and drawing helpers
├── ui_scene.c/.h   Draws the whole screen
├── ui_input.c/.h   Mouse and keyboard -> depot functions
└── main.c          Window, main loop, load/save

The depot logic never calls the interface: it only changes data and reports through logMsg(). The screen reads that data and draws it.

Build and run
Windows (MSYS2 UCRT64)
Install MSYS2, open the MSYS2 UCRT64 terminal and run:
pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-raylib
Put the MSYS2 ucrt64\bin folder on your PATH (or set it for the current terminal), then in the project folder run:
gcc src/main.c src/state.c src/queue.c src/eventlog.c src/depot.c src/storage.c src/ui_draw.c src/ui_scene.c src/ui_input.c -o BusDepot.exe -I src -lraylib -lopengl32 -lgdi32 -lwinmm
./BusDepot.exe

If the compiler cannot find raylib, add -I <msys2>/ucrt64/include -L <msys2>/ucrt64/lib.

Linux
sudo apt install build-essential libraylib-dev     # or build raylib from source
make run
macOS
brew install raylib
gcc src/*.c -Isrc -o BusDepot -lraylib -framework OpenGL -framework Cocoa -framework IOKit
./BusDepot
How to use

Click a bus (fleet list, bay, queue or road) to select it, then click a button.

Button	Action
ADD BUS / ADD DRV	Register a bus or driver (details are generated)
PARK	Park in a free bay, or join the waiting queue if none is free
RELEASE	Bus leaves its bay; a waiting bus takes it automatically
SCHEDULE	Create a trip; a free driver is assigned automatically
ASSIGN	Give the scheduled trip a free driver
DISPATCH	Check all rules, then Parked -> On Trip
RETURN	On Trip -> Available; the driver becomes free
SAVE	Write buses.dat, drivers.dat, trips.dat

Keys: Del deletes the selected bus, M toggles maintenance, Esc saves and quits. To delete a driver, click the driver in the Drivers panel, then press D (a driver assigned to a trip cannot be deleted).

Bus life cycle
Available --park--> Parked --dispatch--> On Trip --return--> Available
    |                  ^
    +-- all bays full -+--> Waiting queue (auto-parked when a bay is free)
Limits

50 buses, 50 drivers, 200 trips, 5 bays. The waiting queue is not saved between runs. Bus numbers, driver names and destinations are generated automatically.

Authors

Ankit Raj Pandey and Akash Saraswat, Chandigarh University (BE CSE, Advanced C Programming).
