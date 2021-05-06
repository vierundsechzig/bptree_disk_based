//
// Created by Nick Liu on 2019/12/24.
//

#ifndef SOFTWARE_LIBRARY_BPTREE_DISK_BASED_H
#define SOFTWARE_LIBRARY_BPTREE_DISK_BASED_H

#include <algorithm>
#include <cstring>
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>

using std::string;
using std::ostream;
using std::ifstream;
using std::ofstream;
using std::streampos;
using std::ios;
using std::lower_bound;
using std::cout;
using std::endl;

const int DEG = 3; // B+树阶数
const int MAXSIZE = 50;

inline int ceiling_of_half(int n) {
    return (n % 2) ? (n/2 + 1) : n/2;
}

struct str_index_type { // 字符串型索引
    char field[MAXSIZE]{};
    int ID{};

    friend bool operator==(const str_index_type &, const str_index_type &);

    friend bool operator!=(const str_index_type &, const str_index_type &);

    friend bool operator<(const str_index_type &, const str_index_type &);

    friend bool operator>(const str_index_type &, const str_index_type &);

    friend bool operator<=(const str_index_type &, const str_index_type &);

    friend bool operator>=(const str_index_type &, const str_index_type &);

    friend ostream &operator<<(ostream &, const str_index_type &);

    str_index_type &operator=(int);
};

struct int_index_type { // 整型索引
    int field{};
    int ID{};

    friend bool operator==(const int_index_type &, const int_index_type &);

    friend bool operator!=(const int_index_type &, const int_index_type &);

    friend bool operator<(const int_index_type &, const int_index_type &);

    friend bool operator>(const int_index_type &, const int_index_type &);

    friend bool operator<=(const int_index_type &, const int_index_type &);

    friend bool operator>=(const int_index_type &, const int_index_type &);

    friend ostream &operator<<(ostream &, const int_index_type &);

    int_index_type &operator=(int);
};

// 注：由于模板类不支持分离式编译，故只能将代码实现均放入头文件中。

template<typename keyType>
struct bptree_node {
    unsigned int parent{}; // 父节点位置
    bool root{}; // 是否为根节点
    bool leaf{}; // 是否为叶节点
    bool last_leaf{}; // 是否为最后一个叶节点
    int child_num{}; // 关键字数。值为0时代表该节点已被删除。
    keyType key[DEG + 1]{}; // 关键字数组。最后一个位置闲置。
    union {
        unsigned int children[DEG + 1]{}; // 当前节点为索引节点时有效，指示子节点在索引文件中的位置。最后一个位置闲置。
        unsigned int record_pos[DEG + 1]; // 当前节点为叶节点时有效，指示对应关键字在数据文件中的位置。最后一个位置闲置。
    };
    unsigned int next{}; // 当前节点为叶节点时有效，指示后驱叶节点在索引文件中的位置。
};

template<typename keyType>
class bptree_disk_based {
private:
    struct bptree_header {
        unsigned int root; // 根节点在索引文件中的位置。在文件中相对一第一个节点的第一个字节的偏移字节数是root * sizeof (bptree_node)。
        unsigned int data; // 第一个叶节点在索引文件中的位置。偏移字节数与上同理。
        bool empty; // 索引文件是否为空
    };
    bptree_header header{}; // 索引文件头
    string filename{}; // 索引文件路径
    void open_for_write(ofstream &); // 以写入的方式打开文件
    void write_node(ofstream &fout, unsigned int id, bptree_node<keyType> &node); // 写入节点数据
    void read_header(ifstream &); // 读取文件头
    void write_header(ofstream &); // 写入文件头
    streampos alloc(ifstream &); // 在索引文件中寻找空位
    streampos seek_node(unsigned int id); // 根据节点编号寻找它在文件中的位置
    void insert_in_leaf(unsigned int, unsigned int, keyType, unsigned int); // 在叶节点插入
    void delete_in_leaf(unsigned int, unsigned int); // 在叶节点删除
    void display_leaf(bptree_node<keyType> &) const; // 展示叶节点信息
    void display_node(bptree_node<keyType> &) const; // 展示节点信息
public:
    explicit bptree_disk_based(const string &filename);

