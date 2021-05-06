//
// Created by Nick Liu on 2019/11/22.
//

#ifndef SOFTWARE_LIBRARY_BOOK_H
#define SOFTWARE_LIBRARY_BOOK_H

#include <iostream>
#include "Date.h"

using std::istream;
using std::ostream;
using std::ifstream;
using std::ofstream;
using std::streampos;
using std::streamoff;

struct LIB_HEADER {
    unsigned int count; // 库存图书量
    unsigned int count_id; // 用于给每本图书唯一标识号
};

const int SIZE = 50;
const char LIB[8] = "LIB";

struct Book {
    int id{}; // 唯一标识号，用-1表示该图书已被删除
    char title[SIZE]{}; // 标题
    char author[SIZE]{}; // 作者
    char category[SIZE]{}; // 分类
    char publisher[SIZE]{}; // 出版社
    int publishing_year{}; // 出版年份
    Date date_of_entry{}; // 入库日期
    unsigned int borrow_count{}; // 被借次数
    bool borrowed{}; // 是否被借
};

class Bk {
private:
    string filename;
    LIB_HEADER lib_header;
public:
    explicit Bk(const string&);

    void open_for_read(ifstream&);
    void open_for_write(ofstream&);
    static Date get_current_date();
    void read_header();
    void write_header();
    streampos add_book(Book &);
    void delete_book(streampos);
    void traverse();
    int show_num_of_books_in_stock();
    Book* find(streampos);
    bool b(streampos);
    bool r(streampos);
// 得出第id条记录的位置
    static inline streampos idtopos(unsigned int id) {
        return static_cast<streampos> (sizeof (LIB_HEADER) + id * sizeof (Book));
    }
// 得出对应位置的记录是第几条
    static inline unsigned int postoid(streampos pos) {
        return (static_cast<unsigned int> (pos) - sizeof (LIB_HEADER)) / sizeof (Book);
    }
};



#endif //SOFTWARE_LIBRARY_BOOK_H
