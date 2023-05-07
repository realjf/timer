#ifndef _MIN_HEAP_H_
#define _MIN_HEAP_H_

#include <vector>
#include <mutex>

namespace CTimer {

    template <typename T>
    using Vec_t = std::vector<T>;

    template <typename T>
    using Iter = typename Vec_t<T>::iterator;

    template <typename T>
    class MinHeap {
    public:
        // 添加元素
        void push(const T &val) {
            std::lock_guard<std::mutex> lock(mutex_);
            heap_.push_back(val);
            siftUp(heap_.size() - 1);
        }

        // 获取堆顶元素
        T top() const {
            std::lock_guard<std::mutex> lock(mutex_);
            return heap_.front();
        }

        // 弹出堆顶元素
        void pop() {
            std::lock_guard<std::mutex> lock(mutex_);
            heap_[0] = heap_.back();
            heap_.pop_back();
            siftDown(0);
        }

        // 获取堆的大小
        size_t size() const {
            std::lock_guard<std::mutex> lock(mutex_);
            return heap_.size();
        }

        // 判断堆是否为空
        bool empty() const {
            std::lock_guard<std::mutex> lock(mutex_);
            return heap_.empty();
        }

        // 遍历
        void Traverse(void (*f)(const T &)) {
            std::lock_guard<std::mutex> lock(mutex_);
            for (auto &elem : heap_) {
                f(elem);
            }
        }

        // 删除
        void Remove(const T &elem) {
            std::lock_guard<std::mutex> lock(mutex_);
            for (int i = 0; i < heap_.size(); ++i) {
                if (heap_[i] == elem) {
                    std::swap(heap_[i], heap_.back());
                    heap_.pop_back();

                    int idx = i, left = 2 * idx + 1, right = 2 * idx + 2, min_idx;

                    while (left < heap_.size()) {
                        min_idx = right < heap_.size() && heap_[right] < heap_[left] ? right : left;

                        if (heap_[idx] <= heap_[min_idx]) 
                            break;

                        std::swap(heap_[idx], heap_[min_idx]);
                        idx = min_idx;
                        left = 2 * idx + 1;
                        right = 2 * idx + 2;
                    }

                    return;
                }
            }
        }

    private:
        std::vector<T> heap_;
        mutable std::mutex mutex_;

        // 上移操作，用于添加元素后的维护
        void siftUp(size_t i) {
            while (i > 0) {
                size_t parent = (i - 1) / 2;
                if (heap_[i] < heap_[parent]) {
                    std::swap(heap_[i], heap_[parent]);
                    i = parent;
                } else {
                    break;
                }
            }
        }

        // 下移操作，用于弹出堆顶元素后的维护
        void siftDown(size_t i) {
            size_t left = 2 * i + 1;
            size_t right = 2 * i + 2;
            size_t smallest = i;

            if (left < heap_.size() && heap_[left] < heap_[smallest]) {
                smallest = left;
            }
            if (right < heap_.size() && heap_[right] < heap_[smallest]) {
                smallest = right;
            }

            if (smallest != i) {
                std::swap(heap_[i], heap_[smallest]);
                siftDown(smallest);
            }
        }
    };
} // namespace CTimer

#endif /* _MIN_HEAP_H_ */
