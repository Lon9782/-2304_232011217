#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <stack>
#include <map>
#include <functional>
#include <stdexcept>
#include <iomanip>

class RPNCalculator {
private:
    std::vector<double> stack;
    std::vector<std::string> history;

    // 数学运算函数映射
    std::map<std::string, std::function<void()>> operations;

    void initializeOperations() {
        // 基础四则运算
        operations["+"] = [this]() { binaryOp([](double a, double b) { return a + b; }); };
        operations["-"] = [this]() { binaryOp([](double a, double b) { return a - b; }); };
        operations["*"] = [this]() { binaryOp([](double a, double b) { return a * b; }); };
        operations["/"] = [this]() { binaryOp([](double a, double b) {
            if (b == 0) throw std::runtime_error("除零错误");   return a / b; }); };

        // 高级数学运算
        operations["sqrt"] = [this]() { unaryOp([](double a) {
            if (a < 0) throw std::runtime_error("负数不能开平方根");    return std::sqrt(a); }); };
        operations["pow"] = [this]() { binaryOp([](double a, double b) { return std::pow(a, b); }); };
        operations["sin"] = [this]() { unaryOp([](double a) { return std::sin(a); }); };
        operations["cos"] = [this]() { unaryOp([](double a) { return std::cos(a); }); };
        operations["tan"] = [this]() { unaryOp([](double a) { return std::tan(a); }); };

        // 特殊操作符
        operations["fib"] = [this]() { fibonacci(); };
        operations["pascal"] = [this]() { pascal(); };

        // 栈操作
        operations["clear"] = [this]() { stack.clear(); };
        operations["drop"] = [this]() { checkStackSize(1);  stack.pop_back(); };
        operations["dup"]  = [this]() { checkStackSize(1);  stack.push_back(stack.back()); };
        operations["swap"] = [this]() {
            checkStackSize(2);  std::swap(stack[stack.size() - 1], stack[stack.size() - 2]); };
    }

    void checkStackSize(size_t required) {      //检查栈中元素个数是否足够
        if (stack.size() < required)
        {    throw std::runtime_error("栈中元素不足，需要 " + std::to_string(required) + " 个元素");    }
    }

    void binaryOp(std::function<double(double, double)> op) {   //取出栈中元素，并将计算结果返回栈中
        checkStackSize(2);                                      //针对需两个操作数的计算
        double b = stack.back(); stack.pop_back();
        double a = stack.back(); stack.pop_back();
        stack.push_back(op(a, b));
    }

    void unaryOp(std::function<double(double)> op) {            //取出栈中元素，并将计算结果返回栈中
        checkStackSize(1);                                      //针对需一个操作数的计算
        double a = stack.back(); stack.pop_back();
        stack.push_back(op(a));
    }

    void fibonacci() {
        checkStackSize(1);
        int n = static_cast<int>(stack.back()); stack.pop_back();
        if (n < 0) throw std::runtime_error("斐波那契数列索引不能为负数");

        if (n == 0) {
            stack.push_back(0);
        } else if (n == 1) {
            stack.push_back(1);
        } else {
            double a = 0, b = 1;
            for (int i = 2; i <= n; i++) {
                double temp = a + b;
                a = b;
                b = temp;
            } stack.push_back(b);
        }
    }

    void pascal() {
        checkStackSize(1);
        int n = static_cast<int>(stack.back()); stack.pop_back();
        if (n < 0) throw std::runtime_error("杨辉三角行数不能为负数");
        stack.push_back(std::pow(2, n));    // 返回杨辉三角第n行的和（2^n）
    }

public:
    RPNCalculator() {
        initializeOperations();
    }

    void push(double value) {
        stack.push_back(value);
    }

    double pop() {
        if (stack.empty())
        {    throw std::runtime_error("栈为空，无法弹出元素");    }
        double value = stack.back();
        stack.pop_back();
        return value;
    }

