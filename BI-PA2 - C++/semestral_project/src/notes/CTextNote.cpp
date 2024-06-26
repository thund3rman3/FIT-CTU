/**
 * @author Josef Jech
 * @date 9.6.23
 */

#include "CTextNote.h"

CTextNote::CTextNote(std::string name, std::string text, CMenu *menu, const std::shared_ptr<CFileManager> &parent)
        : CNote(std::move(name), menu, parent), m_text(std::move(text))
{}

CTextNote::CTextNote(std::string name, std::string text, CMenu *menu, const CDate &date)
        : CNote(std::move(name), menu, date), m_text(std::move(text))
{}

CTextNote::CTextNote(const CTextNote &tn)
        : CNote(tn), m_text(tn.m_text)
{
    m_sTags = tn.m_sTags;
}

CTextNote &CTextNote::operator=(const CTextNote &tn)
{
    if (&tn == this)
        return *this;

    m_file_name = tn.m_file_name;
    m_creation_date = tn.m_creation_date;
    m_text = tn.m_text;
    m_sTags = tn.m_sTags;
    return *this;
}

void CTextNote::show()
{
    m_dLogOps.emplace_back(SHOW);
    werase(m_menu->get_win());
    box(m_menu->get_win(), 0, 0);
    int i = 0;

    mvwprintw(m_menu->get_win(), i++, 1, "%.*s", m_menu->get_xMax(), (m_file_name + " - " + get_date()).c_str());
    mvwprintw(m_menu->get_win(), i++, 1, "%.*s", m_menu->get_xMax(), TAGS.c_str());
    for (const auto &x: m_sTags)
        mvwprintw(m_menu->get_win(), i++, 2, "%.*s", m_menu->get_xMax() - 1, x.get_tag().c_str());
    mvwprintw(m_menu->get_win(), i++, 1, "%.*s", m_menu->get_xMax(), "TEXT:");
    mvwprintw(m_menu->get_win(), i, 2, "%s", m_text.c_str());

    wrefresh(m_menu->get_win());
    m_menu->set_curr_item(shared_from_this());
}


void CTextNote::handle_input(int c)
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

void CTextNote::do_file_func(int choice)
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
            change_text();
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

void CTextNote::read_tags()
{
    echo();
    curs_set(1);
    char tag[tag_note_len];
    std::string tag_string;

    while (true)
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
    }
    noecho();
    curs_set(0);
    m_menu->get_curr_item()->show();
}

void CTextNote::delete_tags()
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

void CTextNote::show_log()
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

void CTextNote::read_name(char *name)
{
    werase(m_menu->get_win());
    curs_set(1);
    echo();
    box(m_menu->get_win(), 0, 0);
    mvwprintw(m_menu->get_win(), 1, 1, "%s", NAME.c_str());
    wrefresh(m_menu->get_win());

    mvwgetnstr(m_menu->get_win(), 2, 1, name, tag_note_len);
}

void CTextNote::read_text(char *text)
{
    werase(m_menu->get_win());
    curs_set(1);
    echo();
    box(m_menu->get_win(), 0, 0);
    mvwprintw(m_menu->get_win(), 1, 1, "%s", NOTE_TEXT.c_str());
    wrefresh(m_menu->get_win());

    mvwgetnstr(m_menu->get_win(), 2, 1, text, text_len);
}

void CTextNote::change_name()
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

void CTextNote::change_text()
{
    char text[text_len];
    read_text(text);
    m_text = text;
    m_dLogOps.emplace_back(TEXT_CHANGED + m_text);
    curs_set(0);
    noecho();
    m_menu->get_curr_item()->show();
}

bool CTextNote::find_string(const std::string &str) const
{
    if (get_date() == str || get_name() == str || m_text == str)
        return true;
    if (m_sTags.find(CTag(str)) != m_sTags.end())
        return true;

    return false;
}

std::string CTextNote::format_to_export() const
{
    std::stringstream sstr;
    sstr << "0" << std::endl << get_name() << std::endl << m_creation_date << std::endl;
    export_tags(sstr);
    sstr << m_text << std::endl;
    sstr << std::endl;
    return sstr.str();
}








