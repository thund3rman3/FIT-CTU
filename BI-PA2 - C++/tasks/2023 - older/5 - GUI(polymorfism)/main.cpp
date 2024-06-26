#ifndef __PROGTEST__

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cmath>
#include <cassert>
#include <typeinfo>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <utility>
#include <vector>
#include <list>
#include <string>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <memory>
#include <algorithm>
#include <type_traits>

using namespace std;

class CRect
{
public:
    CRect(double x, double y, double w, double h)
            : m_X(x), m_Y(y), m_W(w), m_H(h)
    {
    }

    friend ostream &operator<<(ostream &os,
                               const CRect &x)
    {
        return os << '(' << x.m_X << ',' << x.m_Y << ',' << x.m_W << ',' << x.m_H << ')';
    }

    double m_X;
    double m_Y;
    double m_W;
    double m_H;
};

#endif /* __PROGTEST__ */


class CGUI
{
public:
    CGUI(int id, const CRect &relPos)
            : m_id(id), m_relPos(relPos)
    {
    }

    virtual ~CGUI() = default;

    virtual CGUI *clone() const = 0;

    void recalibrate(const CRect &pos)
    {
        m_absPosH = pos.m_H * m_relPos.m_H;
        m_absPosW = pos.m_W * m_relPos.m_W;
        m_absPosX = pos.m_W * m_relPos.m_X + pos.m_X;
        m_absPosY = pos.m_H * m_relPos.m_Y + pos.m_Y;
    }

    //virtual void print(ostream &os) const = 0;
    virtual void print(ostream &os, bool is_last, bool is_first) const = 0;

    friend ostream &operator<<(ostream &os, const CGUI &ui)
    {
        ui.print(os, true, true);
        return os;
    }

    int m_id;
protected:
    CRect m_relPos;
    double m_absPosX;
    double m_absPosY;
    double m_absPosW;
    double m_absPosH;
};

class CWindow
{
public:
    CWindow(int id, string title, const CRect &absPos)
            : m_id(id), m_title(move(title)), m_rect(absPos)
    { // set x,y,w,h
    }

    CWindow(const CWindow &win)
            : m_id(win.m_id), m_title(win.m_title), m_rect(win.m_rect)
    {
        for (const auto &x: win.m_vGUI)
        {
            m_vGUI.emplace_back(move(unique_ptr<CGUI>(x->clone())));
        }
    }

    CWindow &operator=(const CWindow &win)
    {
        if (&win == this)
            return *this;
        m_rect = win.m_rect;
        m_id = win.m_id;
        m_title = win.m_title;
        m_vGUI.clear();
        for (const auto &x: win.m_vGUI)
        {
            m_vGUI.emplace_back(move(unique_ptr<CGUI>(x->clone())));
        }
        return *this;
    }

    ~CWindow() = default;

    // add(x) -add UI x
    CWindow &add(const CGUI &ui)
    {
        CGUI *cloned = ui.clone();
        cloned->recalibrate(m_rect);
        m_vGUI.emplace_back(move(unique_ptr<CGUI>(cloned)));
        return *this;
    }

    // search(id) -vyhleda UI podle id(pokud neexustije return null)
    //    if there are more UIs with same idx returns referenec to first UI
    CGUI *search(int id) const
    {
        for (const auto &x: m_vGUI)
            if (id == x->m_id)
                return x.get();

        return nullptr;
    }

    // setPosition(absPos) -change sizeof window and pos of other UIs
    void setPosition(const CRect &absPos)
    {
        m_rect = absPos;
        for (auto &x: m_vGUI)
        {
            x->recalibrate(absPos);
        }
    }

    friend ostream &operator<<(ostream &os, const CWindow &win)
    {//"[0] Window \"Sample window\" (10,10,600,480)\n"
        os << "[" << win.m_id << "] Window \"" << win.m_title << "\" (" <<
           win.m_rect.m_X << "," << win.m_rect.m_Y << "," << win.m_rect.m_W <<
           "," << win.m_rect.m_H << ")\n";
        size_t idx = 0;
        for (const auto &ui: win.m_vGUI)
        {
            if (win.m_vGUI.size() - 1 == idx)
                ui->print(os, true, false);
            else
                ui->print(os, false, false);
            idx++;
        }
        return os;
    }

private:
    int m_id;
    string m_title;
    CRect m_rect;
    vector<unique_ptr<CGUI>> m_vGUI;
};

