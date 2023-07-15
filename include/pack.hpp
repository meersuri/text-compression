#ifndef PACK_HPP
#define PACK_HPP

#include <vector>
#include <string>

std::vector<uint8_t> pack(const std::vector<std::string>& encoded);

std::vector<std::string> unpack(const std::vector<uint8_t>& packed, std::vector<std::string> codes);

void save(std::string fpath, const std::vector<uint8_t>& packed);

std::vector<uint8_t> load(std::string fpath);

#endif
