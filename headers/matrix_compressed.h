#pragma once

#include "matrix_framework.h"

namespace clazy {

// 矩阵的压缩存储
// 这个部分仅用于408的一些考点上

// 特别注意，在这个文件中的所有示例代码
// 都服从AbstractMatrix的接口定义
// 所以，这个矩阵的行和列都是从0开始的
// 可以发现，当都使用从0开始计算的方法的时候，公式比较简单
// 但考试的时候，不一定是这样

// 行优先的矩阵
template <int R, int C = R, typename T = int>
class RowMajorMatrix : public clazy_framework::AbstractMatrix<R, C, T> {
protected:
    T data[R * C];
public:
    virtual T item(int r, int c) const override {
        return data[r * C + c];
    }

    virtual void set(int r, int c, const T& v) override {
        data[r * C + c] = v;
    }
};

// 列优先的矩阵
template <int R, int C = R, typename T = int>
class ColMajorMatrix : public clazy_framework::AbstractMatrix<R, C, T> {
protected:
    T data[R * C];
public:
    virtual T item(int r, int c) const override {
        return data[c * R + r];
    }

    virtual void set(int r, int c, const T& v) override {
        data[c * R + r] = v;
    }
};

// 对称矩阵，只存储半个矩阵
template <int N, typename T = int>
class SymmetricMatrix : public clazy_framework::AbstractMatrix<N, N, T> {
protected:
    T data[N * (N + 1) / 2];
public:
    virtual T item(int r, int c) const override {
        if (r > c) {
            swap(r, c);
        }
        return data[(r * (r + 1)) / 2 + c];
    }

    virtual void set(int r, int c, const T& v) override {
        if (r > c) {
            swap(r, c);
        }
        data[(r * (r + 1)) / 2 + c] = v;
    }
};

// 下三角矩阵
// 这里是一般的数学的下三角矩阵，也就是上三角区全是0的情况
template <int N, typename T = int>
class LowerTriangularMatrix : public clazy_framework::AbstractMatrix<N, N, T> {
protected:
    T data[N * (N + 1) / 2];
public:
    virtual T item(int r, int c) const override {
        if (r > c) { // 上三角区总是返回0
            return (T)0;
        }
        return data[(r * (r + 1)) / 2 + c];
    }

    virtual void set(int r, int c, const T& v) override { // 不允许对上三角区进行赋值
        if (r <= c) {
            data[(r * (r + 1)) / 2 + c] = v;
        }
    }
};

// 上三角矩阵
// 这里是一般的数学的上三角矩阵，也就是下三角区全是0的情况
template <int N, typename T = int>
class UpperTriangularMatrix : public clazy_framework::AbstractMatrix<N, N, T> {
protected:
    T data[N * (N + 1) / 2];
public:
    virtual T item(int r, int c) const override {
        if (r < c) { // 下三角区总是返回0
            return (T)0;
        }
        return data[(r * (2 * N - 1 - r)) / 2 + c];
    }

    virtual void set(int r, int c, const T& v) override { // 不允许对下三角区进行赋值
        if (r >= c) {
            data[(r * (2 * N - 1 - r)) / 2 + c] = v;
        }
    }
};

// 三对角矩阵
template <int N, typename T = int>
class TridiagonalMatrix : public clazy_framework::AbstractMatrix<N, N, T> {
protected:
    T data[3 * N - 2];
public:
    virtual T item(int r, int c) const override {
        if (abs(r - c) > 1) {
            return (T)0;
        }
        return data[2 * r + c];
    }
    
    virtual void set(int r, int c, const T& v) override {
        if (abs(r - c) > 1) {
            return;
        }
        data[2 * r + c] = v;
    }
};

}