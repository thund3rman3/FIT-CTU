#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <memory>
using namespace std;
#endif /* __PROGTEST__ */

struct sEMPLOYEE
{
  string name;
  string surname;
  string email;
  unsigned int salary;
  sEMPLOYEE(const string &name, const string &surname, const string &email, unsigned int salary = 0)
      : name(name), surname(surname), email(email), salary(salary) {}
};

class CPersonalAgenda
{
public:
  CPersonalAgenda(void);
  ~CPersonalAgenda(void);
  bool add(const string &name,
           const string &surname,
           const string &email,
           unsigned int salary);
  bool del(const string &name,
           const string &surname);
  bool del(const string &email);
  bool changeName(const string &email,
                  const string &newName,
                  const string &newSurname);
  bool changeEmail(const string &name,
                   const string &surname,
                   const string &newEmail);
  bool setSalary(const string &name,
                 const string &surname,
                 unsigned int salary);
  bool setSalary(const string &email,
                 unsigned int salary);
  unsigned int getSalary(const string &name,
                         const string &surname) const;
  unsigned int getSalary(const string &email) const;
  bool getRank(const string &name,
               const string &surname,
               int &rankMin,
               int &rankMax) const;
  /*
  zjistí jak dobře je zaměstnanec placen ve vztahu k ostatním.
   Výsledkem je pozice výplaty zadaného zaměstnance na pomyslném žebříčku výplat
    od nejhorší (nejnižší) k nejlepší (nejvyšší).
    Parametrem je identifikace zaměstnance
    (podle varianty buď jménem a příjmením, nebo e-mailovou adresou),
    parametry rankMin/rankMax jsou výstupní, do nich funkce zapíše pozici výplaty
    hledaného zaměstnance v žebříčku. Protože stejnou výplatu může dostávat
    více zaměstnanců, je výstupem dvojice hodnot - interval min-max.
    Pokud například hledaný zaměstnanec dostává výplatu 20000, 37 zaměstnanců
    dostává výplatu nižší a 21 dalších zaměstnanců dostává výplatu stejnou
    (tedy plat 20000 dostává celkem 22 zaměstnanců), pak výsledkem je rankMin=37
    a rankMax=37+22-1=58. Návratovou hodnotou funkce je true pro úspěch
    (zadaný zaměstnanec nalezen, výstupní parametry vyplněny) nebo
  false (zadaný zaměstnanec nenalezen, výstupní parametry ponechány beze změn).*/
  bool getRank(const string &email,
               int &rankMin,
               int &rankMax) const;
  bool getFirst(string &outName,
                string &outSurname) const;
  bool getNext(const string &name,
               const string &surname,
               string &outName,
               string &outSurname) const;

private:
  int find_email_index(const string &email);
  vector<sEMPLOYEE> vEMPLOYEE;
};

CPersonalAgenda::CPersonalAgenda() {}

CPersonalAgenda::~CPersonalAgenda() {}

int cmp_names(const sEMPLOYEE &a, const sEMPLOYEE &b)
{
  return a.surname == b.surname ? a.name < b.name : a.surname < b.surname;
}

// int cmp_email(const sEMPLOYEE &a, const sEMPLOYEE &b)
// {
//   return a.email < b.email;
// }

int find_email_index(const vector<sEMPLOYEE> &vEMPLOYEE, const string &email)
{
  for (int i = 0; i < vEMPLOYEE.size(); i++)
  {
    if (vEMPLOYEE[i].email == email)
    {
      return i;
    }
  }
  return -1;
}

