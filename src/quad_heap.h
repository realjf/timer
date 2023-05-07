#ifndef _QUAD_HEAP_H_
#define _QUAD_HEAP_H_

#include <vector>
#include <algorithm>

namespace CTimer {

    template <typename T>
    class QuadHeap {
    public:
        QuadHeap() {}

        bool empty() {
            return data_.empty();
        }

        void push(const T &elem) {
            data_.push_back(elem);
            percolate_up(data_.size() - 1);
        }

        void pop() {
            if (empty())
                return;
            std::swap(data_[0], data_[data_.size() - 1]);
            data_.pop_back();
            percolate_down(0);
        }

        const T &top() {
            return data_[0];
        }

    private:
        std::vector<T> data_;

        void percolate_up(int i) {
            while (i > 0) {
                int parent = (i - 1) / 4;
                if (data_[i] > data_[parent]) {
                    std::swap(data_[i], data_[parent]);
                    i = parent;
                } else {
                    break;
                }
            }
        }

        void percolate_down(int i) {
            while (4 * i + 1 < data_.size()) {
                int max_child = 4 * i + 1;
                for (int j = 2; j <= 4; ++j) {
                    if (4 * i + j >= data_.size())
                        break;
                    if (data_[4 * i + j] > data_[max_child])
                        max_child = 4 * i + j;
                }
                if (data_[i] < data_[max_child]) {
                    std::swap(data_[i], data_[max_child]);
                    i = max_child;
                } else {
                    break;
                }
            }
        }
    };
} // namespace CTimer

#endif /* _QUAD_HEAP_H_ */
