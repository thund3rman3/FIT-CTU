/**
 * @author Josef Jech
 * @date 4.6.23
 */

#pragma once

#include "CLeftMenuBar.h"

/**
 * Menu for showing file options
 */
class CLeftFileMenu : public CLeftMenuBar
{
public:
    /**
     * Constructor
     */
    CLeftFileMenu() = default;

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
    static const int m_scFileArrMax = 5;
    
    const std::array<std::string, m_scFileArrMax> m_aFileMenuOpts =
            {
                    "show edit history",
                    "change note name",
                    "edit text/todo/item",
                    "add tags",
                    "delete tag",
            };
};