bool CPersonalAgenda::add(const string &name,
                          const string &surname,
                          const string &email,
                          unsigned int salary)
{
  sEMPLOYEE input(name, surname, email, salary);

  if (vEMPLOYEE.size() == 0)
  {
    vEMPLOYEE.push_back(input);
    cout << "0 " << input.name << " " << input.surname << " " << input.email << endl;
    return true;
  }

  auto lb = lower_bound(vEMPLOYEE.begin(), vEMPLOYEE.end(), input, cmp_names);

  size_t idx = lb - vEMPLOYEE.begin();

  if (idx < vEMPLOYEE.size())
  {
    if ((vEMPLOYEE.at(idx).name == name && vEMPLOYEE.at(idx).surname == surname) ||
        vEMPLOYEE.at(idx).email == email)
    {
      return false;
    }
  }
  cout << idx << " " << vEMPLOYEE.size() << " " << input.name << " "
       << input.surname << " " << input.email << " ";

  vEMPLOYEE.insert(lb, input);

  cout << vEMPLOYEE.at(idx).email << endl;

  return true;
}

bool CPersonalAgenda::del(const string &name,
                          const string &surname)
{
  string s = "";
  sEMPLOYEE input(name, surname, s);
  auto lb = lower_bound(vEMPLOYEE.begin(), vEMPLOYEE.end(), input, cmp_names);

  size_t idx = lb - vEMPLOYEE.begin();

  if (idx < vEMPLOYEE.size())
  {
    if (vEMPLOYEE.at(idx).name == name && vEMPLOYEE.at(idx).surname == surname)
    {
      vEMPLOYEE.erase(lb);
      return true;
    }
  }
  return false;
}

bool CPersonalAgenda::del(const string &email)
{
  // sEMPLOYEE input("", "", email);
  // auto lb = lower_bound(vEMPLOYEE.begin(), vEMPLOYEE.end(), input, cmp_email);

  // size_t idx = lb - vEMPLOYEE.begin();
  size_t idx = find_email_index(email);
  if (idx < vEMPLOYEE.size())
  {
    if (vEMPLOYEE.at(idx).email == email)
    {
      vEMPLOYEE.erase(idx + vEMPLOYEE.begin());
      return true;
    }
  }
  return false;
}

bool CPersonalAgenda::changeName(const string &email,
                                 const string &newName,
                                 const string &newSurname)
{
  sEMPLOYEE input("", "", email);
  auto lb = lower_bound(vEMPLOYEE.begin(), vEMPLOYEE.end(), input, cmp_email);

  size_t idx = lb - vEMPLOYEE.begin();

  if (idx < vEMPLOYEE.size())
  {
    if (vEMPLOYEE.at(idx).email == email)
    {
      sEMPLOYEE input(newName, newSurname, email);
      auto lb2 = lower_bound(vEMPLOYEE.begin(), vEMPLOYEE.end(), input, cmp_names);
      size_t idx2 = lb2 - vEMPLOYEE.begin();

      if (idx2 < vEMPLOYEE.size())
      {
        if (vEMPLOYEE.at(idx2).name == newName && vEMPLOYEE.at(idx2).surname == newSurname)
        {
          return false;
        }
      }
      vEMPLOYEE.at(idx).name = newName;
      vEMPLOYEE.at(idx).surname = newSurname;
      return true;
    }
  }
  return false;
}

bool CPersonalAgenda::changeEmail(const string &name,
                                  const string &surname,
                                  const string &newEmail)
{
  sEMPLOYEE input(name, surname, newEmail);
  auto lb = lower_bound(vEMPLOYEE.begin(), vEMPLOYEE.end(), input, cmp_names);

  size_t idx = lb - vEMPLOYEE.begin();

  if (idx < vEMPLOYEE.size())
  {
    if (vEMPLOYEE.at(idx).name == name && vEMPLOYEE.at(idx).surname == surname)
    {
      auto lb2 = lower_bound(vEMPLOYEE.begin(), vEMPLOYEE.end(), input, cmp_email);
      size_t idx2 = lb2 - vEMPLOYEE.begin();

      if (idx2 < vEMPLOYEE.size())
      {
        if (vEMPLOYEE.at(idx2).email == newEmail)
        {
          return false;
        }
      }
      vEMPLOYEE.at(idx).email = newEmail;
      return true;
    }
  }
  return false;
}

