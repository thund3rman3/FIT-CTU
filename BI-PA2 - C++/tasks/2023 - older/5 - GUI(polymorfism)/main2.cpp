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

class CRect {
public:
    CRect(double x, double y, double w, double h)
            : m_X(x), m_Y(y), m_W(w), m_H(h) {
    }

    friend ostream &operator<<(ostream &os,
                               const CRect &x) {
        return os << '(' << x.m_X << ',' << x.m_Y << ',' << x.m_W << ',' << x.m_H << ')';
    }

    double m_X;
    double m_Y;
    double m_W;
    double m_H;
};

#endif /* __PROGTEST__ */

class CGUI {
public:
    CGUI(int id, const CRect &relPos)
            : m_id(id), m_relPos(relPos), is_panel(false), is_combo(false) {
    }

    virtual ~CGUI() = default;

    virtual CGUI *clone() const = 0;

    virtual CGUI *seraching(int id) const {
        if (m_id == id)
            return const_cast<CGUI *>(this);
        return nullptr;
    }

    virtual void recalibrate(const CRect &pos) { // for window
        m_absPosH = pos.m_H * m_relPos.m_H;
        m_absPosW = pos.m_W * m_relPos.m_W;
        m_absPosX = pos.m_W * m_relPos.m_X + pos.m_X;
        m_absPosY = pos.m_H * m_relPos.m_Y + pos.m_Y;
    }

    virtual void recalibrate(const CGUI *ui) { // for others
        m_absPosH = ui->m_absPosH * m_relPos.m_H;
        m_absPosW = ui->m_absPosW * m_relPos.m_W;
        m_absPosX = ui->m_absPosW * m_relPos.m_X + ui->m_absPosX;
        m_absPosY = ui->m_absPosH * m_relPos.m_Y + ui->m_absPosY;
    }

    virtual void print(ostream &os, string prefix) const = 0;

    friend ostream &operator<<(ostream &os, const CGUI &ui) {
        ui.print(os, "");
        return os;
    }

    int m_id;
    CRect m_relPos;
    double m_absPosX;
    double m_absPosY;
    double m_absPosW;
    double m_absPosH;
    bool is_panel;
    bool is_combo;
};

class CWindow {
public:
    CWindow(int id, string title, const CRect &absPos)
            : m_id(id), m_title(move(title)), m_rect(absPos) { // set x,y,w,h
    }

    ~CWindow() = default;

    CWindow(const CWindow &win)
            : m_id(win.m_id), m_title(win.m_title), m_rect(win.m_rect) {
        for (const auto &x: win.m_vGUI) {
            m_vGUI.emplace_back(move(unique_ptr<CGUI>(x->clone())));
        }
    }

    CWindow &operator=(const CWindow &win) {
        if (&win == this)
            return *this;
        m_rect = win.m_rect;
        m_id = win.m_id;
        m_title = win.m_title;
        m_vGUI.clear();
        for (const auto &x: win.m_vGUI) {
            CGUI *cloned = x->clone();
            m_vGUI.emplace_back(move(unique_ptr<CGUI>(cloned)));
        }
        return *this;
    }

    // add(x) -add UI x
    CWindow &add(const CGUI &ui) {
        CGUI *cloned = ui.clone();
        cloned->recalibrate(m_rect);
        m_vGUI.emplace_back(move(unique_ptr<CGUI>(cloned)));
        return *this;
    }

    // search(id) -vyhleda UI podle id(pokud neexustije return null)
    //    if there are more UIs with same idx returns referenec to first UI
    CGUI *search(int id) const {
        for (const auto &x: m_vGUI) {
            CGUI *s = x->seraching(id);
            if (s != nullptr)
                return s;
        }
        return nullptr;
    }

    // setPosition(absPos) -change sizeof window and pos of other UIs
    void setPosition(const CRect &absPos) {
        m_rect = absPos;
        for (auto &x: m_vGUI) {
            x->recalibrate(m_rect);
        }
    }

    friend ostream &operator<<(ostream &os, const CWindow &win) {
        //"[0] Window \"Sample window\" (10,10,600,480)\n"
        os << "[" << win.m_id << "] Window \"" << win.m_title << "\" (" <<
           win.m_rect.m_X << "," << win.m_rect.m_Y << "," << win.m_rect.m_W <<
           "," << win.m_rect.m_H << ")\n";
        size_t idx = 0;
        for (const auto &ui: win.m_vGUI) {
            if (win.m_vGUI.size() - 1 == idx)
                if (ui->is_panel || ui->is_combo)
                    ui->print(os, "   ");
                else
                    ui->print(os, "+- ");
            else {
                if (ui->is_panel || ui->is_combo)
                    ui->print(os, "|  ");
                else
                    ui->print(os, "+- ");
            }
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

class CPanel : public CGUI {
public:
    CPanel(int id, const CRect &relPos)
            : CGUI(id, relPos) {
        is_panel = true;
    }

    CPanel(const CPanel &panel)
            : CGUI(panel.m_id, panel.m_relPos) {
        m_absPosX = panel.m_absPosX;
        m_absPosY = panel.m_absPosY;
        m_absPosW = panel.m_absPosW;
        m_absPosH = panel.m_absPosH;
        is_combo = false;
        is_panel = true;
        for (const auto &ui: panel.m_vPanelElem) {
            CGUI *cloned = ui->clone();
            cloned->recalibrate(this);
            m_vPanelElem.emplace_back(move(unique_ptr<CGUI>(cloned)));
        }
    }

    CGUI *seraching(int id) const override {
        if (m_id == id)
            return (CGUI *) this;
        for (const auto &x: m_vPanelElem) {
            if (id == x->m_id)
                return x.get();
        }
        return nullptr;
    }

    void recalibrate(const CRect &pos) override { // for window
        m_absPosH = pos.m_H * m_relPos.m_H;
        m_absPosW = pos.m_W * m_relPos.m_W;
        m_absPosX = pos.m_W * m_relPos.m_X + pos.m_X;
        m_absPosY = pos.m_H * m_relPos.m_Y + pos.m_Y;
        for (const auto &x: m_vPanelElem)
            x->recalibrate(this);

    }

    void recalibrate(const CGUI *ui) override { // for others
        m_absPosH = ui->m_absPosH * m_relPos.m_H;
        m_absPosW = ui->m_absPosW * m_relPos.m_W;
        m_absPosX = ui->m_absPosW * m_relPos.m_X + ui->m_absPosX;
        m_absPosY = ui->m_absPosH * m_relPos.m_Y + ui->m_absPosY;
        if (ui->is_panel) {
            for (const auto &x: m_vPanelElem)
                x->recalibrate(this);
        }
    }

    // add
    CPanel &add(const CGUI &ui) {
        CGUI *cloned = ui.clone();
        cloned->recalibrate(this);
        m_vPanelElem.emplace_back(move(unique_ptr<CGUI>(cloned)));
        return *this;
    }


    void print(ostream &os, string prefix) const override {
        //"+- [12] Panel (70,154,480,336)\n"
        size_t len = prefix.length();
        if (len > 2) {
            for (size_t i = 0; i < len - 3; ++i)
                os << prefix[i];
            os << "+- ";
        }

        os << "[" << m_id << "] Panel (" << m_absPosX << "," << m_absPosY <<
           "," << m_absPosW << "," << m_absPosH << ")\n";
        size_t idx = 0;
        for (const auto &x: m_vPanelElem) {
            string new_prefix = "";
            if (idx == m_vPanelElem.size() - 1) {
                if (!x->is_panel && !x->is_combo)
                    new_prefix = prefix + "+- ";
                else
                    new_prefix = prefix + "   ";
                x->print(os, new_prefix);
            } else {
                if (x->is_panel || x->is_combo)
                    new_prefix = prefix + "|  ";
                else
                    new_prefix = prefix + "+- ";
                x->print(os, new_prefix);
            }
            idx++;
        }
    }

    CGUI *clone() const override {
        return new CPanel(*this);
    }

private:
    vector<unique_ptr<CGUI>> m_vPanelElem;
};


class CButton : public CGUI {
public:
    CButton(int id, const CRect &relPos, string name)
            : CGUI(id, relPos), m_str(move(name)) {
    }

    CGUI *clone() const override {
        return new CButton(*this);
    }

    void print(ostream &os, string prefix) const override {
        //"+- [1] Button \"Ok\" (70,394,180,48)\n"
        os << prefix;
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
            : CGUI(id, relPos), m_str(move(value)) {
    }

    CGUI *clone() const override {
        return new CInput(*this);
    }

    // setValue(x) -set string to x
    void setValue(const string &x) {
        m_str = x;
    }

    // getValue -return string
    string getValue() const {
        return m_str;
    }

    void print(ostream &os, string prefix) const override {
        // "+- [11] Input \"chucknorris\" (250,58,300,48)\n"
        os << prefix;
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
            : CGUI(id, relPos), m_str(move(label)) {
    }


    CGUI *clone() const override {
        return new CLabel(*this);
    }

    void print(ostream &os, string prefix) const override {
        //"+- [10] Label \"Username:\" (70,58,120,48)\n"
        os << prefix;
        os << "[" << this->m_id << "] Label \"" << this->m_str << "\" ("
           << this->m_absPosX << "," << this->m_absPosY << ","
           << this->m_absPosW << "," << this->m_absPosH << ")\n";
    }

private:
    string m_str;
};

class CComboBox : public CGUI {
public:
    CComboBox(int id, const CRect &relPos)
            : CGUI(id, relPos) {
        is_combo = true;
    }

    CGUI *clone() const override {
        return new CComboBox(*this);
    }

    // add(x) -add string to array of nabidky
    CComboBox &add(const string &str) {
        vComboOpt.emplace_back(str);
        return *this;
    }

    // setSelected(x=0) - set nabidka with idx=x as chosen
    void setSelected(int id = 0) {
        selected = vComboOpt[id];
    }

    // getSelected() -get idx of selected nabidka
    size_t getSelected() const {
        if (selected.empty())
            return 0;
        size_t i = find(vComboOpt.begin(), vComboOpt.end(), selected) - vComboOpt.begin();
        return i;
    }

    void print(ostream &os, string prefix) const override {
        //"+- [20] ComboBox (70,154,480,48)\n"
        //"   +->Karate<\n"
        size_t len = prefix.length();

        if (len > 2) {
            for (size_t i = 0; i < len - 3; ++i)
                os << prefix[i];
            os << "+- ";
        }
        os << "[" << this->m_id << "] ComboBox ("
           << this->m_absPosX << "," << this->m_absPosY << ","
           << this->m_absPosW << "," << this->m_absPosH << ")\n";
        size_t was_selected = getSelected();
        for (size_t idx = 0; idx < vComboOpt.size(); idx++) {
            if (idx == was_selected)
                os << prefix << "+->" << vComboOpt[idx] << "<\n";
            else
                os << prefix << "+- " << vComboOpt[idx] << "\n";
        }
    }

private:
    vector<string> vComboOpt;
    string selected;
};

// output operators

#ifndef __PROGTEST__

template<typename T_>
string toString(const T_ &x) {
    ostringstream oss;
    oss << x;
    return oss.str();
}

int main() {
    assert (sizeof(CButton) - sizeof(string) < sizeof(CComboBox) - sizeof(vector<string>));
    assert (sizeof(CInput) - sizeof(string) < sizeof(CComboBox) - sizeof(vector<string>));
    assert (sizeof(CLabel) - sizeof(string) < sizeof(CComboBox) - sizeof(vector<string>));
    assert (sizeof(CButton) - sizeof(string) <= sizeof(CPanel) - sizeof(vector<void *>));
    assert (sizeof(CInput) - sizeof(string) <= sizeof(CPanel) - sizeof(vector<void *>));
    assert (sizeof(CLabel) - sizeof(string) <= sizeof(CPanel) - sizeof(vector<void *>));
    CWindow a(0, "Sample window", CRect(10, 10, 600, 480));
    a.add(CButton(1, CRect(0.1, 0.8, 0.3, 0.1), "Ok")).add(CButton(2, CRect(0.6, 0.8, 0.3, 0.1), "Cancel"));
    a.add(CLabel(10, CRect(0.1, 0.1, 0.2, 0.1), "Username:"));
    a.add(CInput(11, CRect(0.4, 0.1, 0.5, 0.1), "chucknorris"));
    a.add(CPanel(12, CRect(0.1, 0.3, 0.8, 0.7)).add(
            CComboBox(20, CRect(0.1, 0.3, 0.8, 0.1)).add("Karate").add("Judo").add("Box").add("Progtest")));
    string sss = toString(a);
    //CGUI *ss = a.search(12)->clone();

    assert (toString(a) ==
            "[0] Window \"Sample window\" (10,10,600,480)\n"
            "+- [1] Button \"Ok\" (70,394,180,48)\n"
            "+- [2] Button \"Cancel\" (370,394,180,48)\n"
            "+- [10] Label \"Username:\" (70,58,120,48)\n"
            "+- [11] Input \"chucknorris\" (250,58,300,48)\n"
            "+- [12] Panel (70,154,480,336)\n"
            "   +- [20] ComboBox (118,254.8,384,33.6)\n"
            "      +->Karate<\n"
            "      +- Judo\n"
            "      +- Box\n"
            "      +- Progtest\n");
    CWindow b = a;
    string gg = toString(*b.search(20));
    assert (toString(*b.search(20)) ==
            "[20] ComboBox (118,254.8,384,33.6)\n"
            "+->Karate<\n"
            "+- Judo\n"
            "+- Box\n"
            "+- Progtest\n");
    assert (dynamic_cast<CComboBox &> ( *b.search(20)).getSelected() == 0);
    dynamic_cast<CComboBox &> ( *b.search(20)).setSelected(3);
    assert (dynamic_cast<CInput &> ( *b.search(11)).getValue() == "chucknorris");
    dynamic_cast<CInput &> ( *b.search(11)).setValue("chucknorris@fit.cvut.cz");
    CPanel &p = dynamic_cast<CPanel &> ( *b.search(12));
    p.add(CComboBox(21, CRect(0.1, 0.5, 0.8, 0.1)).add("PA2").add("OSY").add("Both"));
    string dd = toString(b);

    assert (toString(b) ==
            "[0] Window \"Sample window\" (10,10,600,480)\n"
            "+- [1] Button \"Ok\" (70,394,180,48)\n"
            "+- [2] Button \"Cancel\" (370,394,180,48)\n"
            "+- [10] Label \"Username:\" (70,58,120,48)\n"
            "+- [11] Input \"chucknorris@fit.cvut.cz\" (250,58,300,48)\n"
            "+- [12] Panel (70,154,480,336)\n"
            "   +- [20] ComboBox (118,254.8,384,33.6)\n"
            "   |  +- Karate\n"
            "   |  +- Judo\n"
            "   |  +- Box\n"
            "   |  +->Progtest<\n"
            "   +- [21] ComboBox (118,322,384,33.6)\n"
            "      +->PA2<\n"
            "      +- OSY\n"
            "      +- Both\n");
    assert (toString(a) ==
            "[0] Window \"Sample window\" (10,10,600,480)\n"
            "+- [1] Button \"Ok\" (70,394,180,48)\n"
            "+- [2] Button \"Cancel\" (370,394,180,48)\n"
            "+- [10] Label \"Username:\" (70,58,120,48)\n"
            "+- [11] Input \"chucknorris\" (250,58,300,48)\n"
            "+- [12] Panel (70,154,480,336)\n"
            "   +- [20] ComboBox (118,254.8,384,33.6)\n"
            "      +->Karate<\n"
            "      +- Judo\n"
            "      +- Box\n"
            "      +- Progtest\n");
    string pp = toString(p);
    assert (toString(p) ==
            "[12] Panel (70,154,480,336)\n"
            "+- [20] ComboBox (118,254.8,384,33.6)\n"
            "|  +- Karate\n"
            "|  +- Judo\n"
            "|  +- Box\n"
            "|  +->Progtest<\n"
            "+- [21] ComboBox (118,322,384,33.6)\n"
            "   +->PA2<\n"
            "   +- OSY\n"
            "   +- Both\n");
    b.setPosition(CRect(20, 30, 640, 520));
    string rr = toString(b);

    assert (toString(b) ==
            "[0] Window \"Sample window\" (20,30,640,520)\n"
            "+- [1] Button \"Ok\" (84,446,192,52)\n"
            "+- [2] Button \"Cancel\" (404,446,192,52)\n"
            "+- [10] Label \"Username:\" (84,82,128,52)\n"
            "+- [11] Input \"chucknorris@fit.cvut.cz\" (276,82,320,52)\n"
            "+- [12] Panel (84,186,512,364)\n"
            "   +- [20] ComboBox (135.2,295.2,409.6,36.4)\n"
            "   |  +- Karate\n"
            "   |  +- Judo\n"
            "   |  +- Box\n"
            "   |  +->Progtest<\n"
            "   +- [21] ComboBox (135.2,368,409.6,36.4)\n"
            "      +->PA2<\n"
            "      +- OSY\n"
            "      +- Both\n");
    p.add(p);
    string hhh = toString(p);
    assert (toString(p) ==
            "[12] Panel (84,186,512,364)\n"
            "+- [20] ComboBox (135.2,295.2,409.6,36.4)\n"
            "|  +- Karate\n"
            "|  +- Judo\n"
            "|  +- Box\n"
            "|  +->Progtest<\n"
            "+- [21] ComboBox (135.2,368,409.6,36.4)\n"
            "|  +->PA2<\n"
            "|  +- OSY\n"
            "|  +- Both\n"
            "+- [12] Panel (135.2,295.2,409.6,254.8)\n"
            "   +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
            "   |  +- Karate\n"
            "   |  +- Judo\n"
            "   |  +- Box\n"
            "   |  +->Progtest<\n"
            "   +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
            "      +->PA2<\n"
            "      +- OSY\n"
            "      +- Both\n");
    p.add(p);
    string doublep = toString(p);
    assert (toString(p) ==
            "[12] Panel (84,186,512,364)\n"
            "+- [20] ComboBox (135.2,295.2,409.6,36.4)\n"
            "|  +- Karate\n"
            "|  +- Judo\n"
            "|  +- Box\n"
            "|  +->Progtest<\n"
            "+- [21] ComboBox (135.2,368,409.6,36.4)\n"
            "|  +->PA2<\n"
            "|  +- OSY\n"
            "|  +- Both\n"
            "+- [12] Panel (135.2,295.2,409.6,254.8)\n"
            "|  +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
            "|  |  +- Karate\n"
            "|  |  +- Judo\n"
            "|  |  +- Box\n"
            "|  |  +->Progtest<\n"
            "|  +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
            "|     +->PA2<\n"
            "|     +- OSY\n"
            "|     +- Both\n"
            "+- [12] Panel (135.2,295.2,409.6,254.8)\n"
            "   +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
            "   |  +- Karate\n"
            "   |  +- Judo\n"
            "   |  +- Box\n"
            "   |  +->Progtest<\n"
            "   +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
            "   |  +->PA2<\n"
            "   |  +- OSY\n"
            "   |  +- Both\n"
            "   +- [12] Panel (176.16,371.64,327.68,178.36)\n"
            "      +- [20] ComboBox (208.928,425.148,262.144,17.836)\n"
            "      |  +- Karate\n"
            "      |  +- Judo\n"
            "      |  +- Box\n"
            "      |  +->Progtest<\n"
            "      +- [21] ComboBox (208.928,460.82,262.144,17.836)\n"
            "         +->PA2<\n"
            "         +- OSY\n"
            "         +- Both\n");
    p.add(p);
    assert (toString(p) ==
            "[12] Panel (84,186,512,364)\n"
            "+- [20] ComboBox (135.2,295.2,409.6,36.4)\n"
            "|  +- Karate\n"
            "|  +- Judo\n"
            "|  +- Box\n"
            "|  +->Progtest<\n"
            "+- [21] ComboBox (135.2,368,409.6,36.4)\n"
            "|  +->PA2<\n"
            "|  +- OSY\n"
            "|  +- Both\n"
            "+- [12] Panel (135.2,295.2,409.6,254.8)\n"
            "|  +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
            "|  |  +- Karate\n"
            "|  |  +- Judo\n"
            "|  |  +- Box\n"
            "|  |  +->Progtest<\n"
            "|  +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
            "|     +->PA2<\n"
            "|     +- OSY\n"
            "|     +- Both\n"
            "+- [12] Panel (135.2,295.2,409.6,254.8)\n"
            "|  +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
            "|  |  +- Karate\n"
            "|  |  +- Judo\n"
            "|  |  +- Box\n"
            "|  |  +->Progtest<\n"
            "|  +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
            "|  |  +->PA2<\n"
            "|  |  +- OSY\n"
            "|  |  +- Both\n"
            "|  +- [12] Panel (176.16,371.64,327.68,178.36)\n"
            "|     +- [20] ComboBox (208.928,425.148,262.144,17.836)\n"
            "|     |  +- Karate\n"
            "|     |  +- Judo\n"
            "|     |  +- Box\n"
            "|     |  +->Progtest<\n"
            "|     +- [21] ComboBox (208.928,460.82,262.144,17.836)\n"
            "|        +->PA2<\n"
            "|        +- OSY\n"
            "|        +- Both\n"
            "+- [12] Panel (135.2,295.2,409.6,254.8)\n"
            "   +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
            "   |  +- Karate\n"
            "   |  +- Judo\n"
            "   |  +- Box\n"
            "   |  +->Progtest<\n"
            "   +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
            "   |  +->PA2<\n"
            "   |  +- OSY\n"
            "   |  +- Both\n"
            "   +- [12] Panel (176.16,371.64,327.68,178.36)\n"
            "   |  +- [20] ComboBox (208.928,425.148,262.144,17.836)\n"
            "   |  |  +- Karate\n"
            "   |  |  +- Judo\n"
            "   |  |  +- Box\n"
            "   |  |  +->Progtest<\n"
            "   |  +- [21] ComboBox (208.928,460.82,262.144,17.836)\n"
            "   |     +->PA2<\n"
            "   |     +- OSY\n"
            "   |     +- Both\n"
            "   +- [12] Panel (176.16,371.64,327.68,178.36)\n"
            "      +- [20] ComboBox (208.928,425.148,262.144,17.836)\n"
            "      |  +- Karate\n"
            "      |  +- Judo\n"
            "      |  +- Box\n"
            "      |  +->Progtest<\n"
            "      +- [21] ComboBox (208.928,460.82,262.144,17.836)\n"
            "      |  +->PA2<\n"
            "      |  +- OSY\n"
            "      |  +- Both\n"
            "      +- [12] Panel (208.928,425.148,262.144,124.852)\n"
            "         +- [20] ComboBox (235.142,462.604,209.715,12.4852)\n"
            "         |  +- Karate\n"
            "         |  +- Judo\n"
            "         |  +- Box\n"
            "         |  +->Progtest<\n"
            "         +- [21] ComboBox (235.142,487.574,209.715,12.4852)\n"
            "            +->PA2<\n"
            "            +- OSY\n"
            "            +- Both\n");
    assert (toString(b) ==
            "[0] Window \"Sample window\" (20,30,640,520)\n"
            "+- [1] Button \"Ok\" (84,446,192,52)\n"
            "+- [2] Button \"Cancel\" (404,446,192,52)\n"
            "+- [10] Label \"Username:\" (84,82,128,52)\n"
            "+- [11] Input \"chucknorris@fit.cvut.cz\" (276,82,320,52)\n"
            "+- [12] Panel (84,186,512,364)\n"
            "   +- [20] ComboBox (135.2,295.2,409.6,36.4)\n"
            "   |  +- Karate\n"
            "   |  +- Judo\n"
            "   |  +- Box\n"
            "   |  +->Progtest<\n"
            "   +- [21] ComboBox (135.2,368,409.6,36.4)\n"
            "   |  +->PA2<\n"
            "   |  +- OSY\n"
            "   |  +- Both\n"
            "   +- [12] Panel (135.2,295.2,409.6,254.8)\n"
            "   |  +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
            "   |  |  +- Karate\n"
            "   |  |  +- Judo\n"
            "   |  |  +- Box\n"
            "   |  |  +->Progtest<\n"
            "   |  +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
            "   |     +->PA2<\n"
            "   |     +- OSY\n"
            "   |     +- Both\n"
            "   +- [12] Panel (135.2,295.2,409.6,254.8)\n"
            "   |  +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
            "   |  |  +- Karate\n"
            "   |  |  +- Judo\n"
            "   |  |  +- Box\n"
            "   |  |  +->Progtest<\n"
            "   |  +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
            "   |  |  +->PA2<\n"
            "   |  |  +- OSY\n"
            "   |  |  +- Both\n"
            "   |  +- [12] Panel (176.16,371.64,327.68,178.36)\n"
            "   |     +- [20] ComboBox (208.928,425.148,262.144,17.836)\n"
            "   |     |  +- Karate\n"
            "   |     |  +- Judo\n"
            "   |     |  +- Box\n"
            "   |     |  +->Progtest<\n"
            "   |     +- [21] ComboBox (208.928,460.82,262.144,17.836)\n"
            "   |        +->PA2<\n"
            "   |        +- OSY\n"
            "   |        +- Both\n"
            "   +- [12] Panel (135.2,295.2,409.6,254.8)\n"
            "      +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
            "      |  +- Karate\n"
            "      |  +- Judo\n"
            "      |  +- Box\n"
            "      |  +->Progtest<\n"
            "      +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
            "      |  +->PA2<\n"
            "      |  +- OSY\n"
            "      |  +- Both\n"
            "      +- [12] Panel (176.16,371.64,327.68,178.36)\n"
            "      |  +- [20] ComboBox (208.928,425.148,262.144,17.836)\n"
            "      |  |  +- Karate\n"
            "      |  |  +- Judo\n"
            "      |  |  +- Box\n"
            "      |  |  +->Progtest<\n"
            "      |  +- [21] ComboBox (208.928,460.82,262.144,17.836)\n"
            "      |     +->PA2<\n"
            "      |     +- OSY\n"
            "      |     +- Both\n"
            "      +- [12] Panel (176.16,371.64,327.68,178.36)\n"
            "         +- [20] ComboBox (208.928,425.148,262.144,17.836)\n"
            "         |  +- Karate\n"
            "         |  +- Judo\n"
            "         |  +- Box\n"
            "         |  +->Progtest<\n"
            "         +- [21] ComboBox (208.928,460.82,262.144,17.836)\n"
            "         |  +->PA2<\n"
            "         |  +- OSY\n"
            "         |  +- Both\n"
            "         +- [12] Panel (208.928,425.148,262.144,124.852)\n"
            "            +- [20] ComboBox (235.142,462.604,209.715,12.4852)\n"
            "            |  +- Karate\n"
            "            |  +- Judo\n"
            "            |  +- Box\n"
            "            |  +->Progtest<\n"
            "            +- [21] ComboBox (235.142,487.574,209.715,12.4852)\n"
            "               +->PA2<\n"
            "               +- OSY\n"
            "               +- Both\n");
    assert (toString(a) ==
            "[0] Window \"Sample window\" (10,10,600,480)\n"
            "+- [1] Button \"Ok\" (70,394,180,48)\n"
            "+- [2] Button \"Cancel\" (370,394,180,48)\n"
            "+- [10] Label \"Username:\" (70,58,120,48)\n"
            "+- [11] Input \"chucknorris\" (250,58,300,48)\n"
            "+- [12] Panel (70,154,480,336)\n"
            "   +- [20] ComboBox (118,254.8,384,33.6)\n"
            "      +->Karate<\n"
            "      +- Judo\n"
            "      +- Box\n"
            "      +- Progtest\n");
    return EXIT_SUCCESS;
}

#endif /* __PROGTEST__ */
