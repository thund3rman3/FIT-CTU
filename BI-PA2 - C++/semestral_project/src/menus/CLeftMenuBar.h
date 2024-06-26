/**
 * @author Josef Jech
 * @date 2.6.23
 */

#pragma once

#include <ncurses.h>
#include <string>
#include <array>
#include <iostream>

/**
 * Abstract class for showing option menu on left
 */
class CLeftMenuBar
{
public:
    /**
     * Constructor
     */
    CLeftMenuBar();

    /**
     * Destructor
     */
    virtual ~CLeftMenuBar();

    /**
     * Draw based on input
     * @return index of selected item
     */
    int draw_menu();

    /**
     * Draw menu items and draw selected as marked
     */
    virtual void draw_menu_items() = 0;

    /**
     *  Move index forward
     */
    virtual void select_next() = 0;

    /**
     * Move index backwards
     */
    virtual void select_prev() = 0;

    /**
     * Getter for window
     * @return menu window
     */
    WINDOW *get_win() const;

protected:
    WINDOW *m_Win;
    int m_selected_item = 0;
};
