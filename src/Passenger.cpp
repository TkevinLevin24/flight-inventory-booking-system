#include "Passenger.h"

#include <iostream>
#include <stdexcept>

Passenger::Passenger(
    const std::string& passengerId,
    const std::string& name,
    const std::string& email
)
    : passengerId(passengerId),
      name(name),
      email(email)
{
    if (passengerId.empty()) {
        throw std::invalid_argument(
            "Passenger ID cannot be empty."
        );
    }

    if (name.empty()) {
        throw std::invalid_argument(
            "Passenger name cannot be empty."
        );
    }

    if (email.empty()) {
        throw std::invalid_argument(
            "Passenger email cannot be empty."
        );
    }
}

std::string Passenger::getPassengerId() const {
    return passengerId;
}

std::string Passenger::getName() const {
    return name;
}

std::string Passenger::getEmail() const {
    return email;
}

void Passenger::displayInfo() const {

    std::cout << "\nPassenger ID: "
              << passengerId << "\n";

    std::cout << "Name: "
              << name << "\n";

    std::cout << "Email: "
              << email << "\n";
}