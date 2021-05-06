//
// Created by Nick Liu on 2019/12/23.
//

#ifndef SOFTWARE_LIBRARY_LIBRARY_H
#define SOFTWARE_LIBRARY_LIBRARY_H

#include <list>
#include <string>
#include "Book.h"
#include "bptree_disk_based.h"

using std::list;
using std::string;

enum class Query {TTL=0, ATR=1, CTG=2, PUB=3}; // 在查找中依据的条件

class Library {
private:
    bptree_disk_based<str_index_type>* title;
    bptree_disk_based<str_index_type>* author;
    bptree_disk_based<str_index_type>* category;
    bptree_disk_based<str_index_type>* publisher;
    bptree_disk_based<int_index_type>* pubyear;
    Bk* data;
public:
    Library();
    bool instock(Book&); // 入库
    bool outofstock(unsigned int); // 出库
    bool borrow(unsigned int); // 借书
    bool ret(unsigned int); // 还书
    list<unsigned int>* find_by_str(const char* comparand, Query field); // 给定比较字、条件和比较字段来找书。用于前四个字段的索引。
    list<unsigned int>* find_by_puby(int comparand); // 给定比较字、条件和比较字段来找书。用于出版年份的索引。
    list<Book*>* find_by_id(list<unsigned int>&); // 通过书在索引文件中的位置读取书的信息。
    int get_books_in_store(); // 获取库存图书量
    void traverse_data(); // 仅调试用。遍历数据文件。
    void traverse_str(Query); // 仅调试用。遍历指定的索引文件的叶节点内容。
    void traverse_puby(); // 仅调试用。遍历出版年份索引文件的叶节点内容。
    void dis_str(Query); // 仅调试用。遍历指定的索引文件的所有节点内容。
    void dis_puby(); // 仅调试用。遍历出版年份索引文件的所有节点内容。
    ~Library();
};

#endif //SOFTWARE_LIBRARY_LIBRARY_H
