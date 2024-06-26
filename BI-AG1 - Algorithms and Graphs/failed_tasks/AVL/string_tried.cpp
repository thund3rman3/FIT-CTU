#ifndef __PROGTEST__

#include <cassert>
#include <iomanip>
#include <cstdint>
#include <iostream>
#include <memory>
#include <limits>
#include <optional>
#include <algorithm>
#include <bitset>
#include <list>
#include <array>
#include <vector>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <queue>
#include <random>
#include <type_traits>

#endif


struct TextEditorBackend {
    size_t node_char_count;

    std::vector<std::string> sentences;

    TextEditorBackend(const std::string &text);

    size_t size() const;

    size_t lines() const;

    char at(size_t i) const;

    void edit(size_t i, char c);

    void insert(size_t i, char c);


    void erase(size_t i);

    size_t line_start(size_t r) const;

    size_t line_length(size_t r) const;

    size_t char_to_line(size_t i) const;


};

TextEditorBackend::TextEditorBackend(const std::string &text) {
    node_char_count = 0;
    size_t index = 0;
    bool first = true;
    for (char c: text) {
        if (first) {
            sentences.emplace_back("");
            sentences[index] += c;
            if (c == '\n') {
                sentences.emplace_back("");
                index++;
            }
            first = false;
        } else if (c == '\n') {
            sentences[index] += c;
            index++;
            sentences.emplace_back("");
        } else {
            sentences[index] += c;
        }
        node_char_count++;
    }
}

size_t TextEditorBackend::size() const {
    return node_char_count;
}

size_t TextEditorBackend::lines() const {
    if (sentences.empty())
        return 1;
    return sentences.size();
}

char TextEditorBackend::at(size_t i) const {
    if (i >= size())
        throw std::out_of_range("mimo");
    size_t idx = 0;
    for (const auto &x: sentences) {
        if (i < idx + x.size())
            return x[i - idx];
        idx += x.size();
    }
    return 0;
}

void TextEditorBackend::edit(size_t i, char c) {
    if (i >= size())
        throw std::out_of_range("mimo");

    size_t idx = 0, line_num = 0;
    for (auto &x: sentences) {
        if (i < idx + x.size()) {
            size_t s = sentences[line_num].size();
            if (x[i - idx] == c)
                return;
            if (c == '\n') {
                std::string st;
                st.append(sentences[line_num].begin(), sentences[line_num].begin() + (int) i - (int) idx);
                sentences[line_num].erase(sentences[line_num].begin(),
                                          sentences[line_num].begin() + (int) i - (int) idx + 1);
                st += '\n';
                sentences.insert(sentences.begin() + (int) line_num, st);
            } else if (sentences[line_num][s - 1] == '\n') {
                sentences[line_num][s - 1] = c;
                sentences[line_num] += sentences[line_num + 1];
                sentences.erase(sentences.begin() + (int) line_num + 1);
            } else
                x[i - idx] = c;
            return;
        }
        line_num++;
        idx += x.size();
    }
}

void TextEditorBackend::insert(size_t i, char c) {
    if (i > size())
        throw std::out_of_range("mimo");
    if (sentences.empty()) {
        std::string str;
        str += c;
        node_char_count++;
        sentences.emplace_back(str);
        if (c == '\n')
            sentences.emplace_back("");
        return;
    }
    if (i == size()) {
        size_t it = sentences[lines() - 1].size();
        const char *cc = "\n";
        std::string str;
        str += c;
        if (reinterpret_cast<const char *>(sentences[lines() - 1][it - 1]) == cc)
            sentences.emplace_back(str);
        else {
            sentences[lines() - 1] += c;
        }
        if (c == '\n')
            sentences.emplace_back("");
        node_char_count++;
        return;
    }

    size_t idx = 0, line_num = 0;
    for (auto &x: sentences) {
        if (i < idx + x.size()) {
            if (c == '\n') {
                std::string s;
                s.append(sentences[line_num].begin(), sentences[line_num].begin() + (int) i - (int) idx);
                sentences[line_num].erase(sentences[line_num].begin(),
                                          sentences[line_num].begin() + (int) i - (int) idx);
                s += '\n';
                sentences.insert(sentences.begin() + (int) line_num, s);
            } else {
                x.insert(i - idx, 1, c);
            }
            node_char_count++;
            return;
        }
        idx += x.size();
        line_num++;
    }
}

