#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
using namespace std;
#endif /* __PROGTEST__ */

// Class represents basic string
class CString
{
public:
  // Constructor for empty string initialization
  CString()
      : m_string(nullptr) {}

  /** Constructor
   *  @param str = value to save in string
   */
  CString(const char *&str)
  {
    m_string = strdup(str);
  }

  /** Copy constructor
   *  @param str = value to save in string
   */
  CString(const CString &str)
  {
    m_string = strdup(str.get_string());
  }

  // destructor
  ~CString()
  {
    free(m_string);
  }

  // get value of string
  const char *get_string() const
  {
    return m_string;
  }

  /** Operator=
   *  @param str = string to copy
   *  @return this
   */
  CString &operator=(const CString &str)
  {
    if (m_string != nullptr)
      free(m_string);

    m_string = strdup(str.get_string());
    return *this;
  }

private:
  char *m_string;
};

// Class represents email(sender, recipient, message)
class CMail
{
public:
  CMail() = default;

  /** Constructor
   *  @param from = name of sender
   *  @param to = name of recipient
   *  @param body = message
   */
  CMail(const char *from,
        const char *to,
        const char *body)
      : m_from(from), m_to(to), m_body(body) {}

  ~CMail() = default;

  /** Operator=
   *  @param m = CMail intance
   *  @return this
   */
  CMail &operator=(const CMail &m)
  {
    m_from = m.m_from;
    m_to = m.m_to;
    m_body = m.m_body;
    return *this;
  }

  /** Operator==
   *  @param m = CMail instance
   *  @return true if CMail instances are the same
   *  @return false if they are different
   */
  bool operator==(const CMail &m) const
  {
    return strcmp(m_body.get_string(), m.m_body.get_string()) == 0 &&
           strcmp(m_from.get_string(), m.m_from.get_string()) == 0 &&
           strcmp(m_to.get_string(), m.m_to.get_string()) == 0;
  }

  /** Operator<<
   *  @param os = output stream
   *  @param m = CMail instance
   *  @return os
   */
  friend ostream &operator<<(ostream &os,
                             const CMail &m)
  {
    // format //"From: john, To: alice, Body: deadline notice"
    os << "From: " << m.m_from.get_string() << ", To: " << m.m_to.get_string() << ", Body: " << m.m_body.get_string();
    return os;
  }

  const char *get_from() const
  {
    return m_from.get_string();
  }

  const char *get_to() const
  {
    return m_to.get_string();
  }

private:
  CString m_from;
  CString m_to;
  CString m_body;
};

// Class represents basic vector
class CVector
{
public:
  // Constructor initialiazing array
  CVector()
  {
    m_cap = 10;
    m_size = 0;
    m_array = new CMail[m_cap];
  }

  // Destructor
  ~CVector()
  {
    delete[] m_array;
  }

  /** Place CMail instance at the end
   *  @param mail = CMail instance
   */
  void emplace_back(const CMail &mail)
  {
    if (m_size >= m_cap)
    {
      m_cap = 2 * m_cap;
      CMail *tmp = new CMail[m_cap];
      for (size_t x = 0; x < m_size; x++)
      {
        tmp[x] = m_array[x];
      }
      delete[] m_array;
      m_array = tmp;
    }
    m_array[m_size] = mail;
    m_size++;
  }

  // Size getter
  size_t size() const
  {
    return m_size;
  }

  // Return pointer to the first element
  CMail *begin() const
  {
    return m_array;
  }

  // Return pointer to the last+1 element
  CMail *end() const
  {
    return m_array + m_size;
  }

  // Return element at idx
  CMail &operator[](size_t idx) const
  {
    return m_array[idx];
  }

  // Delete old array and initialize new
  void free_and_initialize_array()
  {
    delete[] m_array;
    m_cap = 10;
    m_size = 0;
    m_array = new CMail[m_cap];
  }

private:
  size_t m_cap;
  size_t m_size;
  CMail *m_array;
};

// Represents email inbox or outbox of one person
class CMailIterator
{
public:
  /** Copy constructor
   *  @param vServer = array of all the emails
   *  @param email = email of person we search
   *  @param outbox = if true search person in senders(from), if false search person in recipients(to)
   */
  CMailIterator(const CVector &vServer, const char *email, bool outbox)
      : idx(0)
  {
    for (const auto &x : vServer)
    {
      if (outbox == true && strcmp(x.get_from(), email) == 0)
        vMail.emplace_back(x);
      else if (outbox == false && strcmp(x.get_to(), email) == 0)
        vMail.emplace_back(x);
    }
  }

  // Returns true if element at idx is valid, else false
  explicit operator bool(void) const
  {
    return idx < vMail.size();
  }

  // Returns true if element at idx is not valid, else false
  bool operator!(void) const
  {
    return idx >= vMail.size();
  }

  // Returns element at idx
  const CMail &operator*(void) const
  {
    return vMail[idx];
  }

  // Moves to the next idx in the email array
  CMailIterator &operator++(void)
  {
    idx++;
    return *this;
  }

private:
  CVector vMail;
  size_t idx;
};

