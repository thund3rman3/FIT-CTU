#ifndef __PROGTEST__

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cctype>
#include <climits>
#include <cfloat>
#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
#include <array>
#include <utility>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <functional>
#include <iterator>
#include <stdexcept>
#include <variant>
#include <optional>
#include <compare>
#include <charconv>
#include <span>
#include <utility>
#include "expression.h"

using namespace std::literals;
using CValue = std::variant<std::monostate, double, std::string>; // variant=union, monostate=no value
constexpr unsigned SPREADSHEET_CYCLIC_DEPS = 0x01;
constexpr unsigned SPREADSHEET_FUNCTIONS = 0x02;
constexpr unsigned SPREADSHEET_FILE_IO = 0x04;
constexpr unsigned SPREADSHEET_SPEED = 0x08;
constexpr unsigned SPREADSHEET_PARSER = 0x10;
#endif /* __PROGTEST__ */

// Class implementing cell 2D coordinates in spreadsheet
class CPos {
public:
    // Constructor from string
    explicit CPos(std::string_view str);

    /**
     * Constructor used just for adding rows and collumns to cell.
     * Can be used for iterating through cells easily.
     * @param coll
     * @param row
     */
    CPos(int coll, int row);

    // Operator <
    bool operator<(const CPos &other) const;

    /**
     * Adds pos2 collumns and rows to pos1
     * @param pos1
     * @param pos2
     * @return new CPos
     */
    friend CPos operator+(const CPos &pos1, const CPos &pos2);

    /**
     * Adds collumns and row form CPos other to this CPos
     * based on this CPos absolute values
     * @param other - CPos to be added
     * @return this CPos
     */
    CPos &operator+=(const CPos &other);

    /**
     * Operator<< used for printing CPos in format [coll row ]
     * @param os - input for data
     * @param pos - CPos to print
     * @return ostream os
     */
    friend std::ostream &operator<<(std::ostream &os, const CPos &pos);

    /**
     * Checks if str is valid CPos identificator and sets absolute flags
     * @param str - checked string
     * @param m_collAbs - if collumn is absolute
     * @param m_rowAbs - if row is absolute
     * @return true - valid, false - invalid CPos identificator
     */
    static bool isValid(std::string_view str, bool &m_collAbs, bool &m_rowAbs);

    // Collumn value getter
    int getColl() const;

    // Row value getter
    int getRow() const;

    // Returns string made from collumn and row value with absolute flags
    std::string getRowCollString() const;

private:
    int m_coll;
    std::string m_collString;
    int m_row;
    bool m_collAbs;
    bool m_rowAbs;
};

CPos::CPos(std::string_view str)
        : m_collAbs(false), m_rowAbs(false) {
    if (!isValid(str, m_collAbs, m_rowAbs))
        throw std::invalid_argument("Wrong cell id.");

    m_row = 0;
    m_coll = 0;
    bool row_begin = false;
    for (const char c: str) {
        if (!row_begin) {
            if (std::isalpha(c)) {
                m_coll = m_coll * 26 + (std::toupper(c) - 'A' + 1);
                m_collString += std::toupper(c);
            }
            else if (std::isdigit(c)) {
                row_begin = true;
                m_row = m_row * 10 + (c - '0');
            }
        }
        else if (std::isdigit(c))
            m_row = m_row * 10 + (c - '0');
    }
}

CPos::CPos(int coll, int row)
        : m_coll(coll), m_row(row), m_collAbs(false), m_rowAbs(false) {
    while (coll > 0) {
        char c = 'A' + ((coll - 1) % 26);
        m_collString = c + m_collString;
        coll = (coll - 1) / 26;
    }
}

bool CPos::operator<(const CPos &other) const {
    if (m_coll == other.m_coll)
        return m_row < other.m_row;
    return m_coll < other.m_coll;
}

bool CPos::isValid(std::string_view str, bool &m_collAbs, bool &m_rowAbs) {
    bool colFound = false;
    bool rowFound = false;
    for (const char c: str) {
        if (!colFound && std::isalpha(c))
            colFound = true;
        else if (!colFound && c == '$' && !m_collAbs)
            m_collAbs = true;
        else if (colFound && c == '$' && !m_rowAbs && !rowFound)
            m_rowAbs = true;
        else if ((!rowFound || m_rowAbs) && !std::isalpha(c)) {
            if (std::isdigit(c))
                rowFound = true;
            else
                return false;
        }
        else if ((rowFound && !std::isdigit(c)) || (m_rowAbs && !std::isdigit(c)))
            return false;
    }
    return colFound && rowFound;
}

std::ostream &operator<<(std::ostream &os, const CPos &pos) {
    os << pos.m_coll << " " << pos.m_row << " ";
    return os;
}

CPos operator+(const CPos &pos1, const CPos &pos2) {
    CPos res(pos1.m_coll + pos2.m_coll, pos1.m_row + pos2.m_row);
    return res;
}

CPos &CPos::operator+=(const CPos &other) {
    if (!m_collAbs) {
        m_coll += other.m_coll;
        int coll = m_coll;
        m_collString = "";
        while (coll > 0) {
            char c = 'A' + ((coll - 1) % 26);
            m_collString = c + m_collString;
            coll = (coll - 1) / 26;
        }
    }
    if (!m_rowAbs)
        m_row += other.m_row;
    return *this;
}

int CPos::getColl() const {
    return m_coll;
}

int CPos::getRow() const {
    return m_row;
}

std::string CPos::getRowCollString() const {
    std::string res;
    if (m_collAbs)
        res += '$';
    res += m_collString;
    if (m_rowAbs)
        res += '$';
    res += std::to_string(m_row);
    return res;
}

/**
 * Adds two CValues
 * @param val1
 * @param val2
 * @return monostate - if at least one of the arguments is monostate,
 * @return double - if both arguments are doubles,
 * @return string - if at least one of the arguments is string
 */
CValue operator+(const CValue &val1, const CValue &val2) {
    if (std::holds_alternative<double>(val1) && std::holds_alternative<double>(val2))
        return std::get<double>(val1) + std::get<double>(val2);

    else if (std::holds_alternative<std::string>(val1) && std::holds_alternative<std::string>(val2))
        return std::get<std::string>(val1) + std::get<std::string>(val2);

    else if (std::holds_alternative<std::string>(val1) && std::holds_alternative<double>(val2))
        return std::get<std::string>(val1) + std::to_string(std::get<double>(val2));

    else if (std::holds_alternative<double>(val1) && std::holds_alternative<std::string>(val2))
        return std::to_string(std::get<double>(val1)) + std::get<std::string>(val2);

    return std::monostate();
}

/**
 * Substracts two CValues
 * @param val1
 * @param val2
 * @return - double if both arguments are doubles
 * @return - monostate if some of them is monostate
 */
CValue operator-(const CValue &val1, const CValue &val2) {
    if (std::holds_alternative<double>(val1) && std::holds_alternative<double>(val2))
        return std::get<double>(val1) - std::get<double>(val2);
    return std::monostate();
}

/**
 * Multiplies two CValues
 * @param val1
 * @param val2
 * @return - double if both arguments are doubles
 * @return - monostate if some of them is monostate
 */
CValue operator*(const CValue &val1, const CValue &val2) {
    if (std::holds_alternative<double>(val1) && std::holds_alternative<double>(val2))
        return std::get<double>(val1) * std::get<double>(val2);
    return std::monostate();
}

/**
 * Divides two CValues
 * @param val1
 * @param val2
 * @return - double if both arguments are doubles
 * @return - monostate if some of them is monostate
 */
CValue operator/(const CValue &val1, const CValue &val2) {
    if (std::holds_alternative<double>(val1) && std::holds_alternative<double>(val2))
        if (std::get<double>(val2) != 0)
            return std::get<double>(val1) / std::get<double>(val2);

    return std::monostate();
}

/**
 * Powers two CValues
 * @param val1
 * @param val2
 * @return double - if both arguments are doubles,
 * @return monostate-  if some of them is monostate
 */
CValue operator^(const CValue &val1, const CValue &val2) {
    if (std::holds_alternative<double>(val1) && std::holds_alternative<double>(val2))
        return std::pow(std::get<double>(val1), std::get<double>(val2));
    return std::monostate();
}

/**
 * Compares two CValues
 * @param lhs
 * @param rhs
 * @return - partial_ordering, unordered if can't be compared
 */
std::partial_ordering operator<=>(const CValue &lhs, const CValue &rhs) {
    if (std::holds_alternative<double>(lhs) && std::holds_alternative<double>(rhs))
        return std::get<double>(lhs) <=> std::get<double>(rhs);

    else if (std::holds_alternative<std::string>(lhs) && std::holds_alternative<std::string>(rhs))
        return std::get<std::string>(lhs) <=> std::get<std::string>(rhs);

    return std::partial_ordering::unordered;
}

class CBuilder;

// Abstract class representing parent AST node
class CExpr {
public:
    /**
     * Constructor
     * @param builder
     */
    explicit CExpr(CBuilder &builder)
            : m_builder(builder) {
    }

    // Destructor
    virtual ~CExpr() = default;

    // Evaluates node
    virtual CValue eval() const = 0;

    /**
     * Sets new expression string based on related nodes and their expressions.
     * @param first - first in expr string
     * @return expr string of this node
     */
    virtual std::string setExprEval(bool first) = 0;

    /**
     * Sets new builder
     * @param builder
     */
    void setBuilder(CBuilder &builder);

    /**
     * Sets string expression, what is evaluated
     * @param expr - string expression
     */
    void setExpr(const std::string &expr) {
        m_expression = expr;
    }

    /**
     * Gets string expression
     * @return string expression
     */
    std::string getExpr() const {
        return m_expression;
    }

    /**
     * Gets cell references, wich is the string expression made from
     * @return set of related cells
     */
    virtual std::set<CPos> getNeighbours() const {
        return {};
    }

    /**
     * Set new set of related cells based of string expression
     * @param n - set of related cells
     */
    virtual void setNeighbours(const std::set<CPos> &n) {
    }

