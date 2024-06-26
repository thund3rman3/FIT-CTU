/**
 * @author Josef Jech
 * @date 4.6.23
 */

#include "CLeftFileMenu.h"

void CLeftFileMenu::draw_menu_items()
{
    for (int i = 0; i < m_scFileArrMax; ++i)
    {
        if (m_selected_item == i)
        {
            wattron(m_Win, A_REVERSE);
            mvwprintw(m_Win, i + 1, 1, "%s", m_aFileMenuOpts[i].c_str());
            wattroff(m_Win, A_REVERSE);
        } else
            mvwprintw(m_Win, i + 1, 1, "%s", m_aFileMenuOpts[i].c_str());
    }
    wrefresh(m_Win);
}

void CLeftFileMenu::select_next()
{
    m_selected_item++;
    if (m_selected_item >= m_scFileArrMax)
        m_selected_item = 0;
}

void CLeftFileMenu::select_prev()
{
    m_selected_item--;
    if (m_selected_item < 0)
        m_selected_item = m_scFileArrMax - 1;
}
