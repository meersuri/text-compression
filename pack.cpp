#include<iostream>
#include<vector>
#include<string>
#include<algorithm>
#include<cassert>
#include<fstream>

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

std::vector<std::string> unpack(const std::vector<uint8_t>& packed, std::vector<std::string> codes) {
    std::vector<std::string> encoded;
    uint8_t chunk_width = 8;
    std::string code;
    for (const uint8_t& chunk: packed) {
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

void save(std::string fpath, const std::vector<uint8_t>& packed) {
    auto file = std::ofstream(fpath, std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("Couldn't open file");
    file.put(static_cast<uint8_t>(packed.size()));
    for (const uint8_t& chunk: packed)
        file.put(chunk);
    file.put('\0');
    file.close();
}

std::vector<uint8_t> load(std::string fpath) {
    auto file = std::ifstream(fpath, std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("Couldn't open file");
    std::vector<uint8_t> packed;
    uint8_t msg_len = file.get();
    uint8_t chunk = 0;
    while ((chunk = file.get()) != '\0')
        packed.push_back(chunk);
    return packed;
}

int main() {
    std::vector<std::string> codes{
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
