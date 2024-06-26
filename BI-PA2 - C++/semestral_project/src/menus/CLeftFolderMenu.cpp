/**
 * @author Josef Jech
 * @date 4.6.23
 * @inspiredBy https://www.youtube.com/watch?v=yCIkFdzz-48&list=PL2U2TQ__OrQ8jTf0_noNKtHMuYlyxQl4v&index=17&t=406s&ab_channel=CasualCoder
 */

#include "CLeftFolderMenu.h"

void CLeftFolderMenu::draw_menu_items()
{
    for (int i = 0; i < m_scFolderArrMax; ++i)
    {
        if (m_selected_item == i)
        {
            wattron(m_Win, A_REVERSE);
            mvwprintw(m_Win, i + 1, 1, "%s", m_aFolderMenuOpts[i].c_str());
            wattroff(m_Win, A_REVERSE);
        } else
            mvwprintw(m_Win, i + 1, 1, "%s", m_aFolderMenuOpts[i].c_str());
    }
    wrefresh(m_Win);
}

void CLeftFolderMenu::select_next()
{
    m_selected_item++;
    if (m_selected_item >= m_scFolderArrMax)
        m_selected_item = 0;
}

void CLeftFolderMenu::select_prev()
{
    m_selected_item--;
    if (m_selected_item < 0)
        m_selected_item = m_scFolderArrMax - 1;
}
