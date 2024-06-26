

#ifndef __PROGTEST__

#include <cassert>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <list>
#include <forward_list>
#include <algorithm>
#include <memory>
#include <stdexcept>

using namespace std;
#endif /* __PROGTEST__ */

template<typename T_>
class CDiagonalMatrix {
public:
    CDiagonalMatrix(int n, int k) : n(n), k(k) {
        if (k < 1 || k > n) throw out_of_range("invalid matrix size");
        // Prostřední diagonála
        for (int i = 0; i < n; i++) {
            m_Diagonals.push_back({});
            m_Diagonals[0].push_back(T_());
        }

        // Diagonály okolo prostředku (Z obou stran v jednom vektoru). Vždycky jich bude (n-i)*2 (Násobíme dvakrát protože bereme diagonálu z obou stran)
        for (int i = 1; i < k; i++) {
            for (int l = 0; l < (n - i) * 2; l++) {
                m_Diagonals[i].push_back(T_());
            }
        }
    }

    T_ &operator()(int row, int col) {
        // Vnější souřadnice je vpodstatě vzdálenost od prostředku
        int outer = abs(row - col);
        int inner;
        if (outer >= k) throw out_of_range("invalid index ( " + to_string(row) + ", " + to_string(col) + " )");

        // Vzhledem k tomu, že máme data uložená tak, že nejdřív máme diagonálu z pravé strany matice a pak z levé, tak když máme souřadnici z pravé strany matice, můžeme vzít rovnou řádek jakožto inner
        // souřadnici. Když jsme na levé straně matice, tak musíme brát souřadnici jakoby z "pravé strany" vektoru, jako bychom od druhé půlky vektoru indexovali zase od nuly a toho docílíme tím, že si
        // vydělíme délku vektoru dvěma a přičteme sloupec)
        if (col > row || col == row) {
            inner = row;
        } else {
            inner = col + (m_Diagonals[outer].size() / 2);
        }

        if (inner > col + (static_cast<int>(m_Diagonals[outer].size()) / 2))
            throw out_of_range("invalid index ( " + to_string(row) + ", " + to_string(col) + " )");

        return m_Diagonals[outer][inner];
    }

    bool exists(int row, int col) const {
        int outer = abs(row - col);
        int inner;
        if (outer >= k) return false;
        if (col > row || col == row) {
            inner = row;
        } else {
            inner = col + (static_cast<int>(m_Diagonals[outer].size()) / 2);
        }

        if (inner > col + (static_cast<int>(m_Diagonals[outer].size()) / 2)) return false;

        return true;
    }

    void reshape(int newK) {
        if (newK > n) throw out_of_range("invalid matrix size");
        // Pokud je nové k menší než staré, zbavíme se přebývajících diagonál. Pokud je větší, alokujeme místo pro nové diagonály.
        if (newK < k) {
            m_Diagonals.erase(m_Diagonals.begin() + newK, m_Diagonals.begin() + k);
        } else if (newK > k) {
            for (int i = k; i < newK; i++) {
                for (int l = 0; l < (n - i) * 2; l++) {
                    m_Diagonals.push_back({});
                    m_Diagonals[i].push_back(T_());
                }
            }
        }
        k = newK;
    }

private:
    int n;
    int k;
    // Vektor vektorů k-ček
    vector<vector<T_>> m_Diagonals;
};

#ifndef __PROGTEST__

class CTestCnt {
public:
    CTestCnt(int val = 0)
            : m_Val(val) {
        c_Inst++;
    }

    CTestCnt(const CTestCnt &src) noexcept
            : m_Val(src.m_Val) {
        c_Inst++;
    }

    CTestCnt(CTestCnt &&src) noexcept
            : m_Val(src.m_Val) {
        c_Inst++;
    }

    ~CTestCnt(void) noexcept {
        c_Inst--;
    }

    CTestCnt &operator=(CTestCnt src) noexcept {
        swap(src.m_Val, m_Val);
        return *this;
    }

    inline static size_t c_Inst = 0;
private:
    int m_Val;
};


int main(void) {
    CDiagonalMatrix<int> m1(5, 4);

    for (int i = 0; i < 5; i++)
        m1(i, i) = 10 + i;
    for (int i = 0; i < 4; i++) {
        m1(i, i + 1) = 20 + i;
        m1(i + 1, i) = 30 + i;
    }
    for (int i = 0; i < 3; i++) {
        m1(i, i + 2) = 40 + i;
        m1(i + 2, i) = 50 + i;
    }
    m1(0, 3) = 60;
    m1(1, 4) = 61;
    m1(3, 0) = 70;
    m1(4, 1) = 71;

    CDiagonalMatrix<int> m2(m1);

    for (int i = 0; i < 5; i++)
        assert (m2(i, i) == 10 + i);
    for (int i = 0; i < 4; i++) {
        assert (m2(i, i + 1) == 20 + i);
        assert (m2(i + 1, i) == 30 + i);
    }
    for (int i = 0; i < 3; i++) {
        assert (m2(i, i + 2) == 40 + i);
        assert (m2(i + 2, i) == 50 + i);
    }
    assert (m2(0, 3) == 60);
    assert (m2(1, 4) == 61);
    assert (m2(3, 0) == 70);
    assert (m2(4, 1) == 71);


    m2(0, 0) = 100;
    assert (m1(0, 0) == 10);
    assert (m2(0, 0) == 100);


    assert (m1.exists(4, 1) && m1(4, 1) == 71);
    m1.reshape(2);
    assert (m1.exists(0, 1) && m1(0, 1) == 20);
    assert (!m1.exists(0, 2));
    try {
        m1(0, 0) = m1(0, 2);
        assert ("Missing an exception" == nullptr);
    }
    catch (const std::out_of_range &e) {
        assert (e.what() == "invalid index ( 0, 2 )"s);
    }


    m1.reshape(4);
    assert (m1.exists(0, 1) && m1(0, 1) == 20);
    assert (m1.exists(0, 2) && m1(0, 2) == 0);

    try {
        m1.reshape(6);
        assert ("Missing an exception" == nullptr);
    }
    catch (const std::out_of_range &e) {
        assert (e.what() == "invalid matrix size"s);
    }

    try {
        CDiagonalMatrix<int> m3(5, 6);
        assert ("Missing an exception" == nullptr);
    }
    catch (const std::out_of_range &e) {
        assert (e.what() == "invalid matrix size"s);
    }


    CDiagonalMatrix<CTestCnt> m10(12, 1);
    assert (CTestCnt::c_Inst == 12);
    m10.reshape(4);
    assert (CTestCnt::c_Inst == 72);
    m10.reshape(2);
    assert (CTestCnt::c_Inst == 34);
    m10.reshape(11);
    assert (CTestCnt::c_Inst == 142);
    m10.reshape(12);
    assert (CTestCnt::c_Inst == 144);
    m10.reshape(8);
    assert (CTestCnt::c_Inst == 124);

    return EXIT_SUCCESS;
}

#endif /* __PROGTEST__ */