// Mirrors the idea in PDF Listing 3.3 — counting team members with std::count_if
// using a lambda that captures `this` and `team_name`. :contentReference[oaicite:14]{index=14}

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>

#include "../../common/person.h" // Provided by the repo's common folder.

class company_t {
public:
    void hire(const person_t& person, const std::string& team_name) {
        m_team_by_person[person.name()] = team_name;
        m_employees.push_back(person);
    }

    std::string team_name_for(const person_t& person) const {
        auto it = m_team_by_person.find(person.name());
        return (it != m_team_by_person.end()) ? it->second : std::string("Unknown");
    }

    int count_team_members(const std::string& team_name) const {
        return static_cast<int>(std::count_if(
            m_employees.cbegin(), m_employees.cend(),
            [this, &team_name](const person_t& employee) {
                return team_name_for(employee) == team_name;
            }
        ));
    }

    void print_roster() const {
        std::cout << "=== Roster ===\n";
        for (const auto& e : m_employees) {
            std::cout << " - " << e.name() << " → " << team_name_for(e) << '\n';
        }
    }

private:
    std::vector<person_t> m_employees;
    std::unordered_map<std::string, std::string> m_team_by_person; // person.name() → team
};

int main() {
    std::cout << "[company_t] lambda capture demo (counting team members)\n";

    company_t company;

    // person_t signature is (name, gender, age)
    company.hire(person_t{"Jane",  person_t::female, 56}, "Mars");
    company.hire(person_t{"David", person_t::male,   24}, "Earth");
    company.hire(person_t{"Tom",   person_t::male,   52}, "Mars");
    company.hire(person_t{"Peter", person_t::male,   84}, "Mars");
    company.hire(person_t{"Martha",person_t::female, 32}, "Moon");
    company.hire(person_t{"Rose",  person_t::female, 31}, "Moon");
    company.hire(person_t{"Jon",   person_t::male,   42}, "Earth");
    company.hire(person_t{"Colin", person_t::male,   29}, "Moon");
    company.hire(person_t{"Clara", person_t::female, 27}, "Moon");

    company.print_roster();

    for (const std::string team : {"Mars", "Earth", "Moon", "Jupiter"}) {
        std::cout << "Team \"" << team << "\" has "
                  << company.count_team_members(team) << " member(s).\n";
    }

    return 0;
}
