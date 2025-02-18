#include <cmath>
#include "framework.h"
using namespace clazy_framework;

// 这个例子展示了复杂度分析评估时间复杂度的意义
// 下面这个函数，您可以自己分析它的时间复杂度
int f(int n) {
    int s = 0;
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= i; j++) {
            for (int k = 1; k <= j; k++) {
                for (int l = 1; l <= j; l *= 2) {
                    s += k * l;
                }
            }
        }
    }
    return s;
}

int main() {
    double t, r;
    int temp; // 通过一个返回值，避免f被优化掉
    // 这里验证算法复杂度确实是Theta(n^3 * logn)
    // 用时间（纳秒）除以n^3 * logn计算比例
        /* 当然，使用示例代码计算时间的时候精度只有三位小数 */
    // 在n充分大的时候，这个值是比较接近的
    for (int n : {100, 250, 400, 700, 800, 900, 1000}) {
        t = calculateTime([&]() { temp = f(n); });
        r = t * 1e9 / (n*n*n*log2(n));
        cout << "time(" << setw(4) << n << ") = " << setw(6) << t << "\t";
        cout << "ratio = " << setw(10) << r << "\t";
        cout << "answer = " << setw(10) << temp << endl;
    }
    return 0;
}
