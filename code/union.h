#pragma once
#include<iostream>
#include<string>
#include<cstdint>
using namespace std;

//⭐下面只是体现联合功能，实际上union不应该包含复杂的逻辑（成员函数），将union包裹在struct/class更好
union UN //联合
{
    //成员变量->成员变量之间共享内存
    std::int64_t long_long_int_value;
    int int_value;
    double double_value;
    char char_value;
    
    // 默认构造函数 - 只能初始化一个平凡成员
    UN() : int_value(0) {}
    
    // 析构函数 - 需要手动管理非平凡类型
    ~UN() {
        // 注意：这里不知道当前活跃成员，需要外部状态（类）跟踪
        //只讨论用默认或者被类包裹的情况，现在这种情况仅用来了解其有该功能
    }
    
    // 设置string值 - 需要手动构造
    void set_string_value(const std::string& str)
    {
        // 使用placement new手动构造string对象
        new (&string_value) std::string(str);
    }
    
    // 获取string值
    const std::string& get_string_value() const
    {
        return string_value;
    }
    
    // 清理string值 - 必须手动调用
    void destroy_string_value()
    {
        string_value.~string();
    }

private:
    std::string string_value; //非平凡成员变量，需要手动管理生命周期
};

void union_test()
{
    UN my_un;
    
    cout << "=== long int 测试 ===" << endl;
    
    // 测试1: long int 设置大值
    my_un.long_long_int_value = 0x1122334455667788;
        cout << "设置 long_long_int_value(64位): 0x" << hex << (uint64_t)my_un.long_long_int_value << endl;
    cout << "当前 int_value: 0x" << hex << my_un.int_value << endl;
    cout << "当前 char_value: '" << my_un.char_value << "'" << endl;
    
    cout << "\n=== int 覆盖 long int ===" << endl;
    // 测试2: int 覆盖 long int 的部分字节
    my_un.int_value = 0x11223344;
    cout << "设置 int_value: 0x" << hex << my_un.int_value << endl;
    cout << "当前 long_long_int_value: 0x" << hex << my_un.long_long_int_value << endl;
    cout << "注意：long_long_int_value 被部分覆盖！" << endl;
    
    cout << "\n=== char 覆盖 ===" << endl;
    // 测试3: char 覆盖第一个字节
    my_un.char_value = 'X';
    cout << "设置 char_value: '" << my_un.char_value << "'" << endl;
    cout << "当前 int_value: 0x" << hex << my_un.int_value << endl;
    cout << "当前 long_long_int_value: 0x" << hex << my_un.long_long_int_value << endl;
    
    cout << "\n=== double 测试 ===" << endl;
    // 测试4: double 完全覆盖
    my_un.double_value = 3.14159;
    cout << "设置 double_value: " << my_un.double_value << endl;
    cout << "当前 long_long_int_value: 0x" << hex << my_un.long_long_int_value << " (无意义的值)" << endl;
    
    cout << "\n=== string 测试 ===" << endl;
    // 测试5: string 非平凡类型
    my_un.set_string_value("hello");
    cout << "string_value: " << my_un.get_string_value() << endl;
    cout << "当前 long_long_int_value: 0x" << hex << my_un.long_long_int_value << " (无意义的值)" << endl;
    
    my_un.destroy_string_value();
    
    cout << "\n=== 内存大小信息 ===" << endl;
    cout << "sizeof(std::int64_t int): " << sizeof(std::int64_t) << " 字节" << endl;
    cout << "sizeof(int): " << sizeof(int) << " 字节" << endl;
    cout << "sizeof(double): " << sizeof(double) << " 字节" << endl;
    cout << "sizeof(UN): " << sizeof(UN) << " 字节" << endl;
    cout << "最大成员是: ";

    if (sizeof(std::string) >= sizeof(double) && sizeof(std::string) >= sizeof(std::int64_t)) {
        cout << "std::string (" << sizeof(std::string) << " 字节)" << endl;
    } else if (sizeof(double) >= sizeof(long int)) {
        cout << "double (" << sizeof(double) << " 字节)" << endl;
    } else {
        cout << "long int (" << sizeof(long int) << " 字节)" << endl;
    }
}


