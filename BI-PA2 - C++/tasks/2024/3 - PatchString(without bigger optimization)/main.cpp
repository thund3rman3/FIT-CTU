#ifndef __PROGTEST__

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <memory>
#include <stdexcept>

#endif /* __PROGTEST__ */

class CString {
public:
    // Constructor for empty string initialization
    CString() {
        m_string = strdup("");
    }

    CString(const char *&str) {
        if (str == nullptr)
            m_string = strdup("");
        else
            m_string = strdup(str);
    }

    CString(const CString &str) {
        m_string = strdup(str.get_string());
    }

    // destructor
    ~CString() {
        if (m_string != nullptr)
            free(m_string);
    }

    CString &operator=(const CString &str) {
        if (this == &str)
            return *this;

        if (m_string != nullptr)
            free(m_string);

        m_string = strdup(str.get_string());
        return *this;
    }

    // get value of string
    const char *get_string() const {
        return m_string;
    }

    size_t length() const {
        if (m_string == nullptr)
            return 0;
        return strlen(m_string);
    }

    void append(const char *str) {
        if (m_string == nullptr) {
            m_string = strdup(str);
        } else {
            size_t len = length() + strlen(str);
            char *tmp = (char *) malloc((len + 1) * sizeof(*tmp));
            strcpy(tmp, m_string);
            strcat(tmp, str);
            free(m_string);
            m_string = tmp;
        }

    }

    void insert(size_t pos, const char *str) {
        size_t len = length() + strlen(str);
        char *res = (char *) malloc((len + 1) * sizeof(*res));
        strncpy(res, m_string, pos);
        res[pos] = '\0';
        strcat(res, str);
        strcat(res, m_string + pos);
        free(m_string);
        m_string = res;
    }

    void erase(size_t from, size_t len) {
        size_t l = length() - len;
        char *tmp = (char *) malloc((l + 1) * sizeof(*tmp));
        strncpy(tmp, m_string, from);
        tmp[from] = '\0';
        strcat(tmp, m_string + from + len);
        free(m_string);
        m_string = tmp;
    }

    const char *substr(size_t from, size_t len) const {
        if (len < 1)
            return "";
        char *tmp = (char *) malloc((len + 1) * sizeof(*tmp));
        for (size_t i = from, j = 0; i <= from + len; i++, j++) {
            tmp[j] = m_string[i];
        }
        tmp[len] = '\0';
        return tmp;
    }

private:
    char *m_string;
};

class CStringNode {
public:
    std::shared_ptr<CString> m_stringPtr;
    size_t m_ofs;
    size_t m_len;

    CStringNode(const CString &string)
            : m_stringPtr(std::make_shared<CString>(string)), m_ofs(0), m_len(string.length()) {}

    CStringNode()
            : m_stringPtr(nullptr), m_ofs(0), m_len(0) {}

    CStringNode& operator=(const CStringNode& other) {
        if (this == &other)
            return *this;

        m_stringPtr = other.m_stringPtr;
        m_ofs = other.m_ofs;
        m_len = other.m_len;
        return *this;
    }

    ~CStringNode() = default;
};


class CVector {
public:
    CVector() {
        m_cap = 10;
        m_size = 0;
        m_array = new CStringNode[m_cap];
        m_len = 0;
    }

    ~CVector() {
        delete[] m_array;
    }

    CVector(const CVector &v) {
        m_cap = v.m_cap;
        m_size = 0;
        m_array = new CStringNode[v.m_cap];
        m_len = 0;
        for (size_t i = 0; i < v.m_size; ++i) {
            emplace_back(v.m_array[i]);
        }
    }

    CVector &operator=(const CVector &v) {
        if (this == &v)
            return *this;

        delete[] m_array;
        m_cap = v.m_cap;
        m_size =0;
        m_array = new CStringNode[v.m_cap];
        m_len = 0;
        for (size_t i = 0; i < v.m_size; ++i) {
            emplace_back(v.m_array[i]);
        }

        return *this;
    }

    void emplace_back(const CStringNode &elem) {
        if (m_size >= m_cap) {
            m_cap = 2 * m_cap;
            CStringNode *tmp = new CStringNode[m_cap];
            for (size_t x = 0; x < m_size; x++) {
                tmp[x] = m_array[x];
            }
            delete[] m_array;
            m_array = tmp;
        }
        m_array[m_size] = elem;
        m_len += elem.m_len;
        m_size++;
    }

    size_t size() const {
        return m_size;
    }

    size_t len() const {
        return m_len;
    }

    CStringNode *begin() const {
        return m_array;
    }

    CStringNode *end() const {
        return m_array + m_size;
    }

    CStringNode &operator[](size_t idx) const {
        return m_array[idx];
    }

    void freeAndInit() {
        delete[] m_array;
        m_cap = 10;
        m_size = 0;
        m_array = new CStringNode[m_cap];
        m_len = 0;
    }

private:
    size_t m_cap;
    size_t m_size;
    CStringNode *m_array;
    size_t m_len;
};

class CPatchStr {
public:
    CPatchStr() = default;

    CPatchStr(const char *str);

    // copy constructor
    CPatchStr(const CPatchStr &str);

    // destructor
    ~CPatchStr() = default;

    // operator =
    CPatchStr &operator=(const CPatchStr &str);

    CPatchStr subStr(size_t from, size_t len) const;

    CPatchStr &append(const CPatchStr &src);

    CPatchStr &insert(size_t pos, const CPatchStr &src);

    CPatchStr &remove(size_t from, size_t len);

    char *toStr() const;

private:
    //CString m_str;
    CVector m_patch;
};

//CPatchStr::CPatchStr() {
//}

CPatchStr::CPatchStr(const char *str) {
    m_patch.emplace_back(CStringNode(str));
}