bool CPersonalAgenda::setSalary(const string &name,
                                const string &surname,
                                unsigned int salary)
{
  sEMPLOYEE input(name, surname, "");
  auto lb = lower_bound(vEMPLOYEE.begin(), vEMPLOYEE.end(), input, cmp_names);

  size_t idx = lb - vEMPLOYEE.begin();

  if (idx < vEMPLOYEE.size())
  {
    if (vEMPLOYEE.at(idx).name == name && vEMPLOYEE.at(idx).surname == surname)
    {
      vEMPLOYEE.at(idx).salary = salary;
      return true;
    }
  }
  return false;
}

bool CPersonalAgenda::setSalary(const string &email,
                                unsigned int salary)
{
  sEMPLOYEE input("", "", email);
  auto lb = lower_bound(vEMPLOYEE.begin(), vEMPLOYEE.end(), input, cmp_email);

  size_t idx = lb - vEMPLOYEE.begin();
  cout << "idx: " << idx << " " << vEMPLOYEE.at(idx).email << endl;
  if (idx < vEMPLOYEE.size())
  {
    if (vEMPLOYEE.at(idx).email == email)
    {
      cout << vEMPLOYEE.at(idx).email << " " << vEMPLOYEE.at(idx).name << " " << vEMPLOYEE.at(idx).surname << endl;
      vEMPLOYEE.at(idx).salary = salary;
      return true;
    }
  }
  return false;
}

unsigned int CPersonalAgenda::getSalary(const string &name,
                                        const string &surname) const
{
  sEMPLOYEE input(name, surname, "");
  auto lb = lower_bound(vEMPLOYEE.begin(), vEMPLOYEE.end(), input, cmp_names);

  size_t idx = lb - vEMPLOYEE.begin();

  if (idx < vEMPLOYEE.size())
  {
    if (vEMPLOYEE.at(idx).name == name && vEMPLOYEE.at(idx).surname == surname)
    {
      return vEMPLOYEE.at(idx).salary;
    }
  }
  return 0;
}

unsigned int CPersonalAgenda::getSalary(const string &email) const
{
  sEMPLOYEE input("", "", email);
  auto lb = lower_bound(vEMPLOYEE.begin(), vEMPLOYEE.end(), input, cmp_email);

  size_t idx = lb - vEMPLOYEE.begin();

  if (idx < vEMPLOYEE.size())
  {
    if (vEMPLOYEE.at(idx).email == email)
    {
      return vEMPLOYEE.at(idx).salary;
    }
  }
  return 0;
}

bool CPersonalAgenda::getRank(const string &name,
                              const string &surname,
                              int &rankMin,
                              int &rankMax) const
{
  sEMPLOYEE input(name, surname, "");
  auto lb = lower_bound(vEMPLOYEE.begin(), vEMPLOYEE.end(), input, cmp_names);

  size_t idx = lb - vEMPLOYEE.begin();

  if (idx < vEMPLOYEE.size())
  {
    if (vEMPLOYEE.at(idx).name == name && vEMPLOYEE.at(idx).surname == surname)
    {
      unsigned int salary_tmp = vEMPLOYEE.at(idx).salary;
      size_t cnt_min = 0;
      size_t cnt_max = 0;
      for (sEMPLOYEE x : vEMPLOYEE)
      {
        if (x.salary < salary_tmp)
          cnt_min++;
        else if (x.salary == salary_tmp)
          cnt_max++;
      }
      rankMin = cnt_min;
      rankMax = cnt_max - 1 + cnt_min;
      return true;
    }
  }
  return false;
}

