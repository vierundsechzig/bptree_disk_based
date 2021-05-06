//
// Created by Nick Liu on 2019/11/22.
//

#ifndef SOFTWARE_LIBRARY_DATE_H
#define SOFTWARE_LIBRARY_DATE_H
#include <string>
#include <iostream>
using std::string;
using std::istream;
using std::ostream;

class Date {
private:
    int year;
    int month;
    int day;
    bool isLeapYear(int) const;
public:
    Date();
    explicit Date(int, int, int);
    int getYear() const;
    int getMonth() const;
    int getDay() const;
    bool isLeapYear() const;
    bool setYear(int);
    bool setMonth(int);
    bool setDay(int);
    bool setDate(int, int, int);
    friend bool operator ==(const Date&, const Date&);
    friend bool operator !=(const Date&, const Date&);
    friend bool operator <(const Date&, const Date&);
    friend bool operator >(const Date&, const Date&);
    friend bool operator <=(const Date&, const Date&);
    friend bool operator >=(const Date&, const Date&);
    friend ostream& operator <<(ostream&, const Date&);
    friend istream& operator >>(istream&, Date&);
    string toString() const;
};


#endif //SOFTWARE_LIBRARY_DATE_H