    void open_for_read(ifstream &); // 以读取的方式打开文件
    void read_node(ifstream &, unsigned int, bptree_node<keyType> &); // 读取节点数据
    unsigned int find(keyType, unsigned int &); // 节点查询
    bool insert(keyType e, unsigned int data_pos); // 节点插入
    bool remove(keyType e); // 节点删除
    void traverse(); // 遍历叶子节点
    void display_tree(); // 展示整棵树的结构
};

template <typename keyType>
bptree_disk_based<keyType>::bptree_disk_based(const string& filename) {
    this->filename = filename;
    this->header.root = 0;
    this->header.data = 0;
    this->header.empty = true;
}

template <typename keyType>
void bptree_disk_based<keyType>::open_for_read(ifstream& fin) {
    fin.open(this->filename, ios::in | ios::binary);
}

template <typename keyType>
void bptree_disk_based<keyType>::open_for_write(ofstream& fout) {
    fout.open(this->filename, ios::in | ios::out | ios::binary);
}

template <typename keyType>
void bptree_disk_based<keyType>::read_header(ifstream& fin) {
    fin.seekg(0);
    fin.read(reinterpret_cast<char*> (&this->header), sizeof this->header);
}

template <typename keyType>
void bptree_disk_based<keyType>::write_header(ofstream& fout) {
    fout.seekp(0);
    fout.write(reinterpret_cast<char*> (&this->header), sizeof this->header);
}

template <typename keyType>
void bptree_disk_based<keyType>::read_node(ifstream& fin, unsigned int id, bptree_node<keyType>& node) {
    fin.clear(); // 清除eof等标志
    fin.seekg(this->seek_node(id));
    fin.read(reinterpret_cast<char*> (&node), sizeof (bptree_node<keyType>));
}

template <typename keyType>
void bptree_disk_based<keyType>::write_node(ofstream& fout, unsigned int id, bptree_node<keyType>& node) {
    fout.clear(); // 清除eof等标志
    fout.seekp(this->seek_node(id));
    fout.write(reinterpret_cast<char*> (&node), sizeof (bptree_node<keyType>));
}

template <typename keyType>
streampos bptree_disk_based<keyType>::alloc(ifstream& fin) {
    fin.clear();
    fin.seekg(sizeof this->header, ios::beg); // 指向第一个记录的起始位置
    bptree_node<keyType> node;
    while (fin.read((char*) &node, sizeof node)) {
        if (node.child_num == 0) // 当前节点被删除
            break;
    }
    if (!fin.eof())
        fin.seekg(-sizeof node, ios::cur); // 退回到可用空间的第一个字节
    fin.clear();
    return fin.tellg();
}

template <typename keyType>
streampos bptree_disk_based<keyType>::seek_node(unsigned int id) {
    return static_cast<streampos> (sizeof this->header + id * sizeof (bptree_node<keyType>));
}

template <typename keyType>
unsigned int bptree_disk_based<keyType>::find(keyType e, unsigned int& leaf_node) {
    ifstream fin;
    bptree_node<keyType> node;
    keyType* pos;
    int index;
    unsigned int node_pos;
    this->open_for_read(fin); // 打开索引文件
    this->read_header(fin); // 读取文件头
    if (this->header.empty) { // B+树为空
        leaf_node = 0; // 叶节点为0
        fin.close(); // 关闭文件
        return 0; // 插入位置为0
    }
    node_pos = this->header.root;
    this->read_node(fin, node_pos, node); // 读取根节点数据
    while (!node.leaf) { // 非叶节点时
        pos = lower_bound(node.key, node.key + node.child_num, e); // 寻找e应该插入在哪棵子树
        index = pos - node.key; // pos对应的数组下标
        if (node.root) { // 该节点为根节点
            if (index == node.child_num){ // 下标越界，说明待查找关键字比B+树中所有关键字都大（只有根节点会发生这种情况）
                unsigned int current_pos = this->header.root; // 当前节点在文件中的相对位置
                while (!node.leaf) {
                    current_pos = node.children[node.child_num - 1];
                    this->read_node(fin, current_pos, node);
                }
                fin.close();
                leaf_node = current_pos;
                return node.child_num; // 插入的位置在最后一个元素的后面
            }
        }
        node_pos = node.children[index];
        this->read_node(fin, node_pos, node); // 读取节点数据
    }
    fin.close(); // 关闭文件
    leaf_node = node_pos;
    pos = lower_bound(node.key, node.key + node.child_num, e);
    index = pos-node.key;
    return index;
    // 返回节点应在数组的哪个位置插入
}

