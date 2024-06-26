#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <deque>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <iterator>
#include <functional>
using namespace std;

class CDate
{
public:
  //---------------------------------------------------------------------------------------------
  CDate(int y,
        int m,
        int d)
      : m_Year(y),
        m_Month(m),
        m_Day(d)
  {
  }
  //---------------------------------------------------------------------------------------------
  int compare(const CDate &x) const
  {
    if (m_Year != x.m_Year)
      return m_Year - x.m_Year;
    if (m_Month != x.m_Month)
      return m_Month - x.m_Month;
    return m_Day - x.m_Day;
  }
  //---------------------------------------------------------------------------------------------
  int year(void) const
  {
    return m_Year;
  }
  //---------------------------------------------------------------------------------------------
  int month(void) const
  {
    return m_Month;
  }
  //---------------------------------------------------------------------------------------------
  int day(void) const
  {
    return m_Day;
  }
  //---------------------------------------------------------------------------------------------
  friend ostream &operator<<(ostream &os,
                             const CDate &x)
  {
    char oldFill = os.fill();
    return os << setfill('0') << setw(4) << x.m_Year << "-"
              << setw(2) << static_cast<int>(x.m_Month) << "-"
              << setw(2) << static_cast<int>(x.m_Day)
              << setfill(oldFill);
  }
  //---------------------------------------------------------------------------------------------
private:
  int16_t m_Year;
  int8_t m_Month;
  int8_t m_Day;
};
#endif /* __PROGTEST__ */

// delete redundant spaces in name
string reformat_name(const string &name)
{
  istringstream name_stream(name);
  string new_name, tmp;
  bool cnt = 0;
  while ((name_stream >> tmp))
  {
    if (cnt == 0)
    {
      new_name += tmp;
      cnt = 1;
    }
    else
    {
      new_name += ' ' + tmp;
    }
  }
  transform(new_name.begin(), new_name.end(), new_name.begin(), ::toupper);
  return new_name;
}


class CInvoice
{
public:
  CInvoice(const CDate &date,
           const string &seller,
           const string &buyer,
           unsigned int amount,
           double vat)
      : m_date(date), m_seller(seller), m_buyer(buyer), m_amount(amount), m_vat(vat)
  {
  }

  CDate date(void) const
  {
    return m_date;
  }
  string buyer(void) const
  {
    return m_buyer;
  }
  string seller(void) const
  {
    return m_seller;
  }
  unsigned int amount(void) const
  {
    return m_amount;
  }
  double vat(void) const
  {
    return m_vat;
  }

  void set_pos(size_t pos)
  {
    m_pos = pos;
  }

  size_t get_pos() const
  {
    return m_pos;
  }

private:
  CDate m_date;
  string m_seller;
  string m_buyer;
  unsigned int m_amount;
  double m_vat;
  size_t m_pos;

};

struct CNamesCMP
{
  bool operator()(const string &a, const string &b) const
  {
    return strcasecmp(a.c_str(), b.c_str()) < 0;
  }
};
struct CInvoiceCMP
{
  bool operator()(const CInvoice &a, const CInvoice &b) const
  {
    if (a.date().compare(b.date()) != 0)
      return a.date().compare(b.date()) < 0;
    if (strcasecmp(a.seller().c_str(), b.seller().c_str()) != 0)
      return strcasecmp(a.seller().c_str(), b.seller().c_str()) < 0;
    if (strcasecmp(a.buyer().c_str(), b.buyer().c_str()) != 0)
      return strcasecmp(a.buyer().c_str(), b.buyer().c_str()) < 0;
    if (a.amount() != b.amount())
      return a.amount() < b.amount();
    if (a.vat() != b.vat())
      return a.vat() < b.vat();
    return false;
  }
};

class CSortOpt
{
public:
  static const int BY_DATE = 0;
  static const int BY_BUYER = 1;
  static const int BY_SELLER = 2;
  static const int BY_AMOUNT = 3;
  static const int BY_VAT = 4;

  CSortOpt() {}

  CSortOpt &addKey(int key,
                   bool ascending = true)
  {
    lSort.emplace_back(key, ascending);
    return *this;
  }

  vector<pair<int, bool>> lSort;

