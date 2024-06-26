#ifndef __PROGTEST__

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <algorithm>
#include <functional>
#include <memory>
#include <stdexcept>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cctype>
#include <cstdint>
#include <cassert>

using namespace std;
#endif /* __PROGTEST__ */

class CFileType {
protected:
    std::string specificToFiles;
    std::string name;
    size_t len = 0;
public:
    explicit CFileType(std::string spec, size_t len)
            : specificToFiles(std::move(spec)), len(len) {
    }

    CFileType() = default;

    virtual ~CFileType() = default;

    virtual size_t Size() const {
        return len;
    }

    std::string getName() const {
        return name;
    }

    virtual std::shared_ptr<CFileType> clone() const = 0;

    void setName(std::string newName){
        name = std::move(newName);
    }

//    virtual CFileType *findFolder(const string &fileName){
//        return nullptr;
//    }

    virtual void print(std::ostream& os) const = 0;

//    virtual void del(const std::string& str){
//    }
};

class CFile : public CFileType {
public:
    CFile(const std::string& hash, size_t fs);

    CFile &Change(std::string newHash, size_t fs);

    void print(std::ostream& os) const override;

    std::shared_ptr<CFileType> clone() const override{
        return std::make_shared<CFile>(*this);
    }
};

CFile::CFile(const std::string& hash, size_t fs)
        : CFileType(hash, fs) {
}

CFile &CFile::Change(std::string newHash, size_t fs) {
    specificToFiles = std::move(newHash);
    len = fs;
    return *this;
}

//"1623\tfile.txt jhwadkhawkdhajwdhawhdaw=\n"
void CFile::print(std::ostream &os) const {
    os << Size() << "\t" << name << " " << specificToFiles << std::endl;
}

class CLink : public CFileType {
public:
    explicit CLink(const std::string& newPath);

    CLink &Change(std::string newPath);

    void print(std::ostream& os) const override;

    std::shared_ptr<CFileType> clone() const override{
        return std::make_shared<CLink>(*this);
    }
};

CLink::CLink(const std::string& newPath)
        : CFileType(newPath, newPath.length() + 1) {
}

CLink &CLink::Change(std::string newPath) {
    specificToFiles = std::move(newPath);
    len = specificToFiles.length() + 1;
    return *this;
}

//"9\tfile.ln -> file.txt\n"
void CLink::print(std::ostream &os) const {
    os << Size() << "\t" << name << " -> " << specificToFiles << std::endl;
}

class CDirectory : public CFileType {
private:
    std::vector<std::shared_ptr<CFileType>> folder;
public:
    CDirectory() = default;

    CDirectory(const CDirectory& dir){
        name = dir.name;
//        folder.reserve(dir.folder.size());
//        for (size_t i = 0; i < dir.folder.size(); ++i) {
//            folder[i] = dir.folder[i]->clone();
//        }
        for (const auto &file: dir.folder)
        {
            folder.emplace_back(file);
        }
    }

    CDirectory& operator=(const CDirectory& dir){
        if(this == &dir)
            return *this;
        name = dir.name;
        folder.clear();
//        folder.reserve(dir.folder.size());
//        for (size_t i = 0; i < dir.folder.size(); ++i) {
//            folder[i] = dir.folder[i]->clone();
//        }
        for (const auto &file: dir.folder)
        {
            folder.emplace_back(file);
        }
        return *this;
    }

    size_t Size() const override;

    CDirectory &Change(const std::string &fileName, const CFileType &file);

    CDirectory &Change(const std::string &fileName, const CFileType *file);

    const CFileType &Get(const std::string &fileName) const;

    CFileType &Get(const std::string &fileName);

    //CFileType* findFolder(const std::string &fileName) override;

    friend std::ostream& operator<<(std::ostream &os, const CDirectory &dir);

    void print(std::ostream& os) const override{
        os << Size() << "\t" << name << "/"<< std::endl;
    }

