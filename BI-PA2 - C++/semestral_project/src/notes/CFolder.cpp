/**
 * @author Josef Jech
 * @date 9.6.23
 */

#include <fstream>
#include "CFolder.h"

CFolder::CFolder(std::string name, CMenu *menu, const std::shared_ptr<CFileManager> &parent)
        : CFileManager(std::move(name), menu, parent)
{}

CFolder::CFolder(std::string name, CMenu *menu, const CDate &date)
        : CFileManager(std::move(name), menu, date)
{}

CFolder::CFolder(const CFolder &folder)
        : CFileManager(folder)
{
    for (const auto &x: folder.m_vFiles)
    {
        m_vFiles.push_back(x);
    }
}

CFolder &CFolder::operator=(const CFolder &folder)
{
    if (&folder == this)
        return *this;

    m_file_name = folder.m_file_name;
    m_creation_date = folder.m_creation_date;
    m_vFiles.clear();
    m_menu = folder.m_menu;
    for (const auto &x: folder.m_vFiles)
    {
        m_vFiles.push_back(x);
    }
    return *this;
}

void CFolder::add(const std::shared_ptr<CFileManager> &file)
{
    file->set_parent(shared_from_this());
    m_vFiles.push_back(file);
}

void CFolder::show()
{
    draw_menu_items();
    wrefresh(m_menu->get_win());
    m_menu->set_curr_item(shared_from_this());
}

void CFolder::delete_item()
{
    size_t idx = 0;
    while (idx < m_vFiles.size())
    {
        m_vFiles[idx]->delete_item();
        idx++;
    }
    m_vFiles.clear();
}

void CFolder::handle_input(int c)
{
    int choice;
    switch (c)
    {
        case KEY_DOWN:
            select_next();
            draw_menu_items();
            break;
        case KEY_UP:
            select_prev();
            draw_menu_items();
            break;
        case '\n':
            if (!m_vFiles.empty())
                m_vFiles[m_selected_item]->show();
            break;
        case 'e': // folder menu
            if (!m_is_search)
            {
                m_folder_menu.draw_menu_items();
                choice = m_folder_menu.draw_menu();
                do_menu_func(choice);
            } else export_file();
            break;
        case 27:
            m_menu->set_curr_item(get_parent());
            break;
        case 0:
            draw_menu_items();
        default:
            break;
    }

}

void CFolder::draw_menu_items()
{
    werase(m_menu->get_win());
    box(m_menu->get_win(), 0, 0);
    mvwprintw(m_menu->get_win(), 0, 1, "%s", (m_file_name + " - " + get_date()).c_str());

    for (size_t i = 0; i < m_vFiles.size(); ++i)
    {
        if (m_selected_item == (int) i)
        {
            wattron(m_menu->get_win(), A_REVERSE);
            mvwprintw(m_menu->get_win(), (int) i + 1, 1, "%.*s", m_menu->get_xMax(), m_vFiles[i]->get_name().c_str());
            wattroff(m_menu->get_win(), A_REVERSE);
        } else
        {
            mvwprintw(m_menu->get_win(), (int) i + 1, 1, "%.*s", m_menu->get_xMax(), m_vFiles[i]->get_name().c_str());
        }
    }
    wrefresh(m_menu->get_win());
}

void CFolder::select_next()
{
    m_selected_item++;
    if (m_selected_item >= (int) m_vFiles.size())
        m_selected_item = 0;
}

void CFolder::select_prev()
{
    m_selected_item--;
    if (m_selected_item < 0)
        m_selected_item = (int) m_vFiles.size() - 1;
}

void CFolder::do_menu_func(int choice)
{
    switch (choice)
    {
        case 0: // delete
            m_vFiles[m_selected_item]->delete_item();
            m_vFiles.erase(m_selected_item + m_vFiles.begin());
            m_selected_item = 0;
            draw_menu_items();
            m_menu->set_curr_item(shared_from_this());
            break;
        case 1: // export
            export_file();
            break;
        case 2: // search
            search();
            break;
        case 3: // add text note
            add_text_note();
            break;
        case 4: // add todoList note
            add_todo_note();
            break;
        case 5: // add shopping list
            add_shop_list();
            break;
        case 6: // add folder
            add_folder();
            break;
        case 7: // import files
            import_files();
            break;
        default:
            break;
    }
}