// Represents server for storing emails
class CMailServer
{
public:
  CMailServer() = default;

  /** Copy constructor
   *  @param src = instance of CMailServer
   */
  CMailServer(const CMailServer &src)
  {
    for (const auto &x : src.vMail)
    {
      vMail.emplace_back(x);
    }
  }

  /** Operator=
   *  @param src = instance of CMailServer
   *  @return this
   */
  CMailServer &operator=(const CMailServer &src)
  {
    vMail.free_and_initialize_array();
    for (const auto &x : src.vMail)
    {
      vMail.emplace_back(x);
    }
    return *this;
  }

  ~CMailServer() = default;

  // Place email m to the email array(vMail)
  void sendMail(const CMail &m)
  {
    vMail.emplace_back(m);
  }

  /** Create CMailIterator constaining only emails sent from the email
   *  @param email = email to search
   *  @return CMailIterator = person's outbox matching email
   */
  CMailIterator outbox(const char *email) const
  {
    return CMailIterator(vMail, email, true);
  }

  /** Create CMailIterator constaining only emails recieved to the email
   *  @param email = email to search
   *  @return CMailIterator = person's inbox matching email
   */
  CMailIterator inbox(const char *email) const
  {
    return CMailIterator(vMail, email, false);
  }

private:
  CVector vMail;
};

#ifndef __PROGTEST__
bool matchOutput(const CMail &m,
                 const char *str)
{
  ostringstream oss;
  oss << m;
  return oss.str() == str;
  // assert(matchOutput(*i0, "From: john, To: alice, Body: deadline notice"));
}

