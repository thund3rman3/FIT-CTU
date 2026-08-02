#ifndef EPC_VECTOR_H
#define EPC_VECTOR_H

#include <algorithm>
#include <cstdlib>
#include <memory>
#include <utility>

namespace epc
{
    template <typename T, size_t N>
    class vector
    {
    public:
        vector() noexcept : size_(0)
        {
        }

        vector(const vector& other) : size_(other.size_)
        {
            std::uninitialized_copy_n(other.data(), other.size(), data());
        }

        vector& operator=(const vector& other)
        {
            if (this == &other)
                return *this;

            vector tmp(other);
            swap(tmp);
            return *this;
        }

        ~vector()
        {
            std::destroy_n(data(), size_);
        }

        T* data()
        {
            return reinterpret_cast<T*>(storage_);
        }

        const T* data() const
        {
            return reinterpret_cast<const T*>(storage_);
        }

        T& operator[](size_t idx)
        {
            return *(data() + idx);
        }

        const T& operator[](size_t idx) const
        {
            return *(data() + idx);
        }

        void push_back(const T& val)
        {
            if (size_ >= N)
                return;
            std::construct_at(data() + size_, val);
            ++size_;
        }

        void pop_back()
        {
            if (!size_)
                return;
            std::destroy_at(data() + size_ - 1);
            --size_;
        }

        void clear()
        {
            std::destroy_n(data(), size_);
            size_ = 0;
        }

        size_t capacity() const
        {
            return N;
        }

        size_t size() const
        {
            return size_;
        }

        void swap(vector& other)
        {
            size_t min = std::min(size_, other.size_);
            std::swap_ranges(data(), data() + min, other.data());

            if (size_ > other.size_)
            {
                std::uninitialized_copy(data() + min, data() + size_, other.data() + min);
                std::destroy(data() + min, data() + size_);
            }
            else if (size_ < other.size_)
            {
                std::uninitialized_copy(other.data() + min, other.data() + other.size_, data() + min);
                std::destroy(other.data() + min, other.data() + other.size_);
            }
            std::swap(size_, other.size_);
        }

    private:
        alignas(T) unsigned char storage_[sizeof(T) * N];
        size_t size_;
    };
}

#endif
