#include <vector>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <cassert>
#include <iterator>

typedef std::pair<char, std::string> Entry;

template <typename T1, typename T2>
std::ostream& operator<< (std::ostream& out, const std::pair<T1, T2>&p) {
    out << "(" << p.first << ", " << p.second << ")";
    return out;
}

// specialization for (string, string)
template <>
std::ostream& operator<< (std::ostream& out,
                          const std::pair<std::string, std::string>&p) {
    out << "{\"" << p.first << "\", \"" << p.second << "\"}";
    return out;
}

template <typename T>
std::ostream& operator<< (std::ostream& out, const std::vector<T>& v) {
    out << "[";
    std::copy(v.begin(), v.end(), std::ostream_iterator<T>(out, ","));
    out << "]";
    return out;
}

template <typename T1, typename T2>
std::ostream& operator<< (std::ostream& out,
                          const std::vector<std::pair<T1, T2>>& v) {
    out << "[";
    // copy seems cannot be called with pair
    // std::copy(v.begin(), v.end(),
    //          std::ostream_iterator<std::pair<T1,T2>>(out, ","));
    for (auto it=v.begin();;) {
        out << *it;
        ++it;
        if (it != v.end()) out << ", " ; else break;
    }
    out << "]";
    return out;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "./a.out char to decode\n";
        return -1;
    }
    // use this table for encode, below tree for decode
    // This code compiles as the value type for an unordered_map
    // is std::pair<>
    // or use
    std::vector<std::pair<std::string, std::string>> table = {
    // using an unordered makes encoding very easy
    // const std::unordered_map<char, std::string> table = {
        {"A", ".-"},
        {"B", "-..."},
        {"C", "-.-."},
        {"D", "-.."},
        {"E", "."},
        {"F", "..-."},
        {"G", "--."},
        {"H", "...."},
        {"I", ".."},
        {"J", ".---"},
        {"K", "-.-"},
        {"L", ".-.."},
        {"M", "--"},
        {"N", "-."},
        {"O", "---"},
        {"P", ".--."},
        {"Q", "--.-"},
        {"R", ".-."},
        {"S", "..."},
        {"T", "-"},
        {"U", "..-"},
        {"V", "...-"},
        {"W", ".--"},
        {"X", "-..-"},
        {"Y", "-.--"},
        {"Z", "--.."},
        {"1", ".----"},
        {"2", "..---"},
        {"3", "...--"},
        {"4", "....-"},
        {"5", "....."},
        {"6", "-...."},
        {"7", "--..."},
        {"8", "---.."},
        {"9", "----."},
        {"0", "-----"},
    };
    std::cout << table << "\n";
    std::vector<std::string> output;
    output.resize(table.size()+1);
    output[0]='$';
    for (auto e : table) {
        auto ch = std::get<0>(e);
        auto code = std::get<1>(e);
        unsigned int idx = 0;
        std::for_each(code.begin(), code.end(),
                      [&idx](char c) {
                          idx = 2*idx + (c == '.' ? 1 : 2);
                      });
        // std::cout << ch << " " << code << " " << idx << "\n";
        if (idx >= output.size()) {
            output.resize(idx+1);
        }
        assert(output[idx].length()==0);
        output[idx] = ch;
    }
    // std::cout << output << "\n";
    std::string coded_string = argv[1];
    unsigned int idx = 0;
    std::for_each(coded_string.begin(), coded_string.end(),
                  [&idx](char c) {
                      idx = 2*idx + (c == '.' ? 1 : 2);
                  });
    assert(output[idx].length()!=0);
    std::cout << output[idx] << "\n";
}
