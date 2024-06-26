#ifndef __PROGTEST__

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <array>
#include <cassert>

using namespace std;
#endif /* __PROGTEST__ */

// (rok, měsíc, den, hodina, minuta, sekunda)
class CTimeStamp {
private:
    int year;
    int month;
    int day;
    int hour;
    int min;
    int sec;
public:
    CTimeStamp(int y, int mon, int d, int h, int min, int s)
            : year(y), month(mon), day(d), hour(h), min(min), sec(s) {
    }

    friend bool within(const CTimeStamp &timeStampThis, const CTimeStamp &timeStamp1, const CTimeStamp &timeStamp2) {
        size_t dhmsThis = timeStampThis.day * 24 * 60 * 60 + timeStampThis.hour * 60 * 60 + timeStampThis.min * 60 +
                          timeStampThis.sec;
        size_t dhms1 = timeStamp1.day * 24 * 60 * 60 + timeStamp1.hour * 60 * 60 + timeStamp1.min * 60 + timeStamp1.sec;
        size_t dhms2 = timeStamp2.day * 24 * 60 * 60 + timeStamp2.hour * 60 * 60 + timeStamp2.min * 60 + timeStamp2.sec;

        if (timeStampThis.year < timeStamp1.year || timeStampThis.year > timeStamp2.year ||
            (timeStampThis.year == timeStamp1.year && timeStampThis.month < timeStamp1.month) ||
            (timeStampThis.year == timeStamp2.year && timeStampThis.month > timeStamp2.month) ||
            (timeStampThis.year == timeStamp1.year && timeStampThis.month == timeStamp1.month && dhmsThis < dhms1) ||
            (timeStampThis.year == timeStamp2.year && timeStampThis.month == timeStamp2.month && dhmsThis > dhms2))
            return false;
        return true;

    }
};

class CContact {
private:
    CTimeStamp stamp;
    int one;
    int two;
public:
    CContact(const CTimeStamp &timeStamp, int one, int two)
            : stamp(timeStamp), one(one), two(two) {}

    int getFirstNum() const {
        return one;
    }

    int getSecNum() const {
        return two;
    }

    int check(int infected, const std::vector<int> &vec) const {
        if (one == two) // dont add
            return 0;
        else if (infected == one) // one
        {
            if (vec.end() == std::find(vec.begin(), vec.end(), two))
                return 1;
        } else if (infected == two) {
            if (vec.end() == std::find(vec.begin(), vec.end(), one))
                return 2;
        }
        return 0;
    }

    bool withinTimeStamp(const CTimeStamp &timeStamp1, const CTimeStamp &timeStamp2) const {
        if (within(stamp, timeStamp1, timeStamp2))
            return true;
        return false;
    }

};

class CEFaceMask {
private:
    std::vector<CContact> contacts;
public:
    CEFaceMask &addContact(const CContact &contact) {
        contacts.emplace_back(contact);
        return *this;
    }

    std::vector<int> listContacts(int number) const {
        std::vector<int> res;
        for (const CContact &contact: contacts) {
            int interres = contact.check(number, res);
            if (interres == 1)
                res.emplace_back(contact.getSecNum());
            else if (interres == 2)
                res.emplace_back(contact.getFirstNum());
        }
        return res;
    }

    std::vector<int> listContacts(int number, const CTimeStamp &timeStamp1, const CTimeStamp &timeStamp2) const {
        std::vector<int> res;
        for (const CContact &contact: contacts) {
            if (!contact.withinTimeStamp(timeStamp1, timeStamp2))
                continue;
            int interees = contact.check(number, res);
            if (interees == 1)
                res.emplace_back(contact.getSecNum());
            else if (interees == 2)
                res.emplace_back(contact.getFirstNum());
        }
        return res;
    }
};


#ifndef __PROGTEST__

int main() {
    CEFaceMask test;

    test.addContact(CContact(CTimeStamp(2021, 1, 10, 12, 40, 10), 123456789, 999888777));
    test.addContact(CContact(CTimeStamp(2021, 1, 12, 12, 40, 10), 123456789, 111222333))
            .addContact(CContact(CTimeStamp(2021, 2, 5, 15, 30, 28), 999888777, 555000222));
    test.addContact(CContact(CTimeStamp(2021, 2, 21, 18, 0, 0), 123456789, 999888777));
    test.addContact(CContact(CTimeStamp(2021, 1, 5, 18, 0, 0), 123456789, 456456456));
    test.addContact(CContact(CTimeStamp(2021, 2, 1, 0, 0, 0), 123456789, 123456789));
    assert (test.listContacts(123456789) == (vector<int>{999888777, 111222333, 456456456}));
    assert (test.listContacts(999888777) == (vector<int>{123456789, 555000222}));
    assert (test.listContacts(191919191) == (vector<int>{}));
    assert (test.listContacts(123456789, CTimeStamp(2021, 1, 5, 18, 0, 0), CTimeStamp(2021, 2, 21, 18, 0, 0)) ==
            (vector<int>{999888777, 111222333, 456456456}));
    assert (test.listContacts(123456789, CTimeStamp(2021, 1, 5, 18, 0, 1), CTimeStamp(2021, 2, 21, 17, 59, 59)) ==
            (vector<int>{999888777, 111222333}));
    assert (test.listContacts(123456789, CTimeStamp(2021, 1, 10, 12, 41, 9), CTimeStamp(2021, 2, 21, 17, 59, 59)) ==
            (vector<int>{111222333}));
    return 0;
}

#endif /* __PROGTEST__ */
