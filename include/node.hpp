#ifndef NODE_HPP
#define NODE_HPP

#include<iostream>
#include<memory>

template<class T>
class Node {
    T _symbol; 
    uint64_t _count;
    std::string _code;
    std::shared_ptr<Node<T>> _left, _right, _parent;
    public:
        Node() = default;
        Node(T s): _symbol(s), _count(1), _left(nullptr), _right(nullptr), _parent(nullptr) {}
        Node(T s, uint64_t c): _symbol(s), _count(c), _left(nullptr), _right(nullptr), _parent(nullptr) {}
        Node(const Node<T> &node) = default;
        T symbol() const { return _symbol; }
        uint64_t count() const { return _count; }
        void set_count(uint64_t new_count) { _count = new_count; }
        void set_code(std::string code) { _code = code; }
        void inc_count() { _count++;}
        void set_parent(std::shared_ptr<Node<T>> node) { _parent = node;}
        void set_left(std::shared_ptr<Node<T>> node) { _left = node; }
        void set_right(std::shared_ptr<Node<T>> node) { _right = node; }
        std::string code() const { return _code; }
        std::shared_ptr<Node<T>> left() const { return _left;}
        std::shared_ptr<Node<T>> right() const { return _right;}
        std::shared_ptr<Node<T>> parent() const { return _parent;}
};

template<class T>
std::ostream& operator<<(std::ostream &os, const Node<T> &node) {
    os << "Node(" << node.symbol() << ',' << node.count();
    if (node.code().size())
        os << node.code();
    os << ")";
    return os;
}

template<class T>
class NodeCompare {
    public:
        bool operator() (const std::shared_ptr<Node<T>> &a, const std::shared_ptr<Node<T>> &b) {
            return a->count() > b->count();
    }
};

#endif
