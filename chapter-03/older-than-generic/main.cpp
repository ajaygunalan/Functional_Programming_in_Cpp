// Demonstrates a generic function object (`older_than`) with perfect forwarding
// and compares it with a generic lambda. Based on §3.1.4 and Listings 3.1–3.2. :contentReference[oaicite:10]{index=10}

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <utility>

#include "../../common/person.h"  
class older_than {
public:
    explicit older_than(int limit) : m_limit(limit) {}

    template <typename T>
    bool operator()(T&& object) const {
        return std::forward<T>(object).age() > m_limit;
    }

private:
    int m_limit;
};

struct car_t {
    std::string model;
    int years_on_road {0};
    int age() const { return years_on_road; }
};

struct project_t {
    std::string code;
    int months_elapsed {0};
    int age() const { return months_elapsed / 12; }
};

int main() {
    std::cout << "[older_than] generic function object demo\n";

    std::vector<car_t> cars = {
        {"Axiom", 6}, {"Lantern", 1}, {"Nova", 3}, {"Vector", 5}, {"Nimbus", 7}, {"Rift", 2}
    };

    older_than older_than_5(5);
    const auto cars_over_5 = std::count_if(cars.begin(), cars.end(), older_than_5);
    std::cout << "Cars older than 5 years: " << cars_over_5 << " out of " << cars.size() << '\n';

    std::vector<project_t> projects = {
        {"PX", 13}, {"QZ", 7}, {"RY", 24}, {"ST", 5}
    };

    older_than older_than_2(2);
    const auto proj_over_2 = std::count_if(projects.begin(), projects.end(), older_than_2);
    std::cout << "Projects older than 2 years: " << proj_over_2 << " out of " << projects.size() << '\n';

    // Generic lambda alternative (PDF §3.2.4) :contentReference[oaicite:11]{index=11}
    const int limit = 4;
    auto predicate = [limit](const auto& object) {
        return object.age() > limit;
    };
    const auto cars_over_4 = std::count_if(cars.begin(), cars.end(), predicate);
    std::cout << "Cars older than " << limit << " years (generic lambda): " << cars_over_4 << '\n';

    // Optional: persons demo
    // person_t constructor signature is (std::string name, gender_t gender, int age)
    std::vector<person_t> persons = {
        person_t{"Jane",  person_t::female, 56},
        person_t{"Tom",   person_t::male,   24},
        person_t{"Martha",person_t::female, 52},
        person_t{"David", person_t::male,   84},
        person_t{"Rose",  person_t::female, 32},
        person_t{"Colin", person_t::male,   31}
    };
    older_than older_than_42(42);
    const auto people_over_42 = std::count_if(persons.cbegin(), persons.cend(), older_than_42);
    std::cout << "People older than 42: " << people_over_42 << " out of " << persons.size() << '\n';
    return 0;
}