void CFolder::add_text_note()
{
    char name[name_len], text[text_len];

    read_name(name);

    werase(m_menu->get_win());
    box(m_menu->get_win(), 0, 0);
    mvwprintw(m_menu->get_win(), 1, 1, "%s", NOTE_TEXT.c_str());
    wrefresh(m_menu->get_win());

    mvwgetnstr(m_menu->get_win(), 2, 1, text, text_len);

    std::shared_ptr<CTextNote> text_note = std::make_shared<CTextNote>(name, text, m_menu);
    text_note->read_tags();

    add(text_note);

    curs_set(0);
    noecho();

    m_menu->get_curr_item()->show();
}

void CFolder::add_todo_note()
{
    char name[name_len];

    read_name(name);

    std::shared_ptr<CToDoList> todo_note = std::make_shared<CToDoList>(name, m_menu);
    todo_note->add_todos();
    todo_note->read_tags();

    add(todo_note);

    curs_set(0);
    noecho();

    m_menu->get_curr_item()->show();
}

void CFolder::add_shop_list()
{
    char name[name_len];

    read_name(name);

    std::shared_ptr<CShopList> shop_list = std::make_shared<CShopList>(name, m_menu);
    shop_list->add_items();
    shop_list->read_tags();

    add(shop_list);

    curs_set(0);
    noecho();

    m_menu->get_curr_item()->show();
}

void CFolder::add_folder()
{
    char name[name_len];
    read_name(name);
    add(std::make_shared<CFolder>(name, m_menu));
    curs_set(0);
    noecho();
    m_menu->get_curr_item()->show();
}

void CFolder::read_name(char *name)
{
    werase(m_menu->get_win());
    curs_set(1);
    echo();
    box(m_menu->get_win(), 0, 0);
    mvwprintw(m_menu->get_win(), 1, 1, "%s", NAME.c_str());
    wrefresh(m_menu->get_win());

    mvwgetnstr(m_menu->get_win(), 2, 1, name, name_len);
}

void CFolder::search()
{
    char name[name_len];
    werase(m_menu->get_win());
    curs_set(1);
    echo();
    box(m_menu->get_win(), 0, 0);
    mvwprintw(m_menu->get_win(), 0, 1, "%s", SEARCH.c_str());
    wrefresh(m_menu->get_win());
    mvwgetnstr(m_menu->get_win(), 1, 1, name, name_len);

    std::string search_str = name;
    std::shared_ptr<CFolder> new_folder = std::make_shared<CFolder>(SEARCH, m_menu);
    for (const auto &x: m_vFiles)
    {
        if (x->find_string(name))
        {
            new_folder->m_vFiles.push_back(x);
        }
    }
    noecho();
    curs_set(0);
    new_folder->set_parent(shared_from_this());
    new_folder->m_is_search = true;
    m_menu->set_curr_item(new_folder);
    m_menu->get_curr_item()->show();
}

bool CFolder::find_string(const std::string &str) const
{
    if (get_date() == str || get_name() == str)
        return true;
    for (const auto &x: m_vFiles)
    {
        if (x->find_string(str))
            return true;
    }
    return false;
}

void CFolder::export_file()
{
    char name[name_len];
    werase(m_menu->get_win());
    curs_set(1);
    echo();
    box(m_menu->get_win(), 0, 0);
    mvwprintw(m_menu->get_win(), 0, 1, "%s", EXPORT_TITLE.c_str());
    mvwprintw(m_menu->get_win(), 1, 1, "%s", WRITE_IMPORT.c_str());
    wrefresh(m_menu->get_win());
    mvwgetnstr(m_menu->get_win(), 2, 1, name, name_len);
    curs_set(0);
    noecho();

    std::string file_name = IMPORT_EXPORT_FOLDER;
    file_name += name;
    file_name += ".md";
    std::ofstream ofs(file_name);
    if (ofs.is_open())
    {
        ofs << m_vFiles.size() << std::endl;
        for (const auto &x: m_vFiles)
        {
            ofs << (x->format_to_export());
        }
        ofs << std::endl;
        ofs.close();
    } else
        throw std::runtime_error(OPEN_ERR);

    m_menu->get_curr_item()->show();
}

