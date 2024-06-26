/**
 * @author Josef Jech
 * @date 9.6.23
 */

#pragma once

#include <string>

/**
 * Holds tag name
 */
class CTag
{
public:
    /**
     * Constructor
     * @param name - tag name
     */
    explicit CTag(std::string name);

    /**
     * Destructor
     */
    ~CTag() = default;

    /**
     * Getter for tag name
     * @return tag name
     */
    std::string get_tag() const;

private:
    std::string m_tag_name;
};

struct Tag_Cmp
{
    bool operator()(const CTag &first, const CTag &second) const
    {
        return first.get_tag() < second.get_tag();
    }
};