  bool operator()(const CInvoice &a, const CInvoice &b) const
  {
    for (const auto &x : lSort)
    {
      switch (x.first)
      {
      case BY_AMOUNT:
      {
        if (a.amount() == b.amount())
          break;
        if (x.second == true)
          return a.amount() < b.amount();
        else
          return a.amount() > b.amount();
        break;
      }
      case BY_BUYER:
      {
        if (strcasecmp(a.buyer().c_str(), b.buyer().c_str()) == 0)
          break;
        if (x.second == true)
          return strcasecmp(a.buyer().c_str(), b.buyer().c_str())< 0;
        else
          return strcasecmp(a.buyer().c_str(), b.buyer().c_str()) >0;
        break;
      }
      case BY_DATE:
      {
        if (a.date().compare(b.date()) == 0)
          break;
        if (x.second == true)
          return a.date().compare(b.date()) < 0;
        else
          return a.date().compare(b.date()) > 0;
        break;
      }
      case BY_SELLER:
      {
        if (strcasecmp(a.seller().c_str(), b.seller().c_str()) == 0)
          break;
        if (x.second == true)
          return strcasecmp(a.seller().c_str(), b.seller().c_str()) < 0;
        else
          return strcasecmp(a.seller().c_str(), b.seller().c_str()) >0;
        break;
      }
      case BY_VAT:
      {
        if (a.vat() == b.vat())
          break;
        if (x.second == true)
          return a.vat() < b.vat();
        else
          return a.vat() > b.vat();
        break;
      }
      default:
        break;
      }
    }
    return a.get_pos() < b.get_pos();
  }

private:
};

class CVATRegister
{
public:
  CVATRegister() = default;

  bool registerCompany(const string &name)
  {
    // if name is already in
    if (mCompanyNames.find(reformat_name(name)) != mCompanyNames.end())
      return false;

    mCompanyNames[reformat_name(name)] = name;
    return true;
  }

  bool addIssued(const CInvoice &x)
  {
    if (mCompanyNames.find(reformat_name(x.seller())) == mCompanyNames.end() ||
        mCompanyNames.find(reformat_name(x.buyer())) == mCompanyNames.end())
      return false;

    string seller = mCompanyNames[reformat_name(x.seller())];
    string buyer = mCompanyNames[reformat_name(x.buyer())];
    CInvoice tmp(x.date(), seller, buyer, x.amount(), x.vat());
    tmp.set_pos(place);
    if (seller == buyer ||
        sInvoicesIssued.find(tmp) != sInvoicesIssued.end())
      return false;
    place++;
    sInvoicesIssued.emplace(tmp);
    unmatch_with_set(sInvoicesAccepted, tmp, true);
    return true;
  }

  bool addAccepted(const CInvoice &x)
  {
    if (mCompanyNames.find(reformat_name(x.seller())) == mCompanyNames.end() ||
        mCompanyNames.find(reformat_name(x.buyer())) == mCompanyNames.end())
      return false;
    string seller = mCompanyNames[reformat_name(x.seller())];
    string buyer = mCompanyNames[reformat_name(x.buyer())];
    CInvoice tmp(x.date(), seller, buyer, x.amount(), x.vat());
    tmp.set_pos(place);
    if (seller == buyer ||
        sInvoicesAccepted.find(tmp) != sInvoicesAccepted.end())
      return false;
    place++;
    sInvoicesAccepted.emplace(tmp);
    unmatch_with_set(sInvoicesIssued, tmp,true);
    return true;
  }

  bool delIssued(const CInvoice &x)
  {
    if (mCompanyNames.find(reformat_name(x.seller())) == mCompanyNames.end() ||
        mCompanyNames.find(reformat_name(x.buyer())) == mCompanyNames.end())
      return false;
    string seller = mCompanyNames[reformat_name(x.seller())];
    string buyer = mCompanyNames[reformat_name(x.buyer())];
    CInvoice tmp(x.date(), seller, buyer, x.amount(), x.vat());

    auto iit = sInvoicesIssued.find(tmp);

    if (seller == buyer ||
        iit == sInvoicesIssued.end())
      return false;

    sInvoicesIssued.erase(iit);

    unmatch_with_set(sInvoicesAccepted, tmp);
    return true;
  }

  bool delAccepted(const CInvoice &x)
  {
    if (mCompanyNames.find(reformat_name(x.seller())) == mCompanyNames.end() ||
        mCompanyNames.find(reformat_name(x.buyer())) == mCompanyNames.end())
      return false;

    string seller = mCompanyNames[reformat_name(x.seller())];
    string buyer = mCompanyNames[reformat_name(x.buyer())];
    CInvoice tmp(x.date(), seller, buyer, x.amount(), x.vat());

    auto iit = sInvoicesAccepted.find(tmp);
    if (seller == buyer ||
        iit == sInvoicesAccepted.end())
      return false;

    sInvoicesAccepted.erase(iit);

    unmatch_with_set(sInvoicesIssued, tmp);
    return true;
  }

