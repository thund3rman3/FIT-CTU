#ifndef __PROGTEST__

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cstdint>
#include <cassert>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>
#include <memory>
#include <functional>
#include <compare>
#include <stdexcept>

#endif /* __PROGTEST__ */

class CBigInt {
public:
    // default constructor
    CBigInt() : str(std::string("0")), sign(true) {
    }

    // copying/assignment/destruction
    CBigInt(const CBigInt &i) = default;

    CBigInt &operator=(const CBigInt &i) {
        if (this == &i)
            return *this;
        str = i.str;
        sign = i.sign;
        return *this;
    }

    ~CBigInt() = default;

    // int constructor
    CBigInt(int i) {
        sign = i > -1;
        if (i >= 0) {
            str = std::to_string(i);
        } else {
            str = std::to_string(-i);
        }
    }

    static void pop_zeros(std::string &s) {
        size_t zero_cnt = 0;
        bool zero_end = false;
        for (const char &itS: s) {
            if (!std::isdigit(itS))
                throw std::invalid_argument("");
            if (!zero_end && itS != '0')
                zero_end = true;
            else if (!zero_end && itS == '0')
                zero_cnt++;
        }
        if(zero_cnt == s.length())
            s = std::string("0");
        else if (zero_cnt)
            s = s.substr(zero_cnt);
    }

    static void fill_zeros(std::string &s, size_t zeros) {
        s = std::string(zeros, '0').append(s);
    }

    CBigInt(const char *s)
            : CBigInt(std::string(s)) {
    }

    // string constructor
    // U konstruktoru předpokládáme, že se musí načíst celý řetězec ->(12x)->throw
    // pokud S neobsahuje cislo -> throw invalid_arg
    CBigInt(std::string s) : str(s), sign(false) {
        if (str[0] != '-') {
            sign = true;
            pop_zeros(str);
        } else {
            str.erase(0, 1);
            pop_zeros(str);
            if(str == "0")
                sign = true;
        }
    }

    // operator +, any combination {CBigInt/int/string} + {CBigInt/int/string}
    friend CBigInt operator+(const CBigInt &i1, const CBigInt &i2) {
        std::string one = i1.str, two = i2.str;
        int sign1 = !i1.sign ? -1 : 1;
        int sign2 = !i2.sign ? -1 : 1;
        if (CBigInt(i1.str) < CBigInt(i2.str)) {
            one = i2.str;
            two = i1.str;
            sign1 = !i2.sign ? -1 : 1;
            sign2 = !i1.sign ? -1 : 1;
        }
        if(one.length() == two.length() && one == two && i1.sign == !i2.sign)
            return {};
        else if (one.length() < two.length())
            fill_zeros(one, two.length() - one.length());
        else
            fill_zeros(two, one.length() - two.length());

        CBigInt res;
        res.str = "";
        int carry = 0;
        int ciph = 0;
        for (int i = one.length() - 1; i >= 0; --i) {
            if (sign1 == sign2) {
                ciph = (one[i] - '0') + (two[i] - '0') + carry;
                carry = ciph / 10;
                ciph %= 10;
            } else {
                if (one[i] < two[i] + carry) {
                    ciph = (one[i] - '0' + 10) - (two[i] - '0') - carry;
                    carry = 1;
                } else {
                    ciph = (one[i] - '0') - (two[i] - '0') - carry;
                    carry = 0;
                }
            }
            res.str.push_back(ciph + '0');
        }
        if (carry)
            res.str.push_back(carry + '0');
        std::reverse(res.str.begin(), res.str.end());
        pop_zeros(res.str);
        if ((sign1 == -1 && sign2 == -1) || (sign1 == -1 && sign2 == 1))
            res.sign = false;
        return res;
    }