int main(void)
{
  char from[100], to[100], body[1024];

  assert(CMail("john", "peter", "progtest deadline") == CMail("john", "peter", "progtest deadline"));
  assert(!(CMail("john", "peter", "progtest deadline") == CMail("john", "progtest deadline", "peter")));
  assert(!(CMail("john", "peter", "progtest deadline") == CMail("peter", "john", "progtest deadline")));
  assert(!(CMail("john", "peter", "progtest deadline") == CMail("peter", "progtest deadline", "john")));
  assert(!(CMail("john", "peter", "progtest deadline") == CMail("progtest deadline", "john", "peter")));
  assert(!(CMail("john", "peter", "progtest deadline") == CMail("progtest deadline", "peter", "john")));
  CMailServer s0;
  s0.sendMail(CMail("john", "peter", "some important mail"));
  strncpy(from, "john", sizeof(from));
  strncpy(to, "thomas", sizeof(to));
  strncpy(body, "another important mail", sizeof(body));
  s0.sendMail(CMail(from, to, body));
  strncpy(from, "john", sizeof(from));
  strncpy(to, "alice", sizeof(to));
  strncpy(body, "deadline notice", sizeof(body));
  s0.sendMail(CMail(from, to, body));
  s0.sendMail(CMail("alice", "john", "deadline confirmation"));
  s0.sendMail(CMail("peter", "alice", "PR bullshit"));
  CMailIterator i0 = s0.inbox("alice");
  assert(i0 && *i0 == CMail("john", "alice", "deadline notice"));
  assert(matchOutput(*i0, "From: john, To: alice, Body: deadline notice"));
  assert(++i0 && *i0 == CMail("peter", "alice", "PR bullshit"));
  assert(matchOutput(*i0, "From: peter, To: alice, Body: PR bullshit"));
  assert(!++i0);

  CMailIterator i1 = s0.inbox("john");
  assert(i1 && *i1 == CMail("alice", "john", "deadline confirmation"));
  assert(matchOutput(*i1, "From: alice, To: john, Body: deadline confirmation"));
  assert(!++i1);

  CMailIterator i2 = s0.outbox("john");
  assert(i2 && *i2 == CMail("john", "peter", "some important mail"));
  assert(matchOutput(*i2, "From: john, To: peter, Body: some important mail"));
  assert(++i2 && *i2 == CMail("john", "thomas", "another important mail"));
  assert(matchOutput(*i2, "From: john, To: thomas, Body: another important mail"));
  assert(++i2 && *i2 == CMail("john", "alice", "deadline notice"));
  assert(matchOutput(*i2, "From: john, To: alice, Body: deadline notice"));
  assert(!++i2);

  CMailIterator i3 = s0.outbox("thomas");
  assert(!i3);

  CMailIterator i4 = s0.outbox("steve");
  assert(!i4);

  CMailIterator i5 = s0.outbox("thomas");
  s0.sendMail(CMail("thomas", "boss", "daily report"));
  assert(!i5);

  CMailIterator i6 = s0.outbox("thomas");
  assert(i6 && *i6 == CMail("thomas", "boss", "daily report"));
  assert(matchOutput(*i6, "From: thomas, To: boss, Body: daily report"));
  assert(!++i6);

  CMailIterator i7 = s0.inbox("alice");
  s0.sendMail(CMail("thomas", "alice", "meeting details"));
  assert(i7 && *i7 == CMail("john", "alice", "deadline notice"));
  assert(matchOutput(*i7, "From: john, To: alice, Body: deadline notice"));
  assert(++i7 && *i7 == CMail("peter", "alice", "PR bullshit"));
  assert(matchOutput(*i7, "From: peter, To: alice, Body: PR bullshit"));
  assert(!++i7);

  CMailIterator i8 = s0.inbox("alice");
  assert(i8 && *i8 == CMail("john", "alice", "deadline notice"));
  assert(matchOutput(*i8, "From: john, To: alice, Body: deadline notice"));
  assert(++i8 && *i8 == CMail("peter", "alice", "PR bullshit"));
  assert(matchOutput(*i8, "From: peter, To: alice, Body: PR bullshit"));
  assert(++i8 && *i8 == CMail("thomas", "alice", "meeting details"));
  assert(matchOutput(*i8, "From: thomas, To: alice, Body: meeting details"));
  assert(!++i8);

  CMailServer s1(s0);
  s0.sendMail(CMail("joe", "alice", "delivery details"));
  s1.sendMail(CMail("sam", "alice", "order confirmation"));
  CMailIterator i9 = s0.inbox("alice");
  assert(i9 && *i9 == CMail("john", "alice", "deadline notice"));
  assert(matchOutput(*i9, "From: john, To: alice, Body: deadline notice"));
  assert(++i9 && *i9 == CMail("peter", "alice", "PR bullshit"));
  assert(matchOutput(*i9, "From: peter, To: alice, Body: PR bullshit"));
  assert(++i9 && *i9 == CMail("thomas", "alice", "meeting details"));
  assert(matchOutput(*i9, "From: thomas, To: alice, Body: meeting details"));
  assert(++i9 && *i9 == CMail("joe", "alice", "delivery details"));
  assert(matchOutput(*i9, "From: joe, To: alice, Body: delivery details"));
  assert(!++i9);

  CMailIterator i10 = s1.inbox("alice");
  assert(i10 && *i10 == CMail("john", "alice", "deadline notice"));
  assert(matchOutput(*i10, "From: john, To: alice, Body: deadline notice"));
  assert(++i10 && *i10 == CMail("peter", "alice", "PR bullshit"));
  assert(matchOutput(*i10, "From: peter, To: alice, Body: PR bullshit"));
  assert(++i10 && *i10 == CMail("thomas", "alice", "meeting details"));
  assert(matchOutput(*i10, "From: thomas, To: alice, Body: meeting details"));
  assert(++i10 && *i10 == CMail("sam", "alice", "order confirmation"));
  assert(matchOutput(*i10, "From: sam, To: alice, Body: order confirmation"));
  assert(!++i10);

  CMailServer s2;
  s2.sendMail(CMail("alice", "alice", "mailbox test"));
  CMailIterator i11 = s2.inbox("alice");
  assert(i11 && *i11 == CMail("alice", "alice", "mailbox test"));
  assert(matchOutput(*i11, "From: alice, To: alice, Body: mailbox test"));
  assert(!++i11);

  s2 = s0;
  s0.sendMail(CMail("steve", "alice", "newsletter"));
  s2.sendMail(CMail("paul", "alice", "invalid invoice"));
  CMailIterator i12 = s0.inbox("alice");
  assert(i12 && *i12 == CMail("john", "alice", "deadline notice"));
  assert(matchOutput(*i12, "From: john, To: alice, Body: deadline notice"));
  assert(++i12 && *i12 == CMail("peter", "alice", "PR bullshit"));
  assert(matchOutput(*i12, "From: peter, To: alice, Body: PR bullshit"));
  assert(++i12 && *i12 == CMail("thomas", "alice", "meeting details"));
  assert(matchOutput(*i12, "From: thomas, To: alice, Body: meeting details"));
  assert(++i12 && *i12 == CMail("joe", "alice", "delivery details"));
  assert(matchOutput(*i12, "From: joe, To: alice, Body: delivery details"));
  assert(++i12 && *i12 == CMail("steve", "alice", "newsletter"));
  assert(matchOutput(*i12, "From: steve, To: alice, Body: newsletter"));
  assert(!++i12);

  CMailIterator i13 = s2.inbox("alice");
  assert(i13 && *i13 == CMail("john", "alice", "deadline notice"));
  assert(matchOutput(*i13, "From: john, To: alice, Body: deadline notice"));
  assert(++i13 && *i13 == CMail("peter", "alice", "PR bullshit"));
  assert(matchOutput(*i13, "From: peter, To: alice, Body: PR bullshit"));
  assert(++i13 && *i13 == CMail("thomas", "alice", "meeting details"));
  assert(matchOutput(*i13, "From: thomas, To: alice, Body: meeting details"));
  assert(++i13 && *i13 == CMail("joe", "alice", "delivery details"));
  assert(matchOutput(*i13, "From: joe, To: alice, Body: delivery details"));
  assert(++i13 && *i13 == CMail("paul", "alice", "invalid invoice"));
  assert(matchOutput(*i13, "From: paul, To: alice, Body: invalid invoice"));
  assert(!++i13);

  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
