#include "../../inc/signInfo.h"

// 构造函数定义可以保留在这里（如果没有特殊逻辑，也可以放在 .h 文件中）
SignInfo::SignInfo(const string &date,const string & id, const string & name, const string & time)
    :date(date) ,id(id), name(name), time(time)
{}


const string & SignInfo::get_date() const
{
    return date;
}
const string & SignInfo::get_id() const
{
    return id;
}


const string & SignInfo::get_name() const
{
    return name;
}


const string & SignInfo::get_time() const
{
    return time;
}

void SignInfo::set_time(string time)

{   
    this->time = time;
}

string SignInfo::to_string(){
    return date + " " + id + " " + name + " " + time;
}