    // operator *, any combination {CBigInt/int/string} * {CBigInt/int/string}
    friend CBigInt operator*(const CBigInt &i1, const CBigInt &i2) {
        std::string one = i1.str, two = i2.str;
        int sign1 = !i1.sign ? -1 : 1;
        int sign2 = !i2.sign ? -1 : 1;
        if (CBigInt(i1.str) < CBigInt(i2.str)) {
            one = i2.str;
            two = i1.str;
            sign1 = !i2.sign ? -1 : 1;
            sign2 = !i1.sign ? -1 : 1;
        }
        if(two == "0")
            return {};
        CBigInt res;
        res.str = "";
        int carry = 0;
        int ciph = 0;
        int out_it = 0;
        for (int i = two.length() - 1; i >= 0; --i, out_it++) {
            std::string u(out_it, '0');
            for (int j = one.length() - 1; j >= 0; --j) {
                ciph = (two[i] - '0') * (one[j] - '0') + carry;
                carry = ciph / 10;
                ciph %= 10;
                u.push_back(ciph + '0');
            }
            if (carry)
                u.push_back(carry + '0');
            carry = 0;
            std::reverse(u.begin(), u.end());
            res += u;

        }
        if ((sign1 == 1 && sign2 == -1) || (sign1 == -1 && sign2 == 1))
            res.sign = false;
        return res;
    }

    // operator +=, any of {CBigInt/int/string}
    CBigInt &operator+=(const CBigInt &i) {
        *this = *this + i;
        return *this;
    }

    // operator *=, any of {CBigInt/int/string}
    CBigInt &operator*=(const CBigInt &i) {
        *this = *this * i;
        return *this;
    }

    // comparison operators, any combination {CBigInt/int/string} {<,<=,>,>=,==,!=} {CBigInt/int/string}
    friend bool operator<(const CBigInt &u, const CBigInt &i) {
        if (u.sign != i.sign)
            return !u.sign;
        if (u.str.length() != i.str.length()) {
            if (!u.sign && !i.sign)
                return u.str.length() > i.str.length();
            else if (u.sign && i.sign) {
                return u.str.length() < i.str.length();
            }
        }
        //same length
        for (size_t j = 0; j < u.str.length(); ++j) {
            if (u.str[j] != i.str[j]) {
                if (!u.sign)
                    return u.str[j] > i.str[j];
                else
                    return u.str[j] < i.str[j];
            }
        }
        return false;
    }

    friend bool operator<=(const CBigInt &u, const CBigInt &i) {
        return !(u > i);
    }

    friend bool operator>(const CBigInt &u, const CBigInt &i) {
        return i < u;
    }

    friend bool operator>=(const CBigInt &u, const CBigInt &i) {
        return !(u < i);
    }

    friend bool operator==(const CBigInt &u, const CBigInt &i) {
        return !(u > i) && !(u < i);
    }

    friend bool operator!=(const CBigInt &u, const CBigInt &i) {
        return !(u == i);
    }

    // output operator <<
    // os bez uvodnich nul
    friend std::ostream &operator<<(std::ostream &os, const CBigInt &i) {
        if (!i.sign)
            os << '-';
        os << i.str;
        return os;
    }

    /**
    * cteme cifry dokud to lze(aspon 1), uvodni whitespaces preskakujeme
    * jinak nastavit failbit - is.setstate(ios::failbit)
    * nascteni znaku ze streamu bez vyjmuti is.peek
    * načítání int se zastaví na prvním znaku, který již nemůže být platnou součástí čteného čísla.
    */
    // input operator >>
    friend std::istream &operator>>(std::istream &is, CBigInt &i) {
        std::stringstream res;
        char c;
        bool resSign = true;
        while(std::isspace(is.peek())){
            is.get();
        }
        if(is.peek() == '-'){
            resSign = false;
            is >> c;
            res << c;
        }
        while(std::isdigit(is.peek())){
            is >> c;
            res << c;
        }
        std::string r(res.str());
        if((r.empty()) || r == "-"){
            is.setstate(std::ios::failbit);
            return is;
        }
        i = CBigInt(r);
        i.sign = resSign;
        return is;
    }

private:
    std::string str;
    bool sign;
};

#ifndef __PROGTEST__

static bool equal(const CBigInt &x, const char val[]) {
    std::ostringstream oss;
    oss << x;
    return oss.str() == val;
}

static bool equalHex(const CBigInt &x, const char val[]) {
    return true; // hex output is needed for bonus tests only
    // std::ostringstream oss;
    // oss << std::hex << x;
    // return oss . str () == val;
}