    /**
     * Clone pointer to this node
     * @param builder
     * @param expr - string expression
     * @return shared pointer to this
     */
    virtual std::shared_ptr<CExpr> clone(CBuilder &builder, const std::string &expr) const = 0;

protected:
    CBuilder &m_builder; // builder for cell evaluation
    std::string m_expression; // string to evaluate
};

using AExpr = std::shared_ptr<CExpr>;

// Number value
class CLiteral : public CExpr {
public:
    explicit CLiteral(double val, CBuilder &builder)
            : CExpr(builder), val(val) {
    }

    ~CLiteral() override = default;

    CValue eval() const override {
        return val;
    }

    std::string setExprEval(bool first) override {
        std::string res;
        res += std::to_string(val);
        return res;
    }

    std::shared_ptr<CExpr> clone(CBuilder &builder, const std::string &expr) const override {
        CLiteral res(val, builder);
        res.setExpr(expr);
        return std::make_shared<CLiteral>(res);
    }

private:
    double val;
};

// String value
class CStringLiteral : public CExpr {
public:
    explicit CStringLiteral(std::string val, CBuilder &builder)
            : CExpr(builder), val(std::move(val)) {
    }

    ~CStringLiteral() override = default;

    CValue eval() const override {
        return val;
    }

    std::string setExprEval(bool first) override {
        std::string res;
        res += val;
        return res;
    }

    std::shared_ptr<CExpr> clone(CBuilder &builder, const std::string &expr) const override {
        CStringLiteral res(val, builder);
        res.setExpr(expr);
        return std::make_shared<CStringLiteral>(res);
    }

private:
    std::string val;
};

// Addition of two nodes
class CAddition : public CExpr {
public:
    explicit CAddition(CBuilder &builder)
            : CExpr(builder) {
    }

    CAddition(AExpr lhs, AExpr rhs, CBuilder &builder)
            : CExpr(builder), m_Lhs(std::move(lhs)), m_Rhs(std::move(rhs)) {
    }

    ~CAddition() override = default;


    CValue eval() const override {
        return m_Lhs->eval() + m_Rhs->eval();
    }

    std::string setExprEval(bool first) override {
        std::string res;
        if (first)
            res = "=";
        res += '(';
        res += m_Lhs->setExprEval(false);
        res += "+";
        res += m_Rhs->setExprEval(false);
        res += ')';
        return res;
    }

    std::shared_ptr<CExpr> clone(CBuilder &builder, const std::string &expr) const override {
        CAddition res(builder);
        res.setExpr(expr);
        res.m_Rhs = m_Rhs->clone(builder, m_Rhs->getExpr());
        res.m_Lhs = m_Lhs->clone(builder, m_Lhs->getExpr());
        return std::make_shared<CAddition>(res);
    }

    std::set<CPos> getNeighbours() const override {
        return m_neighbours;
    }

    void setNeighbours(const std::set<CPos> &n) override {
        m_neighbours = n;
    }

private:
    AExpr m_Lhs;
    AExpr m_Rhs;
    std::set<CPos> m_neighbours;
};

// Subtraction of two nodes
class CSubtraction : public CExpr {
public:
    explicit CSubtraction(CBuilder &builder)
            : CExpr(builder) {
    }

    CSubtraction(AExpr lhs, AExpr rhs, CBuilder &builder)
            : CExpr(builder), m_Lhs(std::move(lhs)), m_Rhs(std::move(rhs)) {
    }

    ~CSubtraction() override = default;

    CValue eval() const override {
        return m_Lhs->eval() - m_Rhs->eval();
    }

    std::string setExprEval(bool first) override {
        std::string res;
        if (first)
            res = "=";
        res += '(';
        res += m_Lhs->setExprEval(false);
        res += "-";
        res += m_Rhs->setExprEval(false);
        res += ')';
        return res;
    }

    std::shared_ptr<CExpr> clone(CBuilder &builder, const std::string &expr) const override {
        CSubtraction res(builder);
        res.setExpr(expr);
        res.m_Rhs = m_Rhs->clone(builder, m_Rhs->getExpr());
        res.m_Lhs = m_Lhs->clone(builder, m_Lhs->getExpr());
        return std::make_shared<CSubtraction>(res);
    }

    std::set<CPos> getNeighbours() const override {
        return m_neighbours;
    }

    void setNeighbours(const std::set<CPos> &n) override {
        m_neighbours = n;
    }

private:
    AExpr m_Lhs;
    AExpr m_Rhs;
    std::set<CPos> m_neighbours;
};

// Multiplication of two nodes
class CMultiplication : public CExpr {
public:
    explicit CMultiplication(CBuilder &builder)
            : CExpr(builder) {
    }

    CMultiplication(AExpr lhs, AExpr rhs, CBuilder &builder)
            : CExpr(builder), m_Lhs(std::move(lhs)), m_Rhs(std::move(rhs)) {
    }

    ~CMultiplication() override = default;

    CValue eval() const override {
        CValue a = m_Lhs->eval();
        CValue b = m_Rhs->eval();
        return a * b;
    }

    std::string setExprEval(bool first) override {
        std::string res;
        if (first)
            res = "=";
        res += '(';
        res += m_Lhs->setExprEval(false);
        res += "*";
        res += m_Rhs->setExprEval(false);
        res += ')';
        return res;
    }

    std::shared_ptr<CExpr> clone(CBuilder &builder, const std::string &expr) const override {
        CMultiplication res(builder);
        res.setExpr(expr);
        res.m_Rhs = m_Rhs->clone(builder, m_Rhs->getExpr());
        res.m_Lhs = m_Lhs->clone(builder, m_Lhs->getExpr());
        return std::make_shared<CMultiplication>(res);
    }

    std::set<CPos> getNeighbours() const override {
        return m_neighbours;
    }

    void setNeighbours(const std::set<CPos> &n) override {
        m_neighbours = n;
    }

private:
    AExpr m_Lhs;
    AExpr m_Rhs;
    std::set<CPos> m_neighbours;
};

// Division of two nodes
class CDivision : public CExpr {
public:
    explicit CDivision(CBuilder &builder)
            : CExpr(builder) {
    }

    CDivision(AExpr lhs, AExpr rhs, CBuilder &builder)
            : CExpr(builder), m_Lhs(std::move(lhs)), m_Rhs(std::move(rhs)) {
    }

    ~CDivision() override = default;

    CValue eval() const override {
        return m_Lhs->eval() / m_Rhs->eval();
    }

    std::string setExprEval(bool first) override {
        std::string res;
        if (first)
            res = "=";
        res += '(';
        res += m_Lhs->setExprEval(false);
        res += "/";
        res += m_Rhs->setExprEval(false);
        res += ')';
        return res;
    }

    std::shared_ptr<CExpr> clone(CBuilder &builder, const std::string &expr) const override {
        CDivision res(builder);
        res.setExpr(expr);
        res.m_Rhs = m_Rhs->clone(builder, m_Rhs->getExpr());
        res.m_Lhs = m_Lhs->clone(builder, m_Lhs->getExpr());
        return std::make_shared<CDivision>(res);
    }

    std::set<CPos> getNeighbours() const override {
        return m_neighbours;
    }

    void setNeighbours(const std::set<CPos> &n) override {
        m_neighbours = n;
    }

private:
    AExpr m_Lhs;
    AExpr m_Rhs;
    std::set<CPos> m_neighbours;
};

// Power of two nodes
class CPower : public CExpr {
public:
    explicit CPower(CBuilder &builder)
            : CExpr(builder) {
    }

    CPower(AExpr lhs, AExpr rhs, CBuilder &builder)
            : CExpr(builder), m_Lhs(std::move(lhs)), m_Rhs(std::move(rhs)) {
    }

    ~CPower() override = default;

    CValue eval() const override {
        return m_Lhs->eval() ^ m_Rhs->eval();
    }

    std::string setExprEval(bool first) override {
        std::string res;
        if (first)
            res = "=";
        res += '(';
        res += m_Lhs->setExprEval(false);
        res += "^";
        res += m_Rhs->setExprEval(false);
        res += ')';
        return res;
    }

    std::shared_ptr<CExpr> clone(CBuilder &builder, const std::string &expr) const override {
        CPower res(builder);
        res.setExpr(expr);
        res.m_Rhs = m_Rhs->clone(builder, m_Rhs->getExpr());
        res.m_Lhs = m_Lhs->clone(builder, m_Lhs->getExpr());
        return std::make_shared<CPower>(res);
    }

    std::set<CPos> getNeighbours() const override {
        return m_neighbours;
    }

    void setNeighbours(const std::set<CPos> &n) override {
        m_neighbours = n;
    }

private:
    AExpr m_Lhs;
    AExpr m_Rhs;
    std::set<CPos> m_neighbours;
};

// Negation node
class CNegation : public CExpr {
public:
    explicit CNegation(CBuilder &builder)
            : CExpr(builder) {
    }

    explicit CNegation(AExpr val, CBuilder &builder)
            : CExpr(builder), m_val(std::move(val)) {
    }

    ~CNegation() override = default;

    CValue eval() const override {
        return m_val->eval() * (-1.0);
    }

    std::string setExprEval(bool first) override {
        std::string res;
        if (first)
            res = "=";
        res += '(';
        res += "-";
        res += m_val->setExprEval(false);
        res += ')';
        return res;
    }

    std::shared_ptr<CExpr> clone(CBuilder &builder, const std::string &expr) const override {
        CNegation res(builder);
        res.setExpr(expr);
        res.m_val = m_val->clone(builder, m_val->getExpr());
        return std::make_shared<CNegation>(res);
    }

    std::set<CPos> getNeighbours() const override {
        return m_neighbours;
    }

    void setNeighbours(const std::set<CPos> &n) override {
        m_neighbours = n;
    }

private:
    AExpr m_val;
    std::set<CPos> m_neighbours;
};