CPatchStr::CPatchStr(const CPatchStr &str) {
    m_patch = str.m_patch;
}

CPatchStr &CPatchStr::operator=(const CPatchStr &str) {
    if (this == &str)
        return *this;
    m_patch = str.m_patch;
    return *this;
}


CPatchStr &CPatchStr::append(const CPatchStr &src) {
    CPatchStr cpy(src);
    for (const auto &i: cpy.m_patch) {
        m_patch.emplace_back(i);
    }
    return *this;
}

//Pokud je hodnota pos větší než délka řetězce this, vyhodí metoda výjimku std::out_of_range.
CPatchStr &CPatchStr::insert(size_t pos, const CPatchStr &src) {
    if (pos > m_patch.len())
        throw std::out_of_range("");
    //m_str.insert(pos, src.m_str.get_string());
    CPatchStr first = subStr(0, pos);
    first.append(src);

    CPatchStr second = subStr(pos, m_patch.len() - pos);
    first.append(second);
    m_patch = first.m_patch;
    return *this;
}

//from + len musí být menší nebo rovné počtu znaků v řetězci this.
// Pokud tato podmínka není splněna, vyhodí metoda výjimku std::out_of_range
CPatchStr &CPatchStr::remove(size_t from, size_t len) {
    if (from + len > m_patch.len())
        throw std::out_of_range("");
    //m_str.erase(from, len);
    CPatchStr first = subStr(0, from);
    first.append(subStr(len + from, m_patch.len() - len - from));
    m_patch = first.m_patch;
    return *this;
}

//from + len musí být menší nebo rovné počtu znaků v řetězci this.
// Pokud tato podmínka není splněna, vyhodí metoda výjimku std::out_of_range
CPatchStr CPatchStr::subStr(size_t from, size_t len) const {
    if (from + len > m_patch.len()) {
        throw std::out_of_range("");
    }
    CPatchStr pstr;
    size_t lenRemaining = len;
    size_t currLen = 0;
    bool firstWord = true;
    for (size_t i = 0; i < m_patch.len(); ++i) {
        if (lenRemaining == 0) {
            return pstr;
        }
        if (m_patch[i].m_len + currLen <= from && firstWord) {
            currLen += m_patch[i].m_len;
            continue;
        }
        CStringNode node(m_patch[i]);
        if (firstWord) {
            node.m_ofs += from - currLen;
            firstWord = false;
        }
        if (from+len < currLen + node.m_len) {
            node.m_len = lenRemaining;
        }
        else
            node.m_len = node.m_len - node.m_ofs +m_patch[i].m_ofs;
        lenRemaining -= node.m_len;
        currLen += m_patch[i].m_len;
        pstr.m_patch.emplace_back(node);
    }
    return pstr;
}

char *CPatchStr::toStr() const {
    char *res = new char[m_patch.len() + 1];
    size_t idx = 0;
    for (size_t i = 0; i < m_patch.size(); i++) {
        for (size_t j = m_patch[i].m_ofs; j < m_patch[i].m_len + m_patch[i].m_ofs; ++j) {
            if ((m_patch[i].m_stringPtr->get_string())[j] != '\0') {
                res[idx] = (m_patch[i].m_stringPtr->get_string())[j];
                ++idx;
            }
        }
    }
    res[m_patch.len()] = '\0';
    return res;
}


#ifndef __PROGTEST__

bool stringMatch(char *str,
                 const char *expected) {
    bool res = std::strcmp(str, expected) == 0;
    delete[] str;
    return res;
}

int main() {
    char tmpStr[100];

    CPatchStr a("test");
    assert (stringMatch(a.toStr(), "test"));
    std::strncpy(tmpStr, " da", sizeof(tmpStr) - 1);
    a.append(tmpStr);
    assert (stringMatch(a.toStr(), "test da"));
    std::strncpy(tmpStr, "ta", sizeof(tmpStr) - 1);
    a.append(tmpStr);
    assert (stringMatch(a.toStr(), "test data"));
    std::strncpy(tmpStr, "foo text", sizeof(tmpStr) - 1);
    CPatchStr b(tmpStr);
    assert (stringMatch(b.toStr(), "foo text"));
    CPatchStr c(a);
    assert (stringMatch(c.toStr(), "test data"));
    CPatchStr d(a.subStr(3, 5));
    assert (stringMatch(d.toStr(), "t dat"));
    d.append(b);
    assert (stringMatch(d.toStr(), "t datfoo text"));
    d.append(b.subStr(3, 4));
    assert (stringMatch(d.toStr(), "t datfoo text tex"));
    c.append(d);
    assert (stringMatch(c.toStr(), "test datat datfoo text tex"));
    c.append(c);
    assert (stringMatch(c.toStr(), "test datat datfoo text textest datat datfoo text tex"));
    d.insert(2, c.subStr(6, 9));
    assert (stringMatch(d.toStr(), "t atat datfdatfoo text tex"));
    b = "abcdefgh";
    assert (stringMatch(b.toStr(), "abcdefgh"));
    assert (stringMatch(d.toStr(), "t atat datfdatfoo text tex"));
    assert (stringMatch(d.subStr(4, 8).toStr(), "at datfd"));
    assert (stringMatch(b.subStr(2, 6).toStr(), "cdefgh"));
    try {
        b.subStr(2, 7).toStr();
        assert ("Exception not thrown" == nullptr);
    }
    catch (const std::out_of_range &e) {
    }
    catch (...) {
        assert ("Invalid exception thrown" == nullptr);
    }
    a.remove(3, 5);
    assert (stringMatch(a.toStr(), "tesa"));
    return EXIT_SUCCESS;
}

#endif /* __PROGTEST__ */