void TextEditorBackend::erase(size_t i) {
    if (i >= size())
        throw std::out_of_range("mimo");

    size_t idx = 0, line_num = 0;
    for (auto &x: sentences) {
        if (i < idx + x.size()) {
            char c = sentences[line_num][i - idx];
            x.erase(sentences[line_num].begin() + (int) i - (int) idx);
            node_char_count--;

            if (c == '\n') {
                sentences[line_num] += sentences[line_num + 1];
                sentences.erase(sentences.begin() + (int) line_num + 1);
            }
            return;
        }
        idx += x.size();
        line_num++;
    }
}

size_t TextEditorBackend::line_start(size_t r) const {
    if (r > lines() - 1)
        throw std::out_of_range("mimo");
    size_t idx = 0, line_num = 0;
    for (auto &x: sentences) {
        if (r == line_num)
            return idx;
        idx += x.size();
        line_num++;
    }
    return 0;
}

size_t TextEditorBackend::line_length(size_t r) const {
    if (r >= lines())
        throw std::out_of_range("mimo");
    return sentences[r].size();
}

size_t TextEditorBackend::char_to_line(size_t i) const {
    if (i >= size())
        throw std::out_of_range("mimo");
    size_t idx = 0, line_num = 0;
    for (auto &x: sentences) {
        if (i < idx + x.size())
            return line_num;
        idx += x.size();
        line_num++;
    }
    return 0;
}


#ifndef __PROGTEST__

////////////////// Dark magic, ignore ////////////////////////

template<typename T>
auto quote(const T &t) { return t; }

std::string quote(const std::string &s) {
    std::string ret = "\"";
    for (char c: s) if (c != '\n') ret += c; else ret += "\\n";
    return ret + "\"";
}

#define STR_(a) #a
#define STR(a) STR_(a)

#define CHECK_(a, b, a_str, b_str) do { \
    auto _a = (a); \
    decltype(a) _b = (b); \
    if (_a != _b) { \
      std::cout << "Line " << __LINE__ << ": Assertion " \
        << a_str << " == " << b_str << " failed!" \
        << " (lhs: " << quote(_a) << ")" << std::endl; \
      fail++; \
    } else ok++; \
  } while (0)

#define CHECK(a, b) CHECK_(a, b, #a, #b)

#define CHECK_ALL(expr, ...) do { \
    std::array _arr = { __VA_ARGS__ }; \
    for (size_t _i = 0; _i < _arr.size(); _i++) \
      CHECK_((expr)(_i), _arr[_i], STR(expr) "(" << _i << ")", _arr[_i]); \
  } while (0)

#define CHECK_EX(expr, ex) do { \
    try { \
      (expr); \
      fail++; \
      std::cout << "Line " << __LINE__ << ": Expected " STR(expr) \
        " to throw " #ex " but no exception was raised." << std::endl; \
    } catch (const ex&) { ok++; \
    } catch (...) { \
      fail++; \
      std::cout << "Line " << __LINE__ << ": Expected " STR(expr) \
        " to throw " #ex " but got different exception." << std::endl; \
    } \
  } while (0)

////////////////// End of dark magic ////////////////////////


std::string text(const TextEditorBackend &t) {
    std::string ret;
    for (size_t i = 0; i < t.size(); i++) ret.push_back(t.at(i));
    return ret;
}

void test1(int &ok, int &fail) {
    TextEditorBackend s("123\n456\n789");
    CHECK(s.size(), 11);
    CHECK(text(s), "123\n456\n789");
    CHECK(s.lines(), 3);
    CHECK_ALL(s.char_to_line, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2);
    CHECK_ALL(s.line_start, 0, 4, 8);
    CHECK_ALL(s.line_length, 4, 4, 3);
}