// Equality node
class CEqual : public CExpr {
public:
    explicit CEqual(CBuilder &builder)
            : CExpr(builder) {
    }

    explicit CEqual(AExpr lhs, AExpr rhs, CBuilder &builder)
            : CExpr(builder), m_Lhs(std::move(lhs)), m_Rhs(std::move(rhs)) {
    }

    ~CEqual() override = default;

    CValue eval() const override {

        if ((m_Lhs->eval() <=> m_Rhs->eval()) == std::partial_ordering::unordered)
            return {};

        return (m_Lhs->eval() == m_Rhs->eval()) ? CValue(1.0) : CValue(0.0);
    }

    std::string setExprEval(bool first) override {
        std::string res;
        if (first)
            res = "=";
        res += '(';
        res += m_Lhs->setExprEval(false);
        res += "=";
        res += m_Rhs->setExprEval(false);
        res += ')';
        return res;
    }

    std::shared_ptr<CExpr> clone(CBuilder &builder, const std::string &expr) const override {
        CEqual res(builder);
        res.setExpr(expr);
        res.m_Rhs = m_Rhs->clone(builder, m_Rhs->getExpr());
        res.m_Lhs = m_Lhs->clone(builder, m_Lhs->getExpr());
        return std::make_shared<CEqual>(res);
    }

    std::set<CPos> getNeighbours() const override {
        return m_neighbours;
    }

    void setNeighbours(const std::set<CPos> &n) override {
        m_neighbours = n;
    }

private:
    AExpr m_Lhs;
    AExpr m_Rhs;
    std::set<CPos> m_neighbours;
};

// Not equality of nodes
class CNEqual : public CExpr {
public:
    explicit CNEqual(CBuilder &builder)
            : CExpr(builder) {
    }

    explicit CNEqual(AExpr lhs, AExpr rhs, CBuilder &builder)
            : CExpr(builder), m_Lhs(std::move(lhs)), m_Rhs(std::move(rhs)) {
    }

    ~CNEqual() override = default;

    CValue eval() const override {

        if ((m_Lhs->eval() <=> m_Rhs->eval()) == std::partial_ordering::unordered)
            return {};

        return (m_Lhs->eval() != m_Rhs->eval()) ? CValue(1.0) : CValue(0.0);
    }

    std::string setExprEval(bool first) override {
        std::string res;
        if (first)
            res = "=";
        res += '(';
        res += m_Lhs->setExprEval(false);
        res += "<>";
        res += m_Rhs->setExprEval(false);
        res += ')';
        return res;
    }

    std::shared_ptr<CExpr> clone(CBuilder &builder, const std::string &expr) const override {
        CNEqual res(builder);
        res.setExpr(expr);
        res.m_Rhs = m_Rhs->clone(builder, m_Rhs->getExpr());
        res.m_Lhs = m_Lhs->clone(builder, m_Lhs->getExpr());
        return std::make_shared<CNEqual>(res);
    }

    std::set<CPos> getNeighbours() const override {
        return m_neighbours;
    }

    void setNeighbours(const std::set<CPos> &n) override {
        m_neighbours = n;
    }

private:
    AExpr m_Lhs;
    AExpr m_Rhs;
    std::set<CPos> m_neighbours;
};

// First node greater than second
class CGreater : public CExpr {
public:

    explicit CGreater(CBuilder &builder)
            : CExpr(builder) {
    }

    explicit CGreater(AExpr lhs, AExpr rhs, CBuilder &builder)
            : CExpr(builder), m_Lhs(std::move(lhs)), m_Rhs(std::move(rhs)) {
    }

    ~CGreater() override = default;

    CValue eval() const override {

        if ((m_Lhs->eval() <=> m_Rhs->eval()) == std::partial_ordering::unordered)
            return {};

        return (m_Lhs->eval() > m_Rhs->eval()) ? CValue(1.0) : CValue(0.0);
    }

    std::string setExprEval(bool first) override {
        std::string res;
        if (first)
            res = "=";
        res += '(';
        res += m_Lhs->setExprEval(false);
        res += ">";
        res += m_Rhs->setExprEval(false);
        res += ')';
        return res;
    }

    std::shared_ptr<CExpr> clone(CBuilder &builder, const std::string &expr) const override {
        CGreater res(builder);
        res.setExpr(expr);
        res.m_Rhs = m_Rhs->clone(builder, m_Rhs->getExpr());
        res.m_Lhs = m_Lhs->clone(builder, m_Lhs->getExpr());
        return std::make_shared<CGreater>(res);
    }

    std::set<CPos> getNeighbours() const override {
        return m_neighbours;
    }

    void setNeighbours(const std::set<CPos> &n) override {
        m_neighbours = n;
    }

private:
    AExpr m_Lhs;
    AExpr m_Rhs;
    std::set<CPos> m_neighbours;
};

// First node greater or equal to second
class CGreaterEqual : public CExpr {
public:

    explicit CGreaterEqual(CBuilder &builder)
            : CExpr(builder) {
    }

    explicit CGreaterEqual(AExpr lhs, AExpr rhs, CBuilder &builder)
            : CExpr(builder), m_Lhs(std::move(lhs)), m_Rhs(std::move(rhs)) {
    }

    ~CGreaterEqual() override = default;

    CValue eval() const override {

        if ((m_Lhs->eval() <=> m_Rhs->eval()) == std::partial_ordering::unordered)
            return {};

        return (m_Lhs->eval() >= m_Rhs->eval()) ? CValue(1.0) : CValue(0.0);
    }

    std::string setExprEval(bool first) override {
        std::string res;
        if (first)
            res = "=";
        res += '(';
        res += m_Lhs->setExprEval(false);
        res += ">=";
        res += m_Rhs->setExprEval(false);
        res += ')';
        return res;
    }

    std::shared_ptr<CExpr> clone(CBuilder &builder, const std::string &expr) const override {
        CGreaterEqual res(builder);
        res.setExpr(expr);
        res.m_Rhs = m_Rhs->clone(builder, m_Rhs->getExpr());
        res.m_Lhs = m_Lhs->clone(builder, m_Lhs->getExpr());
        return std::make_shared<CGreaterEqual>(res);
    }

    std::set<CPos> getNeighbours() const override {
        return m_neighbours;
    }

    void setNeighbours(const std::set<CPos> &n) override {
        m_neighbours = n;
    }

private:
    AExpr m_Lhs;
    AExpr m_Rhs;
    std::set<CPos> m_neighbours;
};

// First node smaller than second
class CSmaller : public CExpr {
public:
    explicit CSmaller(CBuilder &builder)
            : CExpr(builder) {
    }

    explicit CSmaller(AExpr lhs, AExpr rhs, CBuilder &builder)
            : CExpr(builder), m_Lhs(std::move(lhs)), m_Rhs(std::move(rhs)) {
    }

    ~CSmaller() override = default;

    CValue eval() const override {
        if ((m_Lhs->eval() <=> m_Rhs->eval()) == std::partial_ordering::unordered)
            return {};
        return (m_Lhs->eval() < m_Rhs->eval()) ? CValue(1.0) : CValue(0.0);
    }

    std::string setExprEval(bool first) override {
        std::string res;
        if (first)
            res = "=";
        res += '(';
        res += m_Lhs->setExprEval(false);
        res += "<";
        res += m_Rhs->setExprEval(false);
        res += ')';
        return res;
    }

    std::shared_ptr<CExpr> clone(CBuilder &builder, const std::string &expr) const override {
        CSmaller res(builder);
        res.setExpr(expr);
        res.m_Rhs = m_Rhs->clone(builder, m_Rhs->getExpr());
        res.m_Lhs = m_Lhs->clone(builder, m_Lhs->getExpr());
        return std::make_shared<CSmaller>(res);
    }

    std::set<CPos> getNeighbours() const override {
        return m_neighbours;
    }

    void setNeighbours(const std::set<CPos> &n) override {
        m_neighbours = n;
    }

private:
    AExpr m_Lhs;
    AExpr m_Rhs;
    std::set<CPos> m_neighbours;
};

// First node smaller or equal to second
class CSmallerEqual : public CExpr {
public:
    explicit CSmallerEqual(AExpr lhs, AExpr rhs, CBuilder &builder)
            : CExpr(builder), m_Lhs(std::move(lhs)), m_Rhs(std::move(rhs)) {
    }

    explicit CSmallerEqual(CBuilder &builder)
            : CExpr(builder) {
    }

    ~CSmallerEqual() override = default;

    CValue eval() const override {

        if ((m_Lhs->eval() <=> m_Rhs->eval()) == std::partial_ordering::unordered)
            return {};

        return (m_Lhs->eval() <= m_Rhs->eval()) ? CValue(1.0) : CValue(0.0);
    }

    std::string setExprEval(bool first) override {
        std::string res;
        if (first)
            res = "=";
        res += '(';
        res += m_Lhs->setExprEval(false);
        res += "<=";
        res += m_Rhs->setExprEval(false);
        res += ')';
        return res;
    }

    std::shared_ptr<CExpr> clone(CBuilder &builder, const std::string &expr) const override {
        CSmallerEqual res(builder);
        res.setExpr(expr);
        res.m_Rhs = m_Rhs->clone(builder, m_Rhs->getExpr());
        res.m_Lhs = m_Lhs->clone(builder, m_Lhs->getExpr());
        return std::make_shared<CSmallerEqual>(res);
    }

    std::set<CPos> getNeighbours() const override {
        return m_neighbours;
    }

    void setNeighbours(const std::set<CPos> &n) override {
        m_neighbours = n;
    }

private:
    AExpr m_Lhs;
    AExpr m_Rhs;
    std::set<CPos> m_neighbours;
};

// Builder with methods called by parser
class CBuilder : public CExprBuilder {
public:
    /**
     * Constructor
     * @param table - pointing to map in spreadsheet
     */
    explicit CBuilder(std::map<CPos, AExpr> &table);

    /**
     * Copy constructor
     * @param other - other builder
     */
    CBuilder(const CBuilder &other);

