/**
 * @author Josef Jech
 * @date 2.6.23
 */

#include "CApplication.h"
#include "menus/CMenu.h"

using namespace std;

CApplication::CApplication()
{
    initscr();
    noecho();
    curs_set(0);
}

CApplication::~CApplication()
{
    endwin();
}

void CApplication::run()
{
    CMenu menu;
    try
    {
        menu.draw_menu();
    }
    catch (const std::invalid_argument &e)
    {
        endwin();
        std::cout << e.what() << std::endl;
    }
    catch (const std::runtime_error &e)
    {
        endwin();
        std::cout << e.what() << std::endl;
    }
}
