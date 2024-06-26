/**
 * @author Josef Jech
 * @date 9.6.23
 */

#pragma once

#include "CNote.h"
#include "../menus/CMenu.h"
#include <string>
#include <iostream>

/**
 * Text note
 */
class CTextNote : public CNote
{
public:
    /**
     * Constructor taking parent
     * @param name - list name
     * @param text - note text
     * @param menu - menu window view for displaying notes and folders
     * @param parent - parent folder
     */
    CTextNote(std::string name, std::string text, CMenu *menu,
              const std::shared_ptr<CFileManager> &parent = nullptr);

    /**
     * Constructor taking date
     * @param name - list name
     * @param text - note text
     * @param menu - menu window view for displaying notes and folders
     * @param date
     */
    CTextNote(std::string name, std::string text, CMenu *menu, const CDate &date);

    /**
     * Copy constructor
     * @param tn - text note
     */
    CTextNote(const CTextNote &tn);

    /**
     * Operator =
     * @param tn - text note
     * @return this
     */
    CTextNote &operator=(const CTextNote &tn);

    /**
     * Shows note on screen
     */
    void show() override;

    /**
     * Handles input based on user input
     * @param c - input char
     */
    void handle_input(int c) override;

    /**
     * Decide which menu option is handled
     * @param choice
     */
    void do_file_func(int choice);

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
     * Show log
     */
    void show_log();

    /**
     * Read name from user input
     * @param name
     */
    void read_name(char *name);

    /**
     * Read text from user input
     * @param text - where text is stored
     */
    void read_text(char *text);

    /**
     * Change note name
     */
    void change_name();

    /**
     * Change note text
     */
    void change_text();

    /**
     * Find string input in note
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
    std::string m_text;
};