    /**
     * Operator=
     * @param other - other builder
     * @return this builder
     */
    CBuilder &operator=(const CBuilder &other);

    // Destructor
    ~CBuilder() = default;

    /**
     * Get reference to a map
     * @return map - spreadsheet
     */
    std::map<CPos, AExpr> &getMap() const;

    /**
     * Returns node from the top of stack.
     * Should be called after all nodes are parsed and this is result node.
     * @return AExpr - result node
     */
    AExpr getTopStack();

    // Pushes addition node to stack
    void opAdd() override;

    // Pushes subtraction node to stack
    void opSub() override;

    // Pushes multiplication node to stack
    void opMul() override;

    // Pushes divison node to stack
    void opDiv() override;

    // Pushes power node to stack
    void opPow() override;

    // Pushes negation node to stack
    void opNeg() override;

    // Pushes equality node to stack
    void opEq() override;

    // Pushes not equal node to stack
    void opNe() override;

    // Pushes smaller node to stack
    void opLt() override;

    // Pushes smaller-equal node to stack
    void opLe() override;

    // Pushes greater node to stack
    void opGt() override;

    // Pushes greater-equal node to stack
    void opGe() override;

    // Pushes number node to stack
    void valNumber(double val) override;

    // Pushes string node to stack
    void valString(std::string val) override;

    // Pushes reference node to stack
    void valReference(std::string val) override;

    // Unimplemented
    void valRange(std::string val) override;

    // Unimplemented
    void funcCall(std::string fnName, int paramCount) override;

    // Evaluates the node from the top of the stack.
    CValue eval();

    /**
     * Sets row, coll value that should be added to the processed expression in CRef nodes
     * @param pos
     */
    void setOverhead(const CPos &pos);

    // Returns neighbours set of related CRefs
    std::set<CPos> getNodes() const {
        return m_nodes;
    }

    // Clear neighbours set from related CRefs
    void clearNodes() {
        m_nodes.clear();
    }

private:
    std::stack<AExpr> m_expressions; // work stack
    std::map<CPos, AExpr> &m_table; // spreadsheet
    CPos m_overhead; // row, coll value that should be added to the processed expression in CRef nodes
    std::set<CPos> m_nodes; // positions of cells making up the last expression (neighbours)
};

CBuilder::CBuilder(std::map<CPos, AExpr> &table)
        : m_table(table), m_overhead(0, 0) {

}

CBuilder::CBuilder(const CBuilder &other)
        : m_expressions(other.m_expressions), m_table(other.m_table), m_overhead(CPos(0, 0)) {

}

CBuilder &CBuilder::operator=(const CBuilder &other) {
    if (this == &other)
        return *this;
    m_nodes = other.m_nodes;
    m_overhead = CPos(0, 0);
    m_expressions = other.m_expressions;
    m_table = other.m_table;
    return *this;
}

std::map<CPos, AExpr> &CBuilder::getMap() const {
    return m_table;
}

AExpr CBuilder::getTopStack() {
    AExpr top = m_expressions.top();
    m_expressions.pop();
    return top;
}

void CExpr::setBuilder(CBuilder &builder) {
    if (&m_builder != &builder)
        m_builder = builder;
}

class CRef : public CExpr {
public:
    explicit CRef(CPos val, CBuilder &builder)
            : CExpr(builder), m_val(std::move(val)) {
    }

    ~CRef() override = default;

    CRef(const CRef &other)
            : CExpr(other.m_builder), m_val(other.m_val) {
    }

    /**
     * Evaluates reference node.
     * Looks to spreadsheet and evalueates the expression.
     * @return result CValue
     */
    CValue eval() const override {
        auto itMap = m_builder.getMap().find(m_val);

        if (itMap == m_builder.getMap().end())
            return {};
        else {
            CValue exp = itMap->second->eval();
            if (std::holds_alternative<double>(exp)) {
                return exp;
            }
            else if (std::holds_alternative<std::string>(exp)) {
                parseExpression(std::get<std::string>(exp), m_builder);
                return m_builder.eval();
            }
            return exp;
        }
    }

    std::string setExprEval(bool first) override {
        std::string res;
        if (first)
            res = "=";
        res += m_val.getRowCollString();
        return res;
    }

    std::shared_ptr<CExpr> clone(CBuilder &builder, const std::string &expr) const override {
        CRef res(m_val, builder);
        res.setExpr(expr);
        return std::make_shared<CRef>(res);
    }

    std::set<CPos> getNeighbours() const override {
        return m_neighbours;
    }

    void setNeighbours(const std::set<CPos> &n) override {
        m_neighbours = n;
    }

private:
    CPos m_val;
    std::set<CPos> m_neighbours;
};

void CBuilder::opAdd() {
    AExpr rhs = m_expressions.top();
    m_expressions.pop();
    AExpr lhs = m_expressions.top();
    m_expressions.pop();
    m_expressions.push(std::make_shared<CAddition>(std::move(lhs), std::move(rhs), *this));
}

void CBuilder::opSub() {
    AExpr rhs = m_expressions.top();
    m_expressions.pop();
    AExpr lhs = m_expressions.top();
    m_expressions.pop();
    m_expressions.push(std::make_shared<CSubtraction>(std::move(lhs), std::move(rhs), *this));
}

void CBuilder::opMul() {
    AExpr rhs = m_expressions.top();
    m_expressions.pop();
    AExpr lhs = m_expressions.top();
    m_expressions.pop();
    m_expressions.push(std::make_shared<CMultiplication>(std::move(lhs), std::move(rhs), *this));
}

void CBuilder::opDiv() {
    AExpr rhs = m_expressions.top();
    m_expressions.pop();
    AExpr lhs = m_expressions.top();
    m_expressions.pop();
    m_expressions.push(std::make_shared<CDivision>(std::move(lhs), std::move(rhs), *this));
}

void CBuilder::opPow() {
    AExpr rhs = m_expressions.top();
    m_expressions.pop();
    AExpr lhs = m_expressions.top();
    m_expressions.pop();
    m_expressions.push(std::make_shared<CPower>(std::move(lhs), std::move(rhs), *this));
}

void CBuilder::opNeg() {
    AExpr number = m_expressions.top();
    m_expressions.pop();
    m_expressions.push(std::make_shared<CNegation>(number, *this));
}

void CBuilder::opEq() {
    AExpr rhs = m_expressions.top();
    m_expressions.pop();
    AExpr lhs = m_expressions.top();
    m_expressions.pop();
    m_expressions.push(std::make_shared<CEqual>(std::move(lhs), std::move(rhs), *this));
}

void CBuilder::opNe() {
    AExpr rhs = m_expressions.top();
    m_expressions.pop();
    AExpr lhs = m_expressions.top();
    m_expressions.pop();
    m_expressions.push(std::make_shared<CNEqual>(std::move(lhs), std::move(rhs), *this));
}

void CBuilder::opLt() {
    AExpr rhs = m_expressions.top();
    m_expressions.pop();
    AExpr lhs = m_expressions.top();
    m_expressions.pop();
    m_expressions.push(std::make_shared<CSmaller>(std::move(lhs), std::move(rhs), *this));
}

void CBuilder::opLe() {
    AExpr rhs = m_expressions.top();
    m_expressions.pop();
    AExpr lhs = m_expressions.top();
    m_expressions.pop();
    m_expressions.push(std::make_shared<CSmallerEqual>(std::move(lhs), std::move(rhs), *this));
}

void CBuilder::opGt() {
    AExpr rhs = m_expressions.top();
    m_expressions.pop();
    AExpr lhs = m_expressions.top();
    m_expressions.pop();
    m_expressions.push(std::make_shared<CGreater>(std::move(lhs), std::move(rhs), *this));
}

void CBuilder::opGe() {
    AExpr rhs = m_expressions.top();
    m_expressions.pop();
    AExpr lhs = m_expressions.top();
    m_expressions.pop();
    m_expressions.push(std::make_shared<CGreaterEqual>(std::move(lhs), std::move(rhs), *this));
}

void CBuilder::valNumber(double val) {
    m_expressions.push(std::make_shared<CLiteral>(val, *this));
}

void CBuilder::valString(std::string val) {
    m_expressions.push(std::make_shared<CStringLiteral>(val, *this));
}

void CBuilder::valReference(std::string val) {
    CPos pos(val);
    m_expressions.push(std::make_shared<CRef>(pos += m_overhead, *this));
    m_nodes.insert(pos);
}

void CBuilder::valRange(std::string val) {

}

void CBuilder::funcCall(std::string fnName, int paramCount) {

}

CValue CBuilder::eval() {
    AExpr expr = m_expressions.top();
    m_expressions.pop();
    return expr->eval();
}

void CBuilder::setOverhead(const CPos &pos) {
    m_overhead = pos;
}

// Class representing FITExcel spreadsheet
class CSpreadsheet {
public:
    // Flags for progtest testing
    static unsigned capabilities() {
        return SPREADSHEET_CYCLIC_DEPS | SPREADSHEET_FILE_IO;
    }

    // Constructor
    CSpreadsheet()
            : m_builder(m_table) {
    }

    /**
     * Copy constructor
     * @param other - other spreadsheet
     */
    CSpreadsheet(const CSpreadsheet &other);

    /**
     * Operator=
     * @param other - other spreadsheet
     * @return this spreadsheet
     */
    CSpreadsheet &operator=(const CSpreadsheet &other);

    // Destructor
    ~CSpreadsheet() = default;

    /**
     * Loads new CSpreadsheet object from stream
     * @param is - input stream
     * @return true - if loading done without an error,
     * @return false - if the input was in wrong format (and the original spreadsheet left without any changes)
     */
    bool load(std::istream &is);

    /**
     * Saves the data from spreadsheet and writes them into a stream
     * @param os - output stream
     * @return true - all data saved without any issues
     * @return false - stream corrupted, saving failed
     */
    bool save(std::ostream &os) const;