void test2(int &ok, int &fail) {
    TextEditorBackend t("123\n456\n789\n");
    CHECK(t.size(), 12);
    CHECK(text(t), "123\n456\n789\n");
    CHECK(t.lines(), 4);
    CHECK_ALL(t.char_to_line, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2);
    CHECK_ALL(t.line_start, 0, 4, 8, 12);
    CHECK_ALL(t.line_length, 4, 4, 4, 0);
}

void test3(int &ok, int &fail) {
    TextEditorBackend t("asdfasdfasdf");

    CHECK(t.size(), 12);
    CHECK(text(t), "asdfasdfasdf");
    CHECK(t.lines(), 1);
    CHECK_ALL(t.char_to_line, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    CHECK(t.line_start(0), 0);
    CHECK(t.line_length(0), 12);

    t.insert(0, '\n');
    CHECK(t.size(), 13);
    CHECK(text(t), "\nasdfasdfasdf");
    CHECK(t.lines(), 2);
    CHECK_ALL(t.line_start, 0, 1);

    t.insert(4, '\n');
    CHECK(t.size(), 14);
    CHECK(text(t), "\nasd\nfasdfasdf");
    CHECK(t.lines(), 3);
    CHECK_ALL(t.line_start, 0, 1, 5);

    t.insert(t.size(), '\n');
    CHECK(t.size(), 15);
    CHECK(text(t), "\nasd\nfasdfasdf\n");
    CHECK(t.lines(), 4);
    CHECK_ALL(t.line_start, 0, 1, 5, 15);

    t.edit(t.size() - 1, 'H');
    CHECK(t.size(), 15);
    CHECK(text(t), "\nasd\nfasdfasdfH");
    CHECK(t.lines(), 3);
    CHECK_ALL(t.line_start, 0, 1, 5);

    t.erase(8);
    CHECK(t.size(), 14);
    CHECK(text(t), "\nasd\nfasfasdfH");
    CHECK(t.lines(), 3);
    CHECK_ALL(t.line_start, 0, 1, 5);

    t.erase(4);
    CHECK(t.size(), 13);
    CHECK(text(t), "\nasdfasfasdfH");
    CHECK(t.lines(), 2);
    CHECK_ALL(t.line_start, 0, 1);
}

void test_ex(int &ok, int &fail) {
    TextEditorBackend t("123\n456\n789\n");
    CHECK_EX(t.at(12), std::out_of_range);

    CHECK_EX(t.insert(13, 'a'), std::out_of_range);
    CHECK_EX(t.edit(12, 'x'), std::out_of_range);
    CHECK_EX(t.erase(12), std::out_of_range);

    CHECK_EX(t.line_start(4), std::out_of_range);
    CHECK_EX(t.line_start(40), std::out_of_range);
    CHECK_EX(t.line_length(4), std::out_of_range);
    CHECK_EX(t.line_length(6), std::out_of_range);
    CHECK_EX(t.char_to_line(12), std::out_of_range);
    CHECK_EX(t.char_to_line(25), std::out_of_range);
}

int main() {
    int ok = 0, fail = 0;
    if (!fail) test1(ok, fail);
    if (!fail) test2(ok, fail);
    if (!fail) test3(ok, fail);
    if (!fail) test_ex(ok, fail);

    if (!fail) std::cout << "Passed all " << ok << " tests!" << std::endl;
    else std::cout << "Failed " << fail << " of " << (ok + fail) << " tests." << std::endl;

    TextEditorBackend A("");
    for (int i = 0; i < 15; i++) {
        A.insert(i, 'a');
    }
    std::cout << A.lines() << std::endl;
    for (int j = 0; j < 15; j++) {
        A.edit(j / 2, '\n');
    }
    std::cout << A.lines() << std::endl;

    for (int i = 0; i < 15; i++) {
        A.insert(i, 'a');
    }
    std::cout << A.lines() << std::endl;

    for (int i = 0; i < 15; i++) {
        A.erase(0);
    }
    std::cout << A.lines() << std::endl;
    for (int i = 0; i < 15; i++) {
        std::cout << A.char_to_line(i) << std::endl;
    }
}

#endif


