#include<iostream>
#include<vector>
#include<string>
#include<algorithm>
#include<cassert>
#include<fstream>
#include<cstdio>
#include<unordered_set>

std::vector<uint8_t> pack(const std::vector<std::string>& encoded) {
    std::vector<uint8_t> packed;
    uint8_t chunk = 0;
    uint8_t chunk_width = 8;
    uint8_t pos = -1;
    for (const auto& code: encoded) {
        for (int i = 0; i < code.size(); ++i) {
            if (pos == chunk_width - 1) {
                packed.push_back(chunk);
                chunk = 0;
                pos = -1;
            }
            chunk <<= 1;
            if (code[i] == '1')
                chunk |= 1;
            ++pos;
        }
    }
    if (pos < chunk_width - 1) {
        chunk <<= chunk_width - pos - 1;
        packed.push_back(chunk);
    }
    return packed;
}

std::vector<std::string> unpack(const std::vector<uint8_t>& packed, std::unordered_set<std::string> codes) {
    std::vector<std::string> encoded;
    uint8_t chunk_width = 8;
    std::string code;
    for (const uint8_t& chunk: packed) {
        for (int i = 0; i < chunk_width; ++i) {
            if (chunk & (1 << chunk_width - i - 1))
                code += "1";
            else
                code += "0";
            if (codes.find(code) != codes.end()) {
                encoded.push_back(code);
                code = "";
            }
        }
    }
    return encoded;
}

void save(std::string fpath, const std::vector<uint8_t>& packed) {
    auto file = std::ofstream(fpath, std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("Couldn't open file");
    file.put(static_cast<uint8_t>(packed.size()));
    file.put(static_cast<uint8_t>(packed.size() >> 8));
    for (const uint8_t& chunk: packed)
        file.put(chunk);
    file.put(EOF);
    file.close();
}

std::vector<uint8_t> load(std::string fpath) {
    auto file = std::ifstream(fpath, std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("Couldn't open file");
    std::vector<uint8_t> packed;
    char chunk;
    while ((chunk = file.get()) != EOF) {
        packed.push_back(static_cast<uint8_t>(chunk));
    }
    return packed;
}

std::vector<uint8_t> load_compressed(std::string fpath) {
    auto file = std::ifstream(fpath, std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("Couldn't open file");
    std::vector<uint8_t> packed;
    uint16_t msg_len = static_cast<uint8_t>(file.get());
    msg_len |= static_cast<uint16_t>(file.get()) << 8;
    std::cout << "saved bytes " << int(msg_len) << std::endl;
    for (int i = 0; i < msg_len; ++i) {
        packed.push_back(static_cast<uint8_t>(file.get()));
    }
    return packed;
}

int run() {
    std::unordered_set<std::string> codes{
        "01",
        "101",
        "1100",
        "1110",
        "11111"
    };
    std::vector<std::string> encoded{
        "01",
        "1110",
        "1100",
        "1100",
        "01",
        "11111",
        "1100",
        "101",
        "11111",
        "01",
        "1110"
    };
    auto packed = pack(encoded);
    save("packed.bin", packed);
    std::cout << "packed:\n";
    for (const auto& x: packed)
        std::cout << static_cast<int>(x) << ",";
    std::cout << '\n';
    std::cout << "unpacked:\n";
    auto loaded = load("packed.bin");
    auto unpacked = unpack(loaded, codes);
    for (const auto& x: unpacked)
        std::cout << x << ",";
    std::cout << '\n';
    assert (unpacked == encoded);
    return 0;
}
