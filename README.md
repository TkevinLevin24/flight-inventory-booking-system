# Flight Inventory & Booking System

**A C++17 command-line application for managing airline flight inventory, passengers, and booking lifecycles, with CSV persistence and automated tests.**

This project explores core backend concerns found in airline reservation systems: seat availability, booking state transitions, data consistency, file-based persistence, and recovery after application restarts. It is a portfolio and learning project, **not a production airline reservation platform**.

## Features

- **Flight inventory:** register flights, look up flights, and track total and available seats.
- **Passenger management:** register and retrieve passenger records.
- **Booking lifecycle:** create confirmed bookings, cancel them, and retain cancellation history.
- **Seat consistency:** confirmed bookings consume one seat; cancellations release it. Duplicate confirmed bookings for the same passenger and flight are rejected.
- **CSV persistence:** load flights, passengers, and bookings when the application starts; save new flights, passengers, and booking changes.
- **CSV validation:** check headers, required fields, identifiers, references, booking states, and available capacity before restoring bookings.
- **Safer booking writes:** write booking data to a temporary file before attempting to replace the destination CSV.
- **Booking service:** coordinates booking creation, cancellation, and persistence operations.
- **Rollback on save failure:** restores the previous in-memory booking and seat state when a persistence operation reports failure.
- **Failure protection:** blocks further booking modifications after a detected persistence failure.
- **Interactive CLI:** numbered menu with input validation, booking search, and system statistics.
- **Automated tests:** six CTest suites covering domain behavior, booking services, integration, CSV storage, and persistence failure recovery.


## Technology Stack

| Component | Technology |
| --- | --- |
| Language | C++17 |
| Build system | CMake |
| Compiler / environment | MSVC, Visual Studio 2022, Windows PowerShell |
| Testing | CTest and C++ test executables |
| Persistence | CSV files (`std::fstream`, `std::filesystem`) |
| Data structures | `std::unordered_map`, `std::unique_ptr` |

## Project Structure

```text
flight-inventory-booking-system/
├── include/
│   ├── Flight.h
│   ├── Passenger.h
│   ├── Booking.h
│   ├── BookingSystem.h
│   ├── CsvStorage.h
│   └── BookingService.h
├── src/
│   ├── main.cpp
│   ├── Flight.cpp
│   ├── Passenger.cpp
│   ├── Booking.cpp
│   ├── BookingSystem.cpp
│   ├── BookingService.cpp
│   └── CsvStorage.cpp
├── tests/
│   ├── test_flight.cpp
│   ├── test_booking.cpp
│   ├── test_booking_system.cpp
│   ├── test_booking_integration.cpp
│   ├── test_booking_service.cpp
│   └── test_csv_storage.cpp
├── data/
│   ├── flights.csv
│   ├── passengers.csv
│   └── bookings.csv
├── CMakeLists.txt
├── .gitignore
├── LICENSE
└── README.md
```

## Architecture

The application separates domain logic from storage and user interaction:

```text
          Command-line interface
                main.cpp
                   |
                   v
             BookingSystem
           /      |       \
        Flight  Passenger  Booking
                   |
                   v
               CsvStorage
                   |
                   v
          CSV files in data/
```

`BookingSystem` coordinates flight, passenger, and booking records. `Booking` models booking status and seat reservation/release. `CsvStorage` handles serialization and restoration. The domain entities are also exercised independently of the CLI through automated tests.

### Booking states

| State | Meaning | Seat occupied? |
| --- | --- | --- |
| `PENDING` | Booking created but not confirmed | No |
| `CONFIRMED` | Seat successfully reserved | Yes |
| `CANCELLED` | Booking cancelled; history retained | No |

The interactive booking flow creates confirmed bookings. `PENDING` is also supported by the underlying model and CSV restoration logic.

## Getting Started

### Prerequisites

- Windows with **Visual Studio 2022** and the **Desktop development with C++** workload
- CMake
- Git (to clone the repository)

