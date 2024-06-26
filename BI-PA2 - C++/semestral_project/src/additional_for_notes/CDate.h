/**
 * @author Josef Jech
 * @date 9.6.23
 */

#pragma once

#include <ctime>
#include <iostream>
#include <iomanip>

/**
 * Holds info about date and time
 */
class CDate
{
public:
    /**
     * Constructor which saves current time
     */
    CDate() noexcept;

    /**
     * Constructor from read parameters
     * @param y - year
     * @param mon - month
     * @param d - day
     * @param h - hour
     * @param min - minute
     * @param s - second
     * @throws std::invalid_argument - params out of range
     */
    CDate(int y, int mon, int d, int h, int min, int s);

    /**
     * Copy constructor
     * @param date
     */
    CDate(const CDate &date) noexcept = default;

    /**
     * Operator = for change of current object
     * @param date
     * @return this
     */
    CDate &operator=(const CDate &date);

    /**
     * Destructor
     */
    ~CDate() = default;

    /**
     * Getter for date in format used for printing on screen
     * @return date in string
     */
    std::string get() const;

    /**
     * Operator<< for getting date to stream in format for exporting
     * @param os - ostream
     * @param date
     * @return ostream
     */
    friend std::ostream &operator<<(std::ostream &os, const CDate &date);

    /** Nmber of important private variables */
    const size_t m_size = 6;
private:
    /** second, minute, hour, day, month, year */
    int m_sec, m_min, m_hour, m_day, m_month, m_year;

    const std::string INVALID = "Date given in wrong format";
};
