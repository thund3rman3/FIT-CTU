#include<iostream>
#include<cassert>
#include<vector>
#include<set>
#include<map>
#include<string>
#include<unordered_map>
#include<memory>
#include<unordered_set>
#include<queue>
#include<list>
#include<sstream>
#include<cstring>

using namespace std;

class CPkg{
public:
    //TODO constructor
    CPkg(string str)
        : name(std::move(str))
    {}

    ~CPkg() = default;
    //TODO addDep(string)
    CPkg& addDep(const string& dep)
    {
        deps.emplace(dep);
        return *this;
    }

    string get_name() const
    {
        return name;
    }
    set<string> deps;
private:
    string name;

};
class CPkgSys{
public:
    //TODO addPkg
    CPkgSys& addPkg(const CPkg& pack)
    {
        m_available.emplace(pack.get_name(),pack);
        return *this;
    }

    int check(const string& str) const
    {
        if(m_installed.find(str) != m_installed.end())
            return 0;
        else if(m_available.find(str) != m_available.end())
            return 1;
        else return -1;
    }

    //TODO install(list<string> )
    set<string> install(const list<string>& packs)
    {
        for (const auto& x: packs) {
            if(check(x) == -1)
                throw invalid_argument("Package not found.");
        }
        set<string> RES;// return balicky pro instalaci a pridaji se do installed
        for (const auto& x: packs) {
            set<string> visited;
            deque<string> next_to_visit;

            int r = check(x);
            if(r == 1)
            {
                visited.emplace(x);
                next_to_visit.push_back(x);
            }

            while(!next_to_visit.empty())
            {
                string elem = next_to_visit.front();
                next_to_visit.pop_front();

                auto itt = m_available.find(elem);
                for(const auto& dep: itt->second.deps)
                {
                    int ch = check(dep);
                    if(ch == 1 && visited.count(dep) == 0)
                    {
                        visited.emplace(dep);
                        next_to_visit.push_back(dep);
                    }
                    else if(ch==-1)
                        throw invalid_argument("Package not found.");
                }
            }
            for (const auto& y: visited) {
                RES.emplace(y);
            }
        }
        for (const auto& q: RES) {
            m_installed.emplace(q);
        }

        return RES;
    }
    //TODO operator <<
    friend ostream& operator<<(ostream& os, const CPkgSys& sys)
    {
        bool first = true;
        for (const auto& x: sys.m_installed) {
            if(first)
            {
                os<<x;
                first = false;
            }
            else
                os<<", "<<x;
        }
        return os;
    }
private:
    map<string,CPkg> m_available;
    set<string> m_installed;
};

int main(){
    CPkgSys s;
    stringstream ss;
    s.addPkg(CPkg("ssh").addDep("sudo").addDep("apt"))
            .addPkg(CPkg("sudo").addDep("git").addDep("c++"));
    s.addPkg(CPkg("apt"))
            .addPkg(CPkg("c++").addDep("c").addDep("asm").addDep("fortran"));
    s.addPkg(CPkg("git"))
            .addPkg(CPkg("c").addDep("kekw"))
            .addPkg(CPkg("kekw"))
            .addPkg(CPkg("asm"))
            .addPkg(CPkg("fortran"));
    s.addPkg(CPkg("python").addDep("bash").addDep("sadge"))
            .addPkg(CPkg("karel").addDep("python"))
            .addPkg(CPkg("bash").addDep("sadge"))
            .addPkg(CPkg("sadge"))
            .addPkg(CPkg("cython").addDep("dev"));
    s.addPkg(CPkg("perl"));

    ss << s;
    assert(ss.str() == "");
    ss.clear();
    ss.str("");

    set<string> t1 = s.install(list<string> {"sudo"});
    assert(t1 == (set<string> {"asm", "c", "c++", "fortran", "git", "kekw", "sudo"}));
    set<string> t2 = s.install(list<string> {"ssh", "c++"});
    assert(t2 == (set<string> {"apt", "ssh"}));

    ss << s;
    assert(ss.str() == "apt, asm, c, c++, fortran, git, kekw, ssh, sudo");
    ss.clear();
    ss.str("");

    try{
        set<string> e = s.install(list<string> {"karel", "cython"});
        assert("Sem ses nemel dostat debilku" == nullptr);
    }
    catch(const invalid_argument & e){
        assert(strcmp("Package not found.", e.what()) == 0);
    }
    set<string> t3 = s.install(list<string> {"karel", "fortran", "git"});
    assert(t3 == (set<string> {"bash", "karel", "python", "sadge"}));

    s.addPkg(CPkg("java").addDep("utils"))
            .addPkg(CPkg("utils").addDep("VB"))
            .addPkg(CPkg("VB").addDep("java"));

    set<string> t4 = s.install(list<string> {"java", "perl"});
    assert(t4 == (set<string> {"VB", "java", "perl", "utils"}));

    ss << s;
    assert(ss.str() == "VB, apt, asm, bash, c, c++, fortran, git, java, karel, kekw, perl, python, sadge, ssh, sudo, utils");
    ss.clear();
    ss.str("");

    CPkgSys k;
    k.addPkg(CPkg("ssh").addDep("sudo").addDep("apt"))
            .addPkg(CPkg("sudo").addDep("git"));
    k.addPkg(CPkg("apt"));
    k.addPkg(CPkg("git"))
            .addPkg(CPkg("c").addDep("kekw"))
            .addPkg(CPkg("kekw"))
            .addPkg(CPkg("asm"))
            .addPkg(CPkg("fortran"));
    k.addPkg(CPkg("python").addDep("bash").addDep("sadge"))
            .addPkg(CPkg("karel").addDep("python"))
            .addPkg(CPkg("bash").addDep("sadge"))
            .addPkg(CPkg("sadge"));
    k.addPkg(CPkg("perl").addDep("no"));

    set<string> t5 = k.install(list<string> {"asm"});
    assert(t5 == (set<string> {"asm"}));
    set<string> t6 = k.install(list<string> {"python", "ssh"});
    assert(t6 == (set<string> {"apt", "bash", "git", "python", "sadge", "ssh", "sudo"}));

    try{
        set<string> t7 = k.install(list<string> {"perl", "c"});
        assert("Sem ses nemel dostat debilku" == nullptr);
    }
    catch(const invalid_argument & e){
        assert(strcmp("Package not found.", e.what()) == 0);
    }
    set<string> t8 = k.install(list<string> {"c", "ssh", "karel"});

    assert(t8 == (set<string> {"c", "karel", "kekw"}));

    ss << k;
    assert(ss.str() == "apt, asm, bash, c, git, karel, kekw, python, sadge, ssh, sudo");
    ss.clear();
    ss.str("");

}