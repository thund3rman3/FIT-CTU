/**
 * @author Josef Jech
 * @date 9.6.23
 */

#pragma once

#include <memory>
#include <curses.h>
#include "../additional_for_notes/CDate.h"
#include "../additional_for_notes/CTag.h"

class CMenu;

/**
 * Abstract class over folder and notes
 */
class CFileManager : public std::enable_shared_from_this<CFileManager>
{
public:
    /**
     * Constructor taking parent
     * @param name - file name
     * @param menu - menu window view for displaying notes and folders
     * @param parent - parent folder for this file
     */
    CFileManager(std::string name, CMenu *menu, const std::shared_ptr<CFileManager> &parent);

    /**
     * Constructor taking date
     * @param name - file name
     * @param menu - menu window view for displaying notes and folders
     * @param date
     */
    CFileManager(std::string name, CMenu *menu, const CDate &date);

    /**
     * Default copy constructor
     * @param item - note/folder
     */
    CFileManager(const CFileManager &item) = default;

    /**
     * Forbidd operator=
     * @param file - note/folder
     * @return this
     */
    CFileManager &operator=(const CFileManager &file) = delete;

    /**
     * Destructor
     */
    virtual ~CFileManager() = default;

    /**
     * Show note/folder on screen
     */
    virtual void show() = 0;

    /**
     * Handles keyboard input
     * @param c - input char
     */
    virtual void handle_input(int c) = 0;

    /**
     * Delete item
     */
    virtual void delete_item() = 0;

    /**
     * Find string input in files
     * @param str - search string
     * @return true if string was found, false elsewhen
     */
    virtual bool find_string(const std::string &str) const = 0;

    /**
     * Write whole file into string suitable for export
     * @return formatted file in string
     */
    virtual std::string format_to_export() const = 0;

    /**
     * Getter for file name
     * @return file name
     */
    std::string get_name() const;

    /**
     * Getter for date
     * @return date in string
     */
    std::string get_date() const;

    /**
     * Getter for file's parent folder
     * @return file's parent folder
     */
    std::shared_ptr<CFileManager> get_parent() const;

    /**
     * Set parent folder
     * @param parent folder
     */
    void set_parent(const std::shared_ptr<CFileManager> &parent);

protected:
    std::string m_file_name;
    CDate m_creation_date;
    std::shared_ptr<CFileManager> m_parent;
    CMenu *m_menu;

    const std::string NAME = "Note name:";
    const std::string NOTE_TEXT = "Note text:";
};

