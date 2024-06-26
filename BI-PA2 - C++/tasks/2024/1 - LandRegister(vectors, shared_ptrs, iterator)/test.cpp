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
#include <utility>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <memory>

#endif /* __PROGTEST__ */

struct SLand
{
    SLand(std::string owner, std::string city, std::string addr, std::string region, unsigned int id) : owner(
            std::move(owner)), city(std::move(city)), addr(std::move(addr)), region(std::move(region)), id(id)
    {
    }

    std::string owner;
    std::string city;
    std::string addr;
    std::string region;
    unsigned int id;

};

class CIterator
{
public:
    CIterator(std::vector<std::shared_ptr<SLand>> *landReg) : land(landReg), pos(0)
    {
    }

    ~CIterator()
    {
        delete land;
    }

    // true, kdyz jsme na konci
    bool atEnd() const
    {
        return land->size() <= pos;
    }

    // posun na dalsi prvek
    void next()
    {
        pos++;
    }

    // get_city_name
    std::string city() const
    {
        return (*land)[pos]->city;
    }

    // get_addr
    std::string addr() const
    {
        return (*land)[pos]->addr;
    }

    // get_region_name
    std::string region() const
    {
        return (*land)[pos]->region;
    }

    // get_id
    unsigned id() const
    {
        return (*land)[pos]->id;
    }

    // get_owner
    std::string owner() const
    {
        return (*land)[pos]->owner;
    }

private:
    std::vector<std::shared_ptr<SLand>> *land;
    size_t pos;
};

// registr pozemku a vlastniku
// pozemek:
//  katastralni uzemi(string region),
//  cislo pozemku v katstralnim uzemi(unsigned int id),
//  mesto(string city),
//  adresa(string addr).
//pozemek se identifikuje pomoci dvojic (region, id) nebo (city, addr)
// pri strcmp u region,city,addr rozlisujeme mala a velka pismena
// 1 vlastnik (owner) <-> vice pozemku
// nove pomzemky maji owner = "" - bez vlasnika
// pri strcmp jmen vlastniku nerozlisujeme mala a velka pismena
class CLandRegister
{
public:
    CLandRegister() {
        owners.emplace_back("",std::vector<std::shared_ptr<SLand>>());
    }

    ~CLandRegister() = default;

    CLandRegister(const CLandRegister &land) = delete;

    CLandRegister &operator=(const CLandRegister &land) = delete;

    bool add(const std::string &city, const std::string &addr, const std::string &region, unsigned int id)
    {
        auto land = std::make_shared<SLand>("", city, addr, region, id);

        auto itCityAddr = std::lower_bound(cityAddr.begin(), cityAddr.end(), land, cmpCityAddr);
        if (itCityAddr != cityAddr.end() && (*itCityAddr)->city == city && (*itCityAddr)->addr == addr)
            return false;

        auto itRegId = std::lower_bound(regId.begin(), regId.end(), land, cmpRegId);
        if (itRegId != regId.end() && (*itRegId)->region == region && (*itRegId)->id == id)
            return false;

        owners[0].second.emplace_back(land);
        cityAddr.insert(itCityAddr, land);
        regId.insert(itRegId, land);
        return true;
    }

    bool del(const std::string &city, const std::string &addr)
    {
        auto land = std::make_shared<SLand>("", city, addr, "", 0);

        auto itCityAddr = std::lower_bound(cityAddr.begin(), cityAddr.end(), land, cmpCityAddr);
        if (itCityAddr == cityAddr.end() || (*itCityAddr)->city != city || (*itCityAddr)->addr != addr)
            return false;

        auto itRegId = std::lower_bound(regId.begin(), regId.end(), *itCityAddr, cmpRegId);
        if (itRegId == regId.end() || (*itRegId)->region != (*itCityAddr)->region || (*itRegId)->id != (*itCityAddr)->id)
            return false;

        auto ow = std::make_pair((*itCityAddr)->owner, std::vector<std::shared_ptr<SLand>>());
        auto itOw = std::lower_bound(owners.begin(), owners.end(), ow, cmpOwner);
        if (itOw == owners.end())
            return false;

        for(auto it = itOw->second.begin(); it != itOw->second.end(); ++it){
            if((*it)->city == city && (*it)->addr == addr){
                itOw->second.erase(it);
                break;
            }
        }

        cityAddr.erase(itCityAddr);
        regId.erase(itRegId);
        return true;
    }

