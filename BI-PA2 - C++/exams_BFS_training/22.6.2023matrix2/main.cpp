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

/**
 * @class CVector
 * @brief Templated class implementing a Vector.
 *
 * This class is a templated class implementing a Vector that supports
 * negative indices and provides memory management, copy semantics and
 * an accessor for its elements.
 */
template < typename T_ >
class CVector {
public:
    /**
     * @brief Default constructor. Initializes data to nullptr, size and offset to 0.
     */
    CVector( void ) :  data( nullptr ) , size( 0 ) , offset( 0 ) {};

    /**
     * @brief Range constructor. Creates a CVector with a range from lowerIndex to upperIndex.
     * @param lowerIndex - Lower index bound
     * @param upperIndex - Upper index bound
     */
    CVector( int lowerIndex , int upperIndex ) : size( upperIndex - lowerIndex + 1 ), offset( lowerIndex ) {

        if ( size < 0 ) {
            throw std::invalid_argument("Invalid range");
        }

        data = new T_ [ size ]();
    };

    /**
     * @brief Copy constructor. Copies the other CVector into the new one.
     * @param other - CVector to be copied.
     */
    CVector( const CVector & other ) : size( other.size ), offset( other.offset ) {

        data = new T_ [ size ];

        for ( int i = 0; i < size; ++i ) {
            data[ i ] = other.data[ i ];
        }
    };

    /**
     * @brief Assignment operator. Assigns the other CVector to the current one.
     * @param other - CVector to be assigned.
     * @return A reference to the current CVector.
     */
    CVector & operator = ( const CVector & other) {

        if ( & other == this ) {
            return *this;
        }

        size = other.size;
        offset = other.offset;

        delete[] data;
        data = new T_ [ size ];

        for ( int i = 0; i < size; ++i ) {
            data[ i ] = other.data[ i ];
        }

        return *this;
    };

    /**
     * @brief Destructor. Releases allocated memory.
     */
    ~CVector() {
        delete [] data;
    };

    /**
     * @brief Element access operator. Provides access to the elements of the CVector.
     * @param index - Index of the element to be accessed.
     * @return A reference to the element at the specified index.
     */
    T_ & operator [] ( int index ) const {

        index -= offset;

        if ( index >= size || index < 0 ) {
            throw std::out_of_range( "Index out of range" );
        }

        return data[ index ];
    };

//private:
    T_ * data; ///< Pointer to the array storing the elements of the CVector.
    int size; ///< Size of the CVector.
    int offset; ///< Offset for the index.
};

/**
 * @class CMatrix2
 * @brief Templated class implementing a Matrix.
 *
 * This class is a templated class implementing a Matrix that supports
 * negative indices and provides memory management, copy semantics,
 * and accessor for its elements.
 */
template < typename T_ >
class CMatrix2 {
public:
    /**
     * @brief Constructor. Creates a CMatrix2 with ranges from n1 to n2 and n3 to n4.
     * @param n1 - Lower row index bound
     * @param n2 - Upper row index bound
     * @param n3 - Lower column index bound
     * @param n4 - Upper column index bound
     */
    CMatrix2 ( int n1, int n2, int n3, int n4 ) : matrix( n1 , n2 ) {

        for ( int i = n1; i <= n2; ++i ) {
            matrix[ i ] = CVector < T_ > ( n3, n4 );
        }
    };

    /**
     * @brief Copy constructor. Copies the other CMatrix2 into the new one.
     * @param other - CMatrix2 to be copied.
     */
    CMatrix2(const CMatrix2& other) : matrix( other.matrix ) {};

    /**
     * @brief Assignment operator. Assigns the other CMatrix2 to the current one.
     * @param other - CMatrix2 to be assigned.
     * @return A reference to the current CMatrix2.
     */
    CMatrix2 & operator = ( const CMatrix2& other ) {

        if ( & other == this ) {
            return *this;
        }

        matrix = other.matrix;

        return *this;
    };

    /**
     * @brief Element access operator. Provides access to the elements of the CMatrix2.
     * @param index - Index of the element to be accessed.
     * @return A reference to the element at the specified index.
     */
    CVector < T_ > & operator [] ( int index ) {
        return matrix[ index ];
    };

    /**
     * @brief Equality operator. Checks if the other CMatrix2 is equal to the current one.
     * @param other - CMatrix2 to be compared with the current one.
     * @return True if the matrices are equal, false otherwise.
     */
    bool operator == ( const CMatrix2 & other ) const {

        for ( int i = 0; i < matrix.size; i++ ) {
            for ( int j = 0; j < matrix[0].size; j++ ) {
                if ( matrix[ i ][ j ] != other.matrix[ i ][ j ]) {
                    return false;
                }
            }
        }
        return true;
    };

private:
    CVector < CVector < T_ > > matrix; ///< Internal storage for the matrix.
};

#ifndef __PROGTEST__
int main() {

    // Integer test
    CMatrix2<int> m1( -2, 2, -2, 2 );
    assert( ( m1[-2][-2] = 5 ) == 5 );
    assert( ( m1[2][2] = 10 ) == 10 );

    try {
        int x = m1[3][3];
    } catch ( const std::out_of_range& e ) {
        std::cout << e.what() << std::endl;
    }

    // Double test
    CMatrix2<double> m2( -2, 2, -2, 2 );
    assert( ( m2[-2][-2] = 5.5 ) == 5.5 );
    assert( ( m2[2][2] = 10.5 ) == 10.5 );

    try {
        double x = m2[3][3];
    } catch ( const std::out_of_range& e ) {
        std::cout << e.what() << std::endl;
    }

    // String test
    CMatrix2<std::string> m3( -2, 2, -2, 2 );
    assert( ( m3[-2][-2] = "test1" ) == "test1" );
    assert( ( m3[2][2] = "test2" ) == "test2" );

    try {
        std::string x = m3[3][3];
    } catch ( const std::out_of_range& e ) {
        std::cout << e.what() << std::endl;
    }

    // Test copy constructor and equality operator
    CMatrix2<int> m4( -2, 2, -2, 2 );
    m4[-2][-2] = 5;
    m4[2][2] = 10;
    CMatrix2<int> m5( m4 );
    assert( m4 == m5 );

    // Test assignment operator and equality operator
    CMatrix2<int> m6( -2, 2, -2, 2 );
    m6[-2][-2] = 5;
    m6[2][2] = 10;
    CMatrix2<int> m7 = m6;
    assert( m6 == m7 );

    return 0;
}
#endif /* __PROGTEST__ */
