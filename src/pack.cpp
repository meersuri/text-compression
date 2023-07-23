#include<iostream>
#include<vector>
#include<string>
#include<algorithm>
#include<cassert>
#include<fstream>
#include<cstdio>
#include<unordered_set>
#include<memory>

#include "pack.hpp"

std::shared_ptr<PackedData> pack(const std::vector<std::string>& encoded) {
    auto packed = std::make_shared<PackedData>();
    packed->content_len = encoded.size();
    char chunk = 0;
    uint8_t chunk_width = 8;
    uint8_t pos = -1;
    for (const auto& code: encoded) {
        for (int i = 0; i < code.size(); ++i) {
            if (pos == chunk_width - 1) {
                packed->data.push_back(chunk);
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
        packed->data.push_back(chunk);
    }
    return packed;
}

std::vector<std::string> unpack(const std::shared_ptr<PackedData> packed, std::unordered_set<std::string> codes) {
    std::vector<std::string> encoded;
    uint8_t chunk_width = 8;
    std::string code;
    for (const char& chunk: packed->data) {
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
    encoded.resize(packed->content_len);
    return encoded;
}

void save_compressed(std::string fpath, const std::vector<std::string> &codebook, const std::shared_ptr<PackedData> packed) {
    auto file = std::ofstream(fpath, std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("Couldn't open file");
    uint64_t file_size = packed->data.size();
    uint64_t content_len = packed->content_len;
    uint32_t codebook_size = codebook.size();

    file.write(reinterpret_cast<const char*>(&file_size), 8);
    file.write(reinterpret_cast<const char*>(&content_len), 8);
    file.write(reinterpret_cast<const char*>(&codebook_size), 4);
    for (const auto &code: codebook) {
        assert (code.size() <= 255);
        uint8_t code_size = static_cast<uint8_t>(code.size());
        file.put(code_size);
        std::cout << code << " " << static_cast<int>(code_size) << " ";
    }
    std::cout << "\nchunks\n";
    uint8_t chunk = 0;
    uint32_t pos = 0;
    uint32_t packed_codebook_size = 0;
    for (const auto &code: codebook) {
        for (int i = 0; i < code.size(); ++i) {
            if (pos == 8) {
                file.put(chunk);
                std::cout << int(chunk) << " ";
                pos = 0;
                chunk = 0;
                packed_codebook_size++;
            }
            if (code[i] == '1')
                chunk |= 1;
            pos++;
            if (pos < 8)
                chunk <<= 1;
        }
    }
    chunk <<= 8 - pos;
    file.put(chunk);
    packed_codebook_size++;
    file.write(packed->data.data(), packed->data.size());
    file.put(EOF);
    file.close();
    auto header_size = 8 + 8 + 4 + codebook.size() + packed_codebook_size;
    std::cout << "saved bytes " << packed->data.size() << "+" << header_size <<  std::endl;
}

void save(std::string fpath, const std::vector<char> raw) {
    auto file = std::ofstream(fpath, std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("Couldn't open file");
    file.write(raw.data(), raw.size());
    file.put(EOF);
    file.close();
}

std::vector<char> load(std::string fpath) {
    auto file = std::ifstream(fpath, std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("Couldn't open file");
    file.seekg(0, std::ios_base::end);
    auto file_size = file.tellg();
    file.seekg(0, std::ios_base::beg);
    std::vector<char> bytes(file_size);
    file.read(bytes.data(), file_size);
    file.close();
    return bytes;
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
    std::cout << "packed:\n";
    for (const auto& x: packed->data)
        std::cout << static_cast<int>(x) << ",";
    std::cout << '\n';
    return 0;
}
