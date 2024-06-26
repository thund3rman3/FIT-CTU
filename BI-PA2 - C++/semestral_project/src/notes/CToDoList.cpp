/**
 * @author Josef Jech
 * @date 9.6.23
 */

#include "CToDoList.h"

#include <utility>

CToDoList::CToDoList(std::string name, CMenu *menu, std::map<std::string, bool> to_do,
                     const std::shared_ptr<CFileManager> &parent)
        : CNote(std::move(name), menu, parent),
          m_mToDo(std::move(to_do))
{}

CToDoList::CToDoList(std::string name, CMenu *menu, const CDate &date, std::map<std::string, bool> to_do)
        : CNote(std::move(name), menu, date),
          m_mToDo(std::move(to_do))
{}

CToDoList::CToDoList(const CToDoList &tdl)
        : CNote(tdl)
{
    m_mToDo = tdl.m_mToDo;
    m_sTags = tdl.m_sTags;
}

CToDoList &CToDoList::operator=(const CToDoList &tdl)
{
    if (&tdl == this)
        return *this;
    m_file_name = tdl.m_file_name;
    m_creation_date = tdl.m_creation_date;
    m_mToDo = tdl.m_mToDo;
    m_sTags = tdl.m_sTags;
    return *this;
}

void CToDoList::show()
{
    m_dLogOps.emplace_back(SHOW);
    werase(m_menu->get_win());
    box(m_menu->get_win(), 0, 0);
    int i = 0;

    mvwprintw(m_menu->get_win(), i++, 1, "%.*s", m_menu->get_xMax(), (m_file_name + " - " + get_date()).c_str());
    mvwprintw(m_menu->get_win(), i++, 1, "%.*s", m_menu->get_xMax(), TAGS.c_str());
    for (const auto &x: m_sTags)
        mvwprintw(m_menu->get_win(), i++, 2, "%.*s", m_menu->get_xMax() - 1, x.get_tag().c_str());

    mvwprintw(m_menu->get_win(), i++, 1, "%.*s", m_menu->get_xMax(), TODOS.c_str());

    for (const auto &x: m_mToDo)
    {
        if (x.second)
            mvwprintw(m_menu->get_win(), i++, 2, "%.*s", m_menu->get_xMax() - 1, (x.first + DONE).c_str());
        else
            mvwprintw(m_menu->get_win(), i++, 2, "%.*s", m_menu->get_xMax() - 1, (x.first + UNDONE).c_str());
    }
    wrefresh(m_menu->get_win());
    m_menu->set_curr_item(shared_from_this());
}

bool CToDoList::edit(const std::string &item_name)
{
    if (m_mToDo.find(item_name) != m_mToDo.end())
    {
        m_dLogOps.emplace_back(item_name + EDIT);
        m_mToDo[item_name] = !m_mToDo[item_name];
        return true;
    }
    return false;
}

bool CToDoList::add_todo(const std::string &todo)
{
    if (m_mToDo.find(todo) == m_mToDo.end())
    {
        m_dLogOps.emplace_back(todo + ADDED);
        m_mToDo[todo] = false;
        return true;
    } else
    {
        m_dLogOps.emplace_back(todo + ADD_NOT);
        return false;
    }
}

bool CToDoList::del_todo(const std::string &todo)
{
    if (m_mToDo.find(todo) != m_mToDo.end())
    {
        m_dLogOps.emplace_back(todo + DELETED);
        m_mToDo.erase(todo);
        return true;
    } else
    {
        m_dLogOps.emplace_back(todo + DEL_NOT);
        return false;
    }
}

void CToDoList::handle_input(int c)
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

void CToDoList::delete_tags()
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

void CToDoList::read_tags()
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

void CToDoList::add_todos()
{
    char todo[tag_note_len];
    std::string todo_str;
    bool ok = false;

    werase(m_menu->get_win());
    box(m_menu->get_win(), 0, 0);
    mvwprintw(m_menu->get_win(), 0, 1, "%s", PRESS_ADD.c_str());
    mvwprintw(m_menu->get_win(), 1, 1, "%s", ADD_TODO.c_str());

    wrefresh(m_menu->get_win());
    mvwgetnstr(m_menu->get_win(), 2, 1, todo, tag_note_len);
    todo_str = todo;
    if (!todo_str.empty())
    {
        ok = add_todo(todo_str);
        while (true)
        {
            werase(m_menu->get_win());
            box(m_menu->get_win(), 0, 0);
            mvwprintw(m_menu->get_win(), 0, 1, "%s", PRESS_ADD.c_str());
            if (ok)
                mvwprintw(m_menu->get_win(), 1, 1, "%s", (todo_str + ADDED).c_str());
            else
                mvwprintw(m_menu->get_win(), 1, 1, "%s", (todo_str + ADD_NOT).c_str());
            mvwprintw(m_menu->get_win(), 2, 1, "%s", ADD_TODO.c_str());
            wrefresh(m_menu->get_win());
            mvwgetnstr(m_menu->get_win(), 3, 1, todo, tag_note_len);
            todo_str = todo;
            if (todo_str.empty())
                break;
            ok = add_todo(todo_str);
        }
    }
}

void CToDoList::do_file_func(int choice)
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


void CToDoList::show_log()
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

void CToDoList::read_name(char *name)
{
    werase(m_menu->get_win());
    curs_set(1);
    echo();
    box(m_menu->get_win(), 0, 0);
    mvwprintw(m_menu->get_win(), 1, 1, "%s", NAME.c_str());
    wrefresh(m_menu->get_win());

    mvwgetnstr(m_menu->get_win(), 2, 1, name, tag_note_len);
}

void CToDoList::change_name()
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

void CToDoList::show_edit_menu()
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

void CToDoList::edit_item_from_input()
{
    echo();
    curs_set(1);
    char name[tag_note_len];
    read_name(name);
    bool ok = edit(name);
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

void CToDoList::delete_item_from_input()
{
    curs_set(1);
    echo();
    char item_name[tag_note_len];
    read_name(item_name);
    bool ok = del_todo(item_name);

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

void CToDoList::add_item_from_input()
{
    curs_set(1);
    echo();
    char item_name[tag_note_len];
    std::string item_name_str;

    read_name(item_name);
    item_name_str = item_name;

    bool ok = add_todo(item_name_str);
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

bool CToDoList::find_string(const std::string &str) const
{
    if (get_date() == str || get_name() == str)
        return true;
    if (m_sTags.find(CTag(str)) != m_sTags.end() || m_mToDo.find(str) != m_mToDo.end())
        return true;

    return false;
}

std::string CToDoList::format_to_export() const
{
    std::stringstream sstr;
    sstr << "1" << std::endl << get_name() << std::endl << m_creation_date << std::endl;
    export_tags(sstr);
    for (const auto &x: m_mToDo)
    {
        sstr << x.first << ' ' << x.second << std::endl;
    }
    sstr << std::endl;
    return sstr.str();
}



