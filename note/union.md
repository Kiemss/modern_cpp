# 联合 (Union)

## 核心概念

### 本质：内存共享

```cpp
union Data 
{
    int i;
    double d;
    char c;
};
// 所有成员共享同一块内存
// sizeof(Data) = max(sizeof(i), sizeof(d), sizeof(c))
```

## 基本语法

### 定义与声明

```cpp
// 基本定义
union Identifier 
{
    type1 member1;
    type2 member2;
    // ...
};

// 使用
union Data data;
data.i = 42;
```

### 匿名union

```cpp
struct Variant 
{
    enum Type { INT, DOUBLE } type;
    
    union  // 匿名union
    {
        int int_val;
        double double_val;
    };
};

// 直接访问成员
Variant v;
v.int_val = 100;  // 不需要通过union名
```

## 重要限制

### 默认public访问

```cpp
union UN 
{
    int x;           // 默认public
    double y;        // 默认public
    
private:
    std::string z;   // 可以私有，但不推荐
};
```

### 构造函数限制

```cpp
union UN 
{
    int i;
    double d;
    
    UN() : i(0) {}  // 只能初始化一个成员
    // UN() : i(0), d(0.0) {}  // 错误
};
```

## 非平凡类型处理

### 手动生命周期管理

```cpp
union WithString 
{
    int number;
    std::string text;
    
    WithString() : number(0) {}
    
    void set_text(const std::string& str) 
    {
        // 使用placement new手动构造string对象
        new (&text) std::string(str);
    }
    
    void destroy_text() 
    {
        text.~string();  // 手动析构
    }
};
```

### 正确使用流程

```cpp
void demo() 
{
    WithString ws;
    
    ws.number = 42;        // 使用平凡成员
    
    ws.set_text("Hello");  // 切换到非平凡成员
    cout << ws.text << endl;
    
    ws.destroy_text();     // 必须手动清理
    ws.number = 100;       // 切换回平凡成员
}
```



## 工业化最佳实践

### 推荐：std::variant (C++17)

```cpp
#include <variant>
#include <string>

// 自动管理生命周期
std::variant<int, double, std::string> data;

data = 42;                    // 存储int
data = 3.14;                  // 自动清理并存储double
data = std::string("Hello");  // 自动清理并存储string

// 类型安全访问
if (auto* str = std::get_if<std::string>(&data)) 
{
    cout << *str << endl;
}
```

### 类包装模式

```cpp
class SafeVariant 
{
private:
    enum Type { INT, DOUBLE, STRING } current_type;
    
    union 
    {
        int int_val;
        double double_val;
        std::string str_val;
    };

public:
    // 构造函数
    SafeVariant(int val) : current_type(INT), int_val(val) {}
    SafeVariant(double val) : current_type(DOUBLE), double_val(val) {}
    SafeVariant(const std::string& val) : current_type(STRING) 
    {
        new (&str_val) std::string(val);
    }
    
    // 析构函数 - 知道当前类型
    ~SafeVariant() 
    {
        if (current_type == STRING) 
        {
            str_val.~string();
        }
    }
    
    // 禁用拷贝（简化示例）
    SafeVariant(const SafeVariant&) = delete;
    SafeVariant& operator=(const SafeVariant&) = delete;
};
```

## 实际应用场景

### 类型转换

```cpp
union FloatToBits 
{
    float f;
    uint32_t bits;
};

FloatToBits converter;
converter.f = 3.14f;
cout << "Float bits: " << hex << converter.bits << endl;
```

### 协议解析

```cpp
union IPAddress 
{
    uint32_t address;
    uint8_t octets[4];
};

IPAddress ip;
ip.address = 0xC0A80101;  // 192.168.1.1
cout << (int)ip.octets[0] << "." << (int)ip.octets[1] << "."
     << (int)ip.octets[2] << "." << (int)ip.octets[3] << endl;
```

### 内存优化

```cpp
// 游戏开发中的空间优化
union GameObjectData 
{
    struct 
    {
        float health;
        float armor;
    } player_data;
    
    struct 
    {
        float damage;
        float speed;
    } enemy_data;
    
    struct 
    {
        float value;
        bool collectible;
    } item_data;
};
```

## 注意事项

### 内存覆盖风险

```cpp
union RiskExample 
{
    std::string text;
    int number;
    
    void danger() 
    {
        new (&text) std::string("Hello");
        number = 42;  // 内存泄漏！text没被析构
    }
};
```

### 活跃成员跟踪

1. Union无法知道当前哪个成员是活跃的
2. 需要外部状态跟踪或使用std::variant

## 总结对比

| 特性         | 原始union | std::variant | 类包装 |
|--------------|-----------|--------------|---------|
| 内存管理     | 手动      | 自动         | 自动    |
| 类型安全     | 无        | 有           | 有      |
| 活跃成员跟踪 | 无        | 有           | 有      |
| 代码复杂度   | 低        | 中           | 高      |
| C++版本      | 所有      | C++17        | 所有    |