class CButton : public CGUI
{
public:
    CButton(int id, const CRect &relPos, string name)
            : CGUI(id, relPos), m_str(move(name))
    {
    }

    CGUI *clone() const override
    {
        return new CButton(*this);
    }

    void print(ostream &os, bool is_last, bool is_first) const override
    {//"+- [1] Button \"Ok\" (70,394,180,48)\n"
        if (!is_first)
            os << "+- ";
        os << "[" << this->m_id << "] Button \"" << this->m_str << "\" ("
           << this->m_absPosX << "," << this->m_absPosY << ","
           << this->m_absPosW << "," << this->m_absPosH << ")\n";
    }

private:
    string m_str;
};

class CInput : public CGUI // constructor, <<
{
public:
    CInput(int id, const CRect &relPos, string value)
            : CGUI(id, relPos), m_str(move(value))
    {
    }

    CGUI *clone() const override
    {
        return new CInput(*this);
    }

    // setValue(x) -set string to x
    void setValue(const string &x)
    {
        m_str = x;
    }

    // getValue -return string
    string getValue() const
    {
        return m_str;
    }

    void print(ostream &os, bool is_last = true, bool is_first = false) const override
    {// "+- [11] Input \"chucknorris\" (250,58,300,48)\n"
        if (!is_first)
            os << "+- ";
        os << "[" << this->m_id << "] Input \"" << this->m_str << "\" ("
           << this->m_absPosX << "," << this->m_absPosY << ","
           << this->m_absPosW << "," << this->m_absPosH << ")\n";
    }

private:
    string m_str;
};

class CLabel : public CGUI // constructor, <<
{
public:
    CLabel(int id, const CRect &relPos, string label)
            : CGUI(id, relPos), m_str(move(label))
    {
    }

    CGUI *clone() const override
    {
        return new CLabel(*this);
    }

    void print(ostream &os, bool is_last, bool is_first) const override
    {//"+- [10] Label \"Username:\" (70,58,120,48)\n"
        if (!is_first)
            os << "+- ";
        os << "[" << this->m_id << "] Label \"" << this->m_str << "\" ("
           << this->m_absPosX << "," << this->m_absPosY << ","
           << this->m_absPosW << "," << this->m_absPosH << ")\n";
    }

private:
    string m_str;
};

class CComboBox : public CGUI
{
public:
    CComboBox(int id, const CRect &relPos)
            : CGUI(id, relPos)
    {
    }

    CGUI *clone() const override
    {
        return new CComboBox(*this);
    }

    // add(x) -add string to array of nabidky
    CComboBox &add(const string &str)
    {
        vComboOpt.emplace_back(str);
        return *this;
    }

    // setSelected(x=0) - set nabidka with idx=x as chosen
    void setSelected(int id = 0)
    {
        selected = vComboOpt[id];
    }

    // getSelected() -get idx of selected nabidka
    size_t getSelected() const
    {
        if (selected.empty())
            return 0;
        size_t i = find(vComboOpt.begin(), vComboOpt.end(), selected) - vComboOpt.begin();
        return i;
    }

    void print(ostream &os, bool is_last, bool is_first) const override
    {
        //"+- [20] ComboBox (70,154,480,48)\n"
        //"   +->Karate<\n"
        if (!is_first)
            os << "+- ";
        os << "[" << this->m_id << "] ComboBox ("
           << this->m_absPosX << "," << this->m_absPosY << ","
           << this->m_absPosW << "," << this->m_absPosH << ")\n";
        size_t was_selected = getSelected();
        for (size_t idx = 0; idx < vComboOpt.size(); idx++)
        {
            if (is_last)
            {
                if (!is_first)
                    os << "   ";
                if (idx == was_selected)
                    os << "+->" << vComboOpt[idx] << "<\n";
                else
                    os << "+- " << vComboOpt[idx] << "\n";
            } else
            {
                if (idx == was_selected)
                    os << "|  +->" << vComboOpt[idx] << "<\n";
                else
                    os << "|  +- " << vComboOpt[idx] << "\n";
            }
        }
    }

private:
    vector<string> vComboOpt;
    string selected;
};

// output operators

#ifndef __PROGTEST__

template<typename _T>
string toString(const _T &x)
{
    ostringstream oss;
    oss << x;
    return oss.str();
}

