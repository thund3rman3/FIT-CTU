#ifndef EPC_VECTOR_H
#define EPC_VECTOR_H

#include <cstdlib>
#include <memory>
#include <utility>
#include <algorithm>
#include <new>

namespace epc
{
   template <typename T, size_t N>
   class vector
   {
      public:
         vector() noexcept : data_(reinterpret_cast<T*>(storage_)) , capacity_(N), size_(0)  { }

         vector(const vector& other) : data_(reinterpret_cast<T*>(storage_)), capacity_(N), size_(other.size_)
         {
            if (!other.size_)
               return;

            if (other.is_small())
            {
               std::uninitialized_copy_n(other.data_, other.size_, data_);
            }
            else
            {
               T* new_data = static_cast<T*>(::operator new(other.size_ * sizeof(T)));
               try
               {
                  std::uninitialized_copy_n(other.data_, other.size_, new_data);
               }
               catch (...)
               {
                  ::operator delete(new_data);
                  throw;
               }
               data_ = new_data;
               capacity_ = other.size_;
            }
         }

         vector& operator=(const vector& other)
         {
            if (other.size_ <= capacity_)
            {
               std::destroy_n(data_, size_);
               std::uninitialized_copy_n(other.data_, other.size_, data_);
               size_ = other.size_;
               return *this;
            }

            T* new_data = static_cast<T*>(::operator new(other.size_ * sizeof(T)));
            try
            {
               std::uninitialized_copy_n(other.data_, other.size_, new_data);
            }
            catch (...)
            {
               ::operator delete(new_data);
               throw;
            }

            std::destroy_n(data_, size_);
            if (!is_small())
               ::operator delete(data_);

            data_ = new_data;
            size_ = other.size_;
            capacity_ = other.size_;
            return *this;
         }

         vector(vector&& other) : size_(other.size_)
         {
            if (other.is_small())
            {
               data_ = reinterpret_cast<T*>(storage_);
               std::uninitialized_move_n(other.data_, other.size_, data_);
               std::destroy_n(other.data_, other.size_);
            }
            else
            {
               data_ = other.data_;
               capacity_ = other.capacity_;
               other.data_ = reinterpret_cast<T*>(other.storage_);
               other.capacity_ = N;
            }
            other.size_ = 0;
         }

         vector& operator=(vector&& other)
         {
            std::destroy_n(data_, size_);
            if (!is_small())
               ::operator delete(data_);

            if (other.is_small())
            {
               data_ = reinterpret_cast<T*>(storage_);
               capacity_ = N;
               std::uninitialized_move_n(other.data_, other.size_, data_);
               std::destroy_n(other.data_, other.size_);
            }
            else
            {
               data_ = other.data_;
               capacity_ = other.capacity_;
            }
            size_ = other.size_;
            other.data_ = reinterpret_cast<T*>(other.storage_);
            other.size_ = 0;
            return *this;
         }

         ~vector()
         {
            std::destroy_n(data_, size_);
            if (!is_small())
               ::operator delete(data_);
         }

         T* data()
         {
            return data_;
         }

         const T* data() const
         {
            return data_;
         }

         T& operator[](size_t idx)
         {
            return *(data_ + idx);
         }
         const T& operator[](size_t idx) const
         {
            return *(data_ + idx);
         }

         void push_back(const T& val)
         {
            if (size_ >= capacity_)
               reserve(capacity_ * 2);

            std::construct_at(data_ + size_, val);
            ++size_;
         }

         void push_back(T&& val)
         {
            if (size_ >= capacity_)
               reserve(capacity_ * 2);

            std::construct_at(data_ + size_, std::move(val));
            ++size_;
         }

         template <typename... Ts>
         void emplace_back(Ts&&... val)
         {
            if (size_ >= capacity_)
               reserve(capacity_ * 2);

            std::construct_at(data_ + size_, std::forward<Ts>(val)...);
            ++size_;
         }

         void pop_back()
         {
            if (!size_)
               return;
            std::destroy_at(data_ + size_ - 1);
            --size_;
         }

         void clear()
         {
            std::destroy_n(data_, size_);
            size_ = 0;
         }

         void reserve(size_t new_capacity)
         {
            if (new_capacity <= capacity_)
               return;

            T* new_data = static_cast<T*>(::operator new(new_capacity * sizeof(T)));
            try
            {
               std::uninitialized_move_n(data_, size_, new_data);
            }
            catch (...)
            {
               ::operator delete(new_data);
               throw;
            }

            std::destroy_n(data_, size_);
            if (!is_small())
               ::operator delete(data_);
            data_ = new_data;
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

         void swap(vector& other)
         {
            if (!this->is_small() && !other.is_small())
            {
               std::swap(data_, other.data_);
               std::swap(size_, other.size_);
               std::swap(capacity_, other.capacity_);
            }
            else if (this->is_small() && other.is_small())
            {
               size_t min = std::min(size_, other.size_);
               std::swap_ranges(data_, data_ + min, other.data_);

               if (size_ > other.size_)
               {
                  std::uninitialized_move(data_ + min, data_ + size_, other.data_ + min);
                  std::destroy(data_ + min, data_ + size_);
               }
               else if (size_ < other.size_)
               {
                  std::uninitialized_move(other.data_ + min, other.data_ + other.size_, data_ + min);
                  std::destroy(other.data_ + min, other.data_ + other.size_);
               }
               std::swap(size_, other.size_);
            }
            else
            {
               vector* small = is_small() ? this : &other;
               vector* big = !is_small() ? this : &other;
               T* new_data = big->data_;
               big->data_ = reinterpret_cast<T*>(big->storage_);
               std::uninitialized_move_n(small->data_, small->size_, big->data_);
               std::destroy_n(small->data_, small->size_);
               small->data_ = new_data;
               small->capacity_ = big->size_;
               big->capacity_ = N;
               std::swap(small->size_, big->size_);
            }
         }

      private:
         alignas(T) unsigned char storage_[sizeof(T)*N];
         T* data_;
         size_t capacity_;
         size_t size_;

         bool is_small() const {
            return data_ == reinterpret_cast<const T*>(storage_);
         }
   };
}

#endif
