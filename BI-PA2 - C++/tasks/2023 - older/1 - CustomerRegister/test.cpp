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

/**
 * Struct represeting employee
 */
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

  /**
   * Add employee to our DB
   * @param[in] name = name of employee
   * @param[in] surname = surname of employee
   * @param[in] email = email of employee
   * @param[in] salary = salary of employee
   * @return true = valid input,
   * @return false = employee with the same pair of (name,surname) or email already exists
   */
  bool add(const string &name,
           const string &surname,
           const string &email,
           unsigned int salary);

  /**
   * Delete empoyee from our DB
   * @param[in] name = name of employee
   * @param[in] surname = surname of employee
   * @return true = valid input,
   * @return false = employee with the same pair of (name,surname) does not exist
   */
  bool del(const string &name,
           const string &surname);

  /**
   * Delete empoyee from our DB
   * @param[in] email = email of employee
   * @return true = valid input,
   * @return false = employee with the same email does not exist
   */
  bool del(const string &email);

  /**
   * Change employee's name and surname
   * @param[in] email = email of employee
   * @param[in] newName = new name of employee
   * @param[in] newSurname = new surname of employee
   * @return true = valid change
   * @return false = employee with the same email does not exist
   * or pair of (newName, newSurname) is not unique (someone has it already)
   */
  bool changeName(const string &email,
                  const string &newName,
                  const string &newSurname);

  /**
   * Change employee's email
   * @param[in] name = name of employee
   * @param[in] surname = surname of employee
   * @param[in] newEmail = new email of employee
   * @return true = valid change
   * @return false = employee with the same pair of (name,surname) does not exist
   * or newEmail is not unique (someone has it already)
   */
  bool changeEmail(const string &name,
                   const string &surname,
                   const string &newEmail);

  /**
   * Set employee's salary
   * @param[in] name = name of employee
   * @param[in] surname = surname of employee
   * @param[in] salary = salary of employee
   * @return true = salary set
   * @return false = same pair of (namae,surname) does not exist
   */
  bool setSalary(const string &name,
                 const string &surname,
                 unsigned int salary);

  /**
   * Set employee's salary
   * @param[in] email = email of employee
   * @param[in] salary = salary of employee
   * @return true = salary set
   * @return false = same email does not exist
   */
  bool setSalary(const string &email,
                 unsigned int salary);

  /**
   * Get salary amount
   * @param[in] name = name of employee
   * @param[in] surname = surname of employee
   * @return salary = valid input
   * @return 0 = pair of (name, surname) does not exist
   */
  unsigned int getSalary(const string &name,
                         const string &surname) const;

  /**
   * Get salary amount
   * @param[in] email = email of employee
   * @return salary = valid input
   * @return 0 = email does not exist
   */
  unsigned int getSalary(const string &email) const;

  /**
   * Find out how good is employee getting paid in relation to others
   * @param[in] name = name of employee
   * @param[in] surname = surname of employee
   * @param[in,out] rankMin = number of people who are paid less
   * @param[in,out] rankMax = number of people who are paid less and equal
   * @return true = employee found, rank parameters filled
   * @return false = employee not found, rank parameters did not change
   */
  bool getRank(const string &name,
               const string &surname,
               int &rankMin,
               int &rankMax) const;

  /**
   * Find out how good is employee getting paid in relation to others
   * @param[in] email = email of employee
   * @param[in,out] rankMin = number of people who are paid less
   * @param[in,out] rankMax = number of people who are paid less and equal
   * @return true = employee found, rank parameters filled
   * @return false = employee not found, rank parameters did not change
   */
  bool getRank(const string &email,
               int &rankMin,
               int &rankMax) const;

  /**
   * Get first employee based on sorted DB in ascending order by surname
   * or if surnames match then by names
   * @param[out] outName = name of first employee in sorted DB
   * @param[out] outSurname = surname of first employee in sorted DB
   * @return true = employee found, out parameters filled
   * @return false = DB is empty, out parameters not changed
   */
  bool getFirst(string &outName,
                string &outSurname) const;

  /**
   * Get next employee to the (name, surname) employee
   * based on sorted DB in ascending order by surname
   * or if surnames match then by names
   * @param[out] outName = name of next employee in sorted DB
   * @param[out] outSurname = surname of next employee in sorted DB
   * @return true = employee (name,surname)found, employee (name,surname) is not last in DB
   * @return false = employee (name,surname) not found or is last in DB
   */
  bool getNext(const string &name,
               const string &surname,
               string &outName,
               string &outSurname) const;

private:
  // DB = vector of employees
  vector<sEMPLOYEE> vEMPLOYEE;
  vector<sEMPLOYEE> vEMAIL;
};