    /**
     * Set value to a cell in the spreadsheet
     * @param pos - cell coordinates CPos
     * @param contents - content to be set to the cell
     * @return true - set without an error
     * @return false - contents were in the wrong format
     */
    bool setCell(const CPos &pos, const std::string &contents);

    /**
     * Gets value of content in the cell
     * @param pos - cells coordinated CPos
     * @return evaluated expression in cell CValue
     */
    CValue getValue(const CPos &pos) const;

    /**
     * Cpoies rectangle of cells beginning from src and the area is going by the width to the right
     * and by the heigth down. This marked rectangle is likewise copied on the beginning cell - dst.
     * @param dst - destination cell
     * @param src - source cell
     * @param w - width of rectangle
     * @param h - heigth of rectangle
     */
    void copyRect(const CPos &dst, const CPos &src, int w = 1, int h = 1);

    /**
     * Checks weather is in the AST, cycle made of references to cells
     * for example: A0 = A1 + A3, A1 = A0, A3 = A2, if called on A0 -> cycle(A0->A1->A0)
     * @param current - currently processed cell reference CPos
     * @param visited - set of visited cells on current branch of AST
     * @param builder
     * @return true - if has cyclic reference
     * @return false - if there is no cycle
     */
    bool hasCycleDFS(const CPos &current, std::set<CPos> &visited, CBuilder &builder) const ;

private:
    std::map<CPos, AExpr> m_table; // spreadsheet
    CBuilder m_builder; // Cell evaluator
    std::hash<std::string> m_hash; // hash maker for data save/load

    static std::string readContent(std::istream &is, size_t &charsToRead);

    static bool loadFromStream(std::istream &is, std::istringstream& istr, size_t &charsToRead, CSpreadsheet& tmp, size_t& cells, std::string& toHash);
};


CSpreadsheet::CSpreadsheet(const CSpreadsheet &other)
        : m_builder(CBuilder(m_table)) {
    for (const auto &x: other.m_table) {
        std::string expr = x.second->getExpr();
        auto nodes = x.second->getNeighbours();
        m_table[x.first] = x.second->clone(m_builder, expr);
        m_table[x.first]->setExpr(expr);
        m_table[x.first]->setNeighbours(nodes);
    }
}

CSpreadsheet &CSpreadsheet::operator=(const CSpreadsheet &other) {
    if (this == &other)
        return *this;
    m_table.clear();

    m_builder = CBuilder(m_table);
    for (const auto &x: other.m_table) {
        std::string expr = x.second->getExpr();
        auto nodes = x.second->getNeighbours();
        m_table[x.first] = x.second->clone(m_builder, expr);
        m_table[x.first]->setExpr(expr);
        m_table[x.first]->setNeighbours(nodes);
    }
    return *this;
}

std::string CSpreadsheet::readContent(std::istream &is, size_t &charsToRead) {
    std::string expr, input;
    while (std::getline(is, input)) {
        if (input.empty() && !charsToRead)
            break;

        expr += input;
        charsToRead -= input.length();

        if (charsToRead <= 0)
            break;

        expr += '\n';
        charsToRead--;
    }
    return expr;
}

bool CSpreadsheet::loadFromStream(std::istream &is, std::istringstream& istr, size_t &charsToRead, CSpreadsheet& tmp, size_t& cells, std::string& toHash){
    int coll, row;

    if (!(istr >> coll >> row)) // read pos
        return false;

    CPos pos("A0");
    try {
        pos = CPos(coll, row);
    } catch (const std::invalid_argument &e) {
        return false;
    }
    if (!(istr >> charsToRead)) // read expr length
        return false;

    size_t getChars = charsToRead;
    std::string expr = readContent(is, charsToRead);

    if (charsToRead != 0)
        return false;
    std::string getPos = std::to_string(pos.getRow());
    getPos += " " + std::to_string(pos.getColl());
    toHash += getPos += " " + std::to_string(getChars) + " " + expr + " ";

    if (!tmp.setCell(pos, expr))
        return false;
    cells--;
    return true;
}

bool CSpreadsheet::load(std::istream &is) {
    if (!is || is.fail())
        return false;
    std::string input;
    CSpreadsheet tmp;
    size_t charsToRead = 0;
    size_t hashRetrieved = 0;
    size_t cells = 0;
    std::string toHash;
    size_t finHash = 0;

    std::getline(is, input);
    std::istringstream ist(input);
    if (!(ist >> hashRetrieved >> cells)) // read hash
        return false;
    if (!cells)
        return false;

    while (std::getline(is, input)) {
        std::istringstream istr(input);
        if (!cells) {
            if (!(istr >> finHash))
                return false;
            break;
        }
        if(!loadFromStream(is, istr, charsToRead, tmp, cells, toHash))
            return false;
    }
    if ((finHash != hashRetrieved || m_hash(toHash) != hashRetrieved) || cells != 0)
        return false;
    *this = tmp;
    return true;
}


bool CSpreadsheet::save(std::ostream &os) const {
    size_t sum = 0, cells = m_table.size();
    std::string toHash;
    for (const auto &elem: m_table) {
        std::string pos = std::to_string(elem.first.getRow()) + " " + std::to_string(elem.first.getColl());
        toHash += pos + " " + std::to_string(elem.second->getExpr().length()) + " " + elem.second->getExpr() + " ";

    }
    sum = m_hash(toHash);
    if (!(os << sum << " " << cells << std::endl))
        return false;
    for (const auto &elem: m_table) {
        if (!(os << elem.first)) // "CPos "
            return false;
        if (!(os << elem.second->getExpr().length() << std::endl))
            return false;
        if (!(os << elem.second->getExpr() << std::endl))
            return false;
    }
    if (!(os << sum << std::endl))
        return false;
    return true;
}

bool CSpreadsheet::setCell(const CPos &pos, const std::string &contents) {
    try {
        parseExpression(contents, m_builder);
    } catch (const std::invalid_argument &e) {
        return false;
    }
    m_table[pos] = m_builder.getTopStack();
    m_table[pos]->setNeighbours(m_builder.getNodes());
    m_builder.clearNodes();
    m_table[pos]->setExpr(m_table[pos]->setExprEval(true));
    return true;
}

CValue CSpreadsheet::getValue(const CPos &pos) const {
    auto itMap = m_table.find(pos);
    if (itMap == m_table.end()) {
        return CValue();
    }

    std::set<CPos> found;
    CBuilder b = m_builder;
    if (hasCycleDFS(itMap->first, found, b))
        return CValue();

    return itMap->second->eval();
}

void CSpreadsheet::copyRect(const CPos &dst, const CPos &src, int w, int h) {
    std::map<CPos, AExpr> cpyTable;

    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {
            CPos overhead(i, j);
            CPos srcCell = src + overhead;
            auto itSrc = m_table.find(srcCell);
            if (itSrc != m_table.end() && itSrc->first.getColl() == srcCell.getColl() &&
                itSrc->first.getRow() == srcCell.getRow()) {
                std::string expr1 = itSrc->second->getExpr();
                cpyTable.insert({itSrc->first, itSrc->second->clone(m_builder, itSrc->second->getExpr())});
                cpyTable[itSrc->first]->setExpr(expr1);
            }
        }
    }
    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {
            CPos overhead(i, j);
            CPos srcCell = src + overhead;
            CPos destCell = dst + overhead;
            auto itSrc = cpyTable.find(srcCell);
            if (itSrc != m_table.end() && itSrc->first.getColl() == srcCell.getColl() &&
                itSrc->first.getRow() == srcCell.getRow()) {
                std::string expr = itSrc->second->getExpr();
                CPos oh(destCell.getColl() - srcCell.getColl(), destCell.getRow() - srcCell.getRow());
                m_builder.setOverhead(oh);
                setCell(destCell, expr);
                m_table[destCell]->setExpr(m_table[destCell]->setExprEval(true));
            }
            else {
                m_table.erase(destCell);
            }
        }
    }
    m_builder.setOverhead({0, 0});
}

bool CSpreadsheet::hasCycleDFS(const CPos &current, std::set<CPos> &visited, CBuilder &builder) const {
    if (visited.contains(current)) {
        return true;
    }

    visited.insert(current);

    auto itMap = m_table.find(current);
    if (itMap == m_table.end())
        return false;

    for (const auto &neighbour: itMap->second->getNeighbours()) {
        std::set<CPos> tmp(visited);
        if (hasCycleDFS(neighbour, tmp, builder)) {
            return true;
        }
    }
    return false;
}

#ifndef __PROGTEST__

bool valueMatch(const CValue &r, const CValue &s) {
    if (r.index() != s.index())
        return false;
    if (r.index() == 0)
        return true;
    if (r.index() == 2)
        return std::get<std::string>(r) == std::get<std::string>(s);
    if (std::isnan(std::get<double>(r)) && std::isnan(std::get<double>(s)))
        return true;
    if (std::isinf(std::get<double>(r)) && std::isinf(std::get<double>(s)))
        return (std::get<double>(r) < 0 && std::get<double>(s) < 0) ||
               (std::get<double>(r) > 0 && std::get<double>(s) > 0);
    return fabs(std::get<double>(r) - std::get<double>(s)) <= 1e8 * DBL_EPSILON * fabs(std::get<double>(r));
}