bool CPersonalAgenda::getRank(const string &email,
                              int &rankMin,
                              int &rankMax) const
{
  sEMPLOYEE input("", "", email);
  auto lb = lower_bound(vEMPLOYEE.begin(), vEMPLOYEE.end(), input, cmp_email);

  size_t idx = lb - vEMPLOYEE.begin();

  if (idx < vEMPLOYEE.size())
  {
    if (vEMPLOYEE.at(idx).email == email)
    {
      unsigned int salary_tmp = vEMPLOYEE.at(idx).salary;
      size_t cnt_min = 0;
      size_t cnt_max = 0;
      for (sEMPLOYEE x : vEMPLOYEE)
      {
        if (x.salary < salary_tmp)
          cnt_min++;
        else if (x.salary == salary_tmp)
          cnt_max++;
      }
      rankMin = cnt_min;
      rankMax = cnt_max - 1 + cnt_min;
      return true;
    }
  }
  return false;
}

bool CPersonalAgenda::getFirst(string &outName,
                               string &outSurname) const
{
  if (vEMPLOYEE.empty() == false &&
      (outName != vEMPLOYEE.at(0).name || outSurname != vEMPLOYEE.at(0).surname))
  {
    outName = vEMPLOYEE.at(0).name;
    outSurname = vEMPLOYEE.at(0).surname;
    return true;
  }
  return false;
}

bool CPersonalAgenda::getNext(const string &name,
                              const string &surname,
                              string &outName,
                              string &outSurname) const
{
  sEMPLOYEE input(name, surname, "");
  auto lb = lower_bound(vEMPLOYEE.begin(), vEMPLOYEE.end(), input, cmp_names);

  size_t idx = lb - vEMPLOYEE.begin();

  if (vEMPLOYEE.empty() == false && idx < vEMPLOYEE.size() - 1)
  {
    if (vEMPLOYEE.at(idx).name == name || vEMPLOYEE.at(idx).surname == surname)
    {
      outName = vEMPLOYEE.at(idx + 1).name;
      outSurname = vEMPLOYEE.at(idx + 1).surname;
      return true;
    }
  }
  return false;
}