template <typename keyType>
void bptree_disk_based<keyType>::insert_in_leaf(unsigned int leaf_node, unsigned int index, keyType e, unsigned int data_pos) {
    ifstream fin;
    ofstream fout;
    bptree_node<keyType> node, root_node, current_node, parent_node, split_node;
    this->open_for_read(fin);
    this->read_header(fin);
    this->read_node(fin, leaf_node, node);
    this->read_node(fin, this->header.root, root_node);
    int cur_index = node.child_num; // 当前索引值
    while (cur_index != index) {
        node.key[cur_index] = node.key[cur_index - 1];
        node.record_pos[cur_index] = node.record_pos[cur_index - 1];
        --cur_index;
    }
    node.key[index] = e;
    node.record_pos[index] = data_pos;
    ++node.child_num;
    fin.close();
    this->open_for_write(fout);
    this->write_node(fout, leaf_node, node); // 写入磁盘
    fout.close();
    keyType max = root_node.key[root_node.child_num - 1];
    if (e > max) { // 插入的元素比B+树中最大的元素还大
        unsigned int temp_node_pos = this->header.root;
        bptree_node<keyType> temp_node;
        this->open_for_read(fin);
        this->read_node(fin, temp_node_pos, temp_node);
        fin.close();
        while (!temp_node.leaf) {
            this->open_for_write(fout);
            temp_node.key[temp_node.child_num-1] = e;
            this->write_node(fout, temp_node_pos, temp_node);
            fout.close();
            temp_node_pos = temp_node.children[temp_node.child_num - 1];
            this->open_for_read(fin);
            this->read_node(fin, temp_node_pos, temp_node);
            fin.close();
        }
    }
    unsigned int current = leaf_node; // 当前节点在文件中的位置
    unsigned int parent = node.parent; // 父节点在文件中的位置
    unsigned int split; // 分裂节点在文件中的位置
    this->open_for_read(fin);
    this->read_node(fin, current, current_node);
    this->read_node(fin, parent, parent_node);
    fin.close();
    while (current_node.child_num == DEG+1) { // 超过B+树阶数
        current_node.child_num = ceiling_of_half(DEG);
        this->open_for_read(fin);
        split = (static_cast<unsigned int> (this->alloc(fin)) - (sizeof this->header)) / sizeof (bptree_node<keyType>); // 在文件中分配一个位置
        fin.close();
        split_node.leaf = current_node.leaf;
        split_node.child_num = DEG + 1 - current_node.child_num;
        split_node.parent = current_node.parent;
        memcpy(split_node.key, current_node.key + current_node.child_num, split_node.child_num * sizeof (keyType));
        if (current_node.leaf) {
            split_node.last_leaf = current_node.last_leaf;
            split_node.next = current_node.next;
            current_node.last_leaf = false;
            current_node.next = split;
            memcpy(split_node.record_pos, current_node.record_pos + current_node.child_num, split_node.child_num * sizeof (unsigned int));
        }
        else {
            memcpy(split_node.children, current_node.children + current_node.child_num, split_node.child_num * sizeof (unsigned int));
            for (int k=0; k<split_node.child_num; ++k) {
                bptree_node<keyType> child_node;
                // 读
                this->open_for_read(fin);
                this->read_node(fin, split_node.children[k], child_node);
                fin.close();
                // 修改
                child_node.parent = split;
                // 写
                this->open_for_write(fout);
                this->write_node(fout, split_node.children[k], child_node);
                fout.close();
            }
        }
        this->open_for_write(fout);
        this->write_node(fout, current, current_node);
        this->write_node(fout, split, split_node);
        fout.close();
        if (current_node.root) { // 根节点要分裂
            this->open_for_read(fin);
            parent = (static_cast<unsigned int> (this->alloc(fin)) - (sizeof this->header)) / sizeof (bptree_node<keyType>); // 新分配一个位置
            fin.close();
            parent_node.child_num = 2;
            parent_node.root = true;
            parent_node.leaf = false;
            parent_node.key[0] = current_node.key[current_node.child_num - 1];
            parent_node.children[0] = current;
            parent_node.key[1] = split_node.key[split_node.child_num - 1];
            parent_node.children[1] = split;
            current_node.parent = parent;
            current_node.root = false;
            split_node.parent = parent;
            split_node.root = false;
            this->header.root = parent;
            // 写入根节点
            this->open_for_write(fout);
            this->write_header(fout);
            this->write_node(fout, parent, parent_node);
            this->write_node(fout, current, current_node);
            this->write_node(fout, split, split_node);
            fout.close();
            break;
        }
        // 非根节点
        keyType* const key_end = lower_bound(parent_node.key, parent_node.key + parent_node.child_num, current_node.key[current_node.child_num - 1]);
        const int key_index = key_end - parent_node.key; // 父节点要加入的关键字的位置的下标值
        int p_index = parent_node.child_num;
        while (p_index != key_index) {
            parent_node.key[p_index] = parent_node.key[p_index - 1];
            parent_node.children[p_index] = parent_node.children[p_index - 1];
            --p_index;
        }
        parent_node.key[p_index] = current_node.key[current_node.child_num - 1];
        parent_node.children[p_index] = current;
        parent_node.children[p_index + 1] = split;
        ++parent_node.child_num;
        // 写入节点
        this->open_for_write(fout);
        this->write_node(fout, parent, parent_node);
        this->write_node(fout, current, current_node);
        this->write_node(fout, split, split_node);
        fout.close();
        // 更改current和parent位置
        current = parent;
        parent = parent_node.parent;
        // 读取下一次循环时current和parent的节点信息
        this->open_for_read(fin);
        this->read_node(fin, current, current_node);
        this->read_node(fin, parent, parent_node);
        fin.close();
    }
}

