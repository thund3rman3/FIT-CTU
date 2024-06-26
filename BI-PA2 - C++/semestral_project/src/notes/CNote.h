/**
 * @author Josef Jech
 * @date 9.6.23
 */

#pragma once

#include "CFileManager.h"
#include "../additional_for_notes/CTag.h"
#include "../menus/CLeftFileMenu.h"
#include <deque>
#include <set>
#include <memory>
#include <cstring>
#include <iostream>
#include <vector>


class CNote : public CFileManager
{
public:
    /**
     * Constructor taking parent
     * @param name - note name
     * @param menu - menu window view for displaying notes and folders
     * @param parent - parent folder
     */
    CNote(std::string name, CMenu *menu, const std::shared_ptr<CFileManager> &parent);

    /**
     * Constructor taking date
     * @param name - note name
     * @param menu - menu window view for displaying notes and folders
     * @param date
     */
    CNote(std::string name, CMenu *menu, const CDate &date);

    /**
     * Copy constructor
     * @param note
     */
    CNote(const CNote &note);

    CNote &operator=(const CNote &note) = delete;

    /**
     * Do nothing
     */
    void delete_item() override;

    /**
     * Read tags from user input
     */
    virtual void read_tags() = 0;

    /**
     * Add tag to note
     * @param tag
     */
    void add_tag(std::string tag);

    /**
     * Delete tag based on user input
     * @param tag
     */
    void del_tag(std::string tag);

    /**
     * Edit export stream to include tags
     * @param sstr - export stream
     */
    void export_tags(std::stringstream &sstr) const;

    /**
     * Replace tags
     * @param tags
     */
    void set_tags(const std::set<CTag, Tag_Cmp> &tags);

protected:
    /** Menu for showing note options */
    CLeftFileMenu m_file_menu;

    std::set<CTag, Tag_Cmp> m_sTags;

    static constexpr const int tag_note_len = 70;
    static constexpr const int text_len = 200;

    /** Log for edit history */
    std::deque<std::string> m_dLogOps;

    const std::string SHOW = "show";
    const std::string TAGS = "TAGS:";
    const std::string PRESS_ADD = "PRESS ENTER IF YOU DONT WANT TO ADD ANY";
    const std::string PRESS_DEL = "PRESS ENTER IF YOU DONT WANT TO DELETE ANY";
    const std::string ADD_TAG = "Add tag:";
    const std::string DEL_TAG = "Delete tag:";
    const std::string LOG = "LOG";
    const std::string TEXT_CHANGED = "text changed to: ";
    const std::string NAME_CHANGED = "name changed to: ";

    const std::string EDIT = " edited ";
    const std::string ADDED = " added succesfully.";
    const std::string ADD_NOT = " adding failed.";
    const std::string DELETED = " deleted succesfully.";
    const std::string DEL_NOT = " deleting failed.";
    const std::string EDIT_OK = " edited succesfully.";
    const std::string EDIT_NOT = " editing failed.";
};