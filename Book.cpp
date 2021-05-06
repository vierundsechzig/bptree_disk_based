//
// Created by Nick Liu on 2019/11/22.
//

#include <cstring>
#include <ctime>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "Book.h"

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::ios;
using std::quoted;
using std::boolalpha;
using std::noboolalpha;

Date Bk::get_current_date() {
    Date date;
    std::tm t{};
    std::time_t now;
    std::time(&now);
    localtime_s(&t, &now);
    int y = t.tm_year + 1900;
    int m = t.tm_mon + 1;
    int d = t.tm_mday;
    date.setDate(y, m, d);
    return date;
}

Bk::Bk(const string& fn) {
    filename = fn;
    lib_header.count = 0;
    lib_header.count_id = 0;
}

void Bk::open_for_read(ifstream& fin) {
    fin.open(filename, ios::in | ios::binary);
}

void Bk::open_for_write(ofstream& fout) {
    fout.open(filename, ios::in | ios::out | ios::binary);
}

void Bk::read_header() {
    ifstream fin;
    open_for_read(fin);
    fin.seekg(0);
    fin.read(reinterpret_cast<char*> (&lib_header), sizeof lib_header);
    fin.close();
}

void Bk::write_header() {
    ofstream fout;
    open_for_write(fout);
    fout.seekp(0);
    fout.write(reinterpret_cast<char*> (&lib_header), sizeof lib_header);
    fout.close();
}

//文件中寻找空闲位置
streampos alloc(ifstream& fin) {
    fin.clear();
    fin.seekg(sizeof (LIB_HEADER), ios::beg); // 指向第一个记录的起始位置
    Book book;
    while (fin.read((char*) &book, sizeof book)) {
        if (book.id == -1)
            break;
    }
    if (!fin.eof())
        fin.seekg(-sizeof book, ios::cur); // 退回到可用空间的第一个字节
    fin.clear();
    return fin.tellg();
}

// 图书入库
streampos Bk::add_book(Book& book) {
    ifstream fin;
    ofstream fout;
    read_header();
    book.date_of_entry = get_current_date(); // 入库日期设置
    book.id = lib_header.count_id;
    book.borrow_count = 0;
    book.borrowed = false;
    open_for_read(fin);
    streampos pos = alloc(fin); // 寻找空闲位置
    fin.close();
    open_for_write(fout);
    fout.seekp(pos); // 移动文件指针
    fout.write((char*) &book, sizeof (Book));
    fout.close();
    ++lib_header.count;
    ++lib_header.count_id;
    write_header();
    return pos; // 返回插入的位置
}

// 图书出库。pos指明了待删除图书所在位置。
void Bk::delete_book(streampos pos) {
    Book book;
    ifstream fin;
    ofstream fout;
    read_header();
    open_for_read(fin);
    fin.seekg(pos);
    fin.read((char*) &book, sizeof (Book));
    fin.close();
    book.id = -1; // ID改为-1，代表图书已经出库
    open_for_write(fout);
    fout.seekp(pos);
    fout.write((char*) &book, sizeof (Book));
    fout.close();
    --lib_header.count;
    write_header();
}

// 列出所有图书
void Bk::traverse() {
    ifstream fin;
    Book book;
    open_for_read(fin);
    fin.clear();
    fin.seekg(sizeof lib_header, ios::beg); // 文件指针移动到第一个字节的位置
    while (fin.read((char*) &book, sizeof (Book)))
        if (book.id != -1) // 该书仍在库中
            cout << book.id << ", " << quoted(book.title) << ", " << quoted(book.author) << ", "
                 << quoted(book.category) << ", " << quoted(book.publisher) << ", " << book.publishing_year << ", "
                 << book.date_of_entry << ", " << book.borrow_count << ", " << boolalpha << book.borrowed << noboolalpha
                 << endl;
    fin.close();
}

int Bk::show_num_of_books_in_stock() {
    read_header();
    return lib_header.count;
}

// 查询对应位置图书信息
Book* Bk::find(streampos pos) {
    Book* book = new Book();
    ifstream fin;
    open_for_read(fin);
    fin.seekg(pos);
    fin.read((char*) book, sizeof (Book));
    fin.close();
    return book;
}

// 借书。pos指明了待借图书所在位置。
bool Bk::b(streampos pos) {
    Book book;
    ifstream fin;
    ofstream fout;
    open_for_read(fin);
    fin.seekg(pos);
    fin.read((char*) &book, sizeof (Book));
    fin.close();
    if (book.id == -1)
        return false;
    if (book.borrowed)
        return false;
    book.borrowed = true;
    ++book.borrow_count;
    open_for_write(fout);
    fout.seekp(pos);
    fout.write((char*) &book, sizeof (Book));
    fout.close();
    return true;
}

// 还书。pos指明了待还图书所在位置。
bool Bk::r(streampos pos) {
    Book book;
    ifstream fin;
    ofstream fout;
    open_for_read(fin);
    fin.seekg(pos);
    fin.read((char*) &book, sizeof (Book));
    fin.close();
    if (book.id == -1)
        return false;
    if (!book.borrowed)
        return false;
    book.borrowed = false;
    open_for_write(fout);
    fout.seekp(pos);
    fout.write((char*) &book, sizeof (Book));
    fout.close();
    return true;
}
