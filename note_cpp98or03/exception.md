- # C++异常处理语法笔记

  ## 一、异常处理基本概念

  ### 1.1 什么是异常处理

  异常处理是C++中用于处理程序运行时错误的机制。当程序遇到无法正常继续执行的情况时，可以通过抛出异常来中断当前执行流程，并将控制权转移到专门的异常处理代码块。

  ### 1.2 异常处理的核心组件

  异常处理涉及三个关键组成部分：

  - **try块**：包含可能抛出异常的代码
  - **throw语句**：主动抛出异常信号
  - **catch块**：捕获并处理特定类型的异常

  ## 二、基本语法结构

  ### 2.1 try-catch基本形式

  ```cpp
  try 
  {
      // 可能抛出异常的代码
  }
  catch (异常类型 参数名) 
  {
      // 异常处理代码
  }
  ```

  **说明**：这是异常处理的最基本结构。try块包含可能出错的代码，catch块负责捕获并处理特定类型的异常。如果try块中的代码抛出异常，程序会立即跳转到匹配的catch块执行。

  ### 2.2 多catch块结构

  ```cpp
  try 
  {
      // 可能抛出多种异常的代码
  }
  catch (特定异常类型 参数名) 
  {
      // 处理特定异常
  }
  catch (通用异常类型 参数名) 
  {
      // 处理通用异常
  }
  catch (...) 
  {
      // 处理所有其他未知异常
  }
  ```

  **说明**：一个try块可以对应多个catch块，按顺序匹配异常类型。catch(...)是通配符，能捕获所有未被前面catch块处理的异常，通常用作最后的保障。

  ## 三、throw语句语法

  ### 3.1 抛出基本类型异常

  ```cpp
  throw 42;                    // 抛出整型异常
  throw "错误描述";            // 抛出字符串字面量
  throw std::string("错误描述"); // 抛出string对象
  ```

  **说明**：throw可以抛出任意类型的对象。抛出基本类型简单但不推荐，因为缺乏详细的错误信息。字符串字面量会被当作const char*类型处理。

  ### 3.2 抛出标准库异常

  ```cpp
  #include <stdexcept>
  
  throw std::runtime_error("运行时错误描述");
  throw std::logic_error("逻辑错误描述");
  throw std::invalid_argument("无效参数错误");
  ```

  **说明**：标准库异常类提供了丰富的错误信息和标准的接口。runtime_error用于运行时检测到的错误，logic_error用于程序逻辑错误，invalid_argument用于参数验证失败。

  ### 3.3 抛出自定义异常对象

  ```cpp
  class MyException : public std::exception 
  {
  public:
      const char* what() const noexcept override 
      {
          return "自定义异常描述";
      }
  };
  
  throw MyException();
  ```

  **说明**：自定义异常类应继承std::exception，并重写what()方法返回错误描述。noexcept保证what()方法本身不会抛出异常。

  ## 四、catch块语法规则

  ### 4.1 异常捕获顺序

  catch块必须按照从具体到通用的顺序排列：

  ```cpp
  try 
  {
      // 代码
  }
  catch (派生类异常类型 参数名) 
  {
      // 先捕获具体的派生类异常
  }
  catch (基类异常类型 参数名) 
  {
      // 后捕获通用的基类异常
  }
  ```

  **说明**：异常捕获按顺序匹配，一旦找到匹配的catch块就停止搜索。如果基类catch块在前，派生类异常会被基类catch块捕获，导致派生类的专门处理逻辑无法执行。

  ### 4.2 引用捕获语法

  ```cpp
  catch (const std::exception& e) 
  {
      // 使用const引用避免对象切片和拷贝
      std::cout << e.what() << std::endl;
  }
  ```

  **说明**：使用const引用捕获异常可以避免对象切片（当捕获派生类异常时）和提高性能（避免不必要的对象拷贝）。const保证不会修改异常对象。

  ### 4.3 重新抛出语法

  ```cpp
  catch (异常类型 参数名) 
  {
      // 部分处理
      throw; // 重新抛出当前异常
  }
  ```

  **说明**：空的throw语句会重新抛出当前正在处理的异常，让上层调用者继续处理。这在当前catch块只能进行部分处理（如日志记录）时很有用。

  ## 五、异常规格说明语法

  ### 5.1 noexcept说明符

  ```cpp
  void function() noexcept 
  {
      // 保证不抛出任何异常
  }
  
  void function() noexcept(true) 
  {
      // 条件性不抛出异常
  }
  ```

  **说明**：noexcept是C++11引入的异常规格说明，表示函数保证不会抛出异常。noexcept(true)与noexcept等价，noexcept(false)表示可能抛出异常。这有助于编译器优化。

  ### 5.2 动态异常规格（已弃用）

  ```cpp
  // C++11之前使用，现在不推荐
  void function() throw(异常类型列表);
  void function() throw(); // 不抛出任何异常
  ```

  **说明**：这是C++11之前的异常规格语法，现在已被noexcept替代。动态异常规格在运行时检查，而noexcept在编译时检查，效率更高。

  ## 六、标准异常类体系

  ### 6.1 标准异常类继承关系

  ```
  std::exception
      ├── std::logic_error
      │   ├── std::invalid_argument
      │   ├── std::domain_error
      │   ├── std::length_error
      │   └── std::out_of_range
      ├── std::runtime_error
      │   ├── std::range_error
      │   ├── std::overflow_error
      │   ├── std::underflow_error
      │   └── std::system_error
      └── 其他异常类型
  ```

  **说明**：标准异常形成完整的继承体系。logic_error表示程序逻辑错误，应在编码时避免。runtime_error表示运行时无法避免的错误。其他专用异常处理特定场景。

  ### 6.2 标准异常类使用语法

  ```cpp
  #include <stdexcept>
  #include <new>
  #include <typeinfo>
  
  throw std::bad_alloc();           // 内存分配失败
  throw std::bad_cast();            // 动态转换失败
  throw std::bad_typeid();          // typeid操作失败
  ```

  **说明**：bad_alloc在new操作失败时抛出，bad_cast在dynamic_cast失败时抛出，bad_typeid在typeid操作数为空指针时抛出。这些是专用的异常类型。

  ## 七、函数异常声明语法

  ### 7.1 现代C++异常声明

  ```cpp
  // 可能抛出任何异常（默认）
  void function();
  
  // 保证不抛出异常
  void function() noexcept;
  
  // 可能抛出特定类型异常（C++11后不推荐）
  void function() noexcept(false);
  ```

  **说明**：现代C++使用noexcept来说明函数的异常行为。默认情况下函数可能抛出任何异常，noexcept明确声明不抛出异常，noexcept(false)明确声明可能抛出异常。

  ## 八、异常处理流程规则

  ### 8.1 栈展开过程

  当异常被抛出时：

  1. 当前函数执行立即停止
  2. 局部对象按照构造的逆序析构
  3. 在调用栈中查找匹配的catch块
  4. 如果找到，执行catch块代码
  5. 如果未找到，调用std::terminate()

  **说明**：栈展开是异常处理的核心机制，确保异常传播过程中资源被正确释放。如果异常始终未被捕获，程序会调用std::terminate()终止。

  ### 8.2 构造函数中的异常

  ```cpp
  class MyClass 
  {
  public:
      MyClass() 
      {
          // 如果构造函数抛出异常
          // 已构造的成员和基类会被正确析构
          // 但当前对象不会被析构
      }
  };
  ```

  **说明**：构造函数中抛出异常时，已经构造完成的成员和基类子对象会被析构，但当前对象本身不会被析构（因为构造未完成）。这是RAII模式的重要基础。

  ## 九、内存安全与异常

  ### 9.1 RAII模式语法

  ```cpp
  class ResourceHolder 
  {
  private:
      int* resource;
      
  public:
      ResourceHolder() : resource(new int[100]) 
      {
      }
      
      ~ResourceHolder() 
      {
          delete[] resource; // 异常安全：保证资源释放
      }
  };
  ```

  **说明**：RAII（Resource Acquisition Is Initialization）模式通过在构造函数中获取资源，在析构函数中释放资源，利用栈展开机制确保资源总是被释放，即使发生异常。

  ### 9.2 智能指针异常安全

  ```cpp
  #include <memory>
  
  void function() 
  {
      auto ptr = std::make_unique<int[]>(100);
      // 即使抛出异常，内存也会自动释放
      throw std::runtime_error("错误");
  }
  ```

  **说明**：智能指针是RAII模式的典型应用。unique_ptr、shared_ptr等智能指针在析构时自动释放管理的资源，提供强异常安全保证。

  ## 十、最佳实践语法规范

  ### 10.1 异常抛出规范

  - 优先使用标准异常类型
  - 自定义异常继承std::exception
  - 提供有意义的错误信息

  **说明**：标准异常类型有统一的接口和良好的可读性。自定义异常应保持与标准异常一致的接口。错误信息应具体明确，便于调试。

  ### 10.2 异常捕获规范

  - 按从具体到通用顺序排列catch块
  - 使用const引用捕获异常对象
  - 避免过度使用catch(...)

  **说明**：正确的捕获顺序确保异常被最合适的处理器处理。引用捕获提高效率并避免对象切片。catch(...)应谨慎使用，通常只用于日志记录和资源清理。

  ### 10.3 异常安全保证

  - 基本保证：无资源泄漏，对象状态有效
  - 强保证：操作原子性，成功或状态不变
  - 不抛保证：承诺不抛出异常

  **说明**：异常安全保证是代码质量的重要指标。基本保证是最低要求，强保证提供事务语义，不抛保证用于关键函数。RAII模式是实现异常安全的基础。