std::string CFolder::format_to_export() const
{
    std::stringstream sstr;
    sstr << "3" << std::endl << get_name() << std::endl << m_creation_date << std::endl << m_vFiles.size() << std::endl;
    sstr << std::endl;

    for (const auto &x: m_vFiles)
    {
        sstr << (x->format_to_export());
    }
    sstr << std::endl;
    return sstr.str();
}

void CFolder::import_files()
{
    char name[name_len];
    werase(m_menu->get_win());
    curs_set(1);
    echo();
    box(m_menu->get_win(), 0, 0);
    mvwprintw(m_menu->get_win(), 0, 1, "%s", IMPORT_TITLE.c_str());
    mvwprintw(m_menu->get_win(), 1, 1, "%s", WRITE_IMPORT.c_str());
    wrefresh(m_menu->get_win());
    mvwgetnstr(m_menu->get_win(), 2, 1, name, name_len);
    curs_set(0);
    noecho();

    std::string file = IMPORT_EXPORT_FOLDER;

    std::ifstream ifs(file + name);
    size_t len = strlen(name);
    if (ifs.is_open() && len > 3 && name[len - 1] == 'd' && name[len - 2] == 'm' && name[len - 3] == '.')
    {
        import_read_loop(ifs);
        std::string line;
        std::getline(ifs, line);
        if (!line.empty())
            throw std::runtime_error(IMPORT_FAIL);
    } else
        throw std::runtime_error(IMPORT_FAIL);

    if (!ifs.eof())
        throw std::runtime_error(IMPORT_FAIL);

    m_menu->get_curr_item()->show();
}

void CFolder::import_read_loop(std::ifstream &ifs, int cnt_items)
{
    std::string line;

    if (cnt_items < 0)
    {
        std::getline(ifs, line);
        if (line.empty())
            throw std::runtime_error(IMPORT_FAIL);

        std::stringstream sstr(line);
        sstr >> cnt_items;

        if (!sstr.eof() || cnt_items < 1)
            throw std::runtime_error(IMPORT_FAIL);
    }
    for (int i = 0; i < cnt_items; ++i)
    {
        std::getline(ifs, line);

        if (line.size() != 1 || !isdigit(line[0]) ||
            line[0] < 0 + '0' || line[0] > 4 + '0')
            throw std::runtime_error(IMPORT_FAIL);

        switch (line[0])
        {
            case '0':
                import_add_text_note(ifs, line);
                break;
            case '1':
                import_add_todo(ifs, line);
                break;
            case '2':
                import_add_shoplist(ifs, line);
                break;
            case '3':
                import_add_folder(ifs, line);
                break;
        }
    }
    std::getline(ifs, line);
    if (!line.empty())
        throw std::runtime_error(IMPORT_FAIL);

}

CDate *CFolder::import_date(const std::string &line)
{
    if (line.empty())
        return nullptr;
    std::stringstream sstr(line);
    int sec, min, hour, day, month, year;
    sstr >> day >> month >> year >> hour >> min >> sec;
    CDate *date = new CDate(year, month, day, hour, min, sec);
    return date;
}

void CFolder::import_add_folder(std::ifstream &ifs, std::string &line)
{
    std::getline(ifs, line);
    if (line.empty() || line.size() > name_len)
        throw std::runtime_error(IMPORT_FAIL);

    std::string folder_name = line;
    std::getline(ifs, line);
    CDate *date = import_date(line);
    if (date == nullptr)
        throw std::runtime_error(IMPORT_FAIL);

    std::getline(ifs, line);
    int cnt = 0;
    std::stringstream sstr(line);
    line = "";
    sstr >> cnt >> line;
    if (!sstr.eof() || !line.empty() || cnt < 0)
        throw std::runtime_error(IMPORT_FAIL);

    std::getline(ifs, line);
    if (!line.empty())
        throw std::runtime_error(IMPORT_FAIL);

    std::shared_ptr<CFolder> new_folder = std::make_shared<CFolder>(folder_name, m_menu, *date);
    delete date;
    new_folder->import_read_loop(ifs, cnt);
    add(new_folder);
}

