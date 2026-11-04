#ifndef PASSENGER_H
#define PASSENGER_H

#include <string>

class Passenger {

private:
    std::string passengerId;
    std::string name;
    std::string email;

public:
    Passenger(
        const std::string& passengerId,
        const std::string& name,
        const std::string& email
    );

    std::string getPassengerId() const;
    std::string getName() const;
    std::string getEmail() const;

    void displayInfo() const;
};

#endif