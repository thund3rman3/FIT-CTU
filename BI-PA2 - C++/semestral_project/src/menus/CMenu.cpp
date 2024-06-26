/**
 * @author Josef Jech
 * @date 2.6.23
 */

#include "CMenu.h"

CMenu::CMenu()
{
    init_win();
    m_curr_item = std::make_shared<CFolder>(FOLDER_NAME, this);
}

CMenu::~CMenu()
{
    m_curr_item->delete_item();
    delwin(m_Win);
}

void CMenu::init_win()
{
    int win_height, win_width;
    getmaxyx(stdscr, win_height, win_width);
    const int win_top = 1, win_left = win_width / 3;
    m_Win = newwin(win_height - 2, win_width / 2, win_top, win_left);
    box(m_Win, 0, 0);

    m_xMax = win_width;
    m_xMax -= 2;
    keypad(m_Win, true);
}

void CMenu::draw_menu()
{
    box(m_Win, 0, 0);
    mvwprintw(m_Win, 0, 1, "%.*s", m_xMax, (m_curr_item->get_name() + " - " + m_curr_item->get_date()).c_str());
    wrefresh(m_Win);

    int c = 0;
    m_curr_item->handle_input(c);

    while ((c = wgetch(m_Win)))
    {
        if (c == 27 && m_curr_item->get_parent() == nullptr)
            break;
        m_curr_item->handle_input(c);
        if (c == 27)
            m_curr_item->show();
    }
    werase(m_Win);
    wrefresh(m_Win);
}

WINDOW *CMenu::get_win() const
{
    return m_Win;
}

int CMenu::get_xMax() const
{
    return m_xMax;
}

void CMenu::set_curr_item(const std::shared_ptr<CFileManager> &item)
{
    m_curr_item = item;
}

std::shared_ptr<CFileManager> CMenu::get_curr_item()
{
    return m_curr_item;
}