/**
 * Compare function for lower_bound
 * Sorting DB in ascending order by surnames
 * If surnames are equal sort by names
 * @param[in] a = employee 1
 * @param[in] b = employee 2
 * @return true = b is "bigger" than a
 * @return false = a is "bigger" than b
 */
bool cmp_names(const sEMPLOYEE &a, const sEMPLOYEE &b)
{
  return a.surname == b.surname ? a.name < b.name : a.surname < b.surname;
}

/**
 * Compare function for lower_bound
 * Sorting DB in ascending order by emails
 * @param[in] a = employee 1
 * @param[in] b = employee 2
 * @return true = b is "bigger" than a
 * @return false = a is "bigger" than b
 */
bool cmp_email(const sEMPLOYEE &a, const sEMPLOYEE &b)
{
  return a.email < b.email;
}

CPersonalAgenda::CPersonalAgenda() {}

CPersonalAgenda::~CPersonalAgenda() {}

bool CPersonalAgenda::add(const string &name,
                          const string &surname,
                          const string &email,
                          unsigned int salary)
{
  sEMPLOYEE input(name, surname, email, salary);
  // empty vector
  if (vEMPLOYEE.size() == 0)
  {
    vEMPLOYEE.push_back(input);
    vEMAIL.push_back(input);
    return true;
  }
  auto lb_names = lower_bound(vEMPLOYEE.begin(), vEMPLOYEE.end(), input, cmp_names);
  int idx_name = lb_names - vEMPLOYEE.begin();

  auto lb_mail = lower_bound(vEMAIL.begin(), vEMAIL.end(), input, cmp_email);
  int idx_mail = lb_mail - vEMAIL.begin();
  if (vEMAIL[idx_mail].email == email && lb_mail != vEMAIL.end())
    return false;

  // name check with valid index
  if (lb_names != vEMPLOYEE.end() && vEMPLOYEE[idx_name].name == name && vEMPLOYEE[idx_name].surname == surname)
    return false;

  vEMAIL.insert(lb_mail, input);

  vEMPLOYEE.insert(lb_names, input);
  return true;
}

bool CPersonalAgenda::del(const string &name,
                          const string &surname)
{
  sEMPLOYEE input(name, surname, "");
  auto lb = lower_bound(vEMPLOYEE.begin(), vEMPLOYEE.end(), input, cmp_names);
  int idx = lb - vEMPLOYEE.begin();

  auto lb_mail = lower_bound(vEMAIL.begin(), vEMAIL.end(), vEMPLOYEE[idx], cmp_email);

  if (lb == vEMPLOYEE.end() || lb_mail == vEMAIL.end())
    return false;
  // if exists -> delete
  if (vEMPLOYEE[idx].name == name && vEMPLOYEE[idx].surname == surname && vEMAIL[lb_mail - vEMAIL.begin()].email == vEMPLOYEE[idx].email)
  {
    vEMPLOYEE.erase(lb);
    vEMAIL.erase(lb_mail);
    return true;
  }
  return false;
}

bool CPersonalAgenda::del(const string &email)
{
  sEMPLOYEE input("", "", email);
  auto lb_mail = lower_bound(vEMAIL.begin(), vEMAIL.end(), input, cmp_email);
  int idx_mail = lb_mail - vEMAIL.begin();

  if (lb_mail == vEMAIL.end())
    return false;
  if (vEMAIL[idx_mail].email != email)
    return false;

  auto lb = lower_bound(vEMPLOYEE.begin(), vEMPLOYEE.end(), vEMAIL[idx_mail], cmp_names);

  vEMAIL.erase(lb_mail);
  vEMPLOYEE.erase(lb);
  return true;
}

bool CPersonalAgenda::changeName(const string &email,
                                 const string &newName,
                                 const string &newSurname)
{
  // email check
  sEMPLOYEE in("", "", email);
  auto lb_mail = lower_bound(vEMAIL.begin(), vEMAIL.end(), in, cmp_email);
  int idx_mail = lb_mail - vEMAIL.begin();

  if (lb_mail == vEMAIL.end())
    return false;
  if (vEMAIL[idx_mail].email != email)
    return false;

  auto lb_name = lower_bound(vEMPLOYEE.begin(), vEMPLOYEE.end(), vEMAIL[idx_mail], cmp_names);
  int idx2 = lb_name - vEMPLOYEE.begin();

  sEMPLOYEE input(newName, newSurname, email, vEMPLOYEE[idx2].salary);

  auto lb = lower_bound(vEMPLOYEE.begin(), vEMPLOYEE.end(), input, cmp_names);
  int idx = lb - vEMPLOYEE.begin();

  // name check
  if (lb != vEMPLOYEE.end() && vEMPLOYEE[idx].name == newName && vEMPLOYEE[idx].surname == newSurname)
    return false;

  vEMAIL[idx_mail].name = newName;
  vEMAIL[idx_mail].surname = newSurname;
  vEMPLOYEE.erase(lb_name);
  vEMPLOYEE.insert(lb, input);
  return true;
}

