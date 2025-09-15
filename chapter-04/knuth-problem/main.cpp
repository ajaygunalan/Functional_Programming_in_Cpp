// chapter_4_composition_and_lifting.cpp
// Chapter 4 highlights: function composition (top-N word frequency), lifting (pointer & collection),
// and a generic reverse_pairs (Listing 4.19). fileciteturn1file0

#include <algorithm>
#include <cctype>
#include <iostream>
#include <iterator>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

// --- Small utilities -----------------------------------------------------------------------
std::vector<std::string> words(const std::string& text) {
    std::vector<std::string> out;
    std::string cur;
    cur.reserve(32);
    for (char ch : text) {
        if (std::isalpha(static_cast<unsigned char>(ch))) {
            cur.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
        } else {
            if (!cur.empty()) {
                out.push_back(cur);
                cur.clear();
            }
        }
    }
    if (!cur.empty()) out.push_back(cur);
    return out;
}

// Listing-style count_occurrences (generalized for any collection C of T). fileciteturn1file0
template <typename C, typename T = typename C::value_type>
std::unordered_map<T, unsigned int> count_occurrences(const C& collection) {
    std::unordered_map<T, unsigned int> result;
    for (const auto& v : collection) { ++result[v]; }
    return result;
}

// Listing 4.19: reverse_pairs (generic; works with maps/unordered_maps of pairs). fileciteturn1file0
template <
    typename C,
    typename P1 = typename std::remove_cv<typename C::value_type::first_type>::type,
    typename P2 = typename C::value_type::second_type
>
std::vector<std::pair<P2, P1>> reverse_pairs(const C& items) {
    std::vector<std::pair<P2, P1>> out;
    out.reserve(std::distance(std::begin(items), std::end(items)));
    std::transform(std::begin(items), std::end(items), std::back_inserter(out),
                   [](const std::pair<const P1, P2>& p) {
                       return std::make_pair(p.second, p.first);
                   });
    return out;
}

void sort_by_frequency(std::vector<std::pair<unsigned, std::string>>& v) {
    std::sort(v.begin(), v.end(),
              [](const auto& a, const auto& b) {
                  if (a.first != b.first) return a.first > b.first; // desc by count
                  return a.second < b.second;                      // asc by lexeme
              });
}

void print_pairs(const std::vector<std::pair<unsigned, std::string>>& v, std::size_t topN) {
    std::size_t shown = 0;
    for (const auto& p : v) {
        if (shown++ >= topN) break;
        const auto& count = p.first;
        const auto& word  = p.second;
        std::cout << count << " " << word << "\n";
    }
}

// --- 4.4: lifting (Listing 4.18) ----------------------------------------------------------- fileciteturn1file0
void to_upper(std::string& s) {
    for (char& ch : s) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
}

template <typename Function>
auto pointer_lift(Function f) {
    return [f](auto* item) {
        if (item) f(*item);
    };
}

template <typename Function>
auto collection_lift(Function f) {
    return [f](auto& items) {
        for (auto& item : items) f(item);
    };
}

// --- Composition demo (Section 4.3; see Fig. 4.12). --------------------------------------- fileciteturn1file0
void print_common_words(const std::string& text, std::size_t topN) {
    auto counts  = count_occurrences(words(text));
    auto flipped = reverse_pairs(counts);          // vector of (count, word)
    sort_by_frequency(flipped);
    print_pairs(flipped, topN);
}

int main() {
    std::cout << "[Chapter 4] Composition + Lifting demos\n";

    // Composition pipeline (top-N words)
    const std::string text =
        "Mr. Jones of the Manor Farm, had locked the hen-houses for the night, "
        "but was too drunk to remember to shut the popholes.";
    std::cout << "Top words:\n";
    print_common_words(text, 6);

    // Lifting a single-item function to pointer & collection
    std::string title = "Sintel";
    auto ptr_up  = pointer_lift(to_upper);
    auto coll_up = collection_lift(to_upper);

    std::cout << "\nBefore: title = " << title << "\n";
    ptr_up(&title);
    std::cout << "After (pointer_lift): title = " << title << "\n";

    std::vector<std::string> names{"Animal", "farm", "Boxer"};
    std::cout << "Before (collection): ";
    for (auto& n : names) std::cout << n << " ";
    std::cout << "\n";
    coll_up(names);
    std::cout << "After  (collection): ";
    for (auto& n : names) std::cout << n << " ";
    std::cout << "\n";

    return 0;
}
