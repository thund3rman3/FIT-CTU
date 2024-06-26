/**
 * @author Josef Jech
 * @date 9.6.23
 */

#pragma once

#include "CNote.h"
#include "../menus/CMenu.h"
#include "../menus/CLeftEditMenu.h"
#include <map>
#include <string>
#include <iostream>

/**
 * Item to buy
 */
struct Item
{
    int price = 1;
    int count = 1;

    bool operator==(const Item &i) const
    {
        return this->price == i.price &&
               this->count == i.count;
    }
};

/**
 * Note - shopping list
 */
class CShopList : public CNote
{
public:
    /**
     * Constructor taking parent
     * @param name - list name
     * @param menu - menu window view for displaying notes and folders
     * @param to_buy - store items to buy
     * @param parent - parent folder
     */
    CShopList(std::string name, CMenu *menu, std::map<std::string, Item> to_buy = std::map<std::string, Item>(),
              const std::shared_ptr<CFileManager> &parent = nullptr);

    /**
     * Constructor taking date
     * @param name - list name
     * @param menu - menu window view for displaying notes and folders
     * @param date
     * @param to_buy - store items to buy
     */
    CShopList(std::string name, CMenu *menu, const CDate &date,
              std::map<std::string, Item> to_buy = std::map<std::string, Item>());

    /**
     * Copy constructor
     * @param sl - shopping list
     */
    CShopList(const CShopList &sl);

    /**
     * Operator =
     * @param sl - shopping list
     * @return this
     */
    CShopList &operator=(const CShopList &sl);

    /**
     * Shows list content on screen
     */
    void show() override;

    /**
     * Edit item in list
     * @param item_name
     * @param item - count and price
     * @return true if edited, else otherwise
     */
    bool edit(const std::string &item_name, const Item &item);

    /**
     * Add item to list
     * @param item_name
     * @param item - count and price
     * @return true if added, false otherwise
     */
    bool add_item(const std::string &item_name, const Item &item);

    /**
     * Delete item from user input
     * @param item_name
     * @return true if deleted, false otherwise
     */
    bool del_item(const std::string &item_name);

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
     * Add items from user input
     */
    void add_items();

    /**
     * Decide which menu option is handled
     * @param choice
     */
    void do_file_func(int choice);

    /**
     * Show edit menu
     */
    void show_edit_menu();

    /**
     * Show log
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

    /** Store items to buy */
    std::map<std::string, Item> m_mToBuy;

    const std::string BUY = "TO BUY:";
    const std::string ITEM_NAME = "Enter item name";
    const std::string ENTER_CNT = "Enter count of this item:";
    const std::string ENTER_P = "Enter price of that item:";

    /**
     * Read new item from user input
     * @param item_name
     * @param item
     */
    void read_item(char *item_name, Item &item);

    /**
     * Read new item from user input and show if the item before was added correctly
     * @param item_name
     * @param item
     */
    void read_next_item(char *item_name, Item &item, bool ok);
};