#ifndef __PROGTEST__
int main(void)
{
  string outName, outSurname;
  int lo, hi;

  CPersonalAgenda b1;
  assert(b1.add("John", "Smith", "john", 30000));
  assert(b1.add("John", "Miller", "johnm", 35000));
  assert(b1.add("Peter", "Smith", "peter", 23000));
  assert(b1.add("John", "Smith", "john", 230) == false);
  assert(b1.add("John", "Miller", "johnm", 23330) == false);
  // assert(b1.del("John", "Miller"));
  // assert(b1.del("John", "Miller") == false);
  // assert(b1.del("peter"));
  // assert(b1.del("peter") == false);
  assert(b1.getFirst(outName, outSurname) && outName == "John" && outSurname == "Miller");
  assert(b1.getNext("John", "Miller", outName, outSurname) && outName == "John" && outSurname == "Smith");
  assert(b1.getNext("John", "Smith", outName, outSurname) && outName == "Peter" && outSurname == "Smith");
  assert(!b1.getNext("Peter", "Smith", outName, outSurname));
  assert(b1.setSalary("john", 32000));
  assert(b1.getSalary("john") == 32000);
  assert(b1.getSalary("John", "Smith") == 32000);
  assert(b1.getRank("John", "Smith", lo, hi) && lo == 1 && hi == 1);
  assert(b1.getRank("john", lo, hi) && lo == 1 && hi == 1);
  assert(b1.getRank("peter", lo, hi) && lo == 0 && hi == 0);
  assert(b1.getRank("johnm", lo, hi) && lo == 2 && hi == 2);
  assert(b1.setSalary("John", "Smith", 35000));
  assert(b1.getSalary("John", "Smith") == 35000);
  assert(b1.getSalary("john") == 35000);
  assert(b1.getRank("John", "Smith", lo, hi) && lo == 1 && hi == 2);
  assert(b1.getRank("john", lo, hi) && lo == 1 && hi == 2);
  assert(b1.getRank("peter", lo, hi) && lo == 0 && hi == 0);
  assert(b1.getRank("johnm", lo, hi) && lo == 1 && hi == 2);
  assert(b1.changeName("peter", "James", "Bond"));
  assert(b1.getSalary("peter") == 23000);
  assert(b1.getSalary("James", "Bond") == 23000);
  assert(b1.getSalary("Peter", "Smith") == 0);
  assert(b1.getFirst(outName, outSurname) && outName == "James" && outSurname == "Bond");
  assert(b1.getNext("James", "Bond", outName, outSurname) && outName == "John" && outSurname == "Miller");
  assert(b1.getNext("John", "Miller", outName, outSurname) && outName == "John" && outSurname == "Smith");
  assert(!b1.getNext("John", "Smith", outName, outSurname));
  assert(b1.changeEmail("James", "Bond", "james"));
  assert(b1.getSalary("James", "Bond") == 23000);
  assert(b1.getSalary("james") == 23000);
  assert(b1.getSalary("peter") == 0);
  assert(b1.del("james"));
  assert(b1.getRank("john", lo, hi) && lo == 0 && hi == 1);
  assert(b1.del("John", "Miller"));
  assert(b1.getRank("john", lo, hi) && lo == 0 && hi == 0);
  assert(b1.getFirst(outName, outSurname) && outName == "John" && outSurname == "Smith");
  assert(!b1.getNext("John", "Smith", outName, outSurname));
  assert(b1.del("john"));
  assert(!b1.getFirst(outName, outSurname));
  assert(b1.add("John", "Smith", "john", 31000));
  assert(b1.add("john", "Smith", "joHn", 31000));
  assert(b1.add("John", "smith", "jOhn", 31000));

  CPersonalAgenda b2;
  assert(!b2.getFirst(outName, outSurname));
  assert(b2.add("James", "Bond", "james", 70000));
  assert(b2.add("James", "Smith", "james2", 30000));
  assert(b2.add("Peter", "Smith", "peter", 40000));
  assert(!b2.add("James", "Bond", "james3", 60000));
  assert(!b2.add("Peter", "Bond", "peter", 50000));
  assert(!b2.changeName("joe", "Joe", "Black"));
  assert(!b2.changeEmail("Joe", "Black", "joe"));
  assert(!b2.setSalary("Joe", "Black", 90000));
  assert(!b2.setSalary("joe", 90000));
  assert(b2.getSalary("Joe", "Black") == 0);
  assert(b2.getSalary("joe") == 0);
  assert(!b2.getRank("Joe", "Black", lo, hi));
  assert(!b2.getRank("joe", lo, hi));
  assert(!b2.changeName("joe", "Joe", "Black"));
  assert(!b2.changeEmail("Joe", "Black", "joe"));
  assert(!b2.del("Joe", "Black"));
  assert(!b2.del("joe"));
  assert(!b2.changeName("james2", "James", "Bond"));
  assert(!b2.changeEmail("Peter", "Smith", "james"));
  assert(!b2.changeName("peter", "Peter", "Smith"));
  assert(!b2.changeEmail("Peter", "Smith", "peter"));
  assert(b2.del("Peter", "Smith"));
  assert(!b2.changeEmail("Peter", "Smith", "peter2"));
  assert(!b2.setSalary("Peter", "Smith", 35000));
  assert(b2.getSalary("Peter", "Smith") == 0);
  assert(!b2.getRank("Peter", "Smith", lo, hi));
  assert(!b2.changeName("peter", "Peter", "Falcon"));
  assert(!b2.setSalary("peter", 37000));
  assert(b2.getSalary("peter") == 0);
  assert(!b2.getRank("peter", lo, hi));
  assert(!b2.del("Peter", "Smith"));
  assert(!b2.del("peter"));
  assert(b2.add("Peter", "Smith", "peter", 40000));
  assert(b2.getSalary("peter") == 40000);

  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
