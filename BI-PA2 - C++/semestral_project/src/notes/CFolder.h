/**
 * @author Josef Jech
 * @date 9.6.23
 */

#pragma once

#include "CFileManager.h"
#include "../menus/CMenu.h"
#include <vector>
#include <memory>
#include <set>
#include "../menus/CLeftFolderMenu.h"


class CFolder : public CFileManager
{
public:
    /**
     * Constructor taking parent
     * @param name - folder name
     * @param menu - menu window view for displaying notes and folders
     * @param parent - parent folder
     */
    CFolder(std::string name, CMenu *menu, const std::shared_ptr<CFileManager> &parent = nullptr);

    /**
     * Constructor taking date
     * @param name - folder name
     * @param menu - menu window view for displaying notes and folders
     * @param date
     */
    CFolder(std::string name, CMenu *menu, const CDate &date);

    /**
     * Copy constructor
     * @param folder
     */
    CFolder(const CFolder &folder);

    /**
     * Operator=
     * @param folder
     * @return this
     */
    CFolder &operator=(const CFolder &folder);

    /**
     * Show folder on screen
     */
    void show() override;

    /**
     * Add file to folder
     * @param file
     */
    void add(const std::shared_ptr<CFileManager> &file);

    /**
     * Delete all items within the folder
     */
    void delete_item() override;

    /**
     * Handle user input
     * @param c - input char
     */
    void handle_input(int c) override;

    /**
     * Show all folder's files on screen
     */
    void draw_menu_items();

    /**
     *  Move index forward
     */
    void select_next();

    /**
     * Move index backwards
     */
    void select_prev();

    /**
     * Handle menu by chosen option
     * @param choice
     */
    void do_menu_func(int choice);

    /**
     * Add text note based on user input
     */
    void add_text_note();

    /**
     * Add todo_note based on user input
     */
    void add_todo_note();

    /**
     * Add shopping list based on user input
     */
    void add_shop_list();

    /**
     * Add folder based on user input
     */
    void add_folder();

    /**
     * Search in shown files
     */
    void search();

    /**
     * Find string input in files
     * @param str - search string
     * @return true if string was found, false elsewhen
     */
    bool find_string(const std::string &str) const override;

    /**
     * Export currently shown files in folder to formatted markdown
     */
    void export_file();

    /**
     * Write whole folder into string suitable for export
     * @return formatted file in string
     */
    std::string format_to_export() const override;

    /**
     * Import files based on user input
     */
    void import_files();

    /**
     * Read files from import file into folder
     * @param ifs
     * @param cnt_items
     */
    void import_read_loop(std::ifstream &ifs, int cnt_items = -1);

    /**
     * Import text note from import file
     * @param ifs - ifstream
     * @param line - loaded line from import file
     */
    void import_add_text_note(std::ifstream &ifs, std::string &line);

    /**
     * Import todo_note from import file
     * @param ifs - ifstream
     * @param line - loaded line from import file
     */
    void import_add_todo(std::ifstream &ifs, std::string &line);

    /**
     * Import shopping list from import file
     * @param ifs - ifstream
     * @param line - loaded line from import file
     */
    void import_add_shoplist(std::ifstream &ifs, std::string &line);

    /**
     * Import folder from import file
     * @param ifs - ifstream
     * @param line - loaded line from import file
     */
    void import_add_folder(std::ifstream &ifs, std::string &line);

    /**
     * Read date from import file
     * @param line
     * @return
     */
    static CDate *import_date(const std::string &line);

    /**
     * Read tags from import file
     * @param tags - set of tag
     * @param line - loaded line from import file
     */
    void import_tags(std::set<CTag, Tag_Cmp> &tags, const std::string &line);


private:
    /** Folder contents */
    std::vector<std::shared_ptr<CFileManager>> m_vFiles;

    /** Index of selected item in folder */
    int m_selected_item = 0;

    /** Menu for showing folder options */
    CLeftFolderMenu m_folder_menu;

    bool m_is_search = false;

    static constexpr const int name_len = 70, text_len = 200;

    const std::string IMPORT_TITLE = "IMPORT FROM ./examples";
    const std::string EXPORT_TITLE = "EXPORT TO ./examples";
    const std::string WRITE_IMPORT = "Write file name:";
    const std::string IMPORT_EXPORT_FOLDER = "examples/";
    const std::string OPEN_ERR = "FILE CANNOT BE OPENED";
    const std::string SEARCH = "SEARCH";
    const std::string IMPORT_FAIL = "Import failed";

    /**
     * Read name from user input
     * @param name - where name is written
     */
    void read_name(char *name);
};

