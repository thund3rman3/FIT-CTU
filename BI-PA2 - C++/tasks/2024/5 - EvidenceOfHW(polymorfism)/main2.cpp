#ifndef __PROGTEST__

#include <cassert>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <algorithm>
#include <memory>
#include <functional>

#endif /* __PROGTEST__ */

class CNetwork;

class CComputer;

class CComponent {
private:
public:
    CComponent() = default;

    CComponent(const CComponent &component) = default;

    CComponent &operator=(const CComponent &component) = default;

    virtual ~CComponent() = default;

    virtual CComponent *clone() const = 0;

    virtual void print(std::ostream &os, std::string prefix, bool lastComponent, bool lastPC) const = 0;

    virtual CNetwork* findNetwork(const std::string &network){
        return nullptr;
    }

    virtual CComputer* findComputer(const std::string &computer){
        return nullptr;
    }
};

/**
 * metodu findComputer, která vrátí odkaz na nalezený počítač zadaného jména nebo neplatý odkaz,
 * pokud jej nenalezne. Výsledkem může být tento počítač sám nebo i virtuální počítač, který běží
 * na tomto počítači,
metodu findNetwork, která vrátí odkaz na nalezenou síť zadaného jménanebo neplatý odkaz, pokud ji nenalezne.
 Pozor, hledá se i ve virtuálních sítích, které např. běží uvnitř tohoto počítače,
metoda duplicate(remap), která vytvoří kopii tohoto počítače. Vytváří se identická kopie (až na případné změny jména),
 včetně případných virtualizovaných počítačů/sítí. Při vytváření kopie metoda navíc změní jména všech kopírovaných
 počítačů a sítí podle parametru (remap). Tento parametr je tvořen dvojicemi řetězců (původní jméno, nové jméno).
 Pokud se jméno kopírovaného počítače/sítě objeví v parametru remap jako nějaké původní jméno, bude nahrazeno.
 Pokud původní jméno v parametru obsaženo není, bude ponecháno beze změny.
operátor pro výstup, který zobrazí přidělená adresy a komponenty počítače, jako v ukázce.
 Ve výpisu jsou nejprve uvedené adresy (v pořadí zadání) a za nimi komponenty (v pořadí přidávání).
 */
class CComputer : public CComponent {
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

    ~CComputer() override = default;

    std::string getName() const {
        return name;
    }

    void setName(const std::string &newName) {
        name = newName;
    }

    CComponent *clone() const override {
        return new CComputer(*this);
    }

    CComputer &addComponent(const CComponent &component) {
        components.emplace_back(component.clone());
        return *this;
    }

    CComputer &addAddress(const std::string &address) {
        addresses.emplace_back(address);
        return *this;
    }

    CComputer *findComputer(const std::string &computer) override{
        if (name == computer)
            return this;
        for (const auto &pc: components) {
            CComputer *res = pc->findComputer(computer);
            if (res)
                return res;
        }
        return nullptr;
    }

    CNetwork *findNetwork(const std::string &network) override;

    CComputer duplicate(const std::vector<std::pair<std::string, std::string>> &remap) const;

    void print(std::ostream &os, std::string prefix, bool lastComponent, bool lastPC) const override {
        if (prefix.length() > 2) {
            for (size_t i = 0; i < prefix.length() - 2; ++i) {
                os << prefix[i];
            }
        }
        if (lastPC)
            os << "\\-";
        else
            os << "+-";

        os << "Host: " << name << std::endl;

        for (size_t i = 0; i < addresses.size(); ++i) {
            if(components.empty() && i == addresses.size() - 1){
                os << prefix << "\\-" << addresses[i] << std::endl;
            }
            else
                os << prefix << "+-" << addresses[i] << std::endl;
        }
        for (size_t i = 0; i < components.size(); ++i) {
            if (i == components.size() - 1)
                components[i]->print(os, prefix, true, lastPC);
            else
                components[i]->print(os, prefix, false, lastPC);
        }
    }

