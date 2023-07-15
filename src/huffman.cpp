#include<iostream>
#include<string>
#include<vector>
#include<queue>
#include<cmath>
#include<unordered_map>
#include<memory>

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
        std::cout << *a << " " << *b << " " << *parent <<  std::endl;
    }
    auto root = pq.top();
    pq.pop();
    return root;
}

template<class T>
void Huffman<T>::_dfs(std::shared_ptr<Node<T>> root, std::string code) {
    if (!root->left() && !root->right()) {
        root->set_code(code);
        std::cout << root->symbol() << " " << root->code() << std::endl;
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
    std::cout << *root << std::endl;
    for (const auto &pair: _node_map) {
        _codebook[pair.first] = pair.second->code();
    }

}

template<class T>
std::vector<std::string> Huffman<T>::_encode(const std::vector<T>& source) {
    std::vector<std::string> encoded;
    for (const auto &s: source) {
        std::cout << s <<  " " << _node_map[s]->code() << '\n';
        encoded.emplace_back(_node_map[s]->code());
    }
    return encoded;
}

template<class T>
std::vector<std::string> Huffman<T>::compress(const std::vector<T> &source) {
    _make_nodes(source);
    auto root = _build_tree();
    _build_codebook(root);
    return _encode(source);
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        std::cerr << "ERROR: Enter path to input file\n";
        return 1;
    }
    std::string fpath = argv[1];
    std::vector<uint8_t> source = load(fpath);
    Huffman<uint8_t> coder;
    auto encoded = coder.compress(source);
    for (const auto &x: encoded)
        std::cout << x << ",";
    std::cout << std::endl;
    return 0;
}
