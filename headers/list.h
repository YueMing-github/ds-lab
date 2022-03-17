#pragma once

#include "framework.h"
#include "list_framework.h"

namespace clazy {

template <typename T>
using ListNodePos = clazy_framework::ListNodePos<T>;

// 单向（后向）节点
template <typename T>
class ForwardListNode : public clazy_framework::AbstractListNode<T> {
protected:
    T _data;
    ListNodePos<T> _succ = nullptr;
public:
    ForwardListNode() {}
    ForwardListNode(T _data): _data(_data) {}
    virtual ListNodePos<T> setSucc(ListNodePos<T> _succ) { 
        this->_succ = _succ; 
        return this; 
    }
    virtual ListNodePos<T> succ() { return _succ; }
    virtual T& data() { return _data; }
    constexpr static bool isBidirectional() { return false; }
};

// 双向节点
template <typename T>
class ListNode : public ForwardListNode<T> {
protected:
    ListNodePos<T> _pred = nullptr;
public:
    ListNode() {}
    ListNode(T _data): ForwardListNode<T>(_data) {}
    virtual ListNodePos<T> setPred(ListNodePos<T> _pred) { 
        this->_pred = _pred; 
        return this; 
    }
    virtual ListNodePos<T> pred() { return _pred; }
    constexpr static bool isBidirectional() { return true; }
};

template <typename T>
using ListIterator = clazy_framework::AbstractList<T>::Iterator;

// 普通列表
template <typename T, typename Node = ListNode<T>, bool Circular = false>
requires (clazy_framework::ListNodeType<T, Node>)
class List : public clazy_framework::AbstractList<T> {
protected:
    ListNodePos<T> _head;           // 列表的首哨兵
    ListNodePos<T> _tail;           // 列表的尾哨兵
    int _size;                      // 列表的规模

    // 和内存管理相关的接口，在静态链表中，这些接口会被重载
    // 动态链表和静态链表的本质区别，是对申请和释放内存的区别
    // 创建新的节点的接口
    virtual ListNodePos<T> create() { return new Node(); }
    virtual ListNodePos<T> create(const T& e) { return new Node(e); }
    // 删除一个节点的接口
    virtual void destroy(ListNodePos<T> pos) { delete pos; }
    // 删除所有节点的接口
    virtual void destroyAll();

private:
    // 创建一个空表
    void createEmptyList();

public:
    List() { createEmptyList(); }           // 默认构造函数
    List(const List<T, Node, Circular>& L); // 复制构造函数
    ~List() { destroyAll(); }               // 析构函数

    virtual int size() const { return _size; }
    virtual void clear() { destroyAll(); createEmptyList(); } // 删除所有节点
    virtual ListIterator<T> begin() const { return ListIterator<T>(_head->succ()); }
    virtual ListIterator<T> end() const { return ListIterator<T>(_tail); }

    // 插入元素（包括前插和后插），返回被插入元素的指针
    virtual ListNodePos<T> insertAsPred(ListNodePos<T> pos, const T& e);
    virtual ListNodePos<T> insertAsSucc(ListNodePos<T> pos, const T& e);
    
    // 删除元素，返回被删除的元素
    virtual T remove(ListNodePos<T> pos);

    // 查找元素，返回查找到的元素的位置，未找到返回nullptr
    virtual ListNodePos<T> find(const T& e) const;

    // 重载，单向链表需要O(n)找位置
    virtual T& back() const {
        if constexpr (Node::isBidirectional()) {
            return clazy_framework::AbstractList<T>::back();
        } else {
            return *(begin() + (_size - 1));
        }
    }