  list<CInvoice> unmatched(const string &company, const CSortOpt &sortBy) const
  {
    auto itCompany = mCompanyNames.find(reformat_name(company));
    if (itCompany == mCompanyNames.end())
    {
      list<CInvoice> lInvoices;
      return lInvoices;
    }
    auto itUnmatched = mInvoicesUnmatched.find(itCompany->second);
    if (itUnmatched == mInvoicesUnmatched.end())
    {
      list<CInvoice> lInvoices;
      return lInvoices;
    }

    vector<CInvoice> vInvoices(itUnmatched->second.begin(),
                               itUnmatched->second.end());

    sort(vInvoices.begin(), vInvoices.end(), [](const CInvoice &a, const CInvoice &b)
         { return a.get_pos() < b.get_pos(); });

    if (sortBy.lSort.size() == 0)
    {
      list<CInvoice> lInvoices(vInvoices.begin(), vInvoices.end());
      return lInvoices;
    }
    sort(vInvoices.begin(),vInvoices.end(),sortBy);
    list<CInvoice> lInvoices(vInvoices.begin(), vInvoices.end());
    return lInvoices;
  }

private:
  set<CInvoice, CInvoiceCMP> sInvoicesIssued;
  set<CInvoice, CInvoiceCMP> sInvoicesAccepted;
  map<string, set<CInvoice, CInvoiceCMP>> mInvoicesUnmatched;
  map<string, string, CNamesCMP> mCompanyNames; // first formated, second not
  size_t place = 0;

  void unmatch_with_set(const set<CInvoice, CInvoiceCMP> &sInvoice, const CInvoice &tmp, bool add = false)
  {
    if (sInvoice.find(tmp) != sInvoice.end())
    { // is in accepted and in issued
      if (add == true) // adding, found 2 in I&A ,both are now matched
      {
        mInvoicesUnmatched[tmp.seller()].erase(tmp);
        mInvoicesUnmatched[tmp.buyer()].erase(tmp);
      }
      else if (add == false) // removing, found 2 in I&A, 1 is now unmatched
      {
        mInvoicesUnmatched[tmp.seller()].emplace(tmp);
        mInvoicesUnmatched[tmp.buyer()].emplace(tmp);
      }
    }
    else
    { // is in accepted and not in issued or reversed
      if (add == true)  //adding, 1 in I&A, now unmatched
      {
        mInvoicesUnmatched[tmp.seller()].emplace(tmp);
        mInvoicesUnmatched[tmp.buyer()].emplace(tmp);
      }
      else if (add == false) // removing, 1 in I&A, now deleted
      {
        mInvoicesUnmatched[tmp.seller()].erase(tmp);
        mInvoicesUnmatched[tmp.buyer()].erase(tmp);
      }
    }
  }
};

#ifndef __PROGTEST__
bool equalLists(const list<CInvoice> &a, const list<CInvoice> &b)
{
  if (a.size() != b.size())
    return false;

  auto ita = a.begin();
  auto itb = b.begin();
  for (; ita != a.end() && itb != b.end(); ++ita, ++itb)
  {
    if (ita->date().compare(itb->date()) != 0 ||
        ita->amount() != itb->amount() ||
        ita->buyer() != itb->buyer() ||
        ita->seller() != itb->seller() ||
        ita->vat() != itb->vat())
    {
      return false;
    }
  }
  return true;
}

