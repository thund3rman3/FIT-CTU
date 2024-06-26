/**
 * @author Josef Jech
 * @date 9.6.23
 */

#include "CNote.h"

#include <utility>

CNote::CNote(std::string name, CMenu *menu, const std::shared_ptr<CFileManager> &parent)
        : CFileManager(std::move(name), menu, parent),
          m_file_menu()
{}

CNote::CNote(std::string name, CMenu *menu, const CDate &date)
        : CFileManager(std::move(name), menu, date)
{}

CNote::CNote(const CNote &note)
        : CFileManager(note), m_file_menu()
{}

void CNote::add_tag(std::string tag)
{
    CTag t(std::move(tag));
    if (m_sTags.find(t) == m_sTags.end())
    {
        m_dLogOps.emplace_back(t.get_tag() + ADDED);
        m_sTags.emplace(t);
    } else
        m_dLogOps.emplace_back(t.get_tag() + ADD_NOT);
}

void CNote::del_tag(std::string tag)
{
    CTag t(std::move(tag));
    if (m_sTags.find(t) != m_sTags.end())
    {
        m_dLogOps.emplace_back(t.get_tag() + DELETED);
        m_sTags.erase(t);
    } else
        m_dLogOps.emplace_back(t.get_tag() + DEL_NOT);
}

void CNote::delete_item()
{
}

void CNote::export_tags(std::stringstream &sstr) const
{
    if (m_sTags.empty())
        sstr << std::endl;
    else
    {
        for (const auto &x: m_sTags)
            sstr << x.get_tag() << ' ';
        sstr << std::endl;
    }
}

void CNote::set_tags(const std::set<CTag, Tag_Cmp> &tags)
{
    m_sTags = tags;
}