    bool del(const std::string &region, unsigned int id)
    {
        auto land = std::make_shared<SLand>("", "", "", region, id);

        auto itRegId = std::lower_bound(regId.begin(), regId.end(), land, cmpRegId);
        if (itRegId == regId.end() || (*itRegId)->region != region || (*itRegId)->id != id)
            return false;

        auto itCityAddr = std::lower_bound(cityAddr.begin(), cityAddr.end(), *itRegId, cmpCityAddr);
        if (itCityAddr == cityAddr.end() || (*itRegId)->city != (*itCityAddr)->city || (*itRegId)->addr != (*itCityAddr)->addr)
            return false;

        auto ow = std::make_pair((*itCityAddr)->owner, std::vector<std::shared_ptr<SLand>>());
        auto itOw = std::lower_bound(owners.begin(), owners.end(), ow, cmpOwner);
        if (itOw == owners.end())
            return false;

        for(auto it = itOw->second.begin(); it != itOw->second.end(); ++it){
            if((*it)->region == region && (*it)->id == id){
                itOw->second.erase(it);
                break;
            }
        }
        regId.erase(itRegId);
        cityAddr.erase(itCityAddr);
        return true;
    }

    bool getOwner(const std::string &city, const std::string &addr, std::string &owner) const
    {
        auto land = std::make_shared<SLand>("", city, addr, "", 0);

        auto it1 = std::lower_bound(cityAddr.begin(), cityAddr.end(), land, cmpCityAddr);
        if (it1 == cityAddr.end() || (*it1)->city != city || (*it1)->addr != addr)
            return false;

        owner = (*it1)->owner;
        return true;
    }

    bool getOwner(const std::string &region, unsigned int id, std::string &owner) const
    {
        auto land = std::make_shared<SLand>("", "", "", region, id);

        auto it1 = std::lower_bound(regId.begin(), regId.end(), land, cmpRegId);
        if (it1 == regId.end() || (*it1)->region != region || (*it1)->id != id)
            return false;

        owner = (*it1)->owner;
        return true;
    }

    bool newOwner(const std::string &city, const std::string &addr, const std::string &owner)
    {
        auto land = std::make_shared<SLand>(owner, city, addr, "", 0);

        auto itCityAddr = std::lower_bound(cityAddr.begin(), cityAddr.end(), land, cmpCityAddr);
        if (itCityAddr == cityAddr.end() || (*itCityAddr)->city != city || (*itCityAddr)->addr != addr ||
                cmpIgnoreCase((*itCityAddr)->owner, owner))
            return false;

        auto ow = std::make_pair((*itCityAddr)->owner, std::vector<std::shared_ptr<SLand>>());
        auto itOw = std::lower_bound(owners.begin(), owners.end(), ow, cmpOwner);

        for(auto it = itOw->second.begin(); it != itOw->second.end(); ++it){
            if((*it)->city == city && (*it)->addr == addr){
                land = *it;
                itOw->second.erase(it);
                break;
            }
        }
        ow.first = owner;
        auto it4 = std::lower_bound(owners.begin(), owners.end(), ow, cmpOwner);
        if(it4 == owners.end() || !cmpIgnoreCase(it4->first, owner)){
            owners.insert(it4, {owner, std::vector<std::shared_ptr<SLand>>{land}});
        }
        else
            it4->second.emplace_back(land);

        (*itCityAddr)->owner = owner;
        return true;
    }

    bool newOwner(const std::string &region, unsigned int id, const std::string &owner)
    {
        auto land = std::make_shared<SLand>(owner, "", "", region, id);

        auto itRegId = std::lower_bound(regId.begin(), regId.end(), land, cmpRegId);
        if (itRegId == regId.end() || (*itRegId)->region != region || (*itRegId)->id != id ||
                cmpIgnoreCase((*itRegId)->owner, owner))
            return false;

        auto ow = std::make_pair((*itRegId)->owner, std::vector<std::shared_ptr<SLand>>());
        auto itOw = std::lower_bound(owners.begin(), owners.end(), ow, cmpOwner);

        for(auto it = itOw->second.begin(); it != itOw->second.end(); ++it){
            if((*it)->region == region && (*it)->id == id){
                land = *it;
                itOw->second.erase(it);
                break;
            }
        }
        ow.first = owner;
        auto it4 = std::lower_bound(owners.begin(), owners.end(), ow, cmpOwner);
        if(it4 == owners.end() || !cmpIgnoreCase(it4->first, owner)){
            owners.insert(it4, {owner, std::vector<std::shared_ptr<SLand>>{land}});
        }
        else
            it4->second.emplace_back(land);

        (*itRegId)->owner = owner;
        return true;
    }