int main ()
{
    CBigInt ddd("-0");
    CBigInt aaaa(-0);
    CBigInt bbbbb(std::string ("-0"));
    CBigInt a, b;
    std::istringstream is;
    a = 10;
    a += 20;
    assert ( equal ( a, "30" ) );
    a *= 5;
    assert ( equal ( a, "150" ) );
    b = a + 3;
    assert ( equal ( b, "153" ) );
    b = a * 7;
    assert ( equal ( b, "1050" ) );
    assert ( equal ( a, "150" ) );
    assert ( equalHex ( a, "96" ) );

    a = 10;
    a += -20;
    assert ( equal ( a, "-10" ) );
    a *= 5;
    assert ( equal ( a, "-50" ) );
    b = a + 73;
    assert ( equal ( b, "23" ) );
    b = a * -7;
    assert ( equal ( b, "350" ) );
    assert ( equal ( a, "-50" ) );
    assert ( equalHex ( a, "-32" ) );

    a = "12345678901234567890";
    CBigInt aaaaaaaaaaaa = CBigInt("-999999999999") + "-11111111111111";
    assert ( equal ( aaaaaaaaaaaa, "-12111111111110" ) );
    a += "-99999999999999999999";
    assert ( equal ( a, "-87654321098765432109" ) );
    a *= "54321987654321987654";
    assert ( equal ( a, "-4761556948575111126880627366067073182286" ) );
    a *= 0;
    assert ( equal ( a, "0" ) );
    a = 10;
    b = a + "400";
    assert ( equal ( b, "410" ) );
    b = a * "15";
    assert ( equal ( b, "150" ) );
    assert ( equal ( a, "10" ) );
    assert ( equalHex ( a, "a" ) );

    is . clear ();
    is . str ( " 1234" );
    assert ( is >> b );
    assert ( equal ( b, "1234" ) );
    is . clear ();
    is . str ( " 12 34" );
    assert ( is >> b );
    assert ( equal ( b, "12" ) );
    is . clear ();
    is . str ( "999z" );
    assert ( is >> b );
    assert ( equal ( b, "999" ) );
    is . clear ();
    is . str ( "abcd" );
    assert ( ! ( is >> b ) );
    is . clear ();
    is . str ( "- 758" );
    assert ( ! ( is >> b ) );
    a = 42;
    try
    {
        a = "-xyz";
        assert ( "missing an exception" == nullptr );
    }
    catch ( const std::invalid_argument & e )
    {
        assert ( equal ( a, "42" ) );
    }

    a = "73786976294838206464";
    assert ( equal ( a, "73786976294838206464" ) );
    assert ( equalHex ( a, "40000000000000000" ) );
    assert ( a < "1361129467683753853853498429727072845824" );
    assert ( a <= "1361129467683753853853498429727072845824" );
    assert ( ! ( a > "1361129467683753853853498429727072845824" ) );
    assert ( ! ( a >= "1361129467683753853853498429727072845824" ) );
    assert ( ! ( a == "1361129467683753853853498429727072845824" ) );
    assert ( a != "1361129467683753853853498429727072845824" );
    assert ( ! ( a < "73786976294838206464" ) );
    assert ( a <= "73786976294838206464" );
    assert ( ! ( a > "73786976294838206464" ) );
    assert ( a >= "73786976294838206464" );
    assert ( a == "73786976294838206464" );
    assert ( ! ( a != "73786976294838206464" ) );
    assert ( a < "73786976294838206465" );
    assert ( a <= "73786976294838206465" );
    assert ( ! ( a > "73786976294838206465" ) );
    assert ( ! ( a >= "73786976294838206465" ) );
    assert ( ! ( a == "73786976294838206465" ) );
    assert ( a != "73786976294838206465" );
    a = "2147483648";
    assert ( ! ( a < -2147483648 ) );
    assert ( ! ( a <= -2147483648 ) );
    assert ( a > -2147483648 );
    assert ( a >= -2147483648 );
    assert ( ! ( a == -2147483648 ) );
    assert ( a != -2147483648 );
    a = "-12345678";
    assert ( ! ( a < -87654321 ) );
    assert ( ! ( a <= -87654321 ) );
    assert ( a > -87654321 );
    assert ( a >= -87654321 );
    assert ( ! ( a == -87654321 ) );
    assert ( a != -87654321 );

    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
