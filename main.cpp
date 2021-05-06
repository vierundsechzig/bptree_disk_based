#include <iostream>
#include <fstream>
#include <iomanip>
#include <list>
#include <string>
#include "Library.h"

using std::cin;
using std::cout;
using std::endl;
using std::list;
using std::quoted;
using std::boolalpha;
using std::noboolalpha;
using std::string;
using std::ifstream;
using std::ofstream;

void clear_buff() {
    cin.clear();
    cin.sync();
}

void menu() {
    cout << "1. Find" << endl;
    cout << "2. Add a book" << endl;
    cout << "3. Delete a book" << endl;
    cout << "4. Borrow a book" << endl;
    cout << "5. Return a book" << endl;
    cout << "6. List all books" << endl;
    cout << "7. Display leaf nodes" << endl;
    cout << "8. Traverse B+ trees" << endl;
    cout << "9. Show menu" << endl;
    cout << "*. Show number of books" << endl;
    cout << "0. Exit" << endl;
}

int main() {
    char ch;
    char field;
    int pby;
    char str[SIZE];
    Query q;
    Library lib;
    unsigned int id;
    Book book;
    string prompt;
    menu();
    do {
        cout << "Enter your choice:";
        ch = cin.get();
        switch (ch) {
            case '1':
                cout << "Which field (0=title, 1=author, 2=category, 3=publisher, 4=publishing year) would you like to find by?" << endl;
                clear_buff();
                field = cin.get();
                clear_buff();
                if (field <'0' || field > '4')
                    cout << "Error!" << endl;
                else {
                    list<unsigned int>* id_list;
                    list<Book*>* book_ptr_list;
                    if (field == '4') {
                        cout << "Enter publishing year:";
                        cin >> pby;
                        id_list = lib.find_by_puby(pby);
                    }
                    else {
                        switch (field) {
                            case '0':
                                q = Query::TTL;
                                prompt = "title";
                                break;
                            case '1':
                                q = Query::ATR;
                                prompt = "author";
                                break;
                            case '2':
                                q = Query::CTG;
                                prompt = "category";
                                break;
                            case '3':
                                q = Query::PUB;
                                prompt = "publisher";
                                break;
                            default:
                                break;
                        }
                        cout << "Enter " << prompt << ":";
                        cin.getline(str, SIZE);
                        id_list = lib.find_by_str(str, q);
                    }
                    book_ptr_list = lib.find_by_id(*id_list);
                    auto id_pos = id_list->begin();
                    for (auto book : *book_ptr_list) {
                        cout << "Position: " << *id_pos << endl;
                        cout << book->id << ", " << quoted(book->title) << ", " << quoted(book->author) << ", "
                             << quoted(book->category) << ", " << quoted(book->publisher) << ", " << book->publishing_year << ", "
                             << book->date_of_entry << ", " << book->borrow_count << ", " << boolalpha << book->borrowed << noboolalpha
                             << endl;
                        ++id_pos;
                        delete book;
                    }
                    delete book_ptr_list;
                    delete id_list;
                }
                break;
            case '2':
                clear_buff();
                cout << "Title:";
                cin.getline(book.title, SIZE);
                cout << "Author:";
                cin.getline(book.author, SIZE);
                cout << "Category:";
                cin.getline(book.category, SIZE);
                cout << "Publisher:";
                cin.getline(book.publisher, SIZE);
                cout << "Publishing year:";
                cin >> book.publishing_year;
                cout << (lib.instock(book) ? "Successful" : "Failed") << endl;
                break;
            case '3':
                cout << "Enter position:" << endl;
                cin >> id;
                cout << (lib.outofstock(id) ? "Successful" : "Failed") << endl;
                break;
            case '4':
                cout << "Enter position:" << endl;
                cin >> id;
                cout << (lib.borrow(id) ? "Successful" : "Failed") << endl;
                break;
            case '5':
                cout << "Enter position:" << endl;
                cin >> id;
                cout << (lib.ret(id) ? "Successful" : "Failed") << endl;
                break;
            case '6':
                lib.traverse_data();
                break;
            case '7':
                cout << "Title:" << endl;
                lib.traverse_str(Query::TTL);
                cout << "Author:" << endl;
                lib.traverse_str(Query::ATR);
                cout << "Category:" << endl;
                lib.traverse_str(Query::CTG);
                cout << "Publisher:" << endl;
                lib.traverse_str(Query::PUB);
                cout << "Publishing year:" << endl;
                lib.traverse_puby();
                break;
            case '8':
                cout << "Title:" << endl;
                lib.dis_str(Query::TTL);
                cout << "Author:" << endl;
                lib.dis_str(Query::ATR);
                cout << "Category:" << endl;
                lib.dis_str(Query::CTG);
                cout << "Publisher:" << endl;
                lib.dis_str(Query::PUB);
                cout << "Publishing year:" << endl;
                lib.dis_puby();
                break;
            case '9':
                menu();
                break;
            case '*':
                cout << lib.get_books_in_store() << endl;
                break;
            case '0':
                cout << "Thank you for using our program!" << endl;
                break;
            default:
                cout << "Wrong choice. Enter \'9\' for help." << endl;
                break;
        }
        clear_buff();
    } while (ch!='0');
    return 0;
}