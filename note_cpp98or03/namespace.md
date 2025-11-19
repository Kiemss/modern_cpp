# 命名空间 (Namespace) 完整笔记

## 核心概念

### 设计目的：解决命名冲突
```cpp
// 不同库可能有相同名称的函数
namespace PhysicsLib 
{
    void Calculate() 
    {
        // 物理计算实现
    }
}

namespace GraphicsLib 
{
    void Calculate() 
    {
        // 图形计算实现
    }
}
```

## 三种使用方式

### 1. using namespace - 引入整个命名空间
```cpp
using namespace std;  // 引入std所有名称

// 可以直接使用
cout << "Hello";      // 代替 std::cout
vector<int> v;        // 代替 std::vector
```

### 2. using namespace::name - 引入特定名称
```cpp
using std::cout;      // 只引入cout
using std::endl;      // 只引入endl
using std::vector;    // 只引入vector

// 只能使用引入的名称
cout << "Hello" << endl;
vector<int> nums;
// cin >> x;  // 错误！没有引入cin
```

### 3. namespace::name - 显式限定（推荐）
```cpp
// 每次使用时显式指定
std::cout << "Hello" << std::endl;
std::vector<int> numbers;

// 最安全，避免命名冲突
```

## 作用域和优先级

### 局部 vs 全局函数
```cpp
#include <iostream>
using namespace std;

void Print() 
{
    cout << "Global Print" << endl;
}

namespace MyApp 
{
    void Print() 
    {
        cout << "MyApp Print" << endl;
    }
    
    void Test() 
    {
        Print();           // 调用 MyApp::Print (局部优先)
        ::Print();         // 调用全局 Print
        std::cout << "Explicit std" << endl;
    }
}
```

## 命名空间嵌套

### 多层命名空间
```cpp
namespace Game 
{
    namespace Physics 
    {
        class Vector3 
        {
            float x, y, z;
        };
        
        void ApplyForce() 
        {
            // 物理力计算
        }
    }
    
    namespace Graphics 
    {
        class Sprite 
        {
            // 图形精灵
        };
    }
}

// 使用嵌套命名空间
Game::Physics::Vector3 velocity;
Game::Graphics::Sprite player;
```

### C++17 简化嵌套语法
```cpp
// 传统方式
namespace Game 
{
    namespace Physics 
    {
        // ...
    }
}

// C++17 简化方式
namespace Game::Physics 
{
    class Vector3 
    {
        // ...
    };
}
```

## 关于 std 命名空间的说明

### std 中的内容分类
```cpp
#include <iostream>
#include <vector>
#include <string>

// std 中包含：
// - 对象：cout, cin, cerr, clog
std::cout << "Hello";     // cout 是全局对象

// - 类型/类：string, vector, map  
std::string name;         // string 是类
std::vector<int> numbers; // vector 是类模板

// - 函数：getline, sort, find
std::sort(vec.begin(), vec.end());

// - 常量：endl
std::cout << std::endl;   // endl 是函数模板
```

### using 的正确理解
```cpp
using std::cout;    // 引入对象 - 正确
using std::string;  // 引入类型 - 正确  
using std::vector;  // 引入模板 - 正确
using std::endl;    // 引入函数模板 - 正确

// 本质：using 引入的是"名称"，不区分对象/类型/函数
```

## 最佳实践指南

### 头文件中的使用规范
```cpp
// mylib.h
namespace MyLibrary 
{
    class Calculator 
    {
    public:
        int Add(int a, int b);
        int Multiply(int a, int b);
    };
    
    // 在头文件中避免 using namespace
    // 防止污染包含该头文件的其他代码
}

// 实现文件可以根据情况使用
```

### 源文件中的推荐做法
```cpp
// main.cpp
#include <iostream>
#include <vector>

// 推荐：只引入必要的名称
using std::cout;
using std::endl;
using std::vector;

// 或者：在函数内部局部使用
void ProcessData() 
{
    using namespace std;
    vector<int> data;
    cout << "Processing" << endl;
}

// 或者：完全显式使用（最安全）
int main() 
{
    std::vector<std::string> names;
    std::cout << "Hello World" << std::endl;
    return 0;
}
```

## 实际应用场景

### 游戏开发中的命名空间组织
```cpp
namespace DragonGame 
{
    namespace Core 
    {
        class GameEngine 
        {
            // 游戏引擎核心
        };
    }
    
    namespace Entities 
    {
        class Player 
        {
            // 玩家实体
        };
        
        class Enemy 
        {
            // 敌人实体
        };
    }
    
    namespace Utils 
    {
        namespace Math 
        {
            float Lerp(float a, float b, float t);
        }
        
        namespace FileIO 
        {
            bool LoadTexture(const std::string& path);
        }
    }
}

// 使用
DragonGame::Entities::Player hero;
DragonGame::Utils::Math::Lerp(0, 10, 0.5f);
```

## 常见陷阱和解决方案

### 命名冲突处理
```cpp
namespace A 
{
    void Process() 
    {
        std::cout << "A::Process" << std::endl;
    }
}

namespace B 
{
    void Process() 
    {
        std::cout << "B::Process" << std::endl;
    }
}

void Test() 
{
    // using namespace A;  // 危险：可能与B冲突
    // using namespace B;
    
    // 安全做法：显式指定或局部使用
    A::Process();
    B::Process();
    
    // 或者在局部作用域使用
    {
        using namespace A;
        Process();  // 明确知道是 A::Process
    }
}
```