    void calculate(const std::string& input) {  //将获取的数据放入栈中，主函数直接调用
        std::istringstream iss(input);
        std::string token;

        while (iss >> token) {            
            if (std::istringstream(token) >> std::ws, std::isdigit(token[0]) ||
                (token[0] == '-' && token.length() > 1 && std::isdigit(token[1])) ||
                (token[0] == '.' && token.length() > 1 && std::isdigit(token[1])))
            {   // 检查是否为数字
                try {
                    double value = std::stod(token);
                    push(value);
                } catch (const std::exception& e)
                {    throw std::runtime_error("无效的数字格式: " + token);    }
            } else if (operations.find(token) != operations.end()) {  // 检查是否为合法操作符
                try {
                    operations[token]();    //调用计算函数
                } catch (const std::exception& e) {
                    throw std::runtime_error("计算错误: " + std::string(e.what()));
                }
            } else {  throw std::runtime_error("未知的操作符: " + token);   }
        }    
        history.push_back(input);       // 记录历史
    }

    void displayStack() const {     // 显示栈内数据，主函数直接调用
        std::cout << "当前栈: ";
        if (stack.empty()){
            std::cout << "空";
        } else {
            for (size_t i = 0; i < stack.size(); i++) {
                std::cout << std::fixed << std::setprecision(6) << stack[i];
                if (i < stack.size() - 1)   std::cout << " ";
            }
        } std::cout << std::endl;
    }

    void displayHistory() const {   //输出计算历史，主函数直接调用
        std::cout << "计算历史:" << std::endl;
        for (size_t i = 0; i < history.size(); i++)
        {   std::cout << i + 1 << ": " << history[i] << std::endl;    }
    }

    double getResult() const {      //从栈顶取出数据，主函数直接调用
        if (stack.empty()) {        //由于计算过程中会将结果放回栈顶，故可等同于返回结果
            throw std::runtime_error("栈为空");
        } return stack.back();
    }

    void clearStack() {             //清空栈，主函数直接调用
        stack.clear();
    }

    size_t getStackSize() const {   //统计栈内数据个数
        return stack.size();
    }

    const std::vector<std::string>& getHistory() const {
        return history;
    }

    void showHelp() const {         //功能列表，主函数直接调用
        std::cout << "基础操作:" << std::endl;
        std::cout << "  +, -, *, /  - 四则运算" << std::endl;
        std::cout << "  clear       - 清空栈" << std::endl;
        std::cout << "  dup         - 复制栈顶元素" << std::endl;
        std::cout << "  swap        - 交换栈顶两个元素" << std::endl;
        std::cout << "  drop        - 删除栈顶元素" << std::endl;
        std::cout << std::endl;
        std::cout << "高级数学运算:" << std::endl;
        std::cout << "  sqrt        - 平方根" << std::endl;
        std::cout << "  pow         - 幂运算" << std::endl;
        std::cout << "  sin, cos, tan - 三角函数" << std::endl;
        std::cout << std::endl;
        std::cout << "特殊操作符:" << std::endl;
        std::cout << "  fib n       - 计算第n个斐波那契数" << std::endl;
        std::cout << "  pascal n    - 计算杨辉三角第n行的和" << std::endl;
        std::cout << std::endl;
        std::cout << "其他命令:" << std::endl;
        std::cout << "  stack       - 显示当前栈" << std::endl;
        std::cout << "  history     - 显示计算历史" << std::endl;
        std::cout << "  help        - 显示此帮助" << std::endl;
        std::cout << "  q           - 退出程序" << std::endl;
    }
};

int main() {
    RPNCalculator calc;
    std::string input;

    std::cout << "C++ RPN 计算器" << std::endl;
    std::cout << "输入表达式 (例: '5 5 +'), 或 'help' 查看帮助, 'q' 退出." << std::endl;

    while (true) {
        std::cout << "请选择操作或输入数据：";
        std::getline(std::cin, input);

        if (input == "q" || input == "quit") {
            break;
        } else if (input == "help") {
            calc.showHelp();
        } else if (input == "stack") {
            calc.displayStack();
        } else if (input == "history") {
            calc.displayHistory();
        } else if (input.empty()) {
            continue;
        } else {
            try {
                calc.calculate(input);
                if (calc.getStackSize() > 0)    //fixed:设置定点格式输出浮点数    setprecision:设置浮点数的有效位数
                    std::cout << "结果: " << std::fixed << std::setprecision(6) << calc.getResult() << std::endl;
            } catch (const std::exception& e) {
                std::cout << "错误: " << e.what() << std::endl;
            }
        }
    }std::cout << "已退出程序" << std::endl;
    return 0;
}