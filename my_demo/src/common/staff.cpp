#include "../../inc/staff.h"
extern mutex mtx;
Staff::Staff(string id, string name): id(id), name(name) {
    
}
string Staff::get_id(){
    return id;
}
string Staff::get_name(){
    return name;
}

string Staff::to_string(){
    return "id:" + id + " name:" + name;
}