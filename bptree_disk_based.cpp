//
// Created by Nick Liu on 2019/12/24.
//

#include "bptree_disk_based.h"

str_index_type& str_index_type::operator =(int ro) {
    if (ro == 0)
        memset(this->field, 0 , sizeof this->field);
    this->ID = ro;
    return *this;
}

int_index_type& int_index_type::operator =(int ro) {
    if (ro == 0)
        field = 0;
    this->ID = ro;
    return *this;
}

bool operator ==(const str_index_type& left, const str_index_type& right) {
    return (strcmp(left.field, right.field) == 0) && left.ID == right.ID;
}

bool operator !=(const str_index_type& left, const str_index_type& right) {
    return !(left == right);
}

bool operator <(const str_index_type& left, const str_index_type& right) {
    int res = strcmp(left.field, right.field);
    if (res < 0)
        return true;
    if (res > 0)
        return false;
    return left.ID < right.ID;
}

bool operator >(const str_index_type& left, const str_index_type& right) {
    return !(left < right) && !(left == right);
}

bool operator <=(const str_index_type& left, const str_index_type& right) {
    return left < right || left == right;
}

bool operator >=(const str_index_type& left, const str_index_type& right) {
    return !(left < right);
}

ostream& operator <<(ostream& out, const str_index_type& ro) {
    return out << std::quoted(ro.field) << "," << ro.ID;
}

bool operator ==(const int_index_type& left, const int_index_type& right) {
    return left.field == right.field && left.ID == right.ID;
}

bool operator !=(const int_index_type& left, const int_index_type& right) {
    return !(left == right);
}

bool operator <(const int_index_type& left, const int_index_type& right) {
    return (left.field < right.field) || (left.field == right.field && left.ID < right.ID);
}

bool operator >(const int_index_type& left, const int_index_type& right) {
    return !(left < right) && !(left == right);
}

bool operator <=(const int_index_type& left, const int_index_type& right) {
    return left < right || left == right;
}

bool operator >=(const int_index_type& left, const int_index_type& right) {
    return !(left < right);
}

ostream& operator <<(ostream& out, const int_index_type& ro) {
    return out << ro.field << "," << ro.ID;
}