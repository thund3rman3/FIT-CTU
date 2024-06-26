/**
 * @author Josef Jech
 * @date 2.6.23
 */

#pragma once

#include "../notes/CFolder.h"
#include "../notes/CTextNote.h"
#include "../notes/CShopList.h"
#include "../notes/CToDoList.h"


/**
 * Menu window view for displaying notes and folders
 */
class CMenu
{
public:

    /**
     * Constructor
     */
    explicit CMenu();

    /**
     * Destructor
     */
    ~CMenu();

    /**
     * Initialize window size and position
     */
    void init_win();

    /**
     * Draw menu and handle inputs
     */
    void draw_menu();

    void feed_data();

    /**
     * Getter for window
     * @return m_Win - window
     */
    WINDOW *get_win() const;

    /**
     * Getter for highest number of columns suitable to write in window
     * @return m_xMax - window width
     */
    int get_xMax() const;

    /**
     * Set curretly displayed item in window
     * @param item - note/folder
     */
    void set_curr_item(const std::shared_ptr<CFileManager> &item);

    /**
     * Getter for currently displayed item in window
     * @return m_curr_item
     */
    std::shared_ptr<CFileManager> get_curr_item();

private:
    /** Menu window */
    WINDOW *m_Win;

    /** Window width for printing */
    int m_xMax;

    const std::string FOLDER_NAME = "MAIN FOLDER";

    /** Currently shown item */
    std::shared_ptr<CFileManager> m_curr_item = nullptr;
};