Run the following commands from the repository root in PowerShell.

### 1. Clone the repository

```powershell
git clone https://github.com/TkevinLevin24/flight-inventory-booking-system.git
cd flight-inventory-booking-system
```

### 2. Configure and build

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

### 3. Run the application

```powershell
.\build\Release\flight-booking.exe
```

> Run the executable **from the repository root**. The application currently uses relative paths such as `data/flights.csv`.

### Main menu

```text
========================================
       FLIGHT BOOKING SYSTEM
========================================
1. Register flight
2. Register passenger
3. Create booking
4. Cancel booking
5. Display flights
6. Display passengers
7. Display bookings
8. Search booking
9. System statistics
0. Exit
```

## Example: Booking Persistence

Using the included synthetic sample data:

1. Select **3 — Create booking**.
2. Enter booking ID `B001`, flight `AV245`, and passenger `P001`.
3. The application confirms the booking and saves it to `data/bookings.csv`.
4. Exit and restart the application.
5. Select **8 — Search booking** and enter `B001` to retrieve it.
6. Select **4 — Cancel booking** to cancel it and save the updated state.

Example CSV record after cancellation:

```csv
booking_id,flight_number,passenger_id,status
B001,AV245,P001,CANCELLED
```

The flight's available-seat count is reconstructed from confirmed bookings when the application loads the data.

## CSV Storage

| File | Columns |
| --- | --- |
| `data/flights.csv` | `flight_number,origin,destination,total_seats` |
| `data/passengers.csv` | `passenger_id,name,email` |
| `data/bookings.csv` | `booking_id,flight_number,passenger_id,status` |

Bookings are restored only after flights and passengers have been loaded. The booking loader validates all records before beginning restoration, preventing partial application of records when ordinary input validation fails.

The booking writer first creates a `.tmp` file and then attempts to rename it over the destination file. This reduces the risk of leaving a truncated destination after a failed write, but **does not guarantee full transactional behavior or durability across crashes and power loss**.

**CSV limitations:** The current parser uses simple comma-separated fields and does not support quoted fields containing commas or line breaks. Sample data is synthetic; do not use real passenger personal information.

## Testing

Build the project and run all suites:

```powershell
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

Run the CSV storage tests independently:

```powershell
.\build\Release\test-csv-storage.exe
```

| CTest suite | Coverage area |
| --- | --- |
| `FlightTests` | Flight inventory behavior |
| `BookingTests` | Booking status and seat changes |
| `BookingSystemTests` | System-level booking rules |
| `BookingIntegrationTests` | Integration between domain components |
| `CsvStorageTests` | Saving, restoring, invalid statuses, and rejected partial loads |
| `BookingServiceTests` | Booking operations, persistence failures, rollback, and operation blocking |


**Latest verified local result:** 6/6 CTest suites passed (November 2026).

The booking service tests also verify that failed persistence operations restore the expected booking status and seat availability.


## Current Limitations & Roadmap

This is a local, single-process educational application. Planned improvements include:


- Booking creation and cancellation support in-memory rollback when saving reports failure, but crash recovery and full transaction atomicity are not guaranteed.
- Booking CSV writes use temporary files; flight and passenger CSV writes do not use the same replacement mechanism.
- Unexpected process termination, power loss, and filesystem exceptions are not fully covered by the recovery tests.
- Support standards-compliant CSV escaping or migrate to a database.
- Expand tests for corrupted records, duplicates, insufficient seats, and failure injection.
- Add CI builds and tests using GitHub Actions.
- Improve structured error handling and logging.

The project does **not** implement payment processing, multi-user concurrency, airline distribution protocols, or production security controls.

## Engineering Focus

The project demonstrates practical C++ development through object-oriented domain modeling, ownership management with smart pointers, references between entities, state transitions, validation, persistence, automated regression tests, and a reproducible CMake build.

## License

See [LICENSE](LICENSE) for the repository's license terms.
