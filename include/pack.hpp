#ifndef PACK_HPP
#define PACK_HPP

#include <vector>
#include <unordered_set>
#include <string>
#include <memory>

struct PackedData {
    std::vector<char> data;
    uint64_t content_len;
};

std::shared_ptr<PackedData> pack(const std::vector<std::string>& encoded);

std::vector<std::string> unpack(const std::shared_ptr<PackedData> packed, std::unordered_set<std::string> codes);

void save_compressed(std::string fpath, const std::shared_ptr<PackedData> packed);

std::vector<char> load(std::string fpath);

std::shared_ptr<PackedData> load_compressed(std::string fpath);

#endif
