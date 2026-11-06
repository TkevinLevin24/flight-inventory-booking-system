cmake_minimum_required(VERSION 3.16)

project(
    FlightInventoryBookingSystem
    VERSION 1.0.0
    LANGUAGES CXX
)

# C++ configuration
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable testing
enable_testing()

# Core library
add_library(flight-booking-core STATIC
    src/Flight.cpp
    src/Passenger.cpp
    src/Booking.cpp
    src/BookingSystem.cpp
    src/CsvStorage.cpp
    src/BookingService.cpp
)

target_include_directories(flight-booking-core PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

# Main application
add_executable(flight-booking
    src/main.cpp
)

target_link_libraries(flight-booking PRIVATE
    flight-booking-core
)

# Flight tests
add_executable(test-flight
    tests/test_flight.cpp
)

target_link_libraries(test-flight PRIVATE
    flight-booking-core
)

# Booking tests
add_executable(test-booking
    tests/test_booking.cpp
)

target_link_libraries(test-booking PRIVATE
    flight-booking-core
)

# BookingSystem tests
add_executable(test-booking-system
    tests/test_booking_system.cpp
)

target_link_libraries(test-booking-system PRIVATE
    flight-booking-core
)

# Booking integration tests
add_executable(test-booking-integration
    tests/test_booking_integration.cpp
)

target_link_libraries(test-booking-integration PRIVATE
    flight-booking-core
)
# CSV storage tests
add_executable(test-csv-storage
    tests/test_csv_storage.cpp
)

target_link_libraries(test-csv-storage
    PRIVATE flight-booking-core
)

add_test(
    NAME CsvStorageTests
    COMMAND test-csv-storage
)

# Keep assert() active in Release for legacy FlightTests.
# The other test files use explicit check() functions.
if(MSVC)
    target_compile_options(test-flight PRIVATE /UNDEBUG)
endif()

# Register tests
add_test(
    NAME FlightTests
    COMMAND test-flight
)

add_test(
    NAME BookingTests
    COMMAND test-booking
)

add_test(
    NAME BookingSystemTests
    COMMAND test-booking-system
)

add_test(
    NAME BookingIntegrationTests
    COMMAND test-booking-integration
)
add_executable(test-booking-service
    tests/test_booking_service.cpp
)

target_link_libraries(test-booking-service
    PRIVATE flight-booking-core
)

add_test(
    NAME BookingServiceTests
    COMMAND test-booking-service
)