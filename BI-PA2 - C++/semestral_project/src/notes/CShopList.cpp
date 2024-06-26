/**
 * @author Josef Jech
 * @date 9.6.23
 */

#include "CShopList.h"

#include <utility>


CShopList::CShopList(std::string name, CMenu *menu, std::map<std::string, Item> to_buy,
                     const std::shared_ptr<CFileManager> &parent)
        : CNote(std::move(name), menu, parent),
          m_mToBuy(std::move(to_buy))
{}

CShopList::CShopList(std::string name, CMenu *menu, const CDate &date, std::map<std::string, Item> to_buy)
        : CNote(std::move(name), menu, date),
          m_mToBuy(std::move(to_buy))
{}

CShopList::CShopList(const CShopList &sl)
        : CNote(sl)
{
    m_sTags = sl.m_sTags;
    m_mToBuy = sl.m_mToBuy;
}

CShopList &CShopList::operator=(const CShopList &sl)
{
    if (&sl == this)
        return *this;

    m_file_name = sl.m_file_name;
    m_creation_date = sl.m_creation_date;
    m_sTags = sl.m_sTags;
    m_mToBuy = sl.m_mToBuy;
    return *this;
}

void CShopList::show()
{
    m_dLogOps.emplace_back(SHOW);
    werase(m_menu->get_win());
    box(m_menu->get_win(), 0, 0);
    int i = 0;

    mvwprintw(m_menu->get_win(), i++, 1, "%.*s", m_menu->get_xMax(), (m_file_name + " - " + get_date()).c_str());
    mvwprintw(m_menu->get_win(), i++, 1, "%.*s", m_menu->get_xMax(), TAGS.c_str());
    for (const auto &x: m_sTags)
        mvwprintw(m_menu->get_win(), i++, 2, "%.*s", m_menu->get_xMax() - 1, x.get_tag().c_str());

    mvwprintw(m_menu->get_win(), i++, 1, "%.*s", m_menu->get_xMax(), BUY.c_str());
    for (const auto &x: m_mToBuy)
    {
        std::stringstream sstr;
        sstr << x.first << "- price: " << x.second.price << ", count: " << x.second.count;
        mvwprintw(m_menu->get_win(), i++, 2, "%.*s", m_menu->get_xMax() - 1, sstr.str().c_str());
    }
    wrefresh(m_menu->get_win());
    m_menu->set_curr_item(shared_from_this());
}

bool CShopList::edit(const std::string &item_name, const Item &item)
{
    auto it = m_mToBuy.find(item_name);

    if (it != m_mToBuy.end())
    {
        std::stringstream sstr;
        sstr << (item_name + EDIT_OK) << ": " << item.price << " " << item.count;
        m_dLogOps.emplace_back(sstr.str());
        it->second.price = item.price;
        it->second.count = item.count;
        return true;
    } else
    {
        m_dLogOps.emplace_back(item_name + EDIT_NOT);
        return false;
    }
}

bool CShopList::add_item(const std::string &item_name, const Item &item)
{
    if (m_mToBuy.find(item_name) == m_mToBuy.end() && item.price > 0 && item.count > 0)
    {
        m_dLogOps.emplace_back(item_name + ADDED);
        m_mToBuy[item_name].price = item.price;
        m_mToBuy[item_name].count = item.count;
        return true;
    } else
    {
        m_dLogOps.emplace_back(item_name + ADD_NOT);
        return false;
    }
}

bool CShopList::del_item(const std::string &item_name)
{
    if (m_mToBuy.find(item_name) != m_mToBuy.end())
    {
        m_dLogOps.emplace_back(item_name + DELETED);
        m_mToBuy.erase(item_name);
        return true;
    } else
    {
        m_dLogOps.emplace_back(item_name + DEL_NOT);
        return false;
    }
}

void CShopList::handle_input(int c)
{
    int choice = 0;
    switch (c)
    {
        case 27:
            m_menu->set_curr_item(get_parent());
            break;
        case 'e':
            m_file_menu.draw_menu_items();
            choice = m_file_menu.draw_menu();
            do_file_func(choice);
        default:
            break;
    }
}

void CShopList::do_file_func(int choice)
{
    switch (choice)
    {
        case 0: //show edit history
            show_log();
            break;
        case 1: //change note name
            change_name();
            break;
        case 2: //edit
            show_edit_menu();
            break;
        case 3: //add tag
            read_tags();
            break;
        case 4: //del tag
            delete_tags();
            break;
        default:
            break;
    }
}

void CShopList::read_tags()
{
    echo();
    curs_set(1);
    char tag[tag_note_len];
    std::string tag_string;
    do
    {
        werase(m_menu->get_win());
        box(m_menu->get_win(), 0, 0);
        mvwprintw(m_menu->get_win(), 0, 1, "%s", PRESS_ADD.c_str());
        mvwprintw(m_menu->get_win(), 1, 1, "%s", ADD_TAG.c_str());
        wrefresh(m_menu->get_win());
        mvwgetnstr(m_menu->get_win(), 2, 1, tag, tag_note_len);
        tag_string = tag;
        if (tag_string.empty())
            break;
        add_tag(tag_string);
        tag_string = "";
    } while (true);
    noecho();
    curs_set(0);
    m_menu->get_curr_item()->show();
}

void CShopList::delete_tags()
{
    echo();
    curs_set(1);
    char tag[tag_note_len];
    std::string tag_string;

    while (true)
    {
        werase(m_menu->get_win());
        box(m_menu->get_win(), 0, 0);
        mvwprintw(m_menu->get_win(), 0, 1, "%s", PRESS_DEL.c_str());
        mvwprintw(m_menu->get_win(), 1, 1, "%s", DEL_TAG.c_str());
        wrefresh(m_menu->get_win());
        mvwgetnstr(m_menu->get_win(), 2, 1, tag, tag_note_len);
        tag_string = tag;
        if (tag_string.empty())
            break;
        del_tag(tag_string);
        tag_string = "";
    }
    noecho();
    curs_set(0);
    m_menu->get_curr_item()->show();
}

void CShopList::add_items()
{
    char item_name[tag_note_len];
    Item item;
    std::string item_name_str;
    bool ok = false;

    read_item(item_name, item);
    item_name_str = item_name;

    if (!item_name_str.empty())
    {
        ok = add_item(item_name_str, item);
        while (true)
        {
            read_next_item(item_name, item, ok);
            item_name_str = item_name;
            if (item_name_str.empty())
                break;
            ok = add_item(item_name_str, item);
        }
    }
}

void CShopList::read_item(char *item_name, Item &item)
{
    werase(m_menu->get_win());
    box(m_menu->get_win(), 0, 0);
    mvwprintw(m_menu->get_win(), 0, 1, "%s", PRESS_ADD.c_str());
    mvwprintw(m_menu->get_win(), 1, 1, "%s", ITEM_NAME.c_str());
    wrefresh(m_menu->get_win());
    mvwgetnstr(m_menu->get_win(), 2, 1, item_name, tag_note_len);
    std::string item_name_str = item_name;
    if (!item_name_str.empty())
    {
        mvwprintw(m_menu->get_win(), 3, 1, "%s", ENTER_CNT.c_str());
        wrefresh(m_menu->get_win());
        mvwscanw(m_menu->get_win(), 4, 1, "%d", &item.count);

        mvwprintw(m_menu->get_win(), 5, 1, "%s", ENTER_P.c_str());
        wrefresh(m_menu->get_win());
        mvwscanw(m_menu->get_win(), 6, 1, "%d", &item.price);
    }

}
            
void CShopList::read_next_item(char *item_name, Item &item, bool ok)
{
    werase(m_menu->get_win());
    box(m_menu->get_win(), 0, 0);
    mvwprintw(m_menu->get_win(), 0, 1, "%s", PRESS_ADD.c_str());

    if (ok)
        mvwprintw(m_menu->get_win(), 1, 1, "%s", (item_name + ADDED).c_str());
    else
        mvwprintw(m_menu->get_win(), 1, 1, "%s", (item_name + ADD_NOT).c_str());

    mvwprintw(m_menu->get_win(), 2, 1, "%s", ITEM_NAME.c_str());
    wrefresh(m_menu->get_win());
    mvwgetnstr(m_menu->get_win(), 3, 1, item_name, tag_note_len);

    std::string item_name_str = item_name;
    if (!item_name_str.empty())
    {
        mvwprintw(m_menu->get_win(), 4, 1, "%s", ENTER_CNT.c_str());
        wrefresh(m_menu->get_win());
        mvwscanw(m_menu->get_win(), 5, 1, "%d", &item.count);

        mvwprintw(m_menu->get_win(), 6, 1, "%s", ENTER_P.c_str());
        wrefresh(m_menu->get_win());
        mvwscanw(m_menu->get_win(), 7, 1, "%d", &item.price);
    }
}