int main()
{
    assert(sizeof(CButton) - sizeof(string) < sizeof(CComboBox) - sizeof(vector<string>));
    assert(sizeof(CInput) - sizeof(string) < sizeof(CComboBox) - sizeof(vector<string>));
    assert(sizeof(CLabel) - sizeof(string) < sizeof(CComboBox) - sizeof(vector<string>));
    CWindow a(0, "Sample window", CRect(10, 10, 600, 480));
    a.add(CButton(1, CRect(0.1, 0.8, 0.3, 0.1), "Ok")).add(CButton(2, CRect(0.6, 0.8, 0.3, 0.1), "Cancel"));
    a.add(CLabel(10, CRect(0.1, 0.1, 0.2, 0.1), "Username:"));
    a.add(CInput(11, CRect(0.4, 0.1, 0.5, 0.1), "chucknorris"));
    a.add(CComboBox(20, CRect(0.1, 0.3, 0.8, 0.1)).add("Karate").add("Judo").add("Box").add("Progtest"));
    string sss = toString(a);
    assert(toString(a) ==
           "[0] Window \"Sample window\" (10,10,600,480)\n"
           "+- [1] Button \"Ok\" (70,394,180,48)\n"
           "+- [2] Button \"Cancel\" (370,394,180,48)\n"
           "+- [10] Label \"Username:\" (70,58,120,48)\n"
           "+- [11] Input \"chucknorris\" (250,58,300,48)\n"
           "+- [20] ComboBox (70,154,480,48)\n"
           "   +->Karate<\n"
           "   +- Judo\n"
           "   +- Box\n"
           "   +- Progtest\n");
    CWindow b = a;
    string aaaa = toString(*b.search(20));
    assert(toString(*b.search(20)) ==
           "[20] ComboBox (70,154,480,48)\n"
           "+->Karate<\n"
           "+- Judo\n"
           "+- Box\n"
           "+- Progtest\n");
    assert(dynamic_cast<CComboBox &>(*b.search(20)).getSelected() == 0);
    dynamic_cast<CComboBox &>(*b.search(20)).setSelected(3);
    assert(dynamic_cast<CInput &>(*b.search(11)).getValue() == "chucknorris");
    dynamic_cast<CInput &>(*b.search(11)).setValue("chucknorris@fit.cvut.cz");
    b.add(CComboBox(21, CRect(0.1, 0.5, 0.8, 0.1)).add("PA2").add("OSY").add("Both"));
    string ass = toString(b);
    assert(toString(b) ==
           "[0] Window \"Sample window\" (10,10,600,480)\n"
           "+- [1] Button \"Ok\" (70,394,180,48)\n"
           "+- [2] Button \"Cancel\" (370,394,180,48)\n"
           "+- [10] Label \"Username:\" (70,58,120,48)\n"
           "+- [11] Input \"chucknorris@fit.cvut.cz\" (250,58,300,48)\n"
           "+- [20] ComboBox (70,154,480,48)\n"
           "|  +- Karate\n"
           "|  +- Judo\n"
           "|  +- Box\n"
           "|  +->Progtest<\n"
           "+- [21] ComboBox (70,250,480,48)\n"
           "   +->PA2<\n"
           "   +- OSY\n"
           "   +- Both\n");
    assert(toString(a) ==
           "[0] Window \"Sample window\" (10,10,600,480)\n"
           "+- [1] Button \"Ok\" (70,394,180,48)\n"
           "+- [2] Button \"Cancel\" (370,394,180,48)\n"
           "+- [10] Label \"Username:\" (70,58,120,48)\n"
           "+- [11] Input \"chucknorris\" (250,58,300,48)\n"
           "+- [20] ComboBox (70,154,480,48)\n"
           "   +->Karate<\n"
           "   +- Judo\n"
           "   +- Box\n"
           "   +- Progtest\n");
    b.setPosition(CRect(20, 30, 640, 520));
    assert(toString(b) ==
           "[0] Window \"Sample window\" (20,30,640,520)\n"
           "+- [1] Button \"Ok\" (84,446,192,52)\n"
           "+- [2] Button \"Cancel\" (404,446,192,52)\n"
           "+- [10] Label \"Username:\" (84,82,128,52)\n"
           "+- [11] Input \"chucknorris@fit.cvut.cz\" (276,82,320,52)\n"
           "+- [20] ComboBox (84,186,512,52)\n"
           "|  +- Karate\n"
           "|  +- Judo\n"
           "|  +- Box\n"
           "|  +->Progtest<\n"
           "+- [21] ComboBox (84,290,512,52)\n"
           "   +->PA2<\n"
           "   +- OSY\n"
           "   +- Both\n");
    return EXIT_SUCCESS;
}

#endif /* __PROGTEST__ */