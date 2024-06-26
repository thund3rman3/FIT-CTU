#ifndef __PROGTEST__

#include <cassert>
#include <iostream>
#include <memory>
#include <sstream>
#include <iomanip>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <algorithm>
#include <memory>
#include <functional>

#endif /* __PROGTEST__ */

class CComponent {
private:
public:
    CComponent() = default;

    CComponent(const CComponent &component) = default;

    CComponent &operator=(const CComponent &component) = default;

    virtual ~CComponent() = default;

    virtual CComponent *clone() const = 0;

    virtual void print(std::ostream &os, size_t ofs, bool lastComponent, bool lastPC) const = 0;
};

/**
destruktor, kopírující konstruktor a operátor = (pokud je potřeba vlastní implementace),
metoda addComponent, která přidá další komponentu počítače,
metoda addAddress, která přidá další adresu počítače (řetězec),
operátor pro výstup, který zobrazí přidělená adresy a komponenty počítače, jako v ukázce. Ve výpisu jsou nejprve uvedené adresy (v pořadí zadání) a za nimi komponenty (v pořadí přidávání).
 */
class CComputer {
private:
    std::vector<std::string> addresses;
    std::vector<std::unique_ptr<CComponent>> components;
    std::string name;
public:
    explicit CComputer(std::string name)
            : name(std::move(name)) {
    }

    CComputer(const CComputer &computer)
            : addresses(computer.addresses), name(computer.name) {
        for (const auto &c: computer.components) {
            components.emplace_back(std::move(std::unique_ptr<CComponent>(c->clone())));
        }
    }

    CComputer &operator=(const CComputer &computer) {
        if (this == &computer)
            return *this;
        name = computer.name;
        addresses.clear();
        addresses = computer.addresses;
        components.clear();
        for (const auto &c: computer.components) {
            components.emplace_back(std::move(std::unique_ptr<CComponent>(c->clone())));
        }
        return *this;
    }

    ~CComputer() = default;

    std::string getName() const {
        return name;
    }

    CComputer &addComponent(const CComponent &component) {
        components.emplace_back(std::move(std::unique_ptr<CComponent>(component.clone())));
        return *this;
    }

    CComputer &addAddress(const std::string &address) {
        addresses.emplace_back(address);
        return *this;
    }

    void print(std::ostream &os, bool last = false) const {
        if (last)
            os << "\\-";
        else
            os << "+-";
        os << "Host: " << name << std::endl;
        for (const auto &address: addresses) {
            if (last)
                os << "  ";
            else
                os << "| ";
            os << "+-" << address << std::endl;
        }
        for (size_t i = 0; i < components.size(); ++i) {
            if (i == components.size() - 1)
                components[i]->print(os, 1, true, last);
            else
                components[i]->print(os, 1, false, last);
        }
    }

    CComputer *clone() const {
        return new CComputer(*this);
    }

    friend std::ostream &operator<<(std::ostream &os, const CComputer &computer) {
        os << "Host: " << computer.name << std::endl;
        for (const auto &address: computer.addresses) {
            os << "+-" << address << std::endl;
        }
        for (size_t i = 0; i < computer.components.size(); ++i) {
            if (i == computer.components.size() - 1)
                computer.components[i]->print(os, 0, true, true);
            else
                computer.components[i]->print(os, 0, false, false);
        }
        return os;
    }

    friend bool operator<(const CComputer &a, const CComputer &b) {
        return a.name < b.name;
    }

    friend bool operator==(const CComputer &a, const CComputer &b) {
        return a.name == b.name;
    }
};

/**
konstruktor se jménem sítě,
destruktor, kopírující konstruktor a operátor = (pokud je potřeba vlastní implementace),
metodu addComputer, kterou lze přidávat další počítač do sítě,
metodu findComputer, která vrátí odkaz na nalezený počítač zadaného jména nebo neplatný ukazatel, pokud jej nenalezne,
výstupní operátor, který zobrazí strom počítačů a komponent, jako v ukázce. Počítače jsou vypsané v pořadí přidávání.
 */
class CNetwork {
private:
    std::vector<std::unique_ptr<CComputer>> PCs;
    std::string name;
public:
    explicit CNetwork(std::string name)
            : name(std::move(name)) {
    }

    CNetwork(const CNetwork &n)
            : name(n.name) {
        for (const auto &pc: n.PCs) {
            PCs.emplace_back(std::move(std::unique_ptr<CComputer>(pc->clone())));
        }
    }