    friend std::ostream &operator<<(std::ostream &os, const CComputer &computer) {
        os << "Host: " << computer.name << std::endl;
        std::string prefix = "";
        for (size_t i = 0; i < computer.addresses.size(); ++i) {
            if(computer.components.empty() && i == computer.addresses.size() - 1){
                os << prefix << "\\-" << computer.addresses[i] << std::endl;
            }
            else
                os << prefix << "+-" << computer.addresses[i] << std::endl;
        }
        for (size_t i = 0; i < computer.components.size(); ++i) {
            if (i == computer.components.size() - 1)
                computer.components[i]->print(os, prefix, true, false);
            else
                computer.components[i]->print(os, prefix, false, false);
        }
        return os;
    }
};

/**
 * metodu findComputer, která vrátí odkaz na nalezený počítač zadaného jména nebo neplatý odkaz,
 * pokud jej nenalezne. Pozor, hledá se i ve virtuálních počítačích,
 * které např. běží uvnitř jiného počítače, který je připojen k této síti,
metodu findNetwork, která vrátí odkaz na nalezenou síť zadaného jménanebo neplatý odkaz,
 pokud ji nenalezne. Pozor, hledá se i ve virtuálních sítích, které např. běží uvnitř jiného počítače,
 který je připojen k této síti,
 */
class CNetwork : public CComponent {
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
            PCs.emplace_back(std::make_unique<CComputer>(*pc));
        }
    }

    CNetwork &operator=(const CNetwork &n) {
        if (this == &n)
            return *this;
        name = n.name;
        PCs.clear();
        for (const auto &pc: n.PCs) {
            PCs.emplace_back(std::make_unique<CComputer>(*pc));
        }
        return *this;
    }

    ~CNetwork() override = default;

    CComponent *clone() const override {
        return new CNetwork(*this);
    }

    void setName(const std::string &newName) {
        name = newName;
    }

    CNetwork &addComputer(const CComputer &computer) {
        PCs.emplace_back(std::make_unique<CComputer>(computer));
        return *this;
    }

    CComputer *findComputer(const std::string &computer) override{
        for (const auto &pc: PCs) {
            if (pc->getName() == computer)
                return pc.get();
            CComputer *res = pc->findComputer(computer);
            if (res)
                return res;
        }
        return nullptr;
    }

    CNetwork *findNetwork(const std::string &network) override{
        if (name == network)
            return this;
        for (const auto &net: PCs) {
            CNetwork *res = net->findNetwork(network);
            if (res)
                return res;
        }
        return nullptr;
    }

    void print(std::ostream &os, std::string prefix, bool lastComponent, bool lastPC) const override {
        os << prefix;

        if (lastComponent)
            os << "\\-";
        else
            os << "+-";

        os << "Network: " << name << std::endl;
        if (!lastComponent)
            prefix += "| ";
        else
            prefix += "  ";
        for (size_t i = 0; i < PCs.size(); ++i) {
            if (i == PCs.size() - 1)
                PCs[i]->print(os, prefix + "  ", false, true);
            else
                PCs[i]->print(os, prefix + "| ", false, false);
        }
    }

    friend std::ostream &operator<<(std::ostream &os, const CNetwork &n) {
        std::string prefix = "";
        os << "Network: " << n.name << std::endl;
        for (size_t i = 0; i < n.PCs.size(); ++i) {
            if (i == n.PCs.size() - 1)
                n.PCs[i]->print(os, prefix + "  ", false, true);
            else
                n.PCs[i]->print(os, prefix + "| ", false, false);
        }
        return os;
    }
};


CNetwork *CComputer::findNetwork(const std::string &network)  {
    for (const auto &net: components) {
        CNetwork *res = net->findNetwork(network);
        if (res)
            return res;
    }
    return nullptr;
}

