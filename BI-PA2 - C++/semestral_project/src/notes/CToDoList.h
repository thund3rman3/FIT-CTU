/**
 * @author Josef Jech
 * @date 9.6.23
 */

#pragma once

#include "CNote.h"
#include "../menus/CMenu.h"
#include <map>
#include "../menus/CLeftEditMenu.h"

/**
 * List of todos
 */
class CToDoList : public CNote
{
public:
    /**
     * Constructor taking parent
     * @param name - todo_list name
     * @param menu - menu window view for displaying notes and folders
     * @param to_do - store items to do
     * @param parent - parent folder
     */
    CToDoList(std::string name, CMenu *menu, std::map<std::string, bool> to_do = std::map<std::string, bool>(),
              const std::shared_ptr<CFileManager> &parent = nullptr);

    /**
     * Constructor taking date
     * @param name - todo_list name
     * @param menu - menu window view for displaying notes and folders
     * @param date
     * @param to_do - store items to do
     */
    CToDoList(std::string name, CMenu *menu, const CDate &date,
              std::map<std::string, bool> to_do = std::map<std::string, bool>());

    /**
     * Copy constructor
     * @param tdl - todo_list
     */
    CToDoList(const CToDoList &tdl);

    /**
     * Operator =
     * @param tdl - todo_list
     * @return this
     */
    CToDoList &operator=(const CToDoList &tdl);

    /**
     * Shows list content on screen
     */
    void show() override;

    /**
     * Switch done/undone in item
     * @param item_name
     * @return true if edited, else otherwise
     */
    bool edit(const std::string &item_name);

    /**
     * Add todo_item
     * @param todo_item
     * @return true if added, false otherwise
     */
    bool add_todo(const std::string &todo);

    /**
     * Delete item from user input
     * @param todo_item
     * @return true if deleted, false otherwise
     */
    bool del_todo(const std::string &todo);

    /**
     * Handles input based on user input
     * @param c - input char
     */
    void handle_input(int c) override;

    /**
     * Read tags from user input
     */
    void read_tags() override;

    /**
     * Delete tag based on user input
     * @param tag
     */
    void delete_tags();

    /**
     * Add todos from user input
     */
    void add_todos();

    /**
     * Decide which menu option is handled
     * @param choice
     */
    void do_file_func(int choice);

    /**
     * Show edit menu
     */
    void show_log();

    /**
     * Change note name
     */
    void change_name();

    /**
     * Read name from user input
     * @param name
     */
    void read_name(char *name);

    /**
     * Show edit menu
     */
    void show_edit_menu();

    /**
     * Edit item based on user input
     */
    void edit_item_from_input();

    /**
     * Delete item based on user input
     */
    void delete_item_from_input();

    /**
     * Add item from user input
     */
    void add_item_from_input();

    /**
     * Find string input in list
     * @param str - search string
     * @return true if string was found, false elsewhen
     */
    bool find_string(const std::string &str) const override;

    /**
     * Write whole file into string suitable for export
     * @return formatted file in string
     */
    std::string format_to_export() const override;

private:
    /** Menu for showing options for file editing */
    CLeftEditMenu m_LEM;

    /** Store todos */
    std::map<std::string, bool> m_mToDo;

    const std::string TODOS = "TODOS:";
    const std::string DONE = " done";
    const std::string UNDONE = " undone";
    const std::string ADD_TODO = "Add todo:";
};

