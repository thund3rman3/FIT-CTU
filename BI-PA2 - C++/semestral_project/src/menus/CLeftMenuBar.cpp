/**
 * @author Josef Jech
 * @date 2.6.23
 */

#include "CLeftMenuBar.h"


using namespace std;

CLeftMenuBar::CLeftMenuBar()
{
    int win_height, win_width;
    getmaxyx(stdscr, win_height, win_width);
    const int win_top = win_height / 4, win_left = win_width / 6;
    m_Win = newwin(win_height / 2, win_width / 6, win_top, win_left);
    box(m_Win, 0, 0);
    keypad(m_Win, true);
    wrefresh(m_Win);
}

int CLeftMenuBar::draw_menu()
{
    int c = wgetch(m_Win);

    while (c != '\n')
    {
        switch (c)
        {
            case KEY_DOWN:
                select_next();
                draw_menu_items();
                break;
            case KEY_UP:
                select_prev();
                draw_menu_items();
                break;
            case 27:
                werase(m_Win);
                box(m_Win, 0, 0);
                wrefresh(m_Win);
                return 27;
            default:
                break;
        }
        c = wgetch(m_Win);
    }
    werase(m_Win);
    box(m_Win, 0, 0);
    wrefresh(m_Win);
    return m_selected_item;
}

CLeftMenuBar::~CLeftMenuBar()
{
    delwin(m_Win);
}

WINDOW *CLeftMenuBar::get_win() const
{
    return m_Win;
}