int main(void)
{
  CVATRegister r;
  assert(r.registerCompany("first Company"));
  assert(r.registerCompany("Second     Company"));
  assert(r.registerCompany("ThirdCompany, Ltd."));
  assert(r.registerCompany("Third Company, Ltd."));
  assert(!r.registerCompany("Third Company, Ltd."));
  assert(!r.registerCompany(" Third  Company,  Ltd.  "));

  assert(r.addIssued(CInvoice(CDate(2000, 1, 1), "First Company", "Second Company ", 100, 20)));
  assert(r.addIssued(CInvoice(CDate(2000, 1, 2), "FirSt Company", "Second Company ", 200, 30)));
  assert(r.addIssued(CInvoice(CDate(2000, 1, 1), "First Company", "Second Company ", 100, 30)));
  assert(r.addIssued(CInvoice(CDate(2000, 1, 1), "First Company", "Second Company ", 300, 30)));
  assert(r.addIssued(CInvoice(CDate(2000, 1, 1), "First Company", " Third  Company,  Ltd.   ", 200, 30)));
  assert(r.addIssued(CInvoice(CDate(2000, 1, 1), "Second Company ", "First Company", 300, 30)));
  assert(r.addIssued(CInvoice(CDate(2000, 1, 1), "Third  Company,  Ltd.", "  Second    COMPANY ", 400, 34)));
  assert(!r.addIssued(CInvoice(CDate(2000, 1, 1), "First Company", "Second Company ", 300, 30)));
  assert(!r.addIssued(CInvoice(CDate(2000, 1, 4), "First Company", "First   Company", 200, 30)));
  assert(!r.addIssued(CInvoice(CDate(2000, 1, 4), "Another Company", "First   Company", 200, 30)));
  assert(equalLists(r.unmatched("First Company", CSortOpt()),
                    list<CInvoice>{
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 20.000000),
                        CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Third Company, Ltd.", 200, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300, 30.000000)}));
  assert(equalLists(r.unmatched("second company", CSortOpt().addKey(CSortOpt::BY_DATE, false)),
                    list<CInvoice>{
                        CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 20.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "Third Company, Ltd.", "Second     Company", 400, 34.000000)}));
  assert(equalLists(r.unmatched("last company", CSortOpt().addKey(CSortOpt::BY_VAT, true)),
                    list<CInvoice>{}));
  assert(equalLists(r.unmatched("First Company", CSortOpt().addKey(CSortOpt::BY_SELLER, true).addKey(CSortOpt::BY_BUYER, false).addKey(CSortOpt::BY_DATE, false)),
                    list<CInvoice>{
                        CInvoice(CDate(2000, 1, 1), "first Company", "Third Company, Ltd.", 200, 30.000000),
                        CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 20.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300, 30.000000)}));
  assert(equalLists(r.unmatched("First Company", CSortOpt().addKey(CSortOpt::BY_DATE, true).addKey(CSortOpt::BY_SELLER, true).addKey(CSortOpt::BY_BUYER, true)),
                    list<CInvoice>{
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 20.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Third Company, Ltd.", 200, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300, 30.000000),
                        CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200, 30.000000)}));
  assert(equalLists(r.unmatched("First Company", CSortOpt().addKey(CSortOpt::BY_VAT, true).addKey(CSortOpt::BY_AMOUNT, true).addKey(CSortOpt::BY_DATE, true).addKey(CSortOpt::BY_SELLER, true).addKey(CSortOpt::BY_BUYER, true)),
                    list<CInvoice>{
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 20.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Third Company, Ltd.", 200, 30.000000),
                        CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300, 30.000000)}));

  assert(r.addAccepted(CInvoice(CDate(2000, 1, 2), "First Company", "Second Company ", 200, 30)));
  assert(r.addAccepted(CInvoice(CDate(2000, 1, 1), "First Company", " Third  Company,  Ltd.   ", 200, 30)));
  assert(r.addAccepted(CInvoice(CDate(2000, 1, 1), "Second company ", "First Company", 300, 32)));
  assert(equalLists(r.unmatched("First Company", CSortOpt().addKey(CSortOpt::BY_SELLER, true).addKey(CSortOpt::BY_BUYER, true).addKey(CSortOpt::BY_DATE, true)),
                    list<CInvoice>{
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 20.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300, 32.000000)}));

  assert(!r.delIssued(CInvoice(CDate(2001, 1, 1), "First Company", "Second Company ", 200, 30)));
  assert(!r.delIssued(CInvoice(CDate(2000, 1, 1), "A First Company", "Second Company ", 200, 30)));
  assert(!r.delIssued(CInvoice(CDate(2000, 1, 1), "First Company", "Second Hand", 200, 30)));
  assert(!r.delIssued(CInvoice(CDate(2000, 1, 1), "First Company", "Second Company", 1200, 30)));
  assert(!r.delIssued(CInvoice(CDate(2000, 1, 1), "First Company", "Second Company", 200, 130)));
  assert(r.delIssued(CInvoice(CDate(2000, 1, 2), "First Company", "Second Company", 200, 30)));
  assert(equalLists(r.unmatched("First Company", CSortOpt().addKey(CSortOpt::BY_SELLER, true).addKey(CSortOpt::BY_BUYER, true).addKey(CSortOpt::BY_DATE, true)),
                    list<CInvoice>{
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 20.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300, 30.000000),
                        CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300, 32.000000)}));
  assert(r.delAccepted(CInvoice(CDate(2000, 1, 1), "First Company", " Third  Company,  Ltd.   ", 200, 30)));
  assert(equalLists(r.unmatched("First Company", CSortOpt().addKey(CSortOpt::BY_SELLER, true).addKey(CSortOpt::BY_BUYER, true).addKey(CSortOpt::BY_DATE, true)),
                    list<CInvoice>{
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 20.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300, 30.000000),
                        CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Third Company, Ltd.", 200, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300, 32.000000)}));
  assert(r.delIssued(CInvoice(CDate(2000, 1, 1), "First Company", " Third  Company,  Ltd.   ", 200, 30)));
  assert(equalLists(r.unmatched("First Company", CSortOpt().addKey(CSortOpt::BY_SELLER, true).addKey(CSortOpt::BY_BUYER, true).addKey(CSortOpt::BY_DATE, true)),
                    list<CInvoice>{
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 20.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300, 30.000000),
                        CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300, 30.000000),
                        CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300, 32.000000)}));
  /**/
      return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
