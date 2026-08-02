#ifndef EPC_VECTOR_H
#define EPC_VECTOR_H

#include <cstdlib>
#include <memory>
#include <new>
#include <utility>

namespace epc
{
    template <typename T>
    class vector
    {
    public:
        vector() noexcept : storage_(nullptr), capacity_(0), size_(0)
        {
        }

        vector(const vector& other) : storage_(nullptr), capacity_(0), size_(0)
        {
            if (!other.size_)
                return;

            T* new_storage = static_cast<T*>(::operator new(other.size_ * sizeof(T)));
            try
            {
                std::uninitialized_copy_n(other.storage_, other.size_, new_storage);
            }
            catch (...)
            {
                ::operator delete(new_storage);
                throw;
            }
            storage_ = new_storage;
            size_ = other.size_;
            capacity_ = other.size_;
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
            std::destroy_n(storage_, size_);
            ::operator delete(storage_);
        }

        T* data()
        {
            return storage_;
        }

        const T* data() const
        {
            return storage_;
        }

        T& operator[](size_t idx)
        {
            return storage_[idx];
        }

        const T& operator[](size_t idx) const
        {
            return storage_[idx];
        }

        void push_back(const T& val)
        {
            if (size_ >= capacity_)
                reserve(capacity_ == 0 ? 1 : capacity_ * 2);

            std::construct_at(storage_ + size_, val);
            size_++;
        }

        void pop_back()
        {
            if (!size_)
                return;
            std::destroy_at(storage_ + size_ - 1);
            size_--;
        }

        void clear()
        {
            std::destroy_n(storage_, size_);
            size_ = 0;
        }

        void reserve(size_t new_capacity)
        {
            if (new_capacity <= capacity_)
                return;

            T* new_storage = static_cast<T*>(::operator new(new_capacity * sizeof(T)));
            try
            {
                std::uninitialized_copy_n(storage_, size_, new_storage);
            }
            catch (...)
            {
                ::operator delete(new_storage, new_capacity * sizeof(T));
                throw;
            }

            std::destroy_n(storage_, size_);
            ::operator delete(storage_);
            storage_ = new_storage;
            capacity_ = new_capacity;
        }

        size_t capacity() const
        {
            return capacity_;
        }

        size_t size() const
        {
            return size_;
        }

        void swap(vector& other) noexcept
        {
            std::swap(storage_, other.storage_);
            std::swap(capacity_, other.capacity_);
            std::swap(size_, other.size_);
        }

    private:
        T* storage_;
        size_t capacity_;
        size_t size_;
    };
}

#endif