void CFolder::import_tags(std::set<CTag, Tag_Cmp> &tags, const std::string &line)
{
    if (!line.empty())
    {
        std::string string_from_stream;
        std::stringstream sstr(line);
        while (!sstr.eof())
        {
            sstr >> string_from_stream;
            if (tags.find(CTag(string_from_stream)) == tags.end())
                tags.emplace(string_from_stream);
        }
    }
}


void CFolder::import_add_text_note(std::ifstream &ifs, std::string &line)
{
    std::getline(ifs, line);
    if (line.empty() || line.size() > name_len)
        throw std::runtime_error(IMPORT_FAIL);

    std::string note_name = line;
    std::getline(ifs, line);
    CDate *date = import_date(line);
    if (date == nullptr)
        throw std::runtime_error(IMPORT_FAIL);

    std::set<CTag, Tag_Cmp> tags;
    std::getline(ifs, line);
    import_tags(tags, line);

    std::getline(ifs, line);
    if (line.empty() || line.size() > text_len)
        throw std::runtime_error(IMPORT_FAIL);

    std::shared_ptr<CTextNote> txt = std::make_shared<CTextNote>(note_name, line, m_menu, *date);
    if (!tags.empty())
        txt->set_tags(tags);
    add(txt);
    delete date;

    std::getline(ifs, line);
    if (!line.empty())
        throw std::runtime_error(IMPORT_FAIL);
}

void CFolder::import_add_todo(std::ifstream &ifs, std::string &line)
{
    std::getline(ifs, line);
    if (line.empty() || line.size() > name_len)
        throw std::runtime_error(IMPORT_FAIL);

    std::string note_name = line;
    std::getline(ifs, line);
    CDate *date = import_date(line);
    if (date == nullptr)
        throw std::runtime_error(IMPORT_FAIL);

    std::set<CTag, Tag_Cmp> tags;
    std::getline(ifs, line);
    import_tags(tags, line);

    std::map<std::string, bool> todo_map;
    bool done = false;

    std::getline(ifs, line);
    while (!line.empty())
    {
        if (line.size() > name_len + 2)
            throw std::runtime_error(IMPORT_FAIL);

        std::stringstream sstr(line);
        sstr >> line >> done;
        if (todo_map.find(line) == todo_map.end())
            todo_map[line] = done;
        std::getline(ifs, line);
    }
    std::shared_ptr<CToDoList> toDoList = std::make_shared<CToDoList>(note_name, m_menu, *date, todo_map);
    if (!tags.empty())
        toDoList->set_tags(tags);
    add(toDoList);
    delete date;
}

void CFolder::import_add_shoplist(std::ifstream &ifs, std::string &line)
{
    std::getline(ifs, line);
    if (line.empty() || line.size() > name_len)
        throw std::runtime_error(IMPORT_FAIL);

    std::string note_name = line;
    std::getline(ifs, line);
    CDate *date = import_date(line);
    if (date == nullptr)
        throw std::runtime_error(IMPORT_FAIL);

    std::set<CTag, Tag_Cmp> tags;
    std::getline(ifs, line);
    import_tags(tags, line);

    std::map<std::string, Item> item_map;
    std::getline(ifs, line);
    int price = 1, count = 1;
    while (!line.empty())
    {
        if (line.size() > text_len + 2)
            throw std::runtime_error(IMPORT_FAIL);

        std::stringstream sstr(line);
        sstr >> line >> price >> count;
        if (item_map.find(line) == item_map.end())
            item_map[line] = {price, count};
        std::getline(ifs, line);
    }
    std::shared_ptr<CShopList> shopList = std::make_shared<CShopList>(note_name, m_menu, *date, item_map);
    if (!tags.empty())
        shopList->set_tags(tags);
    add(shopList);
    delete date;
}



