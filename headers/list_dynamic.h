#pragma once

#include "list_basic.h"

// 这个文件基于基本列表实现了动态链表

namespace clazy {

// 动态列表节点的抽象基类
template <typename T>
class AbstractDynamicListNode {
protected:
    T _data;
public:
    // 直接前驱和直接后继的获取接口和设置接口（返回本身的指针）
    virtual AbstractDynamicListNode<T>* pred() { return this; }
    virtual AbstractDynamicListNode<T>* succ() { return this; }
    virtual void setPred(AbstractDynamicListNode<T>* _pred) { }
    virtual void setSucc(AbstractDynamicListNode<T>* _succ) { }

    // 获取数据的接口
    virtual T& data() { return _data; }

    // 默认实现的构造函数
    AbstractDynamicListNode() {}
    AbstractDynamicListNode(T _data): _data(_data) {}

    // 默认实现的比较接口
    bool operator==(const AbstractDynamicListNode<T>& other) const { return _data == other._data; };
    bool operator!=(const AbstractDynamicListNode<T>& other) const { return !(*this == other);}
};

// 动态列表使用指针表示位置
template <typename T>
using ListNodePos = AbstractDynamicListNode<T>*;

// 单向（后向）节点
template <typename T>
class ForwardListNode : public AbstractDynamicListNode<T> {
protected:
    ListNodePos<T> _succ = nullptr;
public:
    ForwardListNode() {}
    ForwardListNode(T _data): AbstractDynamicListNode<T>(_data) {}
    virtual ListNodePos<T> succ() override { return _succ; }
    virtual void setSucc(ListNodePos<T> _succ) override { this->_succ = _succ; }
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
    virtual ListNodePos<T> pred() override { return _pred; }
    virtual void setPred(ListNodePos<T> _pred) override { this->_pred = _pred; }
    constexpr static bool isBidirectional() { return true; }
};

// 动态列表
template <typename T, typename Node>
requires (is_base_of_v<AbstractDynamicListNode<T>, Node> && clazy_framework::is_listnode_type<T, Node>)
class DynamicList : public BasicList<T, ListNodePos<T>, Node> {
public:
    // 重载找位置相关的函数
    virtual T& data(ListNodePos<T> pos) const override { return pos->data(); }
    virtual ListNodePos<T> pred(ListNodePos<T> pos) const override { return pos->pred(); }
    virtual ListNodePos<T> succ(ListNodePos<T> pos) const override { return pos->succ(); }
    virtual ListNodePos<T> setPred(ListNodePos<T> pos, ListNodePos<T> _pred) override { 
        if (pos != nullptr) {
            pos->setPred(_pred);
        }
        return pos;
    }
    virtual ListNodePos<T> setSucc(ListNodePos<T> pos, ListNodePos<T> _succ) override {
        if (pos != nullptr) {
            pos->setSucc(_succ);
        }
        return pos;
    }
    virtual ListNodePos<T> invalid() const override { return nullptr; }

    // 重载空间分配相关的函数
    virtual ListNodePos<T> create() override { return new Node(); }
    virtual ListNodePos<T> create(const T& e) override { return new Node(e); }
    virtual void destroy(ListNodePos<T> pos) override { delete pos; }
    virtual void destroyAll() override;

    DynamicList() { this->createEmptyList(); } // 默认构造函数
    DynamicList(const DynamicList<T, Node>& L) { this->duplicateList(L); } // 复制构造函数
    ~DynamicList() { destroyAll(); } // 析构函数

    template <typename Container>
    requires (clazy_framework::is_linear_structure<T, Container>)
    auto& operator=(const Container& L) {
        this->duplicateList(L); 
        return *this;
    } // 线性表类型转换
};

template <typename T, typename Node>
void DynamicList<T, Node>::destroyAll() {
    auto p = this->_head;
    do {
        auto q = succ(p);
        destroy(p);
        p = q;
    } while (p != this->_tail);
}

}
