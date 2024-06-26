/**
 * @author Josef Jech
 * @date 9.6.23
 */

#include "CDate.h"

CDate::CDate() noexcept
{
    time_t raw_time = time(nullptr);
    struct tm *m_date = localtime(&raw_time);
    m_date->tm_year += 1900;
    m_date->tm_mon++;

    m_year = m_date->tm_year;
    m_month = m_date->tm_mon;
    m_day = m_date->tm_mday;
    m_hour = m_date->tm_hour;
    m_min = m_date->tm_min;
    m_sec = m_date->tm_sec;
}

CDate::CDate(int y, int mon, int d, int h, int min, int s)
{
    if (y > 2100 || y < 1900 ||
        mon > 12 || mon < 1 ||
        d > 31 || d < 1 ||
        h > 23 || h < 0 ||
        min > 59 || min < 0 ||
        s > 59 || s < 0)
        throw std::invalid_argument(INVALID);
    m_year = y;
    m_month = mon;
    m_day = d;
    m_hour = h;
    m_min = min;
    m_sec = s;
}

CDate &CDate::operator=(const CDate &date)
{
    if (&date == this)
        return *this;
    m_year = date.m_year;
    m_month = date.m_month;
    m_day = date.m_day;
    m_hour = date.m_hour;
    m_min = date.m_min;
    m_sec = date.m_sec;
    return *this;
}

std::string CDate::get() const
{
    std::stringstream out;
    out << std::setfill('0')
        << std::setw(2) << m_day << "."
        << std::setw(2) << m_month << "."
        << std::setw(4) << m_year << " - "
        << std::setw(2) << m_hour << ":"
        << std::setw(2) << m_min << ":"
        << std::setw(2) << m_sec;

    return out.str();
}

std::ostream &operator<<(std::ostream &os, const CDate &date)
{
    return os << date.m_day << " "
              << date.m_month << " "
              << date.m_year << " "
              << date.m_hour << " "
              << date.m_min << " "
              << date.m_sec;
}




