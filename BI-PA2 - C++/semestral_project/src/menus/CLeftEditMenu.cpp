/**
 * @author Josef Jech
 * @date 8.6.23
 */

#include "CLeftEditMenu.h"

void CLeftEditMenu::draw_menu_items()
{
    for (int i = 0; i < m_scEditArrMax; ++i)
    {
        if (m_selected_item == i)
        {
            wattron(m_Win, A_REVERSE);
            mvwprintw(m_Win, i + 1, 1, "%s", m_aEditMenuOpts[i].c_str());
            wattroff(m_Win, A_REVERSE);
        } else
            mvwprintw(m_Win, i + 1, 1, "%s", m_aEditMenuOpts[i].c_str());
    }
    wrefresh(m_Win);
}

void CLeftEditMenu::select_next()
{
    m_selected_item++;
    if (m_selected_item >= m_scEditArrMax)
        m_selected_item = 0;
}

void CLeftEditMenu::select_prev()
{
    m_selected_item--;
    if (m_selected_item < 0)
        m_selected_item = m_scEditArrMax - 1;
}