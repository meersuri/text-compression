#ifndef HUFFMAN_HPP
#define HUFFMAN_HPP

#include<string>
#include<memory>
#include "node.hpp"

template <class T>
class Huffman {
    std::unordered_map<T, std::shared_ptr<Node<T>>> _node_map;
    std::vector<std::string> _codebook;
    std::priority_queue<Node<T>> _pq;
    void _make_nodes(const std::vector<T> &source);
    std::shared_ptr<Node<T>> _build_tree();
    void _dfs(std::shared_ptr<Node<T>> root, std::string code);
    void _build_codebook(std::shared_ptr<Node<T>> root);
    std::vector<std::string> _encode(const std::vector<T> &source);
    public:
        Huffman();
        std::vector<std::string> compress(const std::vector<T> &source);
};

#endif
    
