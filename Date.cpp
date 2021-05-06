//
// Created by Nick Liu on 2019/11/22.
//

#include <iostream>
#include <sstream>
#include <iomanip>
#include "Date.h"

static int DAYS_IN_A_MONTH[2][12] = {
        {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
        {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
}; // 平年和闰年中一个月的天数。第一维下标为0的是平年，为1的是闰年。

Date::Date() {
    this->year = 1970; // 默认年
    this->month = 1; // 默认月
    this->day = 1; // 默认日
}

Date::Date(int year, int month, int day) {
    this->year = 1970; // 默认年
    this->month = 1; // 默认月
    this->day = 1; // 默认日
    this->setYear(year);
    this->setMonth(month);
    this->setDay(day);
}

bool Date::isLeapYear(int y) const {
    if (y % 400 == 0)
        return true;
    if (y % 100 == 0)
        return false;
    return y % 4 == 0;
}

int Date::getYear() const {
    return this->year;
}

int Date::getMonth() const {
    return this->month;
}

int Date::getDay() const {
    return this->day;
}

bool Date::isLeapYear() const {
    return isLeapYear(this->year);
}

bool Date::setYear(int year) {
    if (year <= 0) { // 年不能为非正整数
        return false;
    }
    if (!this->isLeapYear(year) && this->month == 2 && this->day == 29) { // 当前是2月29日且欲设置为平年
        return false;
    }
    this->year = year;
    return true;
}

bool Date::setMonth(int month) {
    if (month < 1 || month > 12) // 超出月的范围
        return false;
    bool leap = this->isLeapYear();
    if (this->day > DAYS_IN_A_MONTH[leap][month-1]) // 设置后的日超出这个月应该有的天数
        return false;
    this->month = month;
    return true;
}

bool Date::setDay(int day) {
    if (day < 1) // 超出日的范围
        return false;
    bool leap = this->isLeapYear();
    if (day > DAYS_IN_A_MONTH[leap][this->month-1]) // 设置后的日超出这个月应该有的天数
        return false;
    this->day = day;
    return true;
}

bool Date::setDate(int y, int m, int d) {
    bool leap = this->isLeapYear(y);
    if (y < 1)
        return false;
    if (m < 1 || m > 12)
        return false;
    if (d < 1)
        return false;
    if (d > DAYS_IN_A_MONTH[leap][m-1])
        return false;
    this->year = y;
    this->month = m;
    this->day = d;
    return true;
}

string Date::toString() const {
    std::ostringstream ss;
    ss.fill('0');
    ss << std::setw(4) << this->year << "-" << std::setw(2) << this->month << "-" << std::setw(2) << this->day;
    return ss.str();
}

bool operator ==(const Date& ldate, const Date& rdate) {
    return (ldate.year == rdate.year) && (ldate.month == rdate.month) && (ldate.day == rdate.day);
}

bool operator !=(const Date& ldate, const Date& rdate) {
    return !(ldate == rdate);
}

bool operator <(const Date& ldate, const Date& rdate) {
    if (ldate.year < rdate.year)
        return true;
    if (ldate.year > rdate.year)
        return false;
    if (ldate.month < rdate.month)
        return true;
    if (ldate.month > rdate.month)
        return false;
    return ldate.day < rdate.day;
}

bool operator >(const Date& ldate, const Date& rdate) {
    return !(ldate < rdate || ldate == rdate);
}

bool operator <=(const Date& ldate, const Date& rdate) {
    return ldate < rdate || ldate == rdate;
}

bool operator >=(const Date& ldate, const Date& rdate) {
    return !(ldate < rdate);
}

ostream& operator <<(ostream& out, const Date& date) {
    return out << date.toString();
}

istream& operator >>(istream& in, Date& date) {
    char sep; // 分隔符
    int y, m, d;
    in >> y >> sep >> m >> sep >> d;
    if (in)
        date.setDate(y, m, d);
    return in;
}