// (původní jméno, nové jméno) pro PC a network
CComputer CComputer::duplicate(const std::vector<std::pair<std::string, std::string>> &remap) const {
    CComputer res(*this);
    for (const auto &change: remap) {
        CComputer *pc = res.findComputer(change.first);
        if (pc)
            pc->setName(change.second);

        CNetwork *net = res.findNetwork(change.first);
        if (net)
            net->setName(change.second);
    }
    return res;
}

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

    void print(std::ostream &os, std::string prefix, bool lastComponent, bool lastPC) const override {
        os << prefix;
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
    void print(std::ostream &os, std::string prefix, bool lastComponent, bool lastPC) const override {
        os << prefix;
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

    void print(std::ostream &os, std::string prefix, bool lastComponent, bool lastPC) const override {
        os << prefix;

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
                os << prefix + "  ";
            } else {
                os << prefix + "| ";
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
    std::string sss = oss.str();
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
                    CMemory(2000))).addComputer(
            CComputer("courses.fit.cvut.cz").addAddress("147.32.232.213").addComponent(CCPU(4, 1600)).addComponent(
                    CMemory(4000)).addComponent(
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
    n.addComputer(CComputer("vm01.fit.cvut.cz").addAddress("147.32.232.232").addComponent(CCPU(32, 4000)).addComponent(
            CMemory(32768)).addComponent(
            CDisk(CDisk::MAGNETIC, 3000).addPartition(500, "SYSTEM").addPartition(2000, "DATA").addPartition(500,
                                                                                                             "BACKUP")).addComponent(
            CNetwork("dummy00").addComputer(
                    CComputer("testing.fit.cvut.cz").addAddress("192.168.1.1").addComponent(CCPU(1, 300)).addComponent(
                            CMemory(256)))).addComponent(CMemory(16384)).addComponent(CNetwork("vnet00")));
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
                           "+-Host: imap.fit.cvut.cz\n"
                           "| +-147.32.232.238\n"
                           "| +-2001:718:2:2901::238\n"
                           "| +-CPU, 4 cores @ 2500MHz\n"
                           "| \\-Memory, 8000 MiB\n"
                           "\\-Host: vm01.fit.cvut.cz\n"
                           "  +-147.32.232.232\n"
                           "  +-CPU, 32 cores @ 4000MHz\n"
                           "  +-Memory, 32768 MiB\n"
                           "  +-HDD, 3000 GiB\n"
                           "  | +-[0]: 500 GiB, SYSTEM\n"
                           "  | +-[1]: 2000 GiB, DATA\n"
                           "  | \\-[2]: 500 GiB, BACKUP\n"
                           "  +-Network: dummy00\n"
                           "  | \\-Host: testing.fit.cvut.cz\n"
                           "  |   +-192.168.1.1\n"
                           "  |   +-CPU, 1 cores @ 300MHz\n"
                           "  |   \\-Memory, 256 MiB\n"
                           "  +-Memory, 16384 MiB\n"
                           "  \\-Network: vnet00\n");
    auto vmnet = n.findNetwork("vnet00");
    vmnet->addComputer(
            CComputer("monitor1.fit.cvut.cz").addAddress("147.32.232.254").addComponent(CCPU(2, 1500)).addComponent(
                    CMemory(4096)).addComponent(
                    CDisk(CDisk::MAGNETIC, 750).addPartition(100, "root").addPartition(600, "log")));
    assert (toString(*vmnet) == "Network: vnet00\n"
                                "\\-Host: monitor1.fit.cvut.cz\n"
                                "  +-147.32.232.254\n"
                                "  +-CPU, 2 cores @ 1500MHz\n"
                                "  +-Memory, 4096 MiB\n"
                                "  \\-HDD, 750 GiB\n"
                                "    +-[0]: 100 GiB, root\n"
                                "    \\-[1]: 600 GiB, log\n");
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
                           "+-Host: imap.fit.cvut.cz\n"
                           "| +-147.32.232.238\n"
                           "| +-2001:718:2:2901::238\n"
                           "| +-CPU, 4 cores @ 2500MHz\n"
                           "| \\-Memory, 8000 MiB\n"
                           "\\-Host: vm01.fit.cvut.cz\n"
                           "  +-147.32.232.232\n"
                           "  +-CPU, 32 cores @ 4000MHz\n"
                           "  +-Memory, 32768 MiB\n"
                           "  +-HDD, 3000 GiB\n"
                           "  | +-[0]: 500 GiB, SYSTEM\n"
                           "  | +-[1]: 2000 GiB, DATA\n"
                           "  | \\-[2]: 500 GiB, BACKUP\n"
                           "  +-Network: dummy00\n"
                           "  | \\-Host: testing.fit.cvut.cz\n"
                           "  |   +-192.168.1.1\n"
                           "  |   +-CPU, 1 cores @ 300MHz\n"
                           "  |   \\-Memory, 256 MiB\n"
                           "  +-Memory, 16384 MiB\n"
                           "  \\-Network: vnet00\n"
                           "    \\-Host: monitor1.fit.cvut.cz\n"
                           "      +-147.32.232.254\n"
                           "      +-CPU, 2 cores @ 1500MHz\n"
                           "      +-Memory, 4096 MiB\n"
                           "      \\-HDD, 750 GiB\n"
                           "        +-[0]: 100 GiB, root\n"
                           "        \\-[1]: 600 GiB, log\n");
    vmnet->addComputer(n.findComputer("monitor1.fit.cvut.cz")->duplicate(
            {std::pair<std::string, std::string>("monitor1.fit.cvut.cz", "monitor2.fit.cvut.cz")}));
    assert (toString(*vmnet) == "Network: vnet00\n"
                                "+-Host: monitor1.fit.cvut.cz\n"
                                "| +-147.32.232.254\n"
                                "| +-CPU, 2 cores @ 1500MHz\n"
                                "| +-Memory, 4096 MiB\n"
                                "| \\-HDD, 750 GiB\n"
                                "|   +-[0]: 100 GiB, root\n"
                                "|   \\-[1]: 600 GiB, log\n"
                                "\\-Host: monitor2.fit.cvut.cz\n"
                                "  +-147.32.232.254\n"
                                "  +-CPU, 2 cores @ 1500MHz\n"
                                "  +-Memory, 4096 MiB\n"
                                "  \\-HDD, 750 GiB\n"
                                "    +-[0]: 100 GiB, root\n"
                                "    \\-[1]: 600 GiB, log\n");
    n.addComputer(n.findComputer("vm01.fit.cvut.cz")->duplicate(
            {std::pair<std::string, std::string>("monitor1.fit.cvut.cz", "monitor3.fit.cvut.cz"),
             std::pair<std::string, std::string>("monitor2.fit.cvut.cz", "monitor4.fit.cvut.cz"),
             std::pair<std::string, std::string>("vm01.fit.cvut.cz", "vm02.fit.cvut.cz")}));
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
                           "+-Host: imap.fit.cvut.cz\n"
                           "| +-147.32.232.238\n"
                           "| +-2001:718:2:2901::238\n"
                           "| +-CPU, 4 cores @ 2500MHz\n"
                           "| \\-Memory, 8000 MiB\n"
                           "+-Host: vm01.fit.cvut.cz\n"
                           "| +-147.32.232.232\n"
                           "| +-CPU, 32 cores @ 4000MHz\n"
                           "| +-Memory, 32768 MiB\n"
                           "| +-HDD, 3000 GiB\n"
                           "| | +-[0]: 500 GiB, SYSTEM\n"
                           "| | +-[1]: 2000 GiB, DATA\n"
                           "| | \\-[2]: 500 GiB, BACKUP\n"
                           "| +-Network: dummy00\n"
                           "| | \\-Host: testing.fit.cvut.cz\n"
                           "| |   +-192.168.1.1\n"
                           "| |   +-CPU, 1 cores @ 300MHz\n"
                           "| |   \\-Memory, 256 MiB\n"
                           "| +-Memory, 16384 MiB\n"
                           "| \\-Network: vnet00\n"
                           "|   +-Host: monitor1.fit.cvut.cz\n"
                           "|   | +-147.32.232.254\n"
                           "|   | +-CPU, 2 cores @ 1500MHz\n"
                           "|   | +-Memory, 4096 MiB\n"
                           "|   | \\-HDD, 750 GiB\n"
                           "|   |   +-[0]: 100 GiB, root\n"
                           "|   |   \\-[1]: 600 GiB, log\n"
                           "|   \\-Host: monitor2.fit.cvut.cz\n"
                           "|     +-147.32.232.254\n"
                           "|     +-CPU, 2 cores @ 1500MHz\n"
                           "|     +-Memory, 4096 MiB\n"
                           "|     \\-HDD, 750 GiB\n"
                           "|       +-[0]: 100 GiB, root\n"
                           "|       \\-[1]: 600 GiB, log\n"
                           "\\-Host: vm02.fit.cvut.cz\n"
                           "  +-147.32.232.232\n"
                           "  +-CPU, 32 cores @ 4000MHz\n"
                           "  +-Memory, 32768 MiB\n"
                           "  +-HDD, 3000 GiB\n"
                           "  | +-[0]: 500 GiB, SYSTEM\n"
                           "  | +-[1]: 2000 GiB, DATA\n"
                           "  | \\-[2]: 500 GiB, BACKUP\n"
                           "  +-Network: dummy00\n"
                           "  | \\-Host: testing.fit.cvut.cz\n"
                           "  |   +-192.168.1.1\n"
                           "  |   +-CPU, 1 cores @ 300MHz\n"
                           "  |   \\-Memory, 256 MiB\n"
                           "  +-Memory, 16384 MiB\n"
                           "  \\-Network: vnet00\n"
                           "    +-Host: monitor3.fit.cvut.cz\n"
                           "    | +-147.32.232.254\n"
                           "    | +-CPU, 2 cores @ 1500MHz\n"
                           "    | +-Memory, 4096 MiB\n"
                           "    | \\-HDD, 750 GiB\n"
                           "    |   +-[0]: 100 GiB, root\n"
                           "    |   \\-[1]: 600 GiB, log\n"
                           "    \\-Host: monitor4.fit.cvut.cz\n"
                           "      +-147.32.232.254\n"
                           "      +-CPU, 2 cores @ 1500MHz\n"
                           "      +-Memory, 4096 MiB\n"
                           "      \\-HDD, 750 GiB\n"
                           "        +-[0]: 100 GiB, root\n"
                           "        \\-[1]: 600 GiB, log\n");
    vmnet->addComputer(n.findComputer("vm02.fit.cvut.cz")->duplicate(
            {std::pair<std::string, std::string>("monitor3.fit.cvut.cz", "monitor5.fit.cvut.cz"),
             std::pair<std::string, std::string>("monitor4.fit.cvut.cz", "monitor6.fit.cvut.cz"),
             std::pair<std::string, std::string>("vm02.fit.cvut.cz", "vm03.fit.cvut.cz"),
             std::pair<std::string, std::string>("vnet00", "vnet01")}));
    assert (toString(*vmnet) == "Network: vnet00\n"
                                "+-Host: monitor1.fit.cvut.cz\n"
                                "| +-147.32.232.254\n"
                                "| +-CPU, 2 cores @ 1500MHz\n"
                                "| +-Memory, 4096 MiB\n"
                                "| \\-HDD, 750 GiB\n"
                                "|   +-[0]: 100 GiB, root\n"
                                "|   \\-[1]: 600 GiB, log\n"
                                "+-Host: monitor2.fit.cvut.cz\n"
                                "| +-147.32.232.254\n"
                                "| +-CPU, 2 cores @ 1500MHz\n"
                                "| +-Memory, 4096 MiB\n"
                                "| \\-HDD, 750 GiB\n"
                                "|   +-[0]: 100 GiB, root\n"
                                "|   \\-[1]: 600 GiB, log\n"
                                "\\-Host: vm03.fit.cvut.cz\n"
                                "  +-147.32.232.232\n"
                                "  +-CPU, 32 cores @ 4000MHz\n"
                                "  +-Memory, 32768 MiB\n"
                                "  +-HDD, 3000 GiB\n"
                                "  | +-[0]: 500 GiB, SYSTEM\n"
                                "  | +-[1]: 2000 GiB, DATA\n"
                                "  | \\-[2]: 500 GiB, BACKUP\n"
                                "  +-Network: dummy00\n"
                                "  | \\-Host: testing.fit.cvut.cz\n"
                                "  |   +-192.168.1.1\n"
                                "  |   +-CPU, 1 cores @ 300MHz\n"
                                "  |   \\-Memory, 256 MiB\n"
                                "  +-Memory, 16384 MiB\n"
                                "  \\-Network: vnet01\n"
                                "    +-Host: monitor5.fit.cvut.cz\n"
                                "    | +-147.32.232.254\n"
                                "    | +-CPU, 2 cores @ 1500MHz\n"
                                "    | +-Memory, 4096 MiB\n"
                                "    | \\-HDD, 750 GiB\n"
                                "    |   +-[0]: 100 GiB, root\n"
                                "    |   \\-[1]: 600 GiB, log\n"
                                "    \\-Host: monitor6.fit.cvut.cz\n"
                                "      +-147.32.232.254\n"
                                "      +-CPU, 2 cores @ 1500MHz\n"
                                "      +-Memory, 4096 MiB\n"
                                "      \\-HDD, 750 GiB\n"
                                "        +-[0]: 100 GiB, root\n"
                                "        \\-[1]: 600 GiB, log\n");
    n.findComputer("vm02.fit.cvut.cz")->findNetwork("vnet00")->addComputer(
            n.findComputer("vm01.fit.cvut.cz")->duplicate(
                    {std::pair<std::string, std::string>("monitor1.fit.cvut.cz", "monitor11.fit.cvut.cz"),
                     std::pair<std::string, std::string>("monitor2.fit.cvut.cz", "monitor12.fit.cvut.cz"),
                     std::pair<std::string, std::string>("vm01.fit.cvut.cz", "vm11.fit.cvut.cz"),
                     std::pair<std::string, std::string>("vm03.fit.cvut.cz", "vm13.fit.cvut.cz"),
                     std::pair<std::string, std::string>("vnet00", "vnet10"),
                     std::pair<std::string, std::string>("vnet01", "vnet11")}));
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
                           "+-Host: imap.fit.cvut.cz\n"
                           "| +-147.32.232.238\n"
                           "| +-2001:718:2:2901::238\n"
                           "| +-CPU, 4 cores @ 2500MHz\n"
                           "| \\-Memory, 8000 MiB\n"
                           "+-Host: vm01.fit.cvut.cz\n"
                           "| +-147.32.232.232\n"
                           "| +-CPU, 32 cores @ 4000MHz\n"
                           "| +-Memory, 32768 MiB\n"
                           "| +-HDD, 3000 GiB\n"
                           "| | +-[0]: 500 GiB, SYSTEM\n"
                           "| | +-[1]: 2000 GiB, DATA\n"
                           "| | \\-[2]: 500 GiB, BACKUP\n"
                           "| +-Network: dummy00\n"
                           "| | \\-Host: testing.fit.cvut.cz\n"
                           "| |   +-192.168.1.1\n"
                           "| |   +-CPU, 1 cores @ 300MHz\n"
                           "| |   \\-Memory, 256 MiB\n"
                           "| +-Memory, 16384 MiB\n"
                           "| \\-Network: vnet00\n"
                           "|   +-Host: monitor1.fit.cvut.cz\n"
                           "|   | +-147.32.232.254\n"
                           "|   | +-CPU, 2 cores @ 1500MHz\n"
                           "|   | +-Memory, 4096 MiB\n"
                           "|   | \\-HDD, 750 GiB\n"
                           "|   |   +-[0]: 100 GiB, root\n"
                           "|   |   \\-[1]: 600 GiB, log\n"
                           "|   +-Host: monitor2.fit.cvut.cz\n"
                           "|   | +-147.32.232.254\n"
                           "|   | +-CPU, 2 cores @ 1500MHz\n"
                           "|   | +-Memory, 4096 MiB\n"
                           "|   | \\-HDD, 750 GiB\n"
                           "|   |   +-[0]: 100 GiB, root\n"
                           "|   |   \\-[1]: 600 GiB, log\n"
                           "|   \\-Host: vm03.fit.cvut.cz\n"
                           "|     +-147.32.232.232\n"
                           "|     +-CPU, 32 cores @ 4000MHz\n"
                           "|     +-Memory, 32768 MiB\n"
                           "|     +-HDD, 3000 GiB\n"
                           "|     | +-[0]: 500 GiB, SYSTEM\n"
                           "|     | +-[1]: 2000 GiB, DATA\n"
                           "|     | \\-[2]: 500 GiB, BACKUP\n"
                           "|     +-Network: dummy00\n"
                           "|     | \\-Host: testing.fit.cvut.cz\n"
                           "|     |   +-192.168.1.1\n"
                           "|     |   +-CPU, 1 cores @ 300MHz\n"
                           "|     |   \\-Memory, 256 MiB\n"
                           "|     +-Memory, 16384 MiB\n"
                           "|     \\-Network: vnet01\n"
                           "|       +-Host: monitor5.fit.cvut.cz\n"
                           "|       | +-147.32.232.254\n"
                           "|       | +-CPU, 2 cores @ 1500MHz\n"
                           "|       | +-Memory, 4096 MiB\n"
                           "|       | \\-HDD, 750 GiB\n"
                           "|       |   +-[0]: 100 GiB, root\n"
                           "|       |   \\-[1]: 600 GiB, log\n"
                           "|       \\-Host: monitor6.fit.cvut.cz\n"
                           "|         +-147.32.232.254\n"
                           "|         +-CPU, 2 cores @ 1500MHz\n"
                           "|         +-Memory, 4096 MiB\n"
                           "|         \\-HDD, 750 GiB\n"
                           "|           +-[0]: 100 GiB, root\n"
                           "|           \\-[1]: 600 GiB, log\n"
                           "\\-Host: vm02.fit.cvut.cz\n"
                           "  +-147.32.232.232\n"
                           "  +-CPU, 32 cores @ 4000MHz\n"
                           "  +-Memory, 32768 MiB\n"
                           "  +-HDD, 3000 GiB\n"
                           "  | +-[0]: 500 GiB, SYSTEM\n"
                           "  | +-[1]: 2000 GiB, DATA\n"
                           "  | \\-[2]: 500 GiB, BACKUP\n"
                           "  +-Network: dummy00\n"
                           "  | \\-Host: testing.fit.cvut.cz\n"
                           "  |   +-192.168.1.1\n"
                           "  |   +-CPU, 1 cores @ 300MHz\n"
                           "  |   \\-Memory, 256 MiB\n"
                           "  +-Memory, 16384 MiB\n"
                           "  \\-Network: vnet00\n"
                           "    +-Host: monitor3.fit.cvut.cz\n"
                           "    | +-147.32.232.254\n"
                           "    | +-CPU, 2 cores @ 1500MHz\n"
                           "    | +-Memory, 4096 MiB\n"
                           "    | \\-HDD, 750 GiB\n"
                           "    |   +-[0]: 100 GiB, root\n"
                           "    |   \\-[1]: 600 GiB, log\n"
                           "    +-Host: monitor4.fit.cvut.cz\n"
                           "    | +-147.32.232.254\n"
                           "    | +-CPU, 2 cores @ 1500MHz\n"
                           "    | +-Memory, 4096 MiB\n"
                           "    | \\-HDD, 750 GiB\n"
                           "    |   +-[0]: 100 GiB, root\n"
                           "    |   \\-[1]: 600 GiB, log\n"
                           "    \\-Host: vm11.fit.cvut.cz\n"
                           "      +-147.32.232.232\n"
                           "      +-CPU, 32 cores @ 4000MHz\n"
                           "      +-Memory, 32768 MiB\n"
                           "      +-HDD, 3000 GiB\n"
                           "      | +-[0]: 500 GiB, SYSTEM\n"
                           "      | +-[1]: 2000 GiB, DATA\n"
                           "      | \\-[2]: 500 GiB, BACKUP\n"
                           "      +-Network: dummy00\n"
                           "      | \\-Host: testing.fit.cvut.cz\n"
                           "      |   +-192.168.1.1\n"
                           "      |   +-CPU, 1 cores @ 300MHz\n"
                           "      |   \\-Memory, 256 MiB\n"
                           "      +-Memory, 16384 MiB\n"
                           "      \\-Network: vnet10\n"
                           "        +-Host: monitor11.fit.cvut.cz\n"
                           "        | +-147.32.232.254\n"
                           "        | +-CPU, 2 cores @ 1500MHz\n"
                           "        | +-Memory, 4096 MiB\n"
                           "        | \\-HDD, 750 GiB\n"
                           "        |   +-[0]: 100 GiB, root\n"
                           "        |   \\-[1]: 600 GiB, log\n"
                           "        +-Host: monitor12.fit.cvut.cz\n"
                           "        | +-147.32.232.254\n"
                           "        | +-CPU, 2 cores @ 1500MHz\n"
                           "        | +-Memory, 4096 MiB\n"
                           "        | \\-HDD, 750 GiB\n"
                           "        |   +-[0]: 100 GiB, root\n"
                           "        |   \\-[1]: 600 GiB, log\n"
                           "        \\-Host: vm13.fit.cvut.cz\n"
                           "          +-147.32.232.232\n"
                           "          +-CPU, 32 cores @ 4000MHz\n"
                           "          +-Memory, 32768 MiB\n"
                           "          +-HDD, 3000 GiB\n"
                           "          | +-[0]: 500 GiB, SYSTEM\n"
                           "          | +-[1]: 2000 GiB, DATA\n"
                           "          | \\-[2]: 500 GiB, BACKUP\n"
                           "          +-Network: dummy00\n"
                           "          | \\-Host: testing.fit.cvut.cz\n"
                           "          |   +-192.168.1.1\n"
                           "          |   +-CPU, 1 cores @ 300MHz\n"
                           "          |   \\-Memory, 256 MiB\n"
                           "          +-Memory, 16384 MiB\n"
                           "          \\-Network: vnet11\n"
                           "            +-Host: monitor5.fit.cvut.cz\n"
                           "            | +-147.32.232.254\n"
                           "            | +-CPU, 2 cores @ 1500MHz\n"
                           "            | +-Memory, 4096 MiB\n"
                           "            | \\-HDD, 750 GiB\n"
                           "            |   +-[0]: 100 GiB, root\n"
                           "            |   \\-[1]: 600 GiB, log\n"
                           "            \\-Host: monitor6.fit.cvut.cz\n"
                           "              +-147.32.232.254\n"
                           "              +-CPU, 2 cores @ 1500MHz\n"
                           "              +-Memory, 4096 MiB\n"
                           "              \\-HDD, 750 GiB\n"
                           "                +-[0]: 100 GiB, root\n"
                           "                \\-[1]: 600 GiB, log\n");
    return EXIT_SUCCESS;
}

#endif /* __PROGTEST__ */