    CNetwork &operator=(const CNetwork &n) {
        if (this == &n)
            return *this;
        name = n.name;
        PCs.clear();
        for (const auto &pc: n.PCs) {
            PCs.emplace_back(std::move(std::unique_ptr<CComputer>(pc->clone())));
        }
        return *this;
    }

    ~CNetwork() = default;

    CNetwork &addComputer(const CComputer &computer) {
        CComputer *compPtr = new CComputer(computer);
        PCs.emplace_back(std::move(std::unique_ptr<CComputer>(compPtr)));
        return *this;
    }

    CComputer *findComputer(const std::string &computer) {
        CComputer c(computer);
        for (const auto &x: PCs) {
            if (x.get()->getName() == computer)
                return x.get();
        }
        return nullptr;
    }

    friend std::ostream &operator<<(std::ostream &os, const CNetwork &n) {
        os << "Network: " << n.name << std::endl;
        for (size_t i = 0; i < n.PCs.size(); ++i) {
            if (i == n.PCs.size() - 1)
                n.PCs[i]->print(os, true);
            else
                n.PCs[i]->print(os);
        }
        return os;
    }
};

/**
 * konstruktor s parametrem počtu jader (celé číslo) a frekvencí (celé číslo v MHz),
destruktor, kopírující konstruktor a operátor = (pokud je potřeba vlastní implementace).
 */
class CCPU : public CComponent {
private:
    int cores;
    int MHz;
public:
    CCPU(int cores, int MHz)
            : cores(cores), MHz(MHz) {
    }

    CCPU(const CCPU &cpu) = default;

    CCPU &operator=(const CCPU &cpu) = default;

    ~CCPU() override = default;

    void print(std::ostream &os, size_t ofs, bool lastComponent, bool lastPC) const override {
        for (size_t i = 0; i < ofs; ++i) {
            if (lastPC)
                os << "  ";
            else
                os << "| ";
        }

        if (lastComponent)
            os << "\\-";
        else
            os << "+-";

        os << "CPU, " << cores << " cores @ " << MHz << "MHz" << std::endl;
    }

    CComponent *clone() const override {
        return new CCPU(*this);
    }
};

/**
 * konstruktor s parametrem velikosti paměti (celé číslo v MiB),
destruktor, kopírující konstruktor a operátor = (pokud je potřeba vlastní implementace).
 */
class CMemory : public CComponent {
private:
    int MiB;
public:
    explicit CMemory(int MiB)
            : MiB(MiB) {
    }

    CMemory(const CMemory &mem) = default;

    CMemory &operator=(const CMemory &mem) = default;

    ~CMemory() override = default;

    //+-Memory, 4000 MiB\n
    void print(std::ostream &os, size_t ofs, bool lastComponent, bool lastPC) const override {
        for (size_t i = 0; i < ofs; ++i) {
            if (lastPC)
                os << "  ";
            else
                os << "| ";
        }
        if (lastComponent)
            os << "\\-";
        else
            os << "+-";

        os << "Memory, " << MiB << " MiB" << std::endl;
    }

    CComponent *clone() const override {
        return new CMemory(*this);
    }
};

/**
 * konstruktor s parametry typu disku (symbolická konstanta SSD nebo MAGNETIC deklarovaná ve třídě) a velikosti disku (celé číslo v GiB),
destruktor, kopírující konstruktor a operátor = (pokud je potřeba vlastní implementace),
metodu addPartition, která přidá informaci o rozdělení disku. Metoda bude mít dva parametry
 - velikost parcely v GiB a její identifikaci (řetězec). Výpis parcel je v pořadí zadávání.
 */
class CDisk : public CComponent {
public:
    enum DiskType {
        SSD, MAGNETIC
    };
private:
    std::vector<std::pair<int, std::string>> partition;
    DiskType diskType;
    int GiB;
public:
    CDisk(DiskType diskType, int GiB)
            : diskType(diskType), GiB(GiB) {
    }

    CDisk(const CDisk &disk) = default;

    CDisk &operator=(const CDisk &disk) {
        if (this == &disk)
            return *this;
        partition.clear();
        partition = disk.partition;
        diskType = disk.diskType;
        GiB = disk.GiB;
        return *this;
    }

    ~CDisk() override = default;

    CDisk &addPartition(int partSize, const std::string &name) {
        partition.emplace_back(partSize, name);
        return *this;
    }