template <typename keyType>
bool bptree_disk_based<keyType>::insert(keyType e, unsigned int data_pos) {
    ifstream fin;
    ofstream fout;
    this->open_for_read(fin);
    this->read_header(fin);
    fin.close();
    if (this->header.empty) { // 若B+树为空
        bptree_node<keyType> root;
        root.leaf = true;
        root.root = true;
        root.key[0] = e;
        root.record_pos[0] = data_pos;
        root.child_num = 1;
        root.last_leaf = true;
        this->header.root = 0;
        this->header.data = 0;
        this->header.empty = false;
        this->open_for_write(fout);
        this->write_node(fout, 0, root);
        this->write_header(fout);
        fout.close();
        return true;
    }
    unsigned int leaf_node_pos;
    unsigned int key_index = this->find(e, leaf_node_pos);
    bptree_node<keyType> leaf_node;
    this->open_for_read(fin);
    this->read_node(fin, leaf_node_pos, leaf_node);
    fin.close();
    if ((key_index != leaf_node.child_num) && (leaf_node.key[key_index] == e)) // 已经有这个元素
        return false;
    this->insert_in_leaf(leaf_node_pos, key_index, e, data_pos);
    return true;
}

template <typename keyType>
void bptree_disk_based<keyType>::delete_in_leaf(unsigned int leaf_node, unsigned int index) {
    ifstream fin;
    ofstream fout;
    int i;
    this->open_for_read(fin);
    this->read_header(fin);
    bptree_node<keyType> node, parent_node;
    this->read_node(fin, leaf_node, node);
    fin.close();
    bool isMax = (index == node.child_num - 1); // 删除的是否是最大的元素
    const keyType temp = node.key[index]; // 待删除关键字的临时副本
    for (i = index; i < node.child_num - 1; ++i) {
        node.key[i] = node.key[i+1];
        node.record_pos[i] = node.record_pos[i+1];
    }
    node.children[i] = 0;
    node.record_pos[i] = 0;
    --node.child_num;

    this->open_for_write(fout);
    this->write_node(fout, leaf_node, node);
    fout.close();

    if (node.root) { // 该节点为根节点
        if (node.child_num == 0) { // 节点删光了
            this->header.root = 0;
            this->header.data = 0;
            this->header.empty = true;
        }
        // 写入
        this->open_for_write(fout);
        this->write_node(fout, leaf_node, node);
        this->write_header(fout);
        fout.close();
        return; // 操作结束
    }

    if (isMax) { // 删除的是最大值
        bptree_node<keyType> temp_node = node;
        keyType* key_pos;
        int key_index;
        if (!temp_node.root) {
            do {
                this->open_for_read(fin);
                this->read_node(fin, temp_node.parent, parent_node);
                fin.close();
                key_pos = lower_bound(parent_node.key, parent_node.key + parent_node.child_num, temp);
                key_index = key_pos - parent_node.key;
                *key_pos = temp_node.key[temp_node.child_num - 1]; // 索引值改成最新的最大值
                // 写入
                this->open_for_write(fout);
                this->write_node(fout, temp_node.parent, parent_node);
                fout.close();
                temp_node = parent_node;
            } while (!temp_node.root && temp_node.child_num == key_index + 1);  // 不是根节点且修改的值是关键字数组里面的最大值，说明父节点的索引值要变化
        }
    }

    unsigned int current, sibling, parent;
    bptree_node<keyType> cur_node, sib_node;
    current = leaf_node;
    cur_node = node;
    parent = cur_node.parent;
    this->open_for_read(fin);
    this->read_node(fin, parent, parent_node);
    fin.close();

    while (!cur_node.root) { // 不是根节点
        if (cur_node.child_num < ceiling_of_half(DEG)) { // 关键字数不满足B+树所要求的最小个数
            if (parent_node.children[parent_node.child_num - 1] == current) { // 当前节点为父节点的最右边的子节点
                sibling = parent_node.children[parent_node.child_num - 2]; // 对它的左邻兄弟（倒数第二个）节点操作
                this->open_for_read(fin);
                this->read_node(fin, sibling, sib_node);
                fin.close();
                if (sib_node.child_num == ceiling_of_half(DEG)) { // 不够借，则当前节点并入左邻兄弟节点
                    memcpy(sib_node.key + sib_node.child_num, cur_node.key, cur_node.child_num * sizeof (keyType));
                    if (cur_node.leaf) { // 如果是对叶节点合并
                        sib_node.next = cur_node.next;
                        sib_node.last_leaf = cur_node.last_leaf;
                        memcpy(sib_node.record_pos + sib_node.child_num, cur_node.record_pos, cur_node.child_num * sizeof (unsigned int)); // 复制数据在数据文件的索引信息
                    }
                    else {
                        memcpy(sib_node.children + sib_node.child_num, cur_node.children, cur_node.child_num * sizeof (unsigned int)); // 复制子树信息
                        for (int k=0; k<cur_node.child_num; ++k) {
                            bptree_node<keyType> child_node;
                            this->open_for_read(fin);
                            this->read_node(fin, cur_node.children[k], child_node);
                            fin.close();
                            child_node.parent = sibling;
                            this->open_for_write(fout);
                            this->write_node(fout, cur_node.children[k], child_node);
                            fout.close();
                        }
                    }
                    sib_node.child_num += cur_node.child_num;
                    cur_node.child_num = 0; // 标记为删除
                    parent_node.key[parent_node.child_num-2] = parent_node.key[parent_node.child_num-1];
                    parent_node.key[parent_node.child_num-1] = 0;
                    parent_node.children[parent_node.child_num-1] = 0;
                    --parent_node.child_num; // 合并后少了一个节点
                }
                else { // 够借
                    int key_index = cur_node.child_num;
                    while (key_index > 0) {
                        cur_node.key[index] = cur_node.key[index-1];
                        if (cur_node.leaf) // 是叶节点
                            cur_node.record_pos[index] = cur_node.record_pos[index-1];
                        else // 不是叶节点
                            cur_node.children[index] = cur_node.children[index-1];
                        --key_index;
                    }
                    // 以上操作为将元素往后移一位
                    cur_node.key[0] = sib_node.key[sib_node.child_num - 1];
                    ++cur_node.child_num;
                    sib_node.key[sib_node.child_num - 1] = 0;
                    if (cur_node.leaf) // 是叶节点
                        sib_node.record_pos[sib_node.child_num - 1] = 0;
                    else
                        sib_node.children[sib_node.child_num - 1] = 0;
                    --sib_node.child_num; // 借结点后关键字个数要减1，并清除相应关键字内容
                    parent_node.key[parent_node.child_num - 2] = sib_node.key[sib_node.child_num - 1]; // sibling结点的最大关键字发生了变化，所以父节点中对应的关键字值也要变化。
                }
            }
            else { // 不是最右边的子节点
                keyType* const cur_key_pos = lower_bound(parent_node.key, parent_node.key + parent_node.child_num, cur_node.key[cur_node.child_num-1]);
                const int cur_index = cur_key_pos - parent_node.key;
                sibling = parent_node.children[cur_index + 1]; // 对右邻兄弟节点操作
                this->open_for_read(fin);
                this->read_node(fin, sibling, sib_node);
                fin.close();
                if (sib_node.child_num == ceiling_of_half(DEG)) { // 不够借，则将右邻节点合并
                    memcpy(cur_node.key + cur_node.child_num, sib_node.key, sib_node.child_num * sizeof (keyType));
                    if (cur_node.leaf) {
                        cur_node.next = sib_node.next; // 改变后继节点指向
                        cur_node.last_leaf = sib_node.last_leaf;
                        memcpy(cur_node.record_pos + cur_node.child_num, sib_node.record_pos, sib_node.child_num * sizeof (unsigned int)); // 复制记录索引信息
                    }
                    else {
                        memcpy(cur_node.children + cur_node.child_num, sib_node.children, sib_node.child_num * sizeof (unsigned int));
                        for (int k=0; k<sib_node.child_num; ++k) {
                            bptree_node<keyType> child_node;
                            this->open_for_read(fin);
                            this->read_node(fin, sib_node.children[k], child_node);
                            fin.close();
                            child_node.parent = current;
                            this->open_for_write(fout);
                            this->write_node(fout, sib_node.children[k], child_node);
                            fout.close();
                        }
                    }
                    cur_node.child_num += sib_node.child_num;
                    sib_node.child_num = 0; // 相当于删除sibling节点
                    parent_node.key[cur_index] = parent_node.key[cur_index + 1];
                    int prt_index = cur_index + 1;
                    while (prt_index < parent_node.child_num - 1) {
                        parent_node.key[prt_index] = parent_node.key[prt_index + 1];
                        parent_node.children[prt_index] = parent_node.children[prt_index + 1];
                        ++prt_index;
                    }
                    parent_node.key[prt_index] = 0;
                    parent_node.children[prt_index] = 0;
                    --parent_node.child_num;
                }
                else {
                    cur_node.key[cur_node.child_num] = sib_node.key[0];
                    memcpy(sib_node.key, sib_node.key + 1, (sib_node.child_num - 1) * sizeof (keyType));
                    sib_node.key[sib_node.child_num - 1] = 0;
                    if (cur_node.leaf) {
                        cur_node.record_pos[cur_node.child_num] = sib_node.record_pos[0];
                        memcpy(sib_node.record_pos, sib_node.record_pos + 1, (sib_node.child_num - 1) * sizeof (unsigned int));
                        sib_node.record_pos[sib_node.child_num - 1] = 0;
                    }
                    else {
                        cur_node.children[cur_node.child_num] = sib_node.children[0];
                        memcpy(sib_node.children, sib_node.children + 1, (sib_node.child_num - 1) * sizeof (unsigned int));
                        sib_node.children[sib_node.child_num - 1] = 0;
                    }
                    ++cur_node.child_num;
                    --sib_node.child_num;
                    parent_node.key[cur_index] = cur_node.key[cur_node.child_num - 1]; // current结点的最大关键字发生了变化，所以父节点中对应的关键字值也要变化。
                }
            }
            // 写入
            this->open_for_write(fout);
            this->write_node(fout, parent, parent_node);
            this->write_node(fout, current, cur_node);
            this->write_node(fout, sibling, sib_node);
            fout.close();
        }
        // 往上回溯
        current = parent;
        cur_node = parent_node;
        parent = parent_node.parent;
        this->open_for_read(fin);
        this->read_node(fin, parent, parent_node);
        fin.close();
    }
    if (cur_node.child_num == 1) { // 根节点只有一个子节点
        bptree_node<keyType> child_node;
        this->header.root = cur_node.children[0]; // 换根
        this->open_for_read(fin);
        this->read_node(fin, cur_node.children[0], child_node);
        fin.close();
        child_node.root = true;
        this->open_for_write(fout);
        this->write_node(fout, cur_node.children[0], child_node);
        cur_node.child_num = 0; // 表明原来的根节点已被删除
        this->write_node(fout, current, cur_node);
        this->write_header(fout);
        fout.close();
    }
}

