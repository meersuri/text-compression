#include<iostream>
#include<string>
#include<vector>
#include<queue>
#include<cmath>
#include<unordered_map>
#include<unordered_set>
#include<memory>
#include<cassert>
#include<fstream>
#include<limits>

#include "pack.hpp"
#include "node.hpp"
#include "huffman.hpp"

template<class T>
Huffman<T>::Huffman() {
    constexpr uint64_t codebook_size = static_cast<int>(pow(2, 8*sizeof(T)));
    _codebook.resize(codebook_size);
}

template<class T>
void Huffman<T>::_make_nodes(const std::vector<T> &source) {
    for (const auto &s: source) {
        if (_node_map.find(s) != _node_map.end())
            _node_map[s]->inc_count();
        else
            _node_map[s] = std::make_shared<Node<T>>(s);
    }
}

template<class T>
std::shared_ptr<Node<T>> Huffman<T>::_build_tree() {
    std::priority_queue<std::shared_ptr<Node<T>>, std::vector<std::shared_ptr<Node<T>>>, NodeCompare<T>> pq;
    for (const auto &pair: _node_map) {
        pq.push(pair.second);
    }
    while (pq.size() > 1) {
        auto a = pq.top();
        pq.pop();
        auto b = pq.top();
        pq.pop();
        std::shared_ptr<Node<T>> parent = std::make_shared<Node<T>>('\0', a->count() + b->count());
        parent->set_left(a);
        parent->set_right(b);
        a->set_parent(parent);
        b->set_parent(parent);
        pq.push(parent);
    }
    auto root = pq.top();
    pq.pop();
    return root;
}

template<class T>
void Huffman<T>::_dfs(std::shared_ptr<Node<T>> root, std::string code) {
    if (!root->left() && !root->right()) {
        root->set_code(code);
        return;
    }
    if (root->left())
        _dfs(root->left(), code + "0");
    if (root->right())
        _dfs(root->right(), code + "1");
    return;
}
        
template<class T>
void Huffman<T>::_build_codebook(std::shared_ptr<Node<T>> root) {
    _dfs(root, "");
    for (const auto &pair: _node_map) {
        int idx = static_cast<int>(pair.first) - static_cast<int>(std::numeric_limits<T>::lowest());
        _codebook[idx] = pair.second->code();
        _decodebook[pair.second->code()] = pair.second->symbol();
    }

}

template<class T>
std::vector<std::string> Huffman<T>::_encode(const std::vector<T>& source) {
    std::vector<std::string> encoded;
    for (const auto &s: source) {
        encoded.emplace_back(_node_map[s]->code());
    }
    return encoded;
}

template<class T>
std::vector<std::string> Huffman<T>::encode(const std::vector<T> &source) {
    _make_nodes(source);
    auto root = _build_tree();
    _build_codebook(root);
    return _encode(source);
}

template<class T>
std::vector<T> Huffman<T>::_decode(std::shared_ptr<PackedData> data) {
    std::unordered_set<std::string> codes;
    for (const auto &code: _codebook)
        codes.insert(code);
    std::vector<std::string> encoded = unpack(data, codes);
    std::vector<T> decoded;
    for (const auto &code: encoded) {
        T symbol = _decodebook[code];
        decoded.emplace_back(symbol);
    }
    return decoded;
}

template<class T>
std::vector<T> Huffman<T>::decode(std::shared_ptr<PackedData> data) {
    return _decode(data);
}

template<class T>
std::vector<T> Huffman<T>::decode(const std::string &fpath) {
    std::shared_ptr<PackedData> packed = _load_compressed(fpath);
    std::cout << "loaded size " << packed->data.size() << std::endl;
    return _decode(packed);
}

template<class T>
std::shared_ptr<PackedData> Huffman<T>::_load_compressed(const std::string &fpath) {
    auto file = std::ifstream(fpath, std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("Couldn't open file");
    uint64_t file_size, content_len;
    auto packed = std::make_shared<PackedData>();
    file.read(reinterpret_cast<char*>(&file_size), 8);
    packed->data.resize(file_size);
    file.read(reinterpret_cast<char*>(&packed->content_len), 8);
    uint32_t codebook_size;
    file.read(reinterpret_cast<char*>(&codebook_size), 4);
    std::unordered_set<uint32_t> offsets(codebook_size);
    std::vector<bool> missing_codes(codebook_size, false);
    uint32_t offset = 0;
    for (int i = 0; i < codebook_size; ++i) {
        auto code_size = file.get();
        if (code_size == 0) {
            missing_codes[i] = true;
            continue;
        }
        offset += static_cast<uint32_t>(code_size);
        offsets.emplace(offset);
    }
    uint8_t chunk = 0;
    uint32_t pos = 0;
    std::string code = "";
    auto curr_pos = file.tellg();
    file.seekg(0, std::ios_base::end);
    auto total_bytes = file.tellg();
    file.seekg(curr_pos);
    auto packed_codebook_size = total_bytes - curr_pos - (file_size + 1);
    std::vector<char> packed_codebook(packed_codebook_size);
    file.read(packed_codebook.data(), packed_codebook_size); 
    int ci = 0;
    int pi = 0;
    while (pi < packed_codebook.size()) {
        chunk = packed_codebook[pi++];
        for (int i = 7; i >= 0; --i) {
            if (chunk & (1 << i))
                code += "1";
            else
                code += "0";
            if (offsets.find(++pos) != offsets.end()) {
                while (ci < codebook_size && missing_codes[ci]) { ci++; }
                if (ci == codebook_size)
                    break;
                _codebook[ci++] = code;
                code = "";
            }
        }
    }
    file.read(packed->data.data(), file_size);
    for (int i = 0; i < codebook_size; ++i) {
        if (_codebook[i].size() == 0)
            continue;
        _decodebook[_codebook[i]] = i + std::numeric_limits<T>::lowest();
    }
    return packed;
}

int run(int argc, char *argv[]) {
    if (argc == 1) {
        std::cerr << "ERROR: Enter path to input file\n";
        return 1;
    }
    std::string fpath = argv[1];
    std::vector<char> source = load(fpath);
    std::cout << "file size " << source.size() << std::endl;
    Huffman<char> coder;
    auto encoded = coder.encode(source);
    auto packed = pack(encoded);
    std::cout << "compressed size " << packed->data.size() << std::endl;
    save_compressed("compressed.bin", coder.codebook(), packed);
    std::vector<char> decoded = coder.decode("compressed.bin");
    std::cout << "decoded size " << decoded.size() << std::endl;
    assert (decoded == source);
    std::cout << "Success!\n";
    std::cout << "Compressed to compressed.bin\n";
    return 0;
}
