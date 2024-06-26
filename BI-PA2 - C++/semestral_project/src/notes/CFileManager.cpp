/**
 * @author Josef Jech
 * @date 9.6.23
 */

#include "CFileManager.h"

#include <utility>


CFileManager::CFileManager(std::string name, CMenu *menu, const std::shared_ptr<CFileManager> &parent)
        : m_file_name(std::move(name)), m_creation_date(), m_parent(parent), m_menu(menu)
{}

CFileManager::CFileManager(std::string name, CMenu *menu, const CDate &date)
        : m_file_name(std::move(name)), m_creation_date(date), m_menu(menu)
{}


std::string CFileManager::get_name() const
{
    return m_file_name;
}

std::string CFileManager::get_date() const
{
    return m_creation_date.get();
}

std::shared_ptr<CFileManager> CFileManager::get_parent() const
{
    return m_parent;
}

void CFileManager::set_parent(const std::shared_ptr<CFileManager> &parent)
{
    m_parent = std::move(parent);
}











