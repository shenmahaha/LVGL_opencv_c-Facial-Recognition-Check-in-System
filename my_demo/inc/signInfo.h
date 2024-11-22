#ifndef SIGNINFO_H
#define SIGNINFO_H

#include <string>
#include <sstream> // 用于序列化和反序列化
#include <iostream>
using namespace std;
class SignInfo {
  private:
    string date;     // 用户ID
    string id;     // 用户ID
    string name;   // 用户姓名
    string time;   // 签到时间

  public:
    // 构造函数
    SignInfo(const string & date,const string &id, const string &name, const string &time);

     const string &get_date() const ;

    const string &get_id() const ;

    const string &get_name() const ;

    const string &get_time() const;


    void set_time(string time);

    string to_string();
};
#endif // SIGNINFO_H