    // 重载，单向链表需要O(n)找位置
    virtual T pop_back() {
        if constexpr (Node::isBidirectional()) {
            return clazy_framework::AbstractList<T>::pop_back();
        } else {
            return remove((begin() + (_size - 1)).base());
        }
    }
};

// 以下是上述接口的实现部分
// 删除全部节点
template <typename T, typename Node, bool Circular>
void List<T, Node, Circular>::destroyAll() {
    auto p = _head;
    do {
        auto q = p->succ();
        destroy(p);
        p = q;
    } while (p != _tail);
}

// 生成空表
template <typename T, typename Node, bool Circular>
void List<T, Node, Circular>::createEmptyList() {
    _size = 0;
    _head = create();
    _tail = create();
    _head->setSucc(_tail);
    if constexpr (Circular) {
        _tail->setSucc(_tail)->setPred(_tail); // 循环链表，形成自环
    } else {
        _tail->setPred(_head); // 普通链表，直接接在首哨兵上
    }
}

// 复制构造函数
// 这里使用的方法是一正一反
template <typename T, typename Node, bool Circular>
List<T, Node, Circular>::List(const List<T, Node, Circular>& L): _size(0) {
    _head = create();
    _tail = create();
    Node last = _head, cur;     // 正向遍历，建立连接
    for (auto it = L.begin(); it != L.end(); ++it) {
        cur = create(*it);
        last = cur->setPred(last->setSucc(cur));
    }
    _tail->setPred(last->setSucc(_tail)); // 末尾元素连接尾哨兵
    if constexpr (Circular) {   // 在末尾的处理上有区别
        _head->succ()->setPred(_tail);
        _tail->setSucc(_head->succ()); // 循环链表中，将尾哨兵连上L[0]
    }                           // 线性链表则不需要这一步骤
}

// 插入的语义：
// 对于任何链表，首哨兵的前插都是未定义行为
// 对于线性链表，尾哨兵的后插是未定义行为
// 对于循环链表，尾哨兵的后插 等价于 首哨兵的后插

// 前插
// 对于单向链表，这里的方法会导致原先指向pos的指针失效，从而不是很安全
template <typename T, typename Node, bool Circular>
ListNodePos<T> List<T, Node, Circular>::insertAsPred(ListNodePos<T> pos, const T& e) {
    if constexpr (Node::isBidirectional()) {
        pos = insertAsSucc(pos->pred(), e); // 对前驱节点执行后插    
        if constexpr (Circular) {                // 循环链表
            if (begin().base() == pos->succ()) { // 如果前插的节点是第一个元素
                _head->setSucc(pos);             // 需要重新设置首元素指针的位置
            }
        }
    } else {
        insertAsSucc(pos, pos->data());     // 执行后插，复制pos处的节点
        if (end().base() == pos) {          // 若有必要，更新尾哨兵
            _tail = pos->succ();
        }
        pos->data() = e;                    // 将pos处的值改掉
    }
    return pos;
}

// 后插
template <typename T, typename Node, bool Circular>
ListNodePos<T> List<T, Node, Circular>::insertAsSucc(ListNodePos<T> pos, const T& e) {
    _size++;
    if constexpr (Circular) { // 循环链表，插入为首哨兵的后继，相当于插入为第一个元素的前驱
        if (pos == _head) {       
            return insertAsPred(_head->succ(), e);
        }
    }
    auto succ = pos->succ(), cur = create(e);
    cur->setPred(pos->setSucc(cur))->setSucc(succ); // 四次赋值
    if (succ != nullptr) {
        succ->setPred(cur);
    }
    return cur;
}

// 删除元素
template <typename T, typename Node, bool Circular>
T List<T, Node, Circular>::remove(ListNodePos<T> pos) {
    _size--;
    T temp = pos->data();
    if constexpr (Node::isBidirectional()) { // 双向链表，直接删除
        pos->pred()->setSucc(pos->succ());
        pos->succ()->setPred(pos->pred());
        if constexpr (Circular) {
            if (pos == _head->succ()) {      // 循环链表删除第一个节点的时候，需要修改头哨兵的后继
                _head->setSucc(pos->succ());
            }
        }
        destroy(pos);
    } else {                                 // 单向链表，采用和前插类似的技术
        auto vic = pos->succ();
        pos->data() = vic->data();
        pos->setSucc(vic->succ());
        if (vic == _tail) {                  // 删除最后一个节点，需要重置尾哨兵位置
            _tail = pos;
        }
        destroy(vic);       
    }
    return temp;
}

// 查找元素
template <typename T, typename Node, bool Circular>
ListNodePos<T> List<T, Node, Circular>::find(const T& e) const {
    for (auto it = begin(); it != end(); it++) {
        if (*it == e) {
            return it.base();
        }
    }
    return nullptr;
}

// 利用<<输出
template <typename T, typename Node, bool Circular>
ostream& operator<<(ostream& out, const List<T, Node, Circular>& L) {
    out << "L(";
    for (auto it = begin(L); it != end(L); it++) {
        if (it != begin(L)) {
            out << ", ";
        }
        out << *it;
    }
    return out << ")";
}

}
