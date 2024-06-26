#ifndef __PROGTEST__
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <string>
#include <iostream>
#include <stdexcept>
using namespace std;
#endif /* __PROGTEST__ */

template<typename T_>
class CVector
{
public:
    explicit CVector(unsigned s)
        : arr_max(s), arr_size(s)
    {
        array = new T_[s];
    }

    CVector(size_t s, const T_& val)
    {
        arr_size = arr_max = s;
        array = new T_[s];
        for (int i = 0; i < s; ++i) {
            array[i] = val;
        }
    }

    CVector() = default;

    ~CVector(){ delete [] array;}

    CVector(const CVector<T_>& other)
    {
        arr_size = other.arr_size;
        array = new T_[arr_size];
        size_t idx = 0;
        for(; idx<arr_size;++idx)
        {
            array[idx] = other.array[idx];
        }
    }

    CVector<T_>& operator=(const CVector<T_>& other)
    {
        if(this==&other)
            return *this;

        delete [] array;
        arr_size = other.arr_size;
        array = new T_[arr_size];
        size_t idx = 0;
        for(; idx<arr_size;++idx)
        {
            array[idx] = other.array[idx];
        }
        return *this;
    }

    CVector* begin() const
    {
        return array;
    }

    CVector* end() const
    {
        return array + arr_size;
    }

    size_t size() const
    {
        return arr_size;
    }

    void push_back(const T_& elem)
    {
        if(arr_size>=arr_max)
        {
            arr_max*=2;
            T_* tmp = new T_[arr_max];
            for (int i = 0; i < arr_size; ++i) {
                tmp[i] = array[i];
            }
            delete [] array;
            array = tmp;
        }
        array[arr_size++] = elem;
    }

    T_& operator[](size_t idx) const
    {
        if(idx>=arr_size)
            throw std::out_of_range("");
        return array[idx];
    }

private:
    T_* array = nullptr;
    size_t arr_size = 0;
    size_t arr_max = 10;
};

template <typename T_>
class CMatrix3
{
    size_t m_size = 0;
    CVector<CVector<CVector<T_>>> vec3D;

public:
    CMatrix3(unsigned x, unsigned y, unsigned z)
        : vec3D(x, CVector<CVector<T_>>(y, CVector<T_>(z))), m_size(x)
    {}

//    CMatrix3(const CMatrix3& other)
//    {
//
//    }
//
//    CMatrix3& operator=(const CMatrix3& other)
//    {
//        if(this == &other)
//            return *this;
//        return *this;
//    }

    ~CMatrix3() =default;

    CVector<CVector<T_>>& operator[](size_t idx)
    {
        //auto x =vec3D[idx];
        if(m_size<=idx)
            throw out_of_range("");
        return vec3D[idx];
    }

    // operator +
    // operator ==
};

#ifndef __PROGTEST__
int main ( )
{
    CMatrix3<int> m ( 3, 4, 5 );
    m[0][0][0] = 128;
    m[2][3][1] = 7;
    assert ( m[0][0][0] == 128 );
    assert ( m[2][3][1] == 7 );

    m[1][1][1] = m[0][0][0] + m[2][3][1];
    m[2][3][4] = 135;
    assert ( m[1][1][1] == m[2][3][4] );

    CMatrix3<int> copy ( m );
    try { copy[4][1][2]; assert ( "Missing exception for access i0 = 4, limit = 2" == nullptr ); }
    catch ( out_of_range & e ) { }
    catch ( ... ) { assert ( "Incorrect exception thrown" == nullptr ); }
    try { copy[1][4][2]; assert ( "Missing exception for access i1 = 4, limit = 3" == nullptr ); }
    catch ( out_of_range & e ) { }
    catch ( ... ) { assert ( "Incorrect exception thrown" == nullptr ); }
    try { copy[2][1][6]; assert ( "Missing exception for access i2 = 6, limit = 4" == nullptr ); }
    catch ( out_of_range & e ) { }
    catch ( ... ) { assert ( "Incorrect exception thrown" == nullptr ); }

    copy[2][3][1] = 42;
    assert ( m[2][3][1] == 7 );
    assert ( copy[2][3][1] == 42 );

    CMatrix3<int> n ( 2, 2, 2 );
    n[0][1][0] = 56;
    n[1][0][1] = 42;
    m = n;
    n[0][1][0] = 0;
    assert ( m[0][1][0] == 56 );
    try { m[2][1][0]; assert ( "Missing exception for access i0 = 2, limit = 2" == nullptr ); }
    catch ( out_of_range & e ) { }
    catch ( ... ) { assert ( "Incorrect exception thrown" == nullptr ); }

    CMatrix3<string> s ( 2, 1, 1 );
    s[0][0][0] = "Hello ";
    s[1][0][0] = "World!";
    assert ( ( s[0][0][0] + s[1][0][0] ) == string("Hello World!") );

    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
