#ifndef HUFFMAN_HPP
#define HUFFMAN_HPP

#include <string>
#include <memory>
#include <queue>
#include "node.hpp"

template <class T>
class Huffman {
    std::unordered_map<T, std::shared_ptr<Node<T>>> _node_map;
    std::unordered_map<std::string, T> _decodebook;
    std::vector<std::string> _codebook;
    std::priority_queue<Node<T>> _pq;
    void _make_nodes(const std::vector<T> &source);
    std::shared_ptr<Node<T>> _build_tree();
    void _dfs(std::shared_ptr<Node<T>> root, std::string code);
    void _build_codebook(std::shared_ptr<Node<T>> root);
    std::vector<std::string> _encode(const std::vector<T> &source);
    std::vector<T> _decode(const std::shared_ptr<PackedData> packed);
    std::shared_ptr<PackedData> _load_compressed(const std::string &fpath);
    public:
        Huffman();
        const std::vector<std::string> &codebook() const { return _codebook; }
        std::vector<std::string> encode(const std::vector<T> &source);
        std::vector<T> decode(const std::shared_ptr<PackedData> packed);
        std::vector<T> decode(const std::string &fpath);
};

#endif
    