bool CPersonalAgenda::changeEmail(const string &name,
                                  const string &surname,
                                  const string &newEmail)
{
  sEMPLOYEE input(name, surname, newEmail);
  auto lb = lower_bound(vEMPLOYEE.begin(), vEMPLOYEE.end(), input, cmp_names);
  int idx = lb - vEMPLOYEE.begin();

  sEMPLOYEE in(name, surname, vEMPLOYEE[idx].email);
  auto lb_mail = lower_bound(vEMAIL.begin(), vEMAIL.end(), in, cmp_email);

  if (lb != vEMPLOYEE.end() && vEMPLOYEE[idx].name == name && vEMPLOYEE[idx].surname == surname && lb_mail != vEMAIL.end())
  {
    auto res_pos = lower_bound(vEMAIL.begin(), vEMAIL.end(), input, cmp_email);
    int idx3 = res_pos - vEMAIL.begin();

    // email check
    if (vEMAIL[idx3].email == newEmail)
      return false;

    input.salary = vEMPLOYEE[idx].salary;
    vEMAIL.erase(lb_mail);
    vEMPLOYEE[idx].email = newEmail;
    vEMAIL.insert(res_pos, input);
    return true;
  }
  return false;
}

bool CPersonalAgenda::setSalary(const string &name,
                                const string &surname,
                                unsigned int salary)
{
  sEMPLOYEE input(name, surname, "");
  auto lb = lower_bound(vEMPLOYEE.begin(), vEMPLOYEE.end(), input, cmp_names);
  int idx = lb - vEMPLOYEE.begin();

  if (lb != vEMPLOYEE.end() && vEMPLOYEE[idx].name == name && vEMPLOYEE[idx].surname == surname)
  {
    vEMPLOYEE[idx].salary = salary;
    return true;
  }
  return false;
}

bool CPersonalAgenda::setSalary(const string &email,
                                unsigned int salary)
{
  sEMPLOYEE input("", "", email);
  auto lb_mail = lower_bound(vEMAIL.begin(), vEMAIL.end(), input, cmp_email);
  int idx_mail = lb_mail - vEMAIL.begin();

  if (lb_mail == vEMAIL.end())
    return false;
  if (vEMAIL[idx_mail].email != email)
    return false;

  auto lb = lower_bound(vEMPLOYEE.begin(), vEMPLOYEE.end(), vEMAIL[idx_mail], cmp_names);
  int idx = lb - vEMPLOYEE.begin();

  vEMPLOYEE[idx].salary = salary;
  return true;
}

unsigned int CPersonalAgenda::getSalary(const string &name,
                                        const string &surname) const
{
  sEMPLOYEE input(name, surname, "");
  auto lb = lower_bound(vEMPLOYEE.begin(), vEMPLOYEE.end(), input, cmp_names);
  int idx = lb - vEMPLOYEE.begin();

  if (lb != vEMPLOYEE.end() && vEMPLOYEE[idx].name == name && vEMPLOYEE[idx].surname == surname)
  {
    return vEMPLOYEE[idx].salary;
  }
  return 0;
}

unsigned int CPersonalAgenda::getSalary(const string &email) const
{
  // email check
  sEMPLOYEE input("", "", email);
  auto lb_mail = lower_bound(vEMAIL.begin(), vEMAIL.end(), input, cmp_email);
  int idx_mail = lb_mail - vEMAIL.begin();

  if (lb_mail == vEMAIL.end())
    return false;
  if (vEMAIL[idx_mail].email != email)
    return 0;

  auto lb = lower_bound(vEMPLOYEE.begin(), vEMPLOYEE.end(), vEMAIL[idx_mail], cmp_names);
  int idx = lb - vEMPLOYEE.begin();

  return vEMPLOYEE[idx].salary;
}

