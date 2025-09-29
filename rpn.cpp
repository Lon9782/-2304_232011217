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

    // ��ѧ���㺯��ӳ��
    std::map<std::string, std::function<void()>> operations;

    void initializeOperations() {
        // ������������
        operations["+"] = [this]() { binaryOp([](double a, double b) { return a + b; }); };
        operations["-"] = [this]() { binaryOp([](double a, double b) { return a - b; }); };
        operations["*"] = [this]() { binaryOp([](double a, double b) { return a * b; }); };
        operations["/"] = [this]() { binaryOp([](double a, double b) {
            if (b == 0) throw std::runtime_error("�������");   return a / b; }); };

        // �߼���ѧ����
        operations["sqrt"] = [this]() { unaryOp([](double a) {
            if (a < 0) throw std::runtime_error("�������ܿ�ƽ����");    return std::sqrt(a); }); };
        operations["pow"] = [this]() { binaryOp([](double a, double b) { return std::pow(a, b); }); };
        operations["sin"] = [this]() { unaryOp([](double a) { return std::sin(a); }); };
        operations["cos"] = [this]() { unaryOp([](double a) { return std::cos(a); }); };
        operations["tan"] = [this]() { unaryOp([](double a) { return std::tan(a); }); };

        // ���������
        operations["fib"] = [this]() { fibonacci(); };
        operations["pascal"] = [this]() { pascal(); };

        // ջ����
        operations["clear"] = [this]() { stack.clear(); };
        operations["drop"] = [this]() { checkStackSize(1);  stack.pop_back(); };
        operations["dup"]  = [this]() { checkStackSize(1);  stack.push_back(stack.back()); };
        operations["swap"] = [this]() {
            checkStackSize(2);  std::swap(stack[stack.size() - 1], stack[stack.size() - 2]); };
    }

    void checkStackSize(size_t required) {      //���ջ��Ԫ�ظ����Ƿ��㹻
        if (stack.size() < required)
        {    throw std::runtime_error("ջ��Ԫ�ز��㣬��Ҫ " + std::to_string(required) + " ��Ԫ��");    }
    }

    void binaryOp(std::function<double(double, double)> op) {   //ȡ��ջ��Ԫ�أ���������������ջ��
        checkStackSize(2);                                      //����������������ļ���
        double b = stack.back(); stack.pop_back();
        double a = stack.back(); stack.pop_back();
        stack.push_back(op(a, b));
    }

    void unaryOp(std::function<double(double)> op) {            //ȡ��ջ��Ԫ�أ���������������ջ��
        checkStackSize(1);                                      //�����һ���������ļ���
        double a = stack.back(); stack.pop_back();
        stack.push_back(op(a));
    }

    void fibonacci() {
        checkStackSize(1);
        int n = static_cast<int>(stack.back()); stack.pop_back();
        if (n < 0) throw std::runtime_error("쳲�����������������Ϊ����");

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
        if (n < 0) throw std::runtime_error("���������������Ϊ����");
        stack.push_back(std::pow(2, n));    // ����������ǵ�n�еĺͣ�2^n��
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
        {    throw std::runtime_error("ջΪ�գ��޷�����Ԫ��");    }
        double value = stack.back();
        stack.pop_back();
        return value;
    }

    void calculate(const std::string& input) {  //����ȡ�����ݷ���ջ�У�������ֱ�ӵ���
        std::istringstream iss(input);
        std::string token;

        while (iss >> token) {            
            if (std::istringstream(token) >> std::ws, std::isdigit(token[0]) ||
                (token[0] == '-' && token.length() > 1 && std::isdigit(token[1])) ||
                (token[0] == '.' && token.length() > 1 && std::isdigit(token[1])))
            {   // ����Ƿ�Ϊ����
                try {
                    double value = std::stod(token);
                    push(value);
                } catch (const std::exception& e)
                {    throw std::runtime_error("��Ч�����ָ�ʽ: " + token);    }
            } else if (operations.find(token) != operations.end()) {  // ����Ƿ�Ϊ�Ϸ�������
                try {
                    operations[token]();    //���ü��㺯��
                } catch (const std::exception& e) {
                    throw std::runtime_error("�������: " + std::string(e.what()));
                }
            } else {  throw std::runtime_error("δ֪�Ĳ�����: " + token);   }
        }    
        history.push_back(input);       // ��¼��ʷ
    }

    void displayStack() const {     // ��ʾջ�����ݣ�������ֱ�ӵ���
        std::cout << "��ǰջ: ";
        if (stack.empty()){
            std::cout << "��";
        } else {
            for (size_t i = 0; i < stack.size(); i++) {
                std::cout << std::fixed << std::setprecision(6) << stack[i];
                if (i < stack.size() - 1)   std::cout << " ";
            }
        } std::cout << std::endl;
    }

    void displayHistory() const {   //���������ʷ��������ֱ�ӵ���
        std::cout << "������ʷ:" << std::endl;
        for (size_t i = 0; i < history.size(); i++)
        {   std::cout << i + 1 << ": " << history[i] << std::endl;    }
    }

    double getResult() const {      //��ջ��ȡ�����ݣ�������ֱ�ӵ���
        if (stack.empty()) {        //���ڼ�������лὫ����Ż�ջ�����ʿɵ�ͬ�ڷ��ؽ��
            throw std::runtime_error("ջΪ��");
        } return stack.back();
    }

    void clearStack() {             //���ջ��������ֱ�ӵ���
        stack.clear();
    }

    size_t getStackSize() const {   //ͳ��ջ�����ݸ���
        return stack.size();
    }

    const std::vector<std::string>& getHistory() const {
        return history;
    }

    void showHelp() const {         //�����б�������ֱ�ӵ���
        std::cout << "��������:" << std::endl;
        std::cout << "  +, -, *, /  - ��������" << std::endl;
        std::cout << "  clear       - ���ջ" << std::endl;
        std::cout << "  dup         - ����ջ��Ԫ��" << std::endl;
        std::cout << "  swap        - ����ջ������Ԫ��" << std::endl;
        std::cout << "  drop        - ɾ��ջ��Ԫ��" << std::endl;
        std::cout << std::endl;
        std::cout << "�߼���ѧ����:" << std::endl;
        std::cout << "  sqrt        - ƽ����" << std::endl;
        std::cout << "  pow         - ������" << std::endl;
        std::cout << "  sin, cos, tan - ���Ǻ���" << std::endl;
        std::cout << std::endl;
        std::cout << "���������:" << std::endl;
        std::cout << "  fib n       - �����n��쳲�������" << std::endl;
        std::cout << "  pascal n    - ����������ǵ�n�еĺ�" << std::endl;
        std::cout << std::endl;
        std::cout << "��������:" << std::endl;
        std::cout << "  stack       - ��ʾ��ǰջ" << std::endl;
        std::cout << "  history     - ��ʾ������ʷ" << std::endl;
        std::cout << "  help        - ��ʾ�˰���" << std::endl;
        std::cout << "  q           - �˳�����" << std::endl;
    }
};

int main() {
    RPNCalculator calc;
    std::string input;

    std::cout << "C++ RPN ������" << std::endl;
    std::cout << "������ʽ (��: '5 5 +'), �� 'help' �鿴����, 'q' �˳�." << std::endl;

    while (true) {
        std::cout << "��ѡ��������������ݣ�";
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
                if (calc.getStackSize() > 0)    //fixed:���ö����ʽ���������    setprecision:���ø���������Чλ��
                    std::cout << "���: " << std::fixed << std::setprecision(6) << calc.getResult() << std::endl;
            } catch (const std::exception& e) {
                std::cout << "����: " << e.what() << std::endl;
            }
        }
    }std::cout << "���˳�����" << std::endl;
    return 0;
}