根据你的个人经历提示词，我已参考其中关于技术栈和学习特点的内容。

# 智能指针笔记完善

## unique_ptr

### 核心特性
* 独占所有权，一个unique_ptr管理一块内存，当unique_ptr被销毁时自动释放内存
* 不可拷贝，只可移动（符合独占语义）

### 创建语法
```cpp
// 方式1：直接构造
std::unique_ptr<MyClass> ptr1(new MyClass());

// 方式2：make_unique（C++14推荐）
std::unique_ptr<MyClass> ptr2 = std::make_unique<MyClass>();

// 方式3：数组版本
std::unique_ptr<int[]> arr = std::make_unique<int[]>(10);
```

### 常用函数
```cpp
std::unique_ptr<MyClass> ptr = std::make_unique<MyClass>();

MyClass* raw_ptr = ptr.get();        // 返回原始指针，不释放所有权
MyClass& ref = *ptr;                 // 解引用访问对象
ptr->member_function();              // 箭头运算符访问成员

ptr.reset(new MyClass());            // 重设指向，释放原对象
ptr.reset();                         // 释放对象，ptr变为nullptr

MyClass* released_ptr = ptr.release(); // 释放所有权，返回原始指针
// 此时ptr为nullptr，需要手动管理released_ptr

ptr.swap(other_ptr);                 // 与另一个unique_ptr交换
```

### 应用场景与限制
**适用场景**：
- 独占资源管理（文件句柄、网络连接）
- 避免内存泄漏的常规指针替换
- 工厂函数返回值

**不适用场景**：
```cpp
// 链表节点 - 会导致递归析构栈溢出
struct ListNode {
    int value;
    std::unique_ptr<ListNode> next;  // 问题：递归析构
};

// 循环引用场景
struct NodeA {
    std::unique_ptr<NodeB> b;  // 无法形成循环
};
struct NodeB {
    // 无法指向NodeA，因为unique_ptr不可拷贝
};
```

## shared_ptr

### 核心特性
* 多个shared_ptr可以共享同一块内存，使用引用计数
* 当最后一个shared_ptr被销毁时自动释放内存

### 创建语法
```cpp
// 方式1：直接构造
std::shared_ptr<MyClass> ptr1(new MyClass());

// 方式2：make_shared（推荐，更高效）
std::shared_ptr<MyClass> ptr2 = std::make_shared<MyClass>();

// 方式3：从unique_ptr移动
std::unique_ptr<MyClass> unique = std::make_unique<MyClass>();
std::shared_ptr<MyClass> shared = std::move(unique);
```

### 常用函数
```cpp
std::shared_ptr<MyClass> ptr = std::make_shared<MyClass>();

MyClass* raw_ptr = ptr.get();        // 返回原始指针
ptr->member_function();              // 访问成员
MyClass& ref = *ptr;                 // 解引用

size_t count = ptr.use_count();      // 返回引用计数
bool is_unique = ptr.unique();       // 是否唯一所有者（use_count() == 1）

ptr.reset(new MyClass());            // 重设指向
ptr.reset();                         // 释放当前对象

ptr.swap(other_ptr);                 // 交换指针
```

### 类型转换函数
```cpp
std::shared_ptr<Base> base_ptr = std::make_shared<Derived>();

// 静态转换（编译时检查）
std::shared_ptr<Derived> derived1 = std::static_pointer_cast<Derived>(base_ptr);

// 动态转换（运行时检查，失败返回空）
std::shared_ptr<Derived> derived2 = std::dynamic_pointer_cast<Derived>(base_ptr);

// 常量转换
std::shared_ptr<const MyClass> const_ptr = std::const_pointer_cast<const MyClass>(ptr);
```

### 循环引用问题
```cpp
struct NodeA;
struct NodeB;

struct NodeA {
    std::shared_ptr<NodeB> b;
    ~NodeA() { std::cout << "NodeA destroyed\n"; }
};

struct NodeB {
    std::shared_ptr<NodeA> a;  // 循环引用！
    ~NodeB() { std::cout << "NodeB destroyed\n"; }
};

// 使用导致内存泄漏
auto a = std::make_shared<NodeA>();
auto b = std::make_shared<NodeB>();
a->b = b;
b->a = a;  // 循环引用，析构函数不会被调用
```

## weak_ptr

### 核心特性
* 观测shared_ptr而不增加引用计数
* 解决shared_ptr循环引用问题

### 创建语法
```cpp
std::shared_ptr<MyClass> shared = std::make_shared<MyClass>();
std::weak_ptr<MyClass> weak = shared;  // 从shared_ptr创建
```

### 常用函数
```cpp
std::weak_ptr<MyClass> weak;

bool expired = weak.expired();       // 检查对象是否已被释放

std::shared_ptr<MyClass> shared = weak.lock();  // 尝试获取shared_ptr
if (shared) {
    // 对象仍然存在，安全使用
    shared->member_function();
}

size_t count = weak.use_count();     // 返回观测的引用计数
```

### 解决循环引用
```cpp
struct NodeA;
struct NodeB;

struct NodeA {
    std::shared_ptr<NodeB> b;
    ~NodeA() { std::cout << "NodeA destroyed\n"; }
};

struct NodeB {
    std::weak_ptr<NodeA> a;  // 使用weak_ptr打破循环
    ~NodeB() { std::cout << "NodeB destroyed\n"; }
};

// 正常析构
auto a = std::make_shared<NodeA>();
auto b = std::make_shared<NodeB>();
a->b = b;
b->a = a;  // weak_ptr不增加引用计数
```

## 控制块原理

### 内存布局
```
shared_ptr → 控制块 → 对象数据
              ↑
weak_ptr  ────┘

控制块包含：
- use_count: 共享所有权计数
- weak_count: 弱引用计数  
- 删除器
- 分配器
```

### 创建规则
- `std::make_shared`: 对象和控制块单次分配（更高效）
- `std::shared_ptr(new T)`: 对象和控制块分开分配
- 当 `use_count == 0` 时销毁对象数据
- 当 `use_count == 0 && weak_count == 0` 时销毁控制块

## 性能与使用建议

### 推荐使用场景
- `unique_ptr`: 默认选择，性能最优
- `shared_ptr`: 需要共享所有权时
- `weak_ptr`: 观测shared_ptr或解决循环引用
- `make_shared/make_unique`: 优先使用，更安全高效

### 注意事项
- 避免从原始指针创建多个独立的shared_ptr
- 优先使用make_shared，避免显式new
- 在性能敏感场景注意shared_ptr的原子操作开销
- 使用weak_ptr避免循环引用导致的内存泄漏