    std::shared_ptr<CFileType> clone() const override{
        auto dir = std::make_shared<CDirectory>(*this);
//        dir->folder.clear();
//        for (const auto& item : folder) {
//            dir->folder.push_back(item->clone());
//        }
        return dir;
    }

//    void del(const std::string& str) override{
//
//    }

};

//CDirectory::Size() - vrátí velikost složky, která je rovna součtu velikostí souborů
// (vč. adresářů rekurzivně)a počtu znaků v jejich názvech.
size_t CDirectory::Size() const {
    size_t res = 0;
    for (const auto &file: folder) {
        res += file->Size() + file->getName().length();
    }
    return res;
}

//CDirectory::Change( filename, file ) - přidá nebo nahradí v adresáři soubor file s názvem filename,
// volání lze řetězit
//CDirectory::Change( filename, nullptr ) - odstraní z adresáře soubor s názvem filename,
// volání lze řetězit
CDirectory &CDirectory::Change(const string &fileName, const CFileType &file) {
    auto toFind = file.clone();
    toFind->setName(fileName);
    auto it = std::lower_bound(folder.begin(), folder.end(), toFind,
                               [](const std::shared_ptr<CFileType> &a, const std::shared_ptr<CFileType> &b) {
                                   return a->getName() < b->getName();
                               });

    if(it == folder.end())
        folder.emplace_back(toFind);
    else if(it->get()->getName() == fileName){
        folder.erase(it);
        auto in = std::lower_bound(folder.begin(), folder.end(), toFind,
                                   [](const std::shared_ptr<CFileType> &a, const std::shared_ptr<CFileType> &b) {
                                       return a->getName() < b->getName();
                                   });
        folder.insert(in, toFind);
    }
    else
        folder.insert(it, toFind);

    return *this;
}

CDirectory &CDirectory::Change(const string &fileName, const CFileType *file) {
    if (!file) {
        for (auto it = folder.begin(); it != folder.end(); ++it) {
            if (it->get()->getName() == fileName) {
                folder.erase(it);
                break;
            }
        }
    }
    return *this;
}

//CDirectory::Get( filename ) - vrátí z adresáře referenci na soubor s názvem filename
// (vyžaduje i variantu s const, neprohledává rekurzivně), pokud soubor filename v adresáři neexistuje,
// vyhoďte výjimku std::out_of_range
const CFileType &CDirectory::Get(const string &fileName) const {
    for (auto it = folder.begin(); it != folder.end(); ++it) {
        if((*it)->getName() == fileName)
            return *(*it);
    }
    throw std::out_of_range("bumm bum paw");
}

CFileType &CDirectory::Get(const string &fileName) {
    for (auto it = folder.begin(); it != folder.end(); ++it) {
        if(it->get()->getName() == fileName)
            return *(*it);
//        CFileType* res = (*it)->findFolder(fileName);
//        if(res)
//            return *res;
    }
    throw std::out_of_range("bumm bum paw");
}

//CFileType *CDirectory::findFolder(const string &fileName){
//    for (auto it = folder.begin(); it != folder.end(); ++it) {
//        if(it->get()->getName() == fileName)
//            return it->get();
//        CFileType* res = (*it)->findFolder(fileName);
//        if(res)
//            return res;
//    }
//    return nullptr;
//}

//(formát výpisu je velikost souboru,
// tabulátor, název souboru a další parametry dle druhu souboru, viz ukázka
// , soubory ve výpisu jsou řazeny dle názvu souboru)
std::ostream& operator<<(ostream &os, const CDirectory &dir) {
    for (const auto& elem: dir.folder) {
        elem->print(os);
    }
    return os;
}

#ifndef __PROGTEST__

