#include "../../inc/TCP_server.h"
#include "../../inc/home.h"


extern mutex mtx;
TCPServer::TCPServer(int port, string ip) : port(port), ip(ip)
{}

// 用空格分割字符串
vector<string> split(const string & str, char delimiter)
{
    vector<string> tokens;
    stringstream ss(str);
    string token;
    while(getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

void signInDataProcessing(char * buf)
{
    // 将输入的字符数组转换为 C++ 字符串
    string input(buf);

    // 分割字符串按空格
    vector<string> parts = split(input, ' ');

    // 解析各个字段
    string data_id = parts[0].substr(parts[0].find(":") + 1); // data_id
    string date    = parts[1].substr(parts[1].find(":") + 1); // date
    string id      = parts[2].substr(parts[2].find(":") + 1); // id
    string name    = parts[3].substr(parts[3].find(":") + 1); // name

    // 直接获取time字段，时间部分在字符串最后
    string time = input.substr(input.find("time:") + 5); // 找到"time:"后面的部分

    // 输出各字段的值
    cout << "Data ID: " << data_id << endl;
    cout << "Date: " << date << endl;
    cout << "ID: " << id << endl;
    cout << "Name: " << name << endl;
    cout << "Time: " << time << endl;

    // 更新 signIn_vec 中的信息
    for(auto & i : signIn_vec) {
        if(i.get_id() == id&&i.get_date()== date) {
            i.set_time(time);
            cout<<i.get_date()<<endl;
        }
    }
    for(auto & i : signIn_vec) {
        cout<<i.to_string()<<endl;
    }

    // 发送数据回馈客户端
    string data = "data_id:5 id:" + id + " name:" + name;
    server.send_data(server.get_new_sorcket(), data);
    cout << "签到成功" << endl;
}

void sendStaff()
{
    for(auto & staff : staff_vec) {
        usleep(50 * 1000);
        string data = "data_id:3 " + staff.to_string();
        server.send_data(server.get_new_sorcket(), data);
    }
}

void pullFacialData(char * buf)
{
    // 定义需要解析的变量
    int data_id    = 0;
    long long id   = 0;
    char name[100] = {0}; // 假设名字长度不超过 100

    // 使用 sscanf 来解析字符串中的数据
    // 格式：data_id:<data_id> id:<id> name:<name>
    sscanf(buf, "data_id:%d id:%lld name:%99[^\n]", &data_id, &id, name);
    // 输出解析后的数据（可以根据实际需要进行处理）
    cout << "data_id: " << data_id << endl;
    cout << "id: " << id << endl;
    cout << "name: " << name << endl;
    char id_str[50];
    sprintf(id_str, "%lld", id);
    staff_vec.push_back(Staff(string(id_str), string(name)));
    time_t now           = time(nullptr);
    struct tm * timeinfo = localtime(&now);
    char date_str[128];
    strftime(date_str, sizeof(date_str), "%Y-%m-%d", timeinfo);
    signIn_vec.push_back(SignInfo(date_str, string(id_str), string(name), "未签到"));

    // 编写组件
    // 上锁
    mtx.lock();
    lv_obj_t * list_btn = lv_list_add_btn(sign_list, nullptr, signIn_vec[signIn_vec.size() - 1].get_name().c_str());
    signIn_name_btn_vec.push_back(list_btn);
    lv_obj_set_style_text_font(list_btn, font, 0);
    lv_obj_t * signIn_label = lv_label_create(list_btn);
    lv_obj_set_style_text_font(signIn_label, font, 0);
    lv_label_set_text(signIn_label, signIn_vec[signIn_vec.size() - 1].get_time().c_str());
    signIn_label_vec.push_back(signIn_label);
    lv_obj_t * signIn_btn = lv_btn_create(list_btn);
    signIn_btn_vec.push_back(signIn_btn);
    lv_obj_set_size(signIn_btn, 120, 30);
    lv_obj_set_style_radius(signIn_btn, 10, 0);
    lv_obj_t * signIn_btn_label = lv_label_create(signIn_btn);
    signIn_btn_label_vec.push_back(signIn_btn_label);
    lv_obj_center(signIn_btn_label);
    lv_label_set_text(signIn_btn_label, "签到");
    lv_obj_add_event_cb(signIn_btn, staff_item_click, LV_EVENT_CLICKED, nullptr);
    // 设置按钮的用户数据（员工索引）
    lv_obj_set_user_data(signIn_btn, reinterpret_cast<void *>(static_cast<intptr_t>(signIn_vec.size() - 1)));
    mtx.unlock();
 
}
// 删除员工
void deleteStaff(char * buf)
{

    int data_id  = 0;
    long long id = 0;

    sscanf(buf, "data_id:%d id:%lld", &data_id, &id);
    // 输出解析后的数据（可以根据实际需要进行处理）
    cout << "data_id: " << data_id << endl;
    cout << "id: " << id << endl;
    char id_str[50];
    sprintf(id_str, "%lld", id);
    staff_vec.erase(
        remove_if(staff_vec.begin(), staff_vec.end(), [id_str](Staff & s) { return s.get_id() == string(id_str); }));

    mtx.lock();
    for (auto it = signIn_vec.begin(); it != signIn_vec.end(); ) {
    if (it->get_id() == string(id_str)) {
        // 删除lvgl对象
        lv_obj_del(signIn_btn_label_vec[distance(signIn_vec.begin(), it)]);
        lv_obj_del(signIn_btn_vec[distance(signIn_vec.begin(), it)]);
        lv_obj_del(signIn_label_vec[distance(signIn_vec.begin(), it)]);
        lv_obj_del(signIn_name_btn_vec[distance(signIn_vec.begin(), it)]);
        signIn_btn_label_vec.erase(signIn_btn_label_vec.begin() + distance(signIn_vec.begin(), it));
        signIn_btn_vec.erase(signIn_btn_vec.begin() + distance(signIn_vec.begin(), it));
        signIn_label_vec.erase(signIn_label_vec.begin() + distance(signIn_vec.begin(), it));
        signIn_name_btn_vec.erase(signIn_name_btn_vec.begin() + distance(signIn_vec.begin(), it));
        // 删除元素并更新迭代器
        it = signIn_vec.erase(it);  // erase 返回下一个有效元素的迭代器
    } else {
        // 如果没有删除元素，正常前进迭代器
        ++it;
    }
}
   
    mtx.unlock();

}

void TCPServer::distinguish_data(char * buf)
{
    string data(buf);
    size_t pos = data.find("data_id:");
    if(pos != string::npos) {
        pos += 8;                                            // Move past "data_id:"
        size_t end_pos = data.find(' ', pos);                // Find the next space after the data_id
        if(end_pos == string::npos) end_pos = data.length(); // Handle case where data_id is at the end
        string data_id_str = data.substr(pos, end_pos - pos);
        int data_id        = stoi(data_id_str); // Convert the string to an integer

        // 根据 data_id 判断处理类型
        if(data_id == 1) {
            cout << "签到数据" << endl;
            signInDataProcessing(buf);
            cout << buf << endl;
        } else if(data_id == 2) {
            cout << "发送员工数据" << endl;
            sendStaff();
        } else if(data_id == 4) {
            cout << "即将拉取OSS数据并添加新人脸数据" << endl;
            pullFacialData(buf);
        } else if(data_id == 6) {
            cout << "即将删除员工数据" << endl;
            deleteStaff(buf);
        }
    }
}

int TCPServer::TCP_server_connect()
{
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    int on        = 1;
    if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
        perror("设置复用失败\n");
        return -1;
    }

    struct sockaddr_in saddr;
    saddr.sin_family      = AF_INET;
    saddr.sin_port        = htons(port);
    saddr.sin_addr.s_addr = inet_addr(ip.c_str());

    int ret = bind(server_socket, (struct sockaddr *)&saddr, sizeof(saddr));
    if(ret < 0) {
        perror("绑定失败\n");
        return -1;
    } else {
        printf("绑定成功\n");
    }

    ret = listen(server_socket, 5);
    if(ret < 0) {
        perror("监听失败\n");
        return -1;
    } else {
        printf("监听成功\n");
    }

    while(1) {
        struct sockaddr_in clien_addr = {0};
        socklen_t addr_len            = sizeof(clien_addr);
        printf("等待客户端连接...\n");
        new_socket = accept(server_socket, (struct sockaddr *)&clien_addr, &addr_len);
        printf("新的客户端连接：%d\n", new_socket);
        printf("new_socket:%d , ip=%s , port=%d\n", new_socket, inet_ntoa(clien_addr.sin_addr),
               ntohs(clien_addr.sin_port));

        while(1) {
            fd_set set;
            FD_ZERO(&set);
            FD_SET(new_socket, &set);
            printf("开始监视...\n");
            int ret = select(new_socket + 1, &set, NULL, NULL, NULL); // 监视可读事件
            if(ret > 0) {
                printf("有活跃的描述符请处理\n");
                if(FD_ISSET(new_socket, &set)) {
                    char buffer[1024]      = {0};
                    ssize_t bytes_received = recv(new_socket, buffer, sizeof(buffer) - 1, 0); // 接收数据
                    if(bytes_received <= 0) {
                        printf("接收数据失败或连接关闭\n");
                        break;
                    }

                    buffer[bytes_received] = '\0'; // 确保字符串以NULL结尾
                    printf("接收到的数据: %s\n", buffer);
                    distinguish_data(buffer);
                }
            }
        }
        close(new_socket); // 关闭当前客户端连接
    }

    close(server_socket); // 关闭服务器套接字
    return 0;
}

void TCPServer::send_data(int socket, const string & data)
{
    // 将字符串数据转换为 C 风格字符串
    const char * data_to_send = data.c_str();
    // 发送数据
    ssize_t bytes_sent = write(socket, data_to_send, strlen(data_to_send));
    if(bytes_sent < 0) {
        perror("发送数据失败");
    } else {
        printf("成功发送数据: %s\n", data_to_send);
    }
}

int TCPServer::get_new_sorcket()
{
    return new_socket;
}
