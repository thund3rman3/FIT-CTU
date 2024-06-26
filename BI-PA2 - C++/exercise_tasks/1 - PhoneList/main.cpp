#ifndef __PROGTEST__

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <array>
#include <cassert>

using namespace std;
#endif /* __PROGTEST__ */

struct SPerson
{
    std::string name;
    std::string surname;
    std::string number;
};

bool report(const string &fileName, ostream &out)
{
    std::ifstream ifs(fileName);
    if (!ifs.is_open())
        return false;

    std::vector<SPerson> list;
    SPerson person;
    std::string rest;
    std::string line;
    while (ifs.good()) {
        std::getline(ifs,line);
        if(line.empty())
            break;
        std::istringstream iss(line);

        if(!(iss >> person.name >> person.surname >> person.number))
            return false;
        if(iss >> rest || person.number[0] == '0' || person.number.length() != 9)
            return false;
        int numberInt;
        std::istringstream issNum(person.number);
        issNum >> numberInt;
        if(!issNum.eof() || issNum.fail() || numberInt < 0)
            return false;
        list.emplace_back(person);
    }
    std::string query;
    while(ifs.good()){
        std::getline(ifs,line);
        if(line.empty())
            break;
        std::istringstream iss(line);
        size_t cnt = 0;
        if(!(iss >> query) || iss >> rest)
            return false;
        for(const auto& p: list){
            if(p.surname == query || p.name == query){
                out << p.name << " " << p.surname << " " << p.number << std::endl;
                cnt++;
            }
        }
        out << "-> " << cnt << std::endl;
    }
    return true;
}

#ifndef __PROGTEST__

int main()
{
    ostringstream oss;
    oss.str("");
    assert (report("../tests/test0_in.txt", oss) == true);
    assert (oss.str() == "John Christescu 258452362\n"
                         "John Harmson 861647702\n"
                         "-> 2\n"
                         "-> 0\n"
                         "Josh Dakhov 264112084\n"
                         "Dakhov Speechley 865216101\n"
                         "-> 2\n"
                         "John Harmson 861647702\n"
                         "-> 1\n");
    oss.str("");
    assert (report("../tests/test1_in.txt", oss) == false);
    return 0;
}

#endif /* __PROGTEST__ */
