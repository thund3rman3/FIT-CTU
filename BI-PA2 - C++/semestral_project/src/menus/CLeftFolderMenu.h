/**
 * @author Josef Jech
 * @date 4.6.23
 * @inspiredBy https://www.youtube.com/watch?v=yCIkFdzz-48&list=PL2U2TQ__OrQ8jTf0_noNKtHMuYlyxQl4v&index=17&t=406s&ab_channel=CasualCoder
 */

#pragma once

#include "CLeftMenuBar.h"

/**
 * Menu for showing folder options
 */
class CLeftFolderMenu : public CLeftMenuBar
{
public:
    /**
     * Constructor
     */
    CLeftFolderMenu() = default;

    /**
     * Draw menu items and draw selected as marked
     */
    void draw_menu_items() override;

    /**
     *  Move index forward
     */
    void select_next() override;

    /**
     * Move index backwards
     */
    void select_prev() override;

private:
    static const int m_scFolderArrMax = 8;

    const std::array<std::string, m_scFolderArrMax> m_aFolderMenuOpts =
            {"delete",
             "export",
             "search",
             "add text note",
             "add todoList note",
             "add shopping list",
             "add folder",
             "import file"};
};
