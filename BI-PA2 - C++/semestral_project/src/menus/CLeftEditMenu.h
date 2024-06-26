/**
 * @author Josef Jech
 * @date 8.6.23
 */

#pragma once

#include "CLeftMenuBar.h"

/**
 * Menu for showing options for file editing
 */
class CLeftEditMenu : public CLeftMenuBar
{
public:
    /**
     * Constructor
     */
    CLeftEditMenu() = default;

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
    static const int m_scEditArrMax = 3;

    const std::array<std::string, m_scEditArrMax> m_aEditMenuOpts =
            {
                    "edit todo/item",
                    "delete todo/item",
                    "add todo/item"
            };
};