    void print(std::ostream &os, size_t ofs, bool lastComponent, bool lastPC) const override {
        for (size_t i = 0; i < ofs; ++i) {
            if (lastPC)
                os << "  ";
            else
                os << "| ";
        }
        if (lastComponent)
            os << "\\-";
        else
            os << "+-";
        if (diskType == SSD)
            os << "SSD, ";
        else
            os << "HDD, ";
        os << GiB << " GiB" << std::endl;

        for (size_t i = 0; i < partition.size(); ++i) {
            if (lastComponent) {
                for (size_t i = 0; i < ofs + 1; ++i) {
                    if (!lastPC && i == 0)
                        os << "| ";
                    else
                        os << "  ";
                }
            } else {
                for (size_t i = 0; i < ofs + 1; ++i) {
                    if (lastPC){
                        if(i == ofs)
                            os << "| ";
                        else
                            os << "  ";
                    }
                    else
                        os << "| ";
                }
            }

            if (i == partition.size() - 1)
                os << "\\-[";
            else
                os << "+-[";

            os << i << "]: " << partition[i].first << " GiB, " << partition[i].second << std::endl;
        }
    }

    CComponent *clone() const override {
        return new CDisk(*this);
    }
};

#ifndef __PROGTEST__

template<typename T_>
std::string toString(const T_ &x) {
    std::ostringstream oss;
    oss << x;
    auto s = oss.str();
    return oss.str();
}