bool CPersonalAgenda::getRank(const string &name,
                              const string &surname,
                              int &rankMin,
                              int &rankMax) const
{
  sEMPLOYEE input(name, surname, "");
  auto lb = lower_bound(vEMPLOYEE.begin(), vEMPLOYEE.end(), input, cmp_names);

  int idx = lb - vEMPLOYEE.begin();

  if (lb != vEMPLOYEE.end() && vEMPLOYEE[idx].name == name && vEMPLOYEE[idx].surname == surname)
  {
    unsigned int salary_tmp = vEMPLOYEE[idx].salary;
    int cnt_min = 0;
    int cnt_max = 0;

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
  return false;
}

bool CPersonalAgenda::getRank(const string &email,
                              int &rankMin,
                              int &rankMax) const
{
  // email check
  sEMPLOYEE input("", "", email);
  auto lb_mail = lower_bound(vEMAIL.begin(), vEMAIL.end(), input, cmp_email);
  int idx_mail = lb_mail - vEMAIL.begin();

  if (vEMAIL[idx_mail].email != email || lb_mail == vEMAIL.end())
    return false;

  auto lb = lower_bound(vEMPLOYEE.begin(), vEMPLOYEE.end(), vEMAIL[idx_mail], cmp_names);
  int idx = lb - vEMPLOYEE.begin();

  unsigned int salary_tmp = vEMPLOYEE[idx].salary;
  int cnt_min = 0;
  int cnt_max = 0;

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

bool CPersonalAgenda::getFirst(string &outName,
                               string &outSurname) const
{
  if (vEMPLOYEE.empty() == false)
  {
    outName = vEMPLOYEE[0].name;
    outSurname = vEMPLOYEE[0].surname;
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

  if (vEMPLOYEE.empty() == false && idx < (vEMPLOYEE.size() - 1))
  {
    if (vEMPLOYEE[idx].name == name && vEMPLOYEE[idx].surname == surname)
    {
      outName = vEMPLOYEE[idx + 1].name;
      outSurname = vEMPLOYEE[idx + 1].surname;
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

  // CPersonalAgenda b1;
  // assert(b1.add("John", "Smith", "john", 30000));
  // assert(b1.add("John", "Miller", "johnm", 35000));
  // assert(b1.add("Peter", "Smith", "peter", 23000));
  // assert(b1.add("John", "Smith", "john", 230) == false);
  // assert(b1.add("John", "Miller", "johnm", 23330) == false);
  // // assert(b1.del("John", "Miller"));
  // // assert(b1.del("John", "Miller") == false);
  // // assert(b1.del("peter"));
  // // assert(b1.del("peter") == false);
  // assert(b1.getFirst(outName, outSurname) && outName == "John" && outSurname == "Miller");
  // assert(b1.getNext("John", "Miller", outName, outSurname) && outName == "John" && outSurname == "Smith");
  // assert(b1.getNext("John", "Smith", outName, outSurname) && outName == "Peter" && outSurname == "Smith");
  // assert(!b1.getNext("Peter", "Smith", outName, outSurname));
  // assert(b1.setSalary("john", 32000));
  // assert(b1.getSalary("john") == 32000);
  // assert(b1.getSalary("John", "Smith") == 32000);
  // assert(b1.getRank("John", "Smith", lo, hi) && lo == 1 && hi == 1);
  // assert(b1.getRank("john", lo, hi) && lo == 1 && hi == 1);
  // assert(b1.getRank("peter", lo, hi) && lo == 0 && hi == 0);
  // assert(b1.getRank("johnm", lo, hi) && lo == 2 && hi == 2);
  // assert(b1.setSalary("John", "Smith", 35000));
  // assert(b1.getSalary("John", "Smith") == 35000);
  // assert(b1.getSalary("john") == 35000);
  // assert(b1.getRank("John", "Smith", lo, hi) && lo == 1 && hi == 2);
  // assert(b1.getRank("john", lo, hi) && lo == 1 && hi == 2);
  // assert(b1.getRank("peter", lo, hi) && lo == 0 && hi == 0);
  // assert(b1.getRank("johnm", lo, hi) && lo == 1 && hi == 2);
  // assert(b1.changeName("peter", "James", "Bond"));
  // assert(b1.getSalary("peter") == 23000);
  // assert(b1.getSalary("James", "Bond") == 23000);
  // assert(b1.getSalary("Peter", "Smith") == 0);
  // assert(b1.getFirst(outName, outSurname) && outName == "James" && outSurname == "Bond");
  // assert(b1.getNext("James", "Bond", outName, outSurname) && outName == "John" && outSurname == "Miller");
  // assert(b1.getNext("John", "Miller", outName, outSurname) && outName == "John" && outSurname == "Smith");
  // assert(!b1.getNext("John", "Smith", outName, outSurname));
  // assert(b1.changeEmail("James", "Bond", "james"));
  // assert(b1.getSalary("James", "Bond") == 23000);
  // assert(b1.getSalary("james") == 23000);
  // assert(b1.getSalary("peter") == 0);
  // assert(b1.del("james"));
  // assert(b1.getRank("john", lo, hi) && lo == 0 && hi == 1);
  // assert(b1.del("John", "Miller"));
  // assert(b1.getRank("john", lo, hi) && lo == 0 && hi == 0);
  // assert(b1.getFirst(outName, outSurname) && outName == "John" && outSurname == "Smith");
  // assert(!b1.getNext("John", "Smith", outName, outSurname));
  // assert(b1.del("john"));
  // assert(!b1.getFirst(outName, outSurname));
  // assert(b1.add("John", "Smith", "john", 31000));
  // assert(b1.add("john", "Smith", "joHn", 31000));
  // assert(b1.add("John", "smith", "jOhn", 31000));

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
