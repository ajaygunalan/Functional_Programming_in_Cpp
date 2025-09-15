// chapter_4_partial_bind_and_currying.cpp
// Chapter 4 highlights: partial application (custom bind2nd), std::bind & placeholders,
// binding member functions, lambda alternatives, and currying. See Chapter 4 listings & figures
#include <algorithm>
#include <cmath>
#include <functional>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "../../common/person.h" // Provided by the repo (non-member print + member print demo).

// --- 4.1.1 / 4.1.2: a tiny "bind second argument" (bind2nd) -------------------------------
// Listing-style adaptation of the PDF's "partial_application_bind2nd_impl" + helper. 
template <typename Function, typename SecondArgType>
class partial_application_bind2nd_impl {
public:
    partial_application_bind2nd_impl(Function function, SecondArgType second_arg)
        : m_function(std::move(function)), m_second_arg(std::move(second_arg)) {}

    template <typename FirstArgType>
    decltype(auto) operator()(FirstArgType&& first_arg) const {
        return this->m_function(std::forward<FirstArgType>(first_arg), this->m_second_arg);
    }

private:
    Function m_function;
    SecondArgType m_second_arg;
};

template <typename Function, typename SecondArgType>
auto bind2nd(Function&& function, SecondArgType&& second_arg)
    -> partial_application_bind2nd_impl<typename std::decay<Function>::type,
                                        typename std::decay<SecondArgType>::type> {
    using F = typename std::decay<Function>::type;
    using S = typename std::decay<SecondArgType>::type;
    return partial_application_bind2nd_impl<F, S>(std::forward<Function>(function),
                                                  std::forward<SecondArgType>(second_arg));
}

// --- 4.2: a simple curried function ---------------------------------------------------------
// Listing 4.12: greater_curried(first)(second) -> bool. fileciteturn1file0
bool greater(double first, double second) { return first > second; }
auto greater_curried(double first) {
    return [first](double second) { return first > second; };
}

int main() {
    using std::cout;
    using std::endl;
    using namespace std::placeholders; // _1, _2 for std::bind examples

    cout << "[Chapter 4] Partial application, std::bind, and currying demos\n";

    // --- Demo A: partial application via our bind2nd (Listings 4.2–4.6) ------------------ 
    std::vector<int> xs{1, 6, 7, 2, 42, 5, 13, 0};
    auto greater_than_6 = bind2nd(std::greater<int>(), 6);
    auto pivot = std::partition(xs.begin(), xs.end(), greater_than_6);
    cout << "Numbers > 6: ";
    std::copy(xs.begin(), pivot, std::ostream_iterator<int>(cout, " "));
    cout << "\nNumbers <= 6: ";
    std::copy(pivot, xs.end(), std::ostream_iterator<int>(cout, " "));
    cout << "\n";

    // Degrees → radians using bind2nd with multiplies (Listing 4.6). ---------------------- 
    constexpr double PI = 3.14159265358979323846;
    std::vector<double> degrees{0, 30, 45, 60, 90};
    std::vector<double> radians(degrees.size());
    std::transform(degrees.begin(), degrees.end(), radians.begin(),
                   bind2nd(std::multiplies<double>(), PI / 180.0));
    cout << "Radians: ";
    for (double r : radians) cout << r << " ";
    cout << "\n";

    // --- Demo B: std::bind — binding & reordering (Listings 4.7–4.11) -------------------- 
    // Binding all args (nullary function object)
    auto bound_all = std::bind(std::greater<double>(), 6, 42);
    cout << "Is 6 > 42? " << std::boolalpha << bound_all() << "\n";

    // Leaving a hole with _1 (unary function objects)
    auto is_greater_than_42 = std::bind(std::greater<double>(), _1, 42);
    auto is_less_than_42    = std::bind(std::greater<double>(), 42, _1);
    cout << "6 > 42? " << is_greater_than_42(6) << " | 6 < 42? " << is_less_than_42(6) << "\n";

    // Reversing arguments: sort ascending using std::greater with (_2, _1) (Fig. 4.6). ---- 
    std::vector<double> scores{3.5, 2.1, 4.0, 1.7};
    std::sort(scores.begin(), scores.end(), std::bind(std::greater<double>(), _2, _1));
    cout << "Scores ascending: ";
    for (double s : scores) cout << s << " ";
    cout << "\n";

    // Binding non-member and member functions for printing people (Listings 4.10–4.11). --- 
    std::vector<person_t> people{
        {"David", person_t::male}, {"Jane", person_t::female},   {"Martha", person_t::female},
        {"Peter", person_t::male}, {"Rose", person_t::female},   {"Tom", person_t::male},
    };

    // Non-member printing (like the sample in the chapter). fileciteturn1file2
    auto print_person = [](const person_t& person, std::ostream& out, person_t::output_format_t fmt) {
        if (fmt == person_t::name_only) {
            out << person.name() << '\n';
        } else if (fmt == person_t::full_name) {
            out << person.name() << ' ' << person.surname() << '\n';
        }
    };

    cout << "People (names to std::cout via std::bind):\n";
    std::for_each(people.cbegin(), people.cend(),
                  std::bind(print_person, _1, std::ref(std::cout), person_t::name_only));

    // Member-function pointer form (if your person_t exposes print like in the chapter). -- 
    // std::for_each(people.cbegin(), people.cend(),
    //               std::bind(&person_t::print, _1, std::ref(std::cout), person_t::name_only));

    // Lambda alternatives to std::bind (Section 4.1.5). ---------------------------------- 
    cout << "People (names via lambda instead of bind):\n";
    std::for_each(people.cbegin(), people.cend(), [](const person_t& p) {
        std::cout << p.name() << '\n';
    });

    // --- Demo C: currying vs plain call (Listing 4.12) ------------------------------------ 
    cout << "greater(2,3): " << greater(2, 3) << "\n";
    cout << "greater_curried(2)(3): " << greater_curried(2)(3) << "\n";

    // Lambda version of "greater than 42" as a quick partial application
    auto gt42_lambda = [limit = 42](int x) { return x > limit; };
    cout << "7 > 42? " << gt42_lambda(7) << "\n";

    return 0;
}