void CShopList::show_edit_menu()
{
    int choice = 0;
    werase(m_file_menu.get_win());
    box(m_menu->get_win(), 0, 0);
    m_LEM.draw_menu_items();
    while ((choice = m_LEM.draw_menu()) != 27)
    {
        switch (choice)
        {
            case 0: // edit item
                edit_item_from_input();
                return;
            case 1:
                delete_item_from_input();
                return;
            case 2:
                add_item_from_input();
                return;
            default:
                break;
        }
    }
    werase(m_file_menu.get_win());
    box(m_file_menu.get_win(), 0, 0);
    wrefresh(m_file_menu.get_win());

    m_file_menu.draw_menu_items();
    choice = m_file_menu.draw_menu();
    do_file_func(choice);
}

void CShopList::show_log()
{
    werase(m_menu->get_win());
    box(m_menu->get_win(), 0, 0);
    mvwprintw(m_menu->get_win(), 0, 1, "%s", LOG.c_str());
    for (size_t i = 1; i <= m_dLogOps.size(); ++i)
    {
        mvwprintw(m_menu->get_win(), (int) i, 1, "%.*s", m_menu->get_xMax(), m_dLogOps[i - 1].c_str());
    }
    wrefresh(m_menu->get_win());
}

void CShopList::change_name()
{
    char name[tag_note_len];
    read_name(name);
    m_file_name = name;
    m_dLogOps.emplace_back(NAME_CHANGED + m_file_name);
    curs_set(0);
    noecho();
    m_menu->set_curr_item(shared_from_this());
    m_menu->get_curr_item()->show();
}

void CShopList::read_name(char *name)
{
    werase(m_menu->get_win());
    curs_set(1);
    echo();
    box(m_menu->get_win(), 0, 0);
    mvwprintw(m_menu->get_win(), 1, 1, "%s", NAME.c_str());
    wrefresh(m_menu->get_win());

    mvwgetnstr(m_menu->get_win(), 2, 1, name, tag_note_len);
}

void CShopList::edit_item_from_input()
{
    echo();
    curs_set(1);
    char name[tag_note_len];
    Item item;
    read_item(name, item);
    bool ok = edit(name, item);
    werase(m_menu->get_win());
    box(m_menu->get_win(), 0, 0);

    if (ok)
        mvwprintw(m_menu->get_win(), 1, 1, "%s", (name + EDIT_OK).c_str());
    else
        mvwprintw(m_menu->get_win(), 1, 1, "%s", (name + EDIT_NOT).c_str());
    wrefresh(m_menu->get_win());
    curs_set(0);
    noecho();
    m_menu->set_curr_item(shared_from_this());
    m_menu->get_curr_item()->show();
}

void CShopList::delete_item_from_input()
{
    curs_set(1);
    echo();
    char item_name[tag_note_len];
    read_name(item_name);
    bool ok = del_item(item_name);

    werase(m_menu->get_win());
    box(m_menu->get_win(), 0, 0);

    if (ok)
        mvwprintw(m_menu->get_win(), 1, 1, "%s", (item_name + DELETED).c_str());
    else
        mvwprintw(m_menu->get_win(), 1, 1, "%s", (item_name + DEL_NOT).c_str());
    wrefresh(m_menu->get_win());
    curs_set(0);
    noecho();
    m_menu->set_curr_item(shared_from_this());
    m_menu->get_curr_item()->show();
}

void CShopList::add_item_from_input()
{
    curs_set(1);
    echo();
    char item_name[tag_note_len];
    Item item;
    std::string item_name_str;

    read_item(item_name, item);
    item_name_str = item_name;

    bool ok = add_item(item_name_str, item);
    werase(m_menu->get_win());
    box(m_menu->get_win(), 0, 0);

    if (ok)
        mvwprintw(m_menu->get_win(), 1, 1, "%s", (item_name_str + ADDED).c_str());
    else
        mvwprintw(m_menu->get_win(), 1, 1, "%s", (item_name_str + ADD_NOT).c_str());
    wrefresh(m_menu->get_win());
    curs_set(0);
    noecho();
    m_menu->set_curr_item(shared_from_this());
    m_menu->get_curr_item()->show();
}

bool CShopList::find_string(const std::string &str) const
{
    if (get_date() == str || get_name() == str)
        return true;
    if (m_sTags.find(CTag(str)) != m_sTags.end() || m_mToBuy.find(str) != m_mToBuy.end())
        return true;

    return false;
}

std::string CShopList::format_to_export() const
{
    std::stringstream sstr;
    sstr << "2" << std::endl << get_name() << std::endl << m_creation_date << std::endl;
    export_tags(sstr);
    for (const auto &x: m_mToBuy)
    {
        sstr << x.first << ' ' << x.second.price << ' ' << x.second.count << std::endl;
    }
    sstr << std::endl;
    return sstr.str();
}