int main() {
    CNetwork n("FIT network");
    n.addComputer(
            CComputer("progtest.fit.cvut.cz").addAddress("147.32.232.142").addComponent(CCPU(8, 2400)).addComponent(
                    CCPU(8, 1200)).addComponent(
                    CDisk(CDisk::MAGNETIC, 1500).addPartition(50, "/").addPartition(5, "/boot").addPartition(1000,
                                                                                                             "/var")).addComponent(
                    CDisk(CDisk::SSD, 60).addPartition(60, "/data")).addComponent(CMemory(2000)).addComponent(
                    CMemory(2000)))

            .addComputer(CComputer("courses.fit.cvut.cz").addAddress("147.32.232.213").addComponent(
                    CCPU(4, 1600)).addComponent(CMemory(4000)).addComponent(
                    CDisk(CDisk::MAGNETIC, 2000).addPartition(100, "/").addPartition(1900, "/data"))).addComputer(
            CComputer("imap.fit.cvut.cz").addAddress("147.32.232.238").addComponent(CCPU(4, 2500)).addAddress(
                    "2001:718:2:2901::238").addComponent(CMemory(8000)));
    assert (toString(n) == "Network: FIT network\n"
                           "+-Host: progtest.fit.cvut.cz\n"
                           "| +-147.32.232.142\n"
                           "| +-CPU, 8 cores @ 2400MHz\n"
                           "| +-CPU, 8 cores @ 1200MHz\n"
                           "| +-HDD, 1500 GiB\n"
                           "| | +-[0]: 50 GiB, /\n"
                           "| | +-[1]: 5 GiB, /boot\n"
                           "| | \\-[2]: 1000 GiB, /var\n"
                           "| +-SSD, 60 GiB\n"
                           "| | \\-[0]: 60 GiB, /data\n"
                           "| +-Memory, 2000 MiB\n"
                           "| \\-Memory, 2000 MiB\n"
                           "+-Host: courses.fit.cvut.cz\n"
                           "| +-147.32.232.213\n"
                           "| +-CPU, 4 cores @ 1600MHz\n"
                           "| +-Memory, 4000 MiB\n"
                           "| \\-HDD, 2000 GiB\n"
                           "|   +-[0]: 100 GiB, /\n"
                           "|   \\-[1]: 1900 GiB, /data\n"
                           "\\-Host: imap.fit.cvut.cz\n"
                           "  +-147.32.232.238\n"
                           "  +-2001:718:2:2901::238\n"
                           "  +-CPU, 4 cores @ 2500MHz\n"
                           "  \\-Memory, 8000 MiB\n");
    CNetwork x = n;
    auto c = x.findComputer("imap.fit.cvut.cz");
    assert (toString(*c) == "Host: imap.fit.cvut.cz\n"
                            "+-147.32.232.238\n"
                            "+-2001:718:2:2901::238\n"
                            "+-CPU, 4 cores @ 2500MHz\n"
                            "\\-Memory, 8000 MiB\n");
    c->addComponent(CDisk(CDisk::MAGNETIC, 1000).addPartition(100, "system").addPartition(200, "WWW").addPartition(700,
                                                                                                                   "mail"));
    assert (toString(x) == "Network: FIT network\n"
                           "+-Host: progtest.fit.cvut.cz\n"
                           "| +-147.32.232.142\n"
                           "| +-CPU, 8 cores @ 2400MHz\n"
                           "| +-CPU, 8 cores @ 1200MHz\n"
                           "| +-HDD, 1500 GiB\n"
                           "| | +-[0]: 50 GiB, /\n"
                           "| | +-[1]: 5 GiB, /boot\n"
                           "| | \\-[2]: 1000 GiB, /var\n"
                           "| +-SSD, 60 GiB\n"
                           "| | \\-[0]: 60 GiB, /data\n"
                           "| +-Memory, 2000 MiB\n"
                           "| \\-Memory, 2000 MiB\n"
                           "+-Host: courses.fit.cvut.cz\n"
                           "| +-147.32.232.213\n"
                           "| +-CPU, 4 cores @ 1600MHz\n"
                           "| +-Memory, 4000 MiB\n"
                           "| \\-HDD, 2000 GiB\n"
                           "|   +-[0]: 100 GiB, /\n"
                           "|   \\-[1]: 1900 GiB, /data\n"
                           "\\-Host: imap.fit.cvut.cz\n"
                           "  +-147.32.232.238\n"
                           "  +-2001:718:2:2901::238\n"
                           "  +-CPU, 4 cores @ 2500MHz\n"
                           "  +-Memory, 8000 MiB\n"
                           "  \\-HDD, 1000 GiB\n"
                           "    +-[0]: 100 GiB, system\n"
                           "    +-[1]: 200 GiB, WWW\n"
                           "    \\-[2]: 700 GiB, mail\n");
    assert (toString(n) == "Network: FIT network\n"
                           "+-Host: progtest.fit.cvut.cz\n"
                           "| +-147.32.232.142\n"
                           "| +-CPU, 8 cores @ 2400MHz\n"
                           "| +-CPU, 8 cores @ 1200MHz\n"
                           "| +-HDD, 1500 GiB\n"
                           "| | +-[0]: 50 GiB, /\n"
                           "| | +-[1]: 5 GiB, /boot\n"
                           "| | \\-[2]: 1000 GiB, /var\n"
                           "| +-SSD, 60 GiB\n"
                           "| | \\-[0]: 60 GiB, /data\n"
                           "| +-Memory, 2000 MiB\n"
                           "| \\-Memory, 2000 MiB\n"
                           "+-Host: courses.fit.cvut.cz\n"
                           "| +-147.32.232.213\n"
                           "| +-CPU, 4 cores @ 1600MHz\n"
                           "| +-Memory, 4000 MiB\n"
                           "| \\-HDD, 2000 GiB\n"
                           "|   +-[0]: 100 GiB, /\n"
                           "|   \\-[1]: 1900 GiB, /data\n"
                           "\\-Host: imap.fit.cvut.cz\n"
                           "  +-147.32.232.238\n"
                           "  +-2001:718:2:2901::238\n"
                           "  +-CPU, 4 cores @ 2500MHz\n"
                           "  \\-Memory, 8000 MiB\n");

    CNetwork a("sit");
    a.addComputer(CComputer("pc").

            addComponent(CMemory(2)).

    addComponent(CDisk(CDisk::SSD,2).
    addPartition(2, "banan").
    addPartition(1, "lol")).

            addComponent(CMemory(2)).
    addComponent(CDisk(CDisk::SSD,2).
            addPartition(2, "banan").
            addPartition(1, "lol")));

    a.addComputer(CComputer("pc").

            addComponent(CMemory(2)).

            addComponent(CDisk(CDisk::SSD,2).
            addPartition(2, "banan").
            addPartition(1, "lol")).

            addComponent(CMemory(2)).
            addComponent(CDisk(CDisk::SSD,2).
            addPartition(2, "banan").
            addPartition(1, "lol")));

    a.addComputer(CComputer("pc2").
            addComponent(CDisk(CDisk::SSD,2).
            addPartition(2, "banan2").
            addPartition(1, "lol2")).

            addComponent(CMemory(2)).

            addComponent(CDisk(CDisk::SSD,2).
            addPartition(2, "banan2").
            addPartition(3, "banan2").
            addPartition(1, "lol2")).

            addComponent(CMemory(2)).
            addComponent(CMemory(2)).

            addComponent(CDisk(CDisk::SSD,2).
            addPartition(2, "banan2").
            addPartition(3, "banan2").
            addPartition(1, "lol2")));
    std::ostringstream osss;
    osss<<a;
    std::string str = osss.str();
    return EXIT_SUCCESS;
}

#endif /* __PROGTEST__ */
