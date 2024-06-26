/**
 * @author Josef Jech
 * @date 9.6.23
 */

#include "CTag.h"

CTag::CTag(std::string name)
        : m_tag_name(std::move(name))
{}

std::string CTag::get_tag() const
{
    return m_tag_name;
}



