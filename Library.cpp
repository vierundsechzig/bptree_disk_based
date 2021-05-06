//
// Created by Nick Liu on 2019/12/23.
//

#include "Library.h"
#include <cstring>
#include <fstream>

using std::ifstream;
using std::cout;

Library::Library() {
    ifstream fin;
    ofstream fout;
    fin.open("LIB", ios::in | ios::binary);
    if (!fin) { // 文件不存在，则创建以下文件
        fin.close();
        fout.open("LIB", ios::out | ios::binary);
        fout.close();
        fout.open("TITLE", ios::out | ios::binary);
        fout.close();
        fout.open("AUTHOR", ios::out | ios::binary);
        fout.close();
        fout.open("CATEGORY", ios::out | ios::binary);
        fout.close();
        fout.open("PUBLISHR", ios::out | ios::binary);
        fout.close();
        fout.open("PUBYEAR", ios::out | ios::binary);
        fout.close();
    }
    else // 否则直接关闭
        fin.close();
    this->title = new bptree_disk_based<str_index_type>("TITLE");
    this->author = new bptree_disk_based<str_index_type>("AUTHOR");
    this->category = new bptree_disk_based<str_index_type>("CATEGORY");
    this->publisher = new bptree_disk_based<str_index_type>("PUBLISHR");
    this->pubyear = new bptree_disk_based<int_index_type>("PUBYEAR");
    this->data = new Bk("LIB");
}

// 图书入库
bool Library::instock(Book& book) {
    streampos pos = data->add_book(book); // 入库并获取图书的位置
    unsigned int id = Bk::postoid(pos);
    str_index_type ttl{}, atr{}, ctg{}, pub{};
    int_index_type pby{};
    ttl.ID = atr.ID = ctg.ID = pub.ID = pby.ID = book.id;
    strncpy(ttl.field, book.title, SIZE);
    strncpy(atr.field, book.author, SIZE);
    strncpy(ctg.field, book.category, SIZE);
    strncpy(pub.field, book.publisher, SIZE);
    pby.field = book.publishing_year;
    bool a = this->title->insert(ttl, id);
    bool b = this->author->insert(atr, id);
    bool c = this->category->insert(ctg, id);
    bool d = this->publisher->insert(pub, id);
    bool e = this->pubyear->insert(pby, id);
    return a && b && c && d && e;
}

// 图书出库
bool Library::outofstock(unsigned int id) {
    streampos pos = Bk::idtopos(id);
    Book* b_ptr = data->find(pos);
    Book& book = *b_ptr;
    if (book.borrowed) { // 被借的书不能出库
        delete b_ptr;
        return false;
    }
    data->delete_book(pos);
    str_index_type ttl{}, atr{}, ctg{}, pub{};
    int_index_type pby{};
    ttl.ID = atr.ID = ctg.ID = pub.ID = pby.ID = book.id;
    strncpy(ttl.field, book.title, SIZE);
    strncpy(atr.field, book.author, SIZE);
    strncpy(ctg.field, book.category, SIZE);
    strncpy(pub.field, book.publisher, SIZE);
    pby.field = book.publishing_year;
    bool a = this->title->remove(ttl);
    bool b = this->author->remove(atr);
    bool c = this->category->remove(ctg);
    bool d = this->publisher->remove(pub);
    bool e = this->pubyear->remove(pby);
    delete b_ptr;
    return (book.id != -1) && a && b && c && d && e;
}

list<unsigned int>* Library::find_by_str(const char* comparand, Query field) {
    ifstream fin;
    auto* result = new list<unsigned int>();
    str_index_type str{};
    strncpy(str.field, comparand, SIZE);
    str.ID = 0;
    unsigned int leaf, index;
    bptree_node<str_index_type> node{};
    bptree_disk_based<str_index_type>* indexfile;
    switch (field) {
        case Query::TTL:
            indexfile = this->title;
            break;
        case Query::ATR:
            indexfile = this->author;
            break;
        case Query::CTG:
            indexfile = this->category;
            break;
        case Query::PUB:
            indexfile = this->publisher;
            break;
        default:
            goto END;
    }
    index = indexfile->find(str, leaf);
    indexfile->open_for_read(fin);
    indexfile->read_node(fin, leaf, node);
    while (!node.last_leaf) {
        while (index < node.child_num) {
            if (strcmp(node.key[index].field, comparand) == 0)
                result->push_back(node.record_pos[index]);
            else
                goto END;
            ++index;
        }
        index = 0;
        leaf = node.next;
        indexfile->read_node(fin, leaf, node);
    }
    while (index < node.child_num) {
        if (strcmp(node.key[index].field, comparand) == 0)
            result->push_back(node.record_pos[index]);
        else
            goto END;
        ++index;
    }
    END:
        fin.close();
        return result;
}

