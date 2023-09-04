#include "search.h"
#include <unordered_map>
#include <map>
#include <cctype>
#include <string>
#include <math.h>
#include <iostream>
#include <set>
#include <algorithm>

const long double EPS = 1e-16;

struct My {
    long double tf_idf;
    size_t lp;
    size_t rp;
    My() {
        lp = 0;
        rp = 0;
    }
};

bool Comp(const std::pair<My, std::vector<std::string>>& a, const std::pair<My, std::vector<std::string>>& b) {
    return a.first.tf_idf - b.first.tf_idf < -EPS;
}

bool Eq(long double a, long double b) {
    return fabs(a - b) < EPS;
}

std::vector<std::string_view> Search(std::string_view text, std::string_view query, size_t results_count) {
    size_t count_strings = std::count(text.begin(), text.end(), '\n') + 1;
    // std::map<std::string, int64_t> count_words;
    std::vector<std::pair<My, std::vector<std::string>>> strings(count_strings);
    std::vector<std::map<std::string, int64_t>> count_words(count_strings);
    size_t i = 0;
    std::string current;
    size_t ptr = 0;
    // getting words from text
    size_t lp = 0;
    size_t rp = 0;
    while (i < text.size()) {
        strings[ptr].first.tf_idf = 0;
        if (std::isalpha(text[i])) {
            current.push_back(tolower(text[i]));
        } else if (text[i] == '\n') {
            if (!current.empty()) {
                ++count_words[ptr][current];
                strings[ptr].second.push_back(current);
                current = "";
            }
            strings[ptr].first.lp = lp;
            strings[ptr].first.rp = rp;
            ++ptr;
            lp = i + 1;
        } else {
            if (!current.empty()) {
                ++count_words[ptr][current];
                strings[ptr].second.push_back(current);
                current = "";
            }
        }
        ++i;
        ++rp;
    }
    if (!current.empty()) {
        ++count_words[ptr][current];
        strings[ptr].second.push_back(current);
        current = "";
    }
    // getting words from query
    std::set<std::string> query_words;
    i = 0;
    while (i < query.size()) {
        // std::cout << current << std::endl;
        if (std::isalpha(query[i])) {
            current.push_back(tolower(query[i]));
        } else {
            if (!current.empty()) {
                query_words.insert(current);
                current = "";
            }
        }
        ++i;
    }
    if (!current.empty()) {
        query_words.insert(current);
        current = "";
    }
    // count idf
    std::map<std::string, long double> idf;
    for (const auto& str : query_words) {
        size_t count = 0;
        for (size_t j = 0; j < count_strings; ++j) {
            if (count_words[j].count(str)) {
                ++count;
            }
        }
        idf[str] = count_strings / static_cast<long double>(count);
        // std::cout << str << " -------> " << idf[str] << std::endl;
    }
    // count tf-idf
    std::vector<std::map<std::string, long double>> tf(count_strings);
    for (size_t j = 0; j < count_strings; ++j) {
        for (const auto& str : query_words) {
            if (count_words[j].count(str)) {
                strings[j].first.tf_idf -=
                        (count_words[j][str] / static_cast<long double>(strings[j].second.size())) * log(idf[str]);
            }
        }
        // std::cout << strings[j].first << std::endl;
    }
    std::sort(strings.begin(), strings.end(), Comp);
    std::vector<std::string_view> res;
    for (size_t j = 0; j < std::min(results_count, count_strings) && !Eq(strings[j].first.tf_idf, 0); ++j) {
        res.push_back(text.substr(strings[j].first.lp, strings[j].first.rp - strings[j].first.lp));
    }
    return res;
}

