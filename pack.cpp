#include<iostream>
#include<vector>
#include<string>
#include<algorithm>
#include<cassert>

template<class T>
std::vector<T> pack(const std::vector<std::string>& encoded) {
    std::vector<T> packed;
    T chunk = 0;
    uint8_t chunk_width = sizeof(T)*8;
    uint8_t pos = 0;
    for (const auto& code: encoded) {
        if (pos + code.size() > chunk_width) {
            packed.push_back(chunk << (chunk_width - pos - 1));
            chunk = 0;
            pos = 0;
        }
        for (int i = 0; i < code.size(); ++i) {
            if (code[i] == '1')
                chunk |= 1;
            if (pos < chunk_width - 1) {
                chunk <<= 1;
                ++pos;
            }
        }
    }
    packed.push_back(chunk << (chunk_width - pos - 1));
    return packed;
}

template<class T>
std::vector<std::string> unpack(const std::vector<T>& packed, std::vector<std::string> codes) {
    std::vector<std::string> encoded;
    uint8_t chunk_width = sizeof(T)*8;
    for (const auto& chunk: packed) {
        std::string code;
        for (int i = 0; i < chunk_width; ++i) {
            if (chunk & (1 << chunk_width - i - 1))
                code += "1";
            else
                code += "0";
            if (std::find(codes.begin(), codes.end(), code) != codes.end()) {
                encoded.push_back(code);
                code = "";
            }
        }
    }
    return encoded;
}

int main() {
    std::vector<std::string> codes{"01", "101", "1100", "1110", "11111"};
    std::vector<std::string> encoded{"01", "1110", "1100", "1100", "01", "11111", "1100"};
    auto packed = pack<uint16_t>(encoded);
    std::cout << "packed:\n";
    for (const auto& x: packed)
        std::cout << x << ",";
    std::cout << '\n';
    std::cout << "unpacked:\n";
    auto unpacked = unpack<uint16_t>(packed, codes);
    for (const auto& x: unpacked)
        std::cout << x << ",";
    std::cout << '\n';
    assert (unpacked == encoded);
    return 0;
}
