#ifndef EPC_VECTOR_H
#define EPC_VECTOR_H

#include <cstdlib>
#include <utility>

namespace epc
{
    template <typename T>
    class vector
    {
    public:
        vector() noexcept : data_(nullptr), capacity_(0), size_(0)
        {
        }

        vector(const vector& vec)
        {
            T* new_data = new T[vec.capacity()];
            try
            {
                for (size_t i = 0; i < vec.size(); i++)
                    new_data[i] = vec.data_[i];
            }
            catch (...)
            {
                delete[] new_data;
                throw;
            }
            capacity_ = vec.capacity();
            size_ = vec.size();
            data_ = new_data;
        }

        vector& operator=(const vector& vec)
        {
            if (this == &vec)
                return *this;

            T* new_data = new T[vec.capacity()];
            try
            {
                for (size_t i = 0; i < vec.size(); i++)
                    new_data[i] = vec.data_[i];
            }
            catch (...)
            {
                delete[] new_data;
                throw;
            }
            delete[] data_;
            data_ = new_data;
            capacity_ = vec.capacity();
            size_ = vec.size();

            return *this;
        }

        ~vector()
        {
            delete[] data_;
        }

        T* data()
        {
            if (!capacity_)
                return nullptr;
            return data_;
        }

        const T* data() const
        {
            if (!capacity_)
                return nullptr;
            return data_;
        }

        T& operator[](size_t idx)
        {
            return data_[idx];
        }

        const T& operator[](size_t idx) const
        {
            return data_[idx];
        }

        void push_back(const T& val)
        {
            if (size_ >= capacity_)
            {
                capacity_ = (capacity_ == 0) ? 1 : capacity_ *= 2;
                T* new_data = new T[capacity_];
                try
                {
                    for (size_t i = 0; i < size_; i++)
                        new_data[i] = data_[i];
                    new_data[size_] = val;
                }
                catch (...)
                {
                    delete[] new_data;
                    throw;
                }
                delete[] data_;
                data_ = new_data;
                size_++;
            }
            else
            {
                data_[size_] = val;
                size_++;
            }
        }

        void pop_back()
        {
            if (size_ > 0)
                size_--;
        }

        void reserve(size_t capacity)
        {
            if (capacity > capacity_)
            {
                T* new_data = new T[capacity];
                try
                {
                    for (size_t i = 0; i < size_; i++)
                        new_data[i] = data_[i];
                }
                catch (...)
                {
                    delete[] new_data;
                    throw;
                }
                delete[] data_;
                data_ = new_data;
                capacity_ = capacity;
            }
        }

        size_t capacity() const
        {
            return capacity_;
        }

        size_t size() const
        {
            return size_;
        }

        void swap(vector& vec) noexcept
        {
            std::swap(data_, vec.data_);
            std::swap(capacity_, vec.capacity_);
            std::swap(size_, vec.size_);
        }

    private:
        T* data_;
        size_t capacity_;
        size_t size_;
    };
}

#endif