int main() {

    CSpreadsheet x0, x1;
    std::ostringstream oss;
    std::istringstream iss;
    std::string data;

//    x0.setCell(CPos("A1"),
//               "\n\t\tD7wEcVFQ6}?Hrkg+#:ESR!2xQmrA&m1!ZkGB47;EnZKE?CA/_,]eaAaeTJ2Ne?_3[94-ftypw8pM?1HbF1@&N2+uj!KR#A.k8iHAgyh+xhT$}8LqFm-PKMzgVbb@NhS7vQ_L,hcn5$4reXYaSM_MH?WjyuPzJ,Y{k*ULuJ@v=VykpJT2!U):(rXUE)fDQCx@8LJqG54u%Zh!,M1u#$e)izz/A6k[.{j#zSz7/{Hm45p:B)[C&av(4WXf(9(!F8}B{T*8V4krg/6d(HqJG69X!Myd&4JNFZ}Q7Mip1PHN-5H5z({&nKU[iet8#qMaFaG{hE]]SwdwSWgZ_RmUWr[$z:a7-m){7x*yrYRg@@&56UJ56-$xi)!LU2H?9R52w2LNT#QyYA;WN4KXMy#!eF(P3NdB:n5$J}ce5V(x2FVJ4pf(fc$#_gZ/b,PqTeU9+Qg;#d_GnAD?P0L-jzh0,)?$TKw#{EPU{,jm+Kgbu(2E$Ja7h}EuK8-CnAB;2Q?V#Mn;Un1G@,1cw]8[7Z=}GQLv$U3-c.j&)H]y-dUrHMgERe]Wp@B{KL;E@1592A}5.@JvKN![A0&SX/CEVR+T5[Ub!3c]FC8TW#dngJq1+QMSb4Z_G0_.Yf##JKiy:w@$?/BDi58KVVjM6V$Wf)Q(}Z@6gXW}Y*V9BLC@zKH}jEn@!$j&)e468@jkt;PF+,D#iV6yX}Lctegn9h:Q[TR[4pY7#Xdg)bEeJJdTXPSiTP@Dym]x%LpRHS]3iQh6NB;hU?CH}fbj-#a71}ZRa7?4]@Ak=F,4y+nB3Mb7B}1}xCVnE0g$*K8S@zLf{_Y&d{z?8UmNz;y,-=a%E(1q65pEA9DY]iez,w@AkiMF9;6BTfGd0JC?6J$%jS{uCcSND_MPU8d*L;VdeHB?,;wvB2Eq,Z_D8LKuj_%dD0AHpR{YDa/FxjtJi?RKgS}r4*ZUuqxS,F5_BXg?Qti6,Y=Y=]]9LkiXTvF)6GQ=FALtE2A%Yxn\n\"A{2YA=!BTXyP_)67y3Cu$!;t]N?v/HyN%&pC&aRR#MwLkg=#*iCLPL46F-SAzaT(RRjv$CqJ!dumRn=@TF?nacw]NGzmR2K7gKP9eA]UEmYE+Z)x=T:3}NQr?/)%&,q_1iNjcCY[3wv}#5_TFxUnaTWPE3Y![WAF3r5L1Fn9NU48/p6tFDtH;-m16;E8e(-T+-#(JJCquaFYj53S35b,2M]SN3K6ia6$t]]LYGmhmMD/bC}1h;V_:L&92C+35A3*Q2Ak5;]ka,3!)PD-rULqH,Vq2hf-;L=B}.@+nz-;1ZD.YPjY+Nn){1H1@xp@2y!iaUt%DJ#ehZ9Jf8h2*megCD_TN$nJH6/=a*wG#hE7mvx]JD@)38H?-kJi)%Lg;K!1,xwvE(u%[Q{R+/+q*HQ716@Vv?iP:@r8u@LCwgf_p()h!))umA;mSJy8xi&f3?jN5$=+7K?1XMPpjrHWDJgr)qN$ZwBuG3T$ZUw:by..A}rH1/EFWe)YL-WtFcgZYyrpw,UX1,L58GqYyB2V83X0UQH*MnSPkMM8J,{Y{%?-n./;0yhzv*g?ug_#={Zm.{?+dFuiB{YgGUp_WrX#NgM}HG;m+vb%+;Z:=te&QdWzxzurAt*M,+uzVt%?1L-N,-!na3NHFNZ.vD(=Gm=jrXjPui94yR,]:ZSFWb36ZBEw*B&0qT8qY$kUA&!p]@F,M2td7eF6+_9qdq2Hq2Yu&0W;_;Lj6hx,)_rQxi_0N87!*kWAUX;R0x4yeJZSN29VHpK}XMEMw;P.Pp}SQJ79#=p@ia6{z$44LFC}{t:3B{Z7Z$@{%9FuNEpa5p7W,AMGvdkT@MCF(qaacT(n6VBeg3}/DAW?Cu!AUD&PB4k@]%&T(N6B2HS&Z.P*x1m[J+HS}@{n:W0Sqenj%DU{Sj&ru[,3E?y_pbB!F#(P$x%}c2%uBFL_ceAKq);?G:U9XgTc+DA$ib35@=   (72GdS)mqPMw!=,%;qWMq6yC_PW-L2e=uZ\n");
//    assert (valueMatch(x0.getValue(CPos("A1")),
//                       CValue("\n\t\tD7wEcVFQ6}?Hrkg+#:ESR!2xQmrA&m1!ZkGB47;EnZKE?CA/_,]eaAaeTJ2Ne?_3[94-ftypw8pM?1HbF1@&N2+uj!KR#A.k8iHAgyh+xhT$}8LqFm-PKMzgVbb@NhS7vQ_L,hcn5$4reXYaSM_MH?WjyuPzJ,Y{k*ULuJ@v=VykpJT2!U):(rXUE)fDQCx@8LJqG54u%Zh!,M1u#$e)izz/A6k[.{j#zSz7/{Hm45p:B)[C&av(4WXf(9(!F8}B{T*8V4krg/6d(HqJG69X!Myd&4JNFZ}Q7Mip1PHN-5H5z({&nKU[iet8#qMaFaG{hE]]SwdwSWgZ_RmUWr[$z:a7-m){7x*yrYRg@@&56UJ56-$xi)!LU2H?9R52w2LNT#QyYA;WN4KXMy#!eF(P3NdB:n5$J}ce5V(x2FVJ4pf(fc$#_gZ/b,PqTeU9+Qg;#d_GnAD?P0L-jzh0,)?$TKw#{EPU{,jm+Kgbu(2E$Ja7h}EuK8-CnAB;2Q?V#Mn;Un1G@,1cw]8[7Z=}GQLv$U3-c.j&)H]y-dUrHMgERe]Wp@B{KL;E@1592A}5.@JvKN![A0&SX/CEVR+T5[Ub!3c]FC8TW#dngJq1+QMSb4Z_G0_.Yf##JKiy:w@$?/BDi58KVVjM6V$Wf)Q(}Z@6gXW}Y*V9BLC@zKH}jEn@!$j&)e468@jkt;PF+,D#iV6yX}Lctegn9h:Q[TR[4pY7#Xdg)bEeJJdTXPSiTP@Dym]x%LpRHS]3iQh6NB;hU?CH}fbj-#a71}ZRa7?4]@Ak=F,4y+nB3Mb7B}1}xCVnE0g$*K8S@zLf{_Y&d{z?8UmNz;y,-=a%E(1q65pEA9DY]iez,w@AkiMF9;6BTfGd0JC?6J$%jS{uCcSND_MPU8d*L;VdeHB?,;wvB2Eq,Z_D8LKuj_%dD0AHpR{YDa/FxjtJi?RKgS}r4*ZUuqxS,F5_BXg?Qti6,Y=Y=]]9LkiXTvF)6GQ=FALtE2A%Yxn\n\"A{2YA=!BTXyP_)67y3Cu$!;t]N?v/HyN%&pC&aRR#MwLkg=#*iCLPL46F-SAzaT(RRjv$CqJ!dumRn=@TF?nacw]NGzmR2K7gKP9eA]UEmYE+Z)x=T:3}NQr?/)%&,q_1iNjcCY[3wv}#5_TFxUnaTWPE3Y![WAF3r5L1Fn9NU48/p6tFDtH;-m16;E8e(-T+-#(JJCquaFYj53S35b,2M]SN3K6ia6$t]]LYGmhmMD/bC}1h;V_:L&92C+35A3*Q2Ak5;]ka,3!)PD-rULqH,Vq2hf-;L=B}.@+nz-;1ZD.YPjY+Nn){1H1@xp@2y!iaUt%DJ#ehZ9Jf8h2*megCD_TN$nJH6/=a*wG#hE7mvx]JD@)38H?-kJi)%Lg;K!1,xwvE(u%[Q{R+/+q*HQ716@Vv?iP:@r8u@LCwgf_p()h!))umA;mSJy8xi&f3?jN5$=+7K?1XMPpjrHWDJgr)qN$ZwBuG3T$ZUw:by..A}rH1/EFWe)YL-WtFcgZYyrpw,UX1,L58GqYyB2V83X0UQH*MnSPkMM8J,{Y{%?-n./;0yhzv*g?ug_#={Zm.{?+dFuiB{YgGUp_WrX#NgM}HG;m+vb%+;Z:=te&QdWzxzurAt*M,+uzVt%?1L-N,-!na3NHFNZ.vD(=Gm=jrXjPui94yR,]:ZSFWb36ZBEw*B&0qT8qY$kUA&!p]@F,M2td7eF6+_9qdq2Hq2Yu&0W;_;Lj6hx,)_rQxi_0N87!*kWAUX;R0x4yeJZSN29VHpK}XMEMw;P.Pp}SQJ79#=p@ia6{z$44LFC}{t:3B{Z7Z$@{%9FuNEpa5p7W,AMGvdkT@MCF(qaacT(n6VBeg3}/DAW?Cu!AUD&PB4k@]%&T(N6B2HS&Z.P*x1m[J+HS}@{n:W0Sqenj%DU{Sj&ru[,3E?y_pbB!F#(P$x%}c2%uBFL_ceAKq);?G:U9XgTc+DA$ib35@=   (72GdS)mqPMw!=,%;qWMq6yC_PW-L2e=uZ\n")));
//
//    oss.clear();
//    oss.str("");
//    assert (x0.save(oss));
//    data = oss.str();
//    iss.clear();
//    iss.str(data);
//    assert (x1.load(iss));
//    assert (valueMatch(x1.getValue(CPos("A1")),
//                       CValue("\n\t\tD7wEcVFQ6}?Hrkg+#:ESR!2xQmrA&m1!ZkGB47;EnZKE?CA/_,]eaAaeTJ2Ne?_3[94-ftypw8pM?1HbF1@&N2+uj!KR#A.k8iHAgyh+xhT$}8LqFm-PKMzgVbb@NhS7vQ_L,hcn5$4reXYaSM_MH?WjyuPzJ,Y{k*ULuJ@v=VykpJT2!U):(rXUE)fDQCx@8LJqG54u%Zh!,M1u#$e)izz/A6k[.{j#zSz7/{Hm45p:B)[C&av(4WXf(9(!F8}B{T*8V4krg/6d(HqJG69X!Myd&4JNFZ}Q7Mip1PHN-5H5z({&nKU[iet8#qMaFaG{hE]]SwdwSWgZ_RmUWr[$z:a7-m){7x*yrYRg@@&56UJ56-$xi)!LU2H?9R52w2LNT#QyYA;WN4KXMy#!eF(P3NdB:n5$J}ce5V(x2FVJ4pf(fc$#_gZ/b,PqTeU9+Qg;#d_GnAD?P0L-jzh0,)?$TKw#{EPU{,jm+Kgbu(2E$Ja7h}EuK8-CnAB;2Q?V#Mn;Un1G@,1cw]8[7Z=}GQLv$U3-c.j&)H]y-dUrHMgERe]Wp@B{KL;E@1592A}5.@JvKN![A0&SX/CEVR+T5[Ub!3c]FC8TW#dngJq1+QMSb4Z_G0_.Yf##JKiy:w@$?/BDi58KVVjM6V$Wf)Q(}Z@6gXW}Y*V9BLC@zKH}jEn@!$j&)e468@jkt;PF+,D#iV6yX}Lctegn9h:Q[TR[4pY7#Xdg)bEeJJdTXPSiTP@Dym]x%LpRHS]3iQh6NB;hU?CH}fbj-#a71}ZRa7?4]@Ak=F,4y+nB3Mb7B}1}xCVnE0g$*K8S@zLf{_Y&d{z?8UmNz;y,-=a%E(1q65pEA9DY]iez,w@AkiMF9;6BTfGd0JC?6J$%jS{uCcSND_MPU8d*L;VdeHB?,;wvB2Eq,Z_D8LKuj_%dD0AHpR{YDa/FxjtJi?RKgS}r4*ZUuqxS,F5_BXg?Qti6,Y=Y=]]9LkiXTvF)6GQ=FALtE2A%Yxn\n\"A{2YA=!BTXyP_)67y3Cu$!;t]N?v/HyN%&pC&aRR#MwLkg=#*iCLPL46F-SAzaT(RRjv$CqJ!dumRn=@TF?nacw]NGzmR2K7gKP9eA]UEmYE+Z)x=T:3}NQr?/)%&,q_1iNjcCY[3wv}#5_TFxUnaTWPE3Y![WAF3r5L1Fn9NU48/p6tFDtH;-m16;E8e(-T+-#(JJCquaFYj53S35b,2M]SN3K6ia6$t]]LYGmhmMD/bC}1h;V_:L&92C+35A3*Q2Ak5;]ka,3!)PD-rULqH,Vq2hf-;L=B}.@+nz-;1ZD.YPjY+Nn){1H1@xp@2y!iaUt%DJ#ehZ9Jf8h2*megCD_TN$nJH6/=a*wG#hE7mvx]JD@)38H?-kJi)%Lg;K!1,xwvE(u%[Q{R+/+q*HQ716@Vv?iP:@r8u@LCwgf_p()h!))umA;mSJy8xi&f3?jN5$=+7K?1XMPpjrHWDJgr)qN$ZwBuG3T$ZUw:by..A}rH1/EFWe)YL-WtFcgZYyrpw,UX1,L58GqYyB2V83X0UQH*MnSPkMM8J,{Y{%?-n./;0yhzv*g?ug_#={Zm.{?+dFuiB{YgGUp_WrX#NgM}HG;m+vb%+;Z:=te&QdWzxzurAt*M,+uzVt%?1L-N,-!na3NHFNZ.vD(=Gm=jrXjPui94yR,]:ZSFWb36ZBEw*B&0qT8qY$kUA&!p]@F,M2td7eF6+_9qdq2Hq2Yu&0W;_;Lj6hx,)_rQxi_0N87!*kWAUX;R0x4yeJZSN29VHpK}XMEMw;P.Pp}SQJ79#=p@ia6{z$44LFC}{t:3B{Z7Z$@{%9FuNEpa5p7W,AMGvdkT@MCF(qaacT(n6VBeg3}/DAW?Cu!AUD&PB4k@]%&T(N6B2HS&Z.P*x1m[J+HS}@{n:W0Sqenj%DU{Sj&ru[,3E?y_pbB!F#(P$x%}c2%uBFL_ceAKq);?G:U9XgTc+DA$ib35@=   (72GdS)mqPMw!=,%;qWMq6yC_PW-L2e=uZ\n")));

//    for (int i = 0; i < 11; ++i) {
//        std::string st;
//        switch (i) {
//            case 0:
//                st = "AB12";
//                break;
//            case 1:
//                st = "$AB12";
//                break;
//            case 2:
//                st = "A$B12";
//                break;
//            case 3:
//                st = "AB$12";
//                break;
//            case 4:
//                st = "AB1$2";
//                break;
//            case 5:
//                st = "AB12$";
//                break;
//            case 6:
//                st = "$AB$12";
//                break;
//            case 7:
//                st = "$AB.$12";
//                break;
//            case 8:
//                st = "$AB$12.";
//                break;
//            case 9:
//                st = "$.A$B12";
//                break;
//            case 10:
//                st = "$A.B$12";
//                break;
//            default:
//                break;
//        }
//        bool one = false, two = false;
//        bool out = CPos::isValid(st, one, two);
//        std::cout << st << " " << out << " " << one << " " << two << std::endl;
//    }

    assert (x0.setCell(CPos("A1"), "10"));
    assert (x0.setCell(CPos("A2"), "20.5"));
    assert (x0.setCell(CPos("A3"), "3e1"));
    assert (x0.setCell(CPos("A4"), "=40"));
    assert (x0.setCell(CPos("A5"), "=5e+1"));

    assert(x0.setCell(CPos("C0"), "=A1 < A2"));
    assert(x0.setCell(CPos("C1"), "=A1 <> A2"));
    assert(x0.setCell(CPos("C2"), "=A1 = A1"));
    assert(x0.setCell(CPos("C3"), "=A1 > A2"));
    assert (x0.setCell(CPos("A8"), "lol xdd"));
    assert(x0.setCell(CPos("C4"), "=A1 > A8"));
    assert (x0.setCell(CPos("AA5"), "= A5 <= A5"));

    assert (valueMatch(x0.getValue(CPos("C0")), CValue(1.0)));
    assert (valueMatch(x0.getValue(CPos("C1")), CValue(1.0)));
    assert (valueMatch(x0.getValue(CPos("C2")), CValue(1.0)));
    assert (valueMatch(x0.getValue(CPos("C3")), CValue(0.0)));
    assert (valueMatch(x0.getValue(CPos("C4")), CValue()));
    assert (valueMatch(x0.getValue(CPos("AA5")), CValue(1.0)));


    assert (x0.setCell(CPos("A6"), "raw text with any characters, including a quote \" or a newline\n"));
    assert (x0.setCell(CPos("A7"),
                       "=\"quoted string, quotes must be doubled: \"\". Moreover, backslashes are needed for C++.\""));
    assert (valueMatch(x0.getValue(CPos("A1")), CValue(10.0)));
    assert (valueMatch(x0.getValue(CPos("A2")), CValue(20.5)));
    assert (valueMatch(x0.getValue(CPos("A3")), CValue(30.0)));
    assert (valueMatch(x0.getValue(CPos("A4")), CValue(40.0)));
    assert (valueMatch(x0.getValue(CPos("A5")), CValue(50.0)));
    assert (valueMatch(x0.getValue(CPos("A6")),
                       CValue("raw text with any characters, including a quote \" or a newline\n")));
    assert (valueMatch(x0.getValue(CPos("A7")),
                       CValue("quoted string, quotes must be doubled: \". Moreover, backslashes are needed for C++.")));
    assert (valueMatch(x0.getValue(CPos("AAAA9999")), CValue()));
    assert (x0.setCell(CPos("B1"), "=A1+A2*A3"));
    assert (x0.setCell(CPos("B2"), "= -A1 ^ 2 - A2 / 2   "));
    assert (x0.setCell(CPos("B3"), "= 2 ^ $A$1"));
    assert (x0.setCell(CPos("B4"), "=($A1+A$2)^2"));
    assert (x0.setCell(CPos("B5"), "=B1+B2+B3+B4"));
    assert (x0.setCell(CPos("B6"), "=B1+B2+B3+B4+B5"));
    assert (valueMatch(x0.getValue(CPos("B1")), CValue(625.0)));
    assert (valueMatch(x0.getValue(CPos("B2")), CValue(-110.25)));
    assert (valueMatch(x0.getValue(CPos("B3")), CValue(1024.0)));
    assert (valueMatch(x0.getValue(CPos("B4")), CValue(930.25)));
    assert (valueMatch(x0.getValue(CPos("B5")), CValue(2469.0)));
    assert (valueMatch(x0.getValue(CPos("B6")), CValue(4938.0)));
    assert (x0.setCell(CPos("A1"), "12"));
    assert (valueMatch(x0.getValue(CPos("B1")), CValue(627.0)));
    assert (valueMatch(x0.getValue(CPos("B2")), CValue(-154.25)));
    assert (valueMatch(x0.getValue(CPos("B3")), CValue(4096.0)));
    assert (valueMatch(x0.getValue(CPos("B4")), CValue(1056.25)));
    assert (valueMatch(x0.getValue(CPos("B5")), CValue(5625.0)));
    assert (valueMatch(x0.getValue(CPos("B6")), CValue(11250.0)));
    x1 = x0;
    assert (x0.setCell(CPos("A2"), "100"));
    assert (x1.setCell(CPos("A2"), "=A3+A5+A4"));

    assert (valueMatch(x1.getValue(CPos("A2")), CValue(120.0)));

    assert (valueMatch(x0.getValue(CPos("B1")), CValue(3012.0)));
    assert (valueMatch(x0.getValue(CPos("B2")), CValue(-194.0)));
    assert (valueMatch(x0.getValue(CPos("B3")), CValue(4096.0)));
    assert (valueMatch(x0.getValue(CPos("B4")), CValue(12544.0)));
    assert (valueMatch(x0.getValue(CPos("B5")), CValue(19458.0)));
    assert (valueMatch(x0.getValue(CPos("B6")), CValue(38916.0)));
    assert (valueMatch(x1.getValue(CPos("B1")), CValue(3612.0)));
    assert (valueMatch(x1.getValue(CPos("B2")), CValue(-204.0)));
    assert (valueMatch(x1.getValue(CPos("B3")), CValue(4096.0)));
    assert (valueMatch(x1.getValue(CPos("B4")), CValue(17424.0)));
    assert (valueMatch(x1.getValue(CPos("B5")), CValue(24928.0)));
    assert (valueMatch(x1.getValue(CPos("B6")), CValue(49856.0)));
    oss.clear();
    oss.str("");
    assert (x0.save(oss));
    data = oss.str();
    iss.clear();
    iss.str(data);
    assert (x1.load(iss));
    assert (valueMatch(x1.getValue(CPos("B1")), CValue(3012.0)));
    assert (valueMatch(x1.getValue(CPos("B2")), CValue(-194.0)));
    assert (valueMatch(x1.getValue(CPos("B3")), CValue(4096.0)));
    assert (valueMatch(x1.getValue(CPos("B4")), CValue(12544.0)));
    assert (valueMatch(x1.getValue(CPos("B5")), CValue(19458.0)));
    assert (valueMatch(x1.getValue(CPos("B6")), CValue(38916.0)));
    assert (x0.setCell(CPos("A3"), "4e1"));
    assert (valueMatch(x1.getValue(CPos("B1")), CValue(3012.0)));
    assert (valueMatch(x1.getValue(CPos("B2")), CValue(-194.0)));
    assert (valueMatch(x1.getValue(CPos("B3")), CValue(4096.0)));
    assert (valueMatch(x1.getValue(CPos("B4")), CValue(12544.0)));
    assert (valueMatch(x1.getValue(CPos("B5")), CValue(19458.0)));
    assert (valueMatch(x1.getValue(CPos("B6")), CValue(38916.0)));
    oss.clear();
    oss.str("");
    assert (x0.save(oss));
    data = oss.str();
    for (size_t i = 0; i < std::min<size_t>(data.length(), 10); i++)
        data[i] ^= 0x5a;
    iss.clear();
    iss.str(data);
    assert (!x1.load(iss));
    assert (x0.setCell(CPos("D0"), "10"));
    assert (x0.setCell(CPos("D1"), "20"));
    assert (x0.setCell(CPos("D2"), "30"));
    assert (x0.setCell(CPos("D3"), "40"));
    assert (x0.setCell(CPos("D4"), "50"));
    assert (x0.setCell(CPos("E0"), "60"));
    assert (x0.setCell(CPos("E1"), "70"));
    assert (x0.setCell(CPos("E2"), "80"));
    assert (x0.setCell(CPos("E3"), "90"));
    assert (x0.setCell(CPos("E4"), "100"));
    assert (x0.setCell(CPos("F10"), "=D0+5"));// 15
    assert (x0.setCell(CPos("F11"), "=$D0+5"));//$ 15
    assert (x0.setCell(CPos("F12"), "=D$0+5"));//$ 15
    assert (x0.setCell(CPos("F13"), "=$D$0+5"));//$ 15
    x0.copyRect(CPos("G11"), CPos("F10"), 1, 4);
    assert (valueMatch(x0.getValue(CPos("F10")), CValue(15.0)));
    assert (valueMatch(x0.getValue(CPos("F11")), CValue(15.0)));
    assert (valueMatch(x0.getValue(CPos("F12")), CValue(15.0)));
    assert (valueMatch(x0.getValue(CPos("F13")), CValue(15.0)));
    assert (valueMatch(x0.getValue(CPos("F14")), CValue()));
    assert (valueMatch(x0.getValue(CPos("G10")), CValue()));
    assert (valueMatch(x0.getValue(CPos("G11")), CValue(75.0)));
    assert (valueMatch(x0.getValue(CPos("G12")), CValue(25.0)));
    assert (valueMatch(x0.getValue(CPos("G13")), CValue(65.0)));
    assert (valueMatch(x0.getValue(CPos("G14")), CValue(15.0)));
    x0.copyRect(CPos("G11"), CPos("F10"), 2, 4);
    assert (valueMatch(x0.getValue(CPos("F10")), CValue(15.0)));
    assert (valueMatch(x0.getValue(CPos("F11")), CValue(15.0)));
    assert (valueMatch(x0.getValue(CPos("F12")), CValue(15.0)));
    assert (valueMatch(x0.getValue(CPos("F13")), CValue(15.0)));
    assert (valueMatch(x0.getValue(CPos("F14")), CValue()));
    assert (valueMatch(x0.getValue(CPos("G10")), CValue()));
    assert (valueMatch(x0.getValue(CPos("G11")), CValue(75.0)));
    assert (valueMatch(x0.getValue(CPos("G12")), CValue(25.0)));
    assert (valueMatch(x0.getValue(CPos("G13")), CValue(65.0)));
    assert (valueMatch(x0.getValue(CPos("G14")), CValue(15.0)));
    assert (valueMatch(x0.getValue(CPos("H10")), CValue()));
    assert (valueMatch(x0.getValue(CPos("H11")), CValue()));
    assert (valueMatch(x0.getValue(CPos("H12")), CValue()));
    assert (valueMatch(x0.getValue(CPos("H13")), CValue(35.0)));
    assert (valueMatch(x0.getValue(CPos("H14")), CValue()));
    assert (x0.setCell(CPos("F0"), "-27"));
    assert (valueMatch(x0.getValue(CPos("H14")), CValue(-22.0)));
    x0.copyRect(CPos("H12"), CPos("H13"), 1, 2);
    assert (valueMatch(x0.getValue(CPos("H12")), CValue(25.0)));
    assert (valueMatch(x0.getValue(CPos("H13")), CValue(-22.0)));
    assert (valueMatch(x0.getValue(CPos("H14")), CValue(-22.0)));

    oss.clear();
    oss.str("");
    assert (x0.save(oss));
    data = oss.str();
    iss.clear();
    iss.str(data);
    CSpreadsheet x2(x0);
    //assert (x2.load(iss));
    assert (valueMatch(x2.getValue(CPos("F10")), CValue(15.0)));
    assert (valueMatch(x2.getValue(CPos("F11")), CValue(15.0)));
    assert (valueMatch(x2.getValue(CPos("F12")), CValue(15.0)));
    assert (valueMatch(x2.getValue(CPos("F13")), CValue(15.0)));
    assert (valueMatch(x2.getValue(CPos("F14")), CValue()));
    assert (valueMatch(x2.getValue(CPos("G10")), CValue()));
    assert (valueMatch(x2.getValue(CPos("G11")), CValue(75.0)));
    assert (valueMatch(x2.getValue(CPos("G12")), CValue(25.0)));
    assert (valueMatch(x2.getValue(CPos("G13")), CValue(65.0)));
    assert (valueMatch(x2.getValue(CPos("G14")), CValue(15.0)));
    assert (valueMatch(x2.getValue(CPos("H10")), CValue()));
    assert (valueMatch(x2.getValue(CPos("H11")), CValue()));
    assert (valueMatch(x2.getValue(CPos("H12")), CValue(25.0)));
    assert (valueMatch(x2.getValue(CPos("H13")), CValue(-22.0)));
    assert (valueMatch(x2.getValue(CPos("H14")), CValue(-22.0)));

    CSpreadsheet x12;
    assert (x12.setCell(CPos("A1"), "10"));
    assert (x12.setCell(CPos("A2"), "20.5"));
    assert (x12.setCell(CPos("A3"), "3e1"));
    assert (x12.setCell(CPos("A4"), "=40"));
    assert (x12.setCell(CPos("A5"), "=B2"));
    assert (x12.setCell(CPos("B1"), "=A1+A2*A3"));
    assert (x12.setCell(CPos("B2"), "= -A1 ^ 2 - A5 / 2   "));
    assert (x12.setCell(CPos("B3"), "= 2 ^ $A$1"));
    assert (x12.setCell(CPos("B4"), "=($A1+A$2)^2"));
    assert (x12.setCell(CPos("B5"), "=B1+B2+B3+B4"));
    assert (x12.setCell(CPos("B6"), "=B1+B2+B3+B4+B5"));

    assert (valueMatch(x12.getValue(CPos("A1")), CValue(10.0)));
    assert (valueMatch(x12.getValue(CPos("A2")), CValue(20.5)));
    assert (valueMatch(x12.getValue(CPos("A3")), CValue(30.0)));
    assert (valueMatch(x12.getValue(CPos("A4")), CValue(40.0)));
    assert (valueMatch(x12.getValue(CPos("A5")), CValue()));
    assert (valueMatch(x12.getValue(CPos("B1")), CValue(625.0)));
    assert (valueMatch(x12.getValue(CPos("B2")), CValue()));
    assert (valueMatch(x12.getValue(CPos("B3")), CValue(1024.0)));
    assert (valueMatch(x12.getValue(CPos("B4")), CValue(930.25)));
    assert (valueMatch(x12.getValue(CPos("B5")), CValue()));
    assert (valueMatch(x12.getValue(CPos("B6")), CValue()));
    CSpreadsheet bb;
    oss.clear();
    oss.str("");
    assert (bb.save(oss));
    data = oss.str();
    iss.clear();
    iss.str(data);
    return EXIT_SUCCESS;
}

#endif /* __PROGTEST__ */