template <typename keyType>
bool bptree_disk_based<keyType>::remove(keyType e) {
    unsigned int leaf;
    unsigned int key_index = this->find(e, leaf);
    ifstream fin;
    bptree_node<keyType> node;
    this->open_for_read(fin);
    this->read_header(fin);
    this->read_node(fin, leaf, node);
    fin.close();
    if (this->header.empty || key_index == node.child_num || node.key[key_index] != e) // 树为空，或指针指向的区域为越界位置，或对应位置的元素不是待删除元素
        return false;
    this->delete_in_leaf(leaf, key_index);
    return true;
}

template <typename keyType>
void bptree_disk_based<keyType>::display_leaf(bptree_node<keyType>& node) const {
    cout << "Next: " << node.next << "\n"
         << "Number of keys: " << node.child_num << "\n"
         << "Key list:" << endl;
    for (int i=0; i<node.child_num; ++i)
        cout << node.key[i] << " ";
    cout << endl;
    cout << "Last leaf: " << std::boolalpha << node.last_leaf << std::noboolalpha << endl;
}

template <typename keyType>
void bptree_disk_based<keyType>::display_node(bptree_node<keyType>& node) const {
    if (node.root)
        cout << "Root node" << endl;
    else
        cout << "Parent: " << node.parent << endl;
    cout << "Leaf: " << std::boolalpha << node.leaf << endl;
    if (node.leaf) {
        cout << "Next: " << node.next << "\n"
             << "Last_leaf: " << node.last_leaf << endl;
    }
    cout << std::noboolalpha;
    cout << "Degrees: " << node.child_num << "\n"
         << "Key list: " << endl;
    for (int i=0; i<node.child_num; ++i) {
        cout << node.key[i] << endl;
        if (node.leaf)
            cout << "Record position: " << node.record_pos[i];
        else
            cout << "Child position: " << node.children[i];
        cout << endl;
    }
}

template <typename keyType>
void bptree_disk_based<keyType>::traverse() {
    ifstream fin;
    this->open_for_read(fin);
    this->read_header(fin);
    unsigned int leaf;
    bptree_node<keyType> node;
    leaf = this->header.data;
    this->read_node(fin, leaf, node);
    while (!node.last_leaf) {
        this->display_leaf(node);
        leaf = node.next;
        this->read_node(fin, leaf, node);
    }
    this->display_leaf(node);
    fin.close();
}

template <typename keyType>
void bptree_disk_based<keyType>::display_tree() {
    ifstream fin;
    this->open_for_read(fin);
    this->read_header(fin);
    unsigned int pos = 0;
    bptree_node<keyType> node;
    this->read_node(fin, pos, node);
    while (!fin.eof()) {
        cout << "Position " << pos << ":";
        if (node.child_num == 0)
            cout << " deleted." <<endl;
        else {
            cout << " \n";
            this->display_node(node);
        }
        ++pos;
        this->read_node(fin, pos, node);
    }
    fin.close();
}

#endif //SOFTWARE_LIBRARY_BPTREE_DISK_BASED_H