int main() {
    CDirectory root;
    stringstream sout;

    root.Change("file.txt", CFile("jhwadkhawkdhajwdhawhdaw=", 1623)).Change("file.ln",
                                                                            CLink("").Change("file.txt")).Change(
            "folder", CDirectory().Change("fileA.txt", CFile("", 0).Change("skjdajdakljdljadkjwaljdlaw=", 1713)).Change(
                    "fileB.txt", CFile("kadwjkwajdwhoiwhduqwdqwuhd=", 71313)).Change("fileC.txt",
                                                                                     CFile("aihdqhdqudqdiuwqhdquwdqhdi=",
                                                                                           8193)));

    sout.str("");
    sout << root;
    assert(sout.str() == "9\tfile.ln -> file.txt\n"
                         "1623\tfile.txt jhwadkhawkdhajwdhawhdaw=\n"
                         "81246\tfolder/\n");
    assert(root.Size() == 82899);

    string filename = "folder";
    const CDirectory &inner = dynamic_cast<const CDirectory &>( root.Get(filename));

    sout.str("");
    sout << inner;
    assert(sout.str() == "1713\tfileA.txt skjdajdakljdljadkjwaljdlaw=\n"
                         "71313\tfileB.txt kadwjkwajdwhoiwhduqwdqwuhd=\n"
                         "8193\tfileC.txt aihdqhdqudqdiuwqhdquwdqhdi=\n");
    assert(inner.Size() == 81246);

//    CDirectory h;
//    h.Change("file.txt", CFile("jhwadkhawkdhajwdhawhdaw=", 1623))
//    .Change("file.ln",CLink("").Change("file.txt"))
//    .Change("folder", CDirectory()
//    .Change("fileA.txt", CFile("", 0)
//    .Change("skjdajdakljdljadkjwaljdlaw=", 1713))
//    .Change("fileB.txt", CFile("kadwjkwajdwhoiwhduqwdqwuhd=", 71313))
//    .Change("fileC.txt",CFile("aihdqhdqudqdiuwqhdquwdqhdi=",8193))
//    .Change("folder2", CDirectory()
//            .Change("fileA.txt2", CFile("", 0)
//                    .Change("2", 1713))
//            .Change("fileB.txt2", CFile("2=", 71313))
//            .Change("fileC.txt2",CFile("2=",8193))
//            .Change("folder3", CDirectory()
//                    .Change("fileA.txt3", CFile("", 0)
//                            .Change("3", 1713))
//                    .Change("fileB.txt3", CFile("3=", 71313))
//                    .Change("fileC.txt3",CFile("3=",8193)))));
//    std::cout << h;
//    const CDirectory &aaa = dynamic_cast<const CDirectory &>( h.Get("folder2"));
//    std::cout << aaa;
//    const CDirectory &bbb = dynamic_cast<const CDirectory &>( h.Get("folder"));
//    std::cout << bbb;
//    std::cout << h;
//    h.Change("fileA.txt3", CFile("3=",8193));
//
//    CDirectory z(root);
//    sout.str("");
//    sout << z;
//    assert(sout.str() == "9\tfile.ln -> file.txt\n"
//                         "1623\tfile.txt jhwadkhawkdhajwdhawhdaw=\n"
//                         "81246\tfolder/\n");
//    assert(z.Size() == 82899);
//
//    string dd = "folder";
//    const CDirectory &ee = dynamic_cast<const CDirectory &>( z.Get(dd));
//
//    sout.str("");
//    sout << ee;
//    assert(sout.str() == "1713\tfileA.txt skjdajdakljdljadkjwaljdlaw=\n"
//                         "71313\tfileB.txt kadwjkwajdwhoiwhduqwdqwuhd=\n"
//                         "8193\tfileC.txt aihdqhdqudqdiuwqhdquwdqhdi=\n");
//    assert(ee.Size() == 81246);
//    z = h;
//    std::cout<<"-------------------------"<<std::endl;
//    std::cout << z;
//    const CDirectory &mm = dynamic_cast<const CDirectory &>( z.Get("folder2"));
//    std::cout << mm;
//    const CDirectory &nn = dynamic_cast<const CDirectory &>( z.Get("folder"));
//    std::cout << nn;
//    std::cout << z;
    return 0;
}

#endif /* __PROGTEST__ */