    size_t count(const std::string &owner) const
    {
        auto ow = std::make_pair(owner, std::vector<std::shared_ptr<SLand>>());
        auto it = std::lower_bound(owners.begin(), owners.end(), ow, cmpOwner);
        if(it == owners.end() || !cmpIgnoreCase(owner, it->first))
            return 0;
        return it->second.size();
    }

    CIterator listByAddr() const
    {
        std::vector<std::shared_ptr<SLand>> *ownerList = new std::vector<std::shared_ptr<SLand>>(cityAddr);
        return {ownerList};
    }

    CIterator listByOwner(const std::string &owner) const
    {
        auto ow = std::make_pair(owner, std::vector<std::shared_ptr<SLand>>());
        auto it = std::lower_bound(owners.begin(), owners.end(), ow, cmpOwner);
        if(it == owners.end() || !cmpIgnoreCase(owner, it->first))
            return {new std::vector<std::shared_ptr<SLand>>()};
        std::vector<std::shared_ptr<SLand>> *ownerList = new std::vector<std::shared_ptr<SLand>>(it->second);
        return {ownerList};
    }

private:

    static bool cmpIgnoreCase(const std::string &str1, const std::string &str2)
    {
        if (str1.empty() && str2.empty())
            return true;
        if (str1.length() != str2.length())
            return false;

        std::string low1 = str1;
        std::string low2 = str2;

        std::transform(low1.begin(), low1.end(), low1.begin(), ::tolower);
        std::transform(low2.begin(), low2.end(), low2.begin(), ::tolower);

        return low1 == low2;
    }

    static int cmpCityAddr(const std::shared_ptr<SLand> &a, const std::shared_ptr<SLand> &b)
    {
        if (a->city != b->city)
            return a->city < b->city;
        return a->addr < b->addr;
    }

    static int cmpRegId(const std::shared_ptr<SLand> &a, const std::shared_ptr<SLand> &b)
    {
        if (a->region != b->region)
            return a->region < b->region;
        return a->id < b->id;
    }

    static int cmpOwner(const std::pair<std::string, std::vector<std::shared_ptr<SLand>>> &land1,
                        const std::pair<std::string, std::vector<std::shared_ptr<SLand>>> &land2)
    {
        if (land1.first.empty() && land2.first.empty())
            return 0;
        std::string low1 = land1.first;
        std::string low2 = land2.first;

        std::transform(low1.begin(), low1.end(), low1.begin(), ::tolower);
        std::transform(low2.begin(), low2.end(), low2.begin(), ::tolower);

        return low1 < low2;
    }


    //pozemek se identifikuje pomoci dvojic (region, id) nebo (city, addr)
    std::vector<std::shared_ptr<SLand>> regId; // serazeno podle region->id
    std::vector<std::shared_ptr<SLand>> cityAddr;// podle city->addr
    std::vector<std::pair<std::string, std::vector<std::shared_ptr<SLand>>>> owners;// podle owneru, kde je owner a jeho lands
};

#ifndef __PROGTEST__

static void test0()
{
    CLandRegister x;
    std::string owner;

    assert (x.add("Prague", "Thakurova", "Dejvice", 12345));
    assert (x.add("Prague", "Evropska", "Vokovice", 12345));
    assert (x.add("Prague", "Technicka", "Dejvice", 9873));
    assert (x.add("Plzen", "Evropska", "Plzen mesto", 78901));
    assert (x.add("Liberec", "Evropska", "Librec", 4552));
    CIterator i0 = x.listByAddr();
    assert (!i0.atEnd() && i0.city() == "Liberec" && i0.addr() == "Evropska" && i0.region() == "Librec" &&
            i0.id() == 4552 && i0.owner() == "");
    i0.next();
    assert (!i0.atEnd() && i0.city() == "Plzen" && i0.addr() == "Evropska" && i0.region() == "Plzen mesto" &&
            i0.id() == 78901 && i0.owner() == "");
    i0.next();
    assert (!i0.atEnd() && i0.city() == "Prague" && i0.addr() == "Evropska" && i0.region() == "Vokovice" &&
            i0.id() == 12345 && i0.owner() == "");
    i0.next();
    assert (!i0.atEnd() && i0.city() == "Prague" && i0.addr() == "Technicka" && i0.region() == "Dejvice" &&
            i0.id() == 9873 && i0.owner() == "");
    i0.next();
    assert (!i0.atEnd() && i0.city() == "Prague" && i0.addr() == "Thakurova" && i0.region() == "Dejvice" &&
            i0.id() == 12345 && i0.owner() == "");
    i0.next();
    assert (i0.atEnd());

    assert (x.count("") == 5);
    CIterator i1 = x.listByOwner("");
    assert (!i1.atEnd() && i1.city() == "Prague" && i1.addr() == "Thakurova" && i1.region() == "Dejvice" &&
            i1.id() == 12345 && i1.owner() == "");
    i1.next();
    assert (!i1.atEnd() && i1.city() == "Prague" && i1.addr() == "Evropska" && i1.region() == "Vokovice" &&
            i1.id() == 12345 && i1.owner() == "");
    i1.next();
    assert (!i1.atEnd() && i1.city() == "Prague" && i1.addr() == "Technicka" && i1.region() == "Dejvice" &&
            i1.id() == 9873 && i1.owner() == "");
    i1.next();
    assert (!i1.atEnd() && i1.city() == "Plzen" && i1.addr() == "Evropska" && i1.region() == "Plzen mesto" &&
            i1.id() == 78901 && i1.owner() == "");
    i1.next();
    assert (!i1.atEnd() && i1.city() == "Liberec" && i1.addr() == "Evropska" && i1.region() == "Librec" &&
            i1.id() == 4552 && i1.owner() == "");
    i1.next();
    assert (i1.atEnd());

    assert (x.count("CVUT") == 0);
    CIterator i2 = x.listByOwner("CVUT");
    assert (i2.atEnd());

    assert (x.newOwner("Prague", "Thakurova", "CVUT"));
    assert (x.newOwner("Dejvice", 9873, "CVUT"));
    assert (x.newOwner("Plzen", "Evropska", "Anton Hrabis"));
    assert (x.newOwner("Librec", 4552, "Cvut"));
    assert (x.getOwner("Prague", "Thakurova", owner) && owner == "CVUT");
    assert (x.getOwner("Dejvice", 12345, owner) && owner == "CVUT");
    assert (x.getOwner("Prague", "Evropska", owner) && owner == "");
    assert (x.getOwner("Vokovice", 12345, owner) && owner == "");
    assert (x.getOwner("Prague", "Technicka", owner) && owner == "CVUT");
    assert (x.getOwner("Dejvice", 9873, owner) && owner == "CVUT");
    assert (x.getOwner("Plzen", "Evropska", owner) && owner == "Anton Hrabis");
    assert (x.getOwner("Plzen mesto", 78901, owner) && owner == "Anton Hrabis");
    assert (x.getOwner("Liberec", "Evropska", owner) && owner == "Cvut");
    assert (x.getOwner("Librec", 4552, owner) && owner == "Cvut");
    CIterator i3 = x.listByAddr();
    assert (!i3.atEnd() && i3.city() == "Liberec" && i3.addr() == "Evropska" && i3.region() == "Librec" &&
            i3.id() == 4552 && i3.owner() == "Cvut");
    i3.next();
    assert (!i3.atEnd() && i3.city() == "Plzen" && i3.addr() == "Evropska" && i3.region() == "Plzen mesto" &&
            i3.id() == 78901 && i3.owner() == "Anton Hrabis");
    i3.next();
    assert (!i3.atEnd() && i3.city() == "Prague" && i3.addr() == "Evropska" && i3.region() == "Vokovice" &&
            i3.id() == 12345 && i3.owner() == "");
    i3.next();
    assert (!i3.atEnd() && i3.city() == "Prague" && i3.addr() == "Technicka" && i3.region() == "Dejvice" &&
            i3.id() == 9873 && i3.owner() == "CVUT");
    i3.next();
    assert (!i3.atEnd() && i3.city() == "Prague" && i3.addr() == "Thakurova" && i3.region() == "Dejvice" &&
            i3.id() == 12345 && i3.owner() == "CVUT");
    i3.next();
    assert (i3.atEnd());

    assert (x.count("cvut") == 3);
    CIterator i4 = x.listByOwner("cVuT");
    assert (!i4.atEnd() && i4.city() == "Prague" && i4.addr() == "Thakurova" && i4.region() == "Dejvice" &&
            i4.id() == 12345 && i4.owner() == "CVUT");
    i4.next();
    assert (!i4.atEnd() && i4.city() == "Prague" && i4.addr() == "Technicka" && i4.region() == "Dejvice" &&
            i4.id() == 9873 && i4.owner() == "CVUT");
    i4.next();
    assert (!i4.atEnd() && i4.city() == "Liberec" && i4.addr() == "Evropska" && i4.region() == "Librec" &&
            i4.id() == 4552 && i4.owner() == "Cvut");
    i4.next();
    assert (i4.atEnd());

    assert (x.newOwner("Plzen mesto", 78901, "CVut"));
    assert (x.count("CVUT") == 4);
    CIterator i5 = x.listByOwner("CVUT");
    assert (!i5.atEnd() && i5.city() == "Prague" && i5.addr() == "Thakurova" && i5.region() == "Dejvice" &&
            i5.id() == 12345 && i5.owner() == "CVUT");
    i5.next();
    assert (!i5.atEnd() && i5.city() == "Prague" && i5.addr() == "Technicka" && i5.region() == "Dejvice" &&
            i5.id() == 9873 && i5.owner() == "CVUT");
    i5.next();
    assert (!i5.atEnd() && i5.city() == "Liberec" && i5.addr() == "Evropska" && i5.region() == "Librec" &&
            i5.id() == 4552 && i5.owner() == "Cvut");
    i5.next();
    assert (!i5.atEnd() && i5.city() == "Plzen" && i5.addr() == "Evropska" && i5.region() == "Plzen mesto" &&
            i5.id() == 78901 && i5.owner() == "CVut");
    i5.next();
    assert (i5.atEnd());

    assert (x.del("Liberec", "Evropska"));
    assert (x.del("Plzen mesto", 78901));
    assert (x.count("cvut") == 2);
    CIterator i6 = x.listByOwner("cVuT");
    assert (!i6.atEnd() && i6.city() == "Prague" && i6.addr() == "Thakurova" && i6.region() == "Dejvice" &&
            i6.id() == 12345 && i6.owner() == "CVUT");
    i6.next();
    assert (!i6.atEnd() && i6.city() == "Prague" && i6.addr() == "Technicka" && i6.region() == "Dejvice" &&
            i6.id() == 9873 && i6.owner() == "CVUT");
    i6.next();
    assert (i6.atEnd());

    assert (x.add("Liberec", "Evropska", "Librec", 4552));
}

static void test1()
{
    CLandRegister x;
    std::string owner;

    assert (x.add("Prague", "Thakurova", "Dejvice", 12345));
    assert (x.add("Prague", "Evropska", "Vokovice", 12345));
    assert (x.add("Prague", "Technicka", "Dejvice", 9873));
    assert (!x.add("Prague", "Technicka", "Hradcany", 7344));
    assert (!x.add("Brno", "Bozetechova", "Dejvice", 9873));
    assert (!x.getOwner("Prague", "THAKUROVA", owner));
    assert (!x.getOwner("Hradcany", 7343, owner));
    CIterator i0 = x.listByAddr();
    assert (!i0.atEnd() && i0.city() == "Prague" && i0.addr() == "Evropska" && i0.region() == "Vokovice" &&
            i0.id() == 12345 && i0.owner() == "");
    i0.next();
    assert (!i0.atEnd() && i0.city() == "Prague" && i0.addr() == "Technicka" && i0.region() == "Dejvice" &&
            i0.id() == 9873 && i0.owner() == "");
    i0.next();
    assert (!i0.atEnd() && i0.city() == "Prague" && i0.addr() == "Thakurova" && i0.region() == "Dejvice" &&
            i0.id() == 12345 && i0.owner() == "");
    i0.next();
    assert (i0.atEnd());

    assert (x.newOwner("Prague", "Thakurova", "CVUT"));
    assert (!x.newOwner("Prague", "technicka", "CVUT"));
    assert (!x.newOwner("prague", "Technicka", "CVUT"));
    assert (!x.newOwner("dejvice", 9873, "CVUT"));
    assert (!x.newOwner("Dejvice", 9973, "CVUT"));
    assert (!x.newOwner("Dejvice", 12345, "CVUT"));
    assert (x.count("CVUT") == 1);
    CIterator i1 = x.listByOwner("CVUT");
    assert (!i1.atEnd() && i1.city() == "Prague" && i1.addr() == "Thakurova" && i1.region() == "Dejvice" &&
            i1.id() == 12345 && i1.owner() == "CVUT");
    i1.next();
    assert (i1.atEnd());

    assert (!x.del("Brno", "Technicka"));
    assert (!x.del("Karlin", 9873));
    assert (x.del("Prague", "Technicka"));
    assert (!x.del("Prague", "Technicka"));
    assert (!x.del("Dejvice", 9873));
}

int main()
{
    test0();
    test1();
    return EXIT_SUCCESS;
}

#endif /* __PROGTEST__ */