list<unsigned int>* Library::find_by_puby(int comparand) {
    ifstream fin;
    auto* result = new list<unsigned int>();
    int_index_type pby{};
    pby.field = comparand;
    pby.ID = 0;
    unsigned int leaf, index;
    bptree_node<int_index_type> node{};
    index = this->pubyear->find(pby, leaf);
    this->pubyear->open_for_read(fin);
    this->pubyear->read_node(fin, leaf, node);
    while (!node.last_leaf) {
        while (index < node.child_num) {
            if (node.key[index].field == comparand)
                result->push_back(node.record_pos[index]);
            else
                goto END;
            ++index;
        }
        index = 0;
        leaf = node.next;
        this->pubyear->read_node(fin, leaf, node);
    }
    while (index < node.child_num) {
        if (node.key[index].field == comparand)
            result->push_back(node.record_pos[index]);
        else
            goto END;
        ++index;
    }
    END:
        fin.close();
        return result;
}

// 根据图书在索引文件中的位置返回图书列表。注意使用完成后要用delete释放列表中每个Book的空间，再释放list的空间。
list<Book*>* Library::find_by_id(list<unsigned int>& id_list) {
    auto book_list = new list<Book*>();
    auto list_ptr = id_list.begin();
    while (list_ptr != id_list.end()) {
        book_list->push_back(data->find(Bk::idtopos(*list_ptr++)));
    }
    return book_list;
}

// 返回值指示借书是否成功。对应图书不在库中或已借出则借书失败。
bool Library::borrow(unsigned int id) {
    streampos pos = static_cast<streampos> (sizeof (LIB_HEADER) + id * sizeof (Book));
    return data->b(pos);
}

// 返回值指示还书是否成功。对应图书不在库中或未借出则还书失败。
bool Library::ret(unsigned int id) {
    streampos pos = static_cast<streampos> (sizeof (LIB_HEADER) + id * sizeof (Book));
    return data->r(pos);
}

// 获取库存中的图书量
int Library::get_books_in_store() {
    return data->show_num_of_books_in_stock();
}

// 仅调试用。遍历数据文件。
void Library::traverse_data() {
    data->traverse();
}

// 仅调试用。遍历指定的索引文件的叶节点内容。
void Library::traverse_str(Query q) {
    bptree_disk_based<str_index_type>* indexfile;
    switch (q) {
        case Query::TTL:
            indexfile = this->title;
            break;
        case Query::ATR:
            indexfile = this->author;
            break;
        case Query::CTG:
            indexfile = this->category;
            break;
        case Query::PUB:
            indexfile = this->publisher;
            break;
        default:
            return;
    }
    indexfile->traverse();
}

// 仅调试用。遍历出版年份索引文件的叶节点内容。
void Library::traverse_puby() {
    this->pubyear->traverse();
}

// 仅调试用。遍历指定的索引文件的所有节点内容。
void Library::dis_str(Query q) {
    bptree_disk_based<str_index_type>* indexfile;
    switch (q) {
        case Query::TTL:
            indexfile = this->title;
            break;
        case Query::ATR:
            indexfile = this->author;
            break;
        case Query::CTG:
            indexfile = this->category;
            break;
        case Query::PUB:
            indexfile = this->publisher;
            break;
        default:
            return;
    }
    indexfile->display_tree();
}

// 仅调试用。遍历出版年份索引文件的所有节点内容。
void Library::dis_puby() {
    this->pubyear->display_tree();
}

Library::~Library() {
    delete title;
    delete author;
    delete category;
    delete publisher;
    delete pubyear;
    delete data;
}