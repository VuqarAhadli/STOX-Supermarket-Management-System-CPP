#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <stdexcept>
#include <limits>

using namespace std;

// colour macros for cli styling (may not work in online compilers but in gdb it works)
#define RESET   "\033[0m" /* for returning to default colour*/
#define BOLD    "\033[1m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define BG_BLUE "\033[44m"
#define BG_GREEN "\033[42m"
#define BG_RED  "\033[41m"
#define BRIGHT_GREEN "\033[92m"
#define BRIGHT_YELLOW "\033[93m"
#define BRIGHT_BLUE "\033[94m"
#define BRIGHT_MAGENTA "\033[95m"
#define BRIGHT_CYAN "\033[96m"

class InvalidPriceException : public runtime_error {
public:
    InvalidPriceException() : runtime_error("Price must be positive!") {}
};

class InsufficientStockException : public runtime_error {
public:
    InsufficientStockException() : runtime_error("Insufficient stock available!") {}
};

class InvalidQuantityException : public runtime_error {
public:
    InvalidQuantityException() : runtime_error("Quantity must be positive!") {}
};

class ProductNotFoundException : public runtime_error {
public:
    ProductNotFoundException() : runtime_error("Product not found!") {}
};

class FileOperationException : public runtime_error {
public:
    FileOperationException(const string& msg) : runtime_error("File operation failed: " + msg) {}
};

class Person {
protected:
    string name;
    int id;
    string phone;
public:
    Person(string n = "", int i = 0, string p = "") : name(n), id(i), phone(p) {}
    virtual ~Person() {}
    virtual void display() const {
        cout << CYAN << "ID: " << RESET << id << CYAN << ", Name: " << RESET << name 
             << CYAN << ", Phone: " << RESET << phone << endl;
    }
    int getId() const { return id; }
    string getName() const { return name; }
};

class Employee : public Person {
protected:
    double salary;
    string position;
public:
    Employee(string n = "", int i = 0, string p = "", double s = 0, string pos = "")
        : Person(n, i, p), salary(s), position(pos) {}
    virtual double calculateSalary() const = 0;
    virtual void display() const {
        Person::display();
        cout << CYAN << "Position: " << RESET << position 
             << CYAN << ", Salary: " << RESET << GREEN << "$" << fixed << setprecision(2) << salary << RESET << endl;
    }
    virtual void saveToFile(ofstream& out) const {
        out << id << "," << name << "," << phone << "," << position << "," << salary << "\n";
    }
    double getSalary() const { return salary; }
    string getPosition() const { return position; }
};

class Manager : public Employee {
private:
    double bonus;
public:
    Manager(string n = "", int i = 0, string p = "", double s = 0, double b = 0)
        : Employee(n, i, p, s, "Manager"), bonus(b) {}
    double calculateSalary() const { return salary + bonus; }
    void display() const {
        Employee::display();
        cout << CYAN << "Bonus: " << RESET << YELLOW << "$" << bonus << RESET
             << CYAN << ", Total: " << RESET << BRIGHT_GREEN << "$" << calculateSalary() << RESET << endl;
    }
};

class Cashier : public Employee {
private:
    int transactionCount;
public:
    Cashier(string n = "", int i = 0, string p = "", double s = 0)
        : Employee(n, i, p, s, "Cashier"), transactionCount(0) {}
    double calculateSalary() const { return salary + (transactionCount * 0.5); }
    void incrementTransactions() { transactionCount++; }
    void display() const {
        Employee::display();
        cout << CYAN << "Transactions: " << RESET << transactionCount 
             << CYAN << ", Total Salary: " << RESET << BRIGHT_GREEN << "$" << calculateSalary() << RESET << endl;
    }
};

class Product {
private:
    int productId;
    string name;
    double price;
    int stock;
    string category;
    string barcode;
    map<char, string> getCode39Map() {
        map<char, string> code39;
        code39['0']="nnnwwnwnn"; code39['1']="wnnwnnnnw"; code39['2']="nnwwnnnnw";
        code39['3']="wnwwnnnnn"; code39['4']="nnnwwnnnw"; code39['5']="wnnwwnnnn";
        code39['6']="nnwwwnnnn"; code39['7']="nnnwnnwnw"; code39['8']="wnnwnnwnn";
        code39['9']="nnwwnnwnn"; code39['A']="wnnnnwnnw"; code39['B']="nnwnnwnnw";
        code39['C']="wnwnnwnnn"; code39['D']="nnnnwwnnw"; code39['E']="wnnnwwnnn";
        code39['F']="nnwnwwnnn"; code39['G']="nnnnnwwnw"; code39['H']="wnnnnwwnn";
        code39['I']="nnwnnwwnn"; code39['J']="nnnnwwwnn"; code39['K']="wnnnnnnww";
        code39['L']="nnwnnnnww"; code39['M']="wnwnnnnwn"; code39['N']="nnnnwnnww";
        code39['O']="wnnnwnnwn"; code39['P']="nnwnwnnwn"; code39['Q']="nnnnnnwww";
        code39['R']="wnnnnnwwn"; code39['S']="nnwnnnwwn"; code39['T']="nnnnwnwwn";
        code39['U']="wwnnnnnnw"; code39['V']="nwwnnnnnw"; code39['W']="wwwnnnnnn";
        code39['X']="nwnnwnnnw"; code39['Y']="wwnnwnnnn"; code39['Z']="nwwnwnnnn";
        code39['-']="nwnnnnwnw"; code39['.']="wwnnnnwnn"; code39[' ']="nwwnnnwnn";
        code39['*']="nwnnwnwnn";
        return code39;
    }
public:
    Product(int id=0,string n="",double p=0,int s=0,string cat="")
        : productId(id), name(n), price(p), stock(s), category(cat) {
        if (price < 0) throw InvalidPriceException();
        if (s < 0) throw InvalidQuantityException();
        barcode = "PRD" + to_string(id);
    }
    void updateStock(int quantity) {
        if (quantity < 0) throw InvalidQuantityException();
        stock += quantity;
    }
    void reduceStock(int quantity) {
        if (quantity < 0) throw InvalidQuantityException();
        if (stock < quantity) throw InsufficientStockException();
        stock -= quantity;
    }
    void generateBarcode(const string& filename) {
        const int NARROW = 1, WIDE = 3; /* barcode line width macros*/
        map<char,string> code39 = getCode39Map();
        string full="*"+barcode+"*";
        int height=50;
        int totalWidth=0;
        for(size_t i=0;i<full.length();i++){
            string pattern=code39[full[i]];
            for(int j=0;j<9;j++) totalWidth += (pattern[j]=='n'?NARROW:WIDE);
            totalWidth+=NARROW;
        }
        ofstream out(filename);
        if(!out) throw FileOperationException("Cannot create barcode file");
        out<<"P1\n"<<totalWidth<<" "<<height<<"\n";
        for(int row=0;row<height;row++){
            for(size_t i=0;i<full.length();i++){
                string pattern=code39[full[i]];
                for(int j=0;j<9;j++){
                    int width=(pattern[j]=='n'?NARROW:WIDE);
                    bool isBar=(j%2==0);
                    for(int w=0;w<width;w++) out<<(isBar?"1 ":"0 ");
                }
                for(int w=0;w<NARROW;w++) out<<"0 ";
            }
            out<<"\n";
        }
        out.close();
    }
    void display() const {
        string stockColor = stock < 20 ? RED : (stock < 50 ? YELLOW : GREEN);
        cout << BRIGHT_CYAN << "ID: " << RESET << productId 
             << BRIGHT_CYAN << " | " << RESET << BOLD << name << RESET 
             << BRIGHT_CYAN << " | " << RESET << GREEN << "$" << fixed << setprecision(2) << price << RESET
             << BRIGHT_CYAN << " | Stock: " << RESET << stockColor << stock << RESET
             << BRIGHT_CYAN << " | Category: " << RESET << MAGENTA << category << RESET
             << BRIGHT_CYAN << " | Barcode: " << RESET << YELLOW << barcode << RESET << endl;
    }
    void saveToFile(ofstream& out) const {
        out<<productId<<","<<name<<","<<price<<","<<stock<<","<<category<<"\n";
    }
    int getId() const { return productId; }
    string getName() const { return name; }
    double getPrice() const { return price; }
    int getStock() const { return stock; }
    string getBarcode() const { return barcode; }
};

class Transaction {
private:
    int transactionId;
    vector<pair<Product*, int>> items;
    double totalAmount;
    string date;
    int cashierId;
    string getCurrentDate() {
        time_t now=time(0);
        struct tm* t=localtime(&now);
        char buf[80]; /* we need to use c style char* to use time structs */
        strftime(buf,sizeof(buf),"%Y-%m-%d %H:%M:%S",t);
        return string(buf);
    }
public:
    Transaction(int id,int cashier)
        : transactionId(id), totalAmount(0), cashierId(cashier) {
        date = getCurrentDate();
    }
    void addItem(Product* product,int quantity){
        if(!product) throw ProductNotFoundException();
        if(quantity<=0) throw InvalidQuantityException();
        product->reduceStock(quantity);
        items.push_back(make_pair(product,quantity));
        totalAmount += product->getPrice()*quantity;
    }
    void generateReceipt(const string& filename){
        ofstream out(filename);
        if(!out) throw FileOperationException("Cannot create receipt file");
        out<<"========================================\n";
        out<<"               OBA MARKET\n";
        out<<"========================================\n";
        out<<"Transaction ID: "<<transactionId<<"\n";
        out<<"Date: "<<date<<"\n";
        out<<"Cashier ID: "<<cashierId<<"\n";
        out<<"----------------------------------------\n";
        out<<left<<setw(20)<<"Item"<<setw(8)<<"Qty"<<setw(10)<<"Price"<<"Total\n";
        out<<"----------------------------------------\n";
        for(size_t i=0;i<items.size();i++){
            out<<left<<setw(20)<<items[i].first->getName()
                <<setw(8)<<items[i].second
                <<"$"<<setw(9)<<fixed<<setprecision(2)<<items[i].first->getPrice()
                <<"$"<<(items[i].first->getPrice()*items[i].second)<<"\n";
        }
        out<<"========================================\n";
        out<<"TOTAL: $"<<fixed<<setprecision(2)<<totalAmount<<"\n";
        out<<"========================================\n";
        out<<"      Qarabağ Azərbaycandır!\n";
        out<<"========================================\n";
    }
    double getTotal() const { return totalAmount; }
    void display() const {
        cout << BRIGHT_YELLOW << "Transaction ID: " << RESET << transactionId 
             << BRIGHT_YELLOW << " | Date: " << RESET << date
             << BRIGHT_YELLOW << " | Cashier: " << RESET << cashierId 
             << BRIGHT_YELLOW << " | Total: " << RESET << BRIGHT_GREEN << "$" << fixed
             << setprecision(2) << totalAmount << RESET << endl;
    }
};

class SupermarketManager {
private:
    map<int,Product*> products;
    vector<Employee*> employees;
    queue<Transaction*> transactions;
    int nextProductId;
    int nextEmployeeId;
    int nextTransactionId;
public:
    SupermarketManager()
        : nextProductId(1001), nextEmployeeId(1), nextTransactionId(1) {}
    ~SupermarketManager(){
        for(auto& p:products) delete p.second;
        for(auto e:employees) delete e;
        while(!transactions.empty()){
            delete transactions.front();
            transactions.pop();
        }
    }
    void addProduct(string name,double price,int stock,string category){
        Product* p=new Product(nextProductId++,name,price,stock,category);
        products[p->getId()] = p;
        cout << BRIGHT_GREEN << "✓ Product added successfully! ID: " << p->getId() << RESET << endl;
    }
    void displayProducts(){
        if(products.empty()){ 
            cout << YELLOW << "⚠ No products in inventory." << RESET << "\n"; 
            return; 
        }
        cout << "\n" << BOLD << BG_BLUE << WHITE << "           PRODUCT INVENTORY           " << RESET << "\n";
        cout << BRIGHT_BLUE << "═══════════════════════════════════════════════════════════════════════════════" << RESET << "\n";
        for(auto& p:products) p.second->display();
        cout << BRIGHT_BLUE << "═══════════════════════════════════════════════════════════════════════════════" << RESET << "\n";
    }
    void updateProductStock(int id,int quantity){
        if(products.find(id)==products.end()) throw ProductNotFoundException();
        products[id]->updateStock(quantity);
        cout << BRIGHT_GREEN << "✓ Stock updated successfully!" << RESET << "\n";
    }
    void generateProductBarcode(int id){
        if(products.find(id)==products.end()) throw ProductNotFoundException();
        string filename = products[id]->getBarcode()+".pbm";
        products[id]->generateBarcode(filename);
        cout << BRIGHT_GREEN << "✓ Barcode generated: " << RESET << CYAN << filename << RESET << endl;
    }
    void addManager(string name,string phone,double salary,double bonus){
        Manager* m = new Manager(name,nextEmployeeId++,phone,salary,bonus);
        employees.push_back(m);
        cout << BRIGHT_GREEN << "✓ Manager added successfully! ID: " << m->getId() << RESET << endl;
    }
    void addCashier(string name,string phone,double salary){
        Cashier* c = new Cashier(name,nextEmployeeId++,phone,salary);
        employees.push_back(c);
        cout << BRIGHT_GREEN << "✓ Cashier added successfully! ID: " << c->getId() << RESET << endl;
    }
    void displayEmployees(){
        if(employees.empty()){ 
            cout << YELLOW << "⚠ No employees registered." << RESET << "\n"; 
            return; 
        }
        cout << "\n" << BOLD << BG_GREEN << WHITE << "           EMPLOYEE LIST           " << RESET << "\n";
        cout << BRIGHT_MAGENTA << "══════════════════════════════════════════════════════════" << RESET << "\n";
        for(auto e:employees){ 
            e->display(); 
            cout << BRIGHT_MAGENTA << "──────────────────────────────────────────────────────────" << RESET << "\n"; 
        }
    }
    void processTransaction(int cashierId){
        Transaction* trans = new Transaction(nextTransactionId++,cashierId);
        cout << "\n" << BOLD << BRIGHT_CYAN << "━━━━━━━ NEW TRANSACTION ━━━━━━━" << RESET << "\n";
        while(true){
            cout << YELLOW << "Enter Product ID (0 to finish): " << RESET;
            int pid; cin>>pid;
            if(pid==0) break;
            if(products.find(pid)==products.end()){
                cout << RED << "✗ Product not found!" << RESET << "\n"; 
                continue;
            }
            cout << YELLOW << "Enter quantity: " << RESET;
            int qty; cin>>qty;
            try{
                trans->addItem(products[pid],qty);
                cout << BRIGHT_GREEN << "✓ Item added: " << RESET << products[pid]->getName() 
                     << CYAN << " x " << qty << RESET << endl;
            } catch(const exception& e){
                cout << RED << "✗ Error: " << e.what() << RESET << endl;
            }
        }
        trans->display();
        transactions.push(trans);
        for(auto e:employees){
            if(e->getId()==cashierId){
                Cashier* c=dynamic_cast<Cashier*>(e);
                if(c) c->incrementTransactions();
                break;
            }
        }
        stringstream ss;
        ss<<"receipt_"<<trans->getTotal()<<".txt";
        trans->generateReceipt(ss.str());
        cout << BRIGHT_GREEN << "✓ Receipt saved to: " << RESET << CYAN << ss.str() << RESET << endl;
    }
    void displayTransactions(){
        if(transactions.empty()){ 
            cout << YELLOW << "⚠ No transactions recorded." << RESET << "\n"; 
            return; 
        }
        cout << "\n" << BOLD << BG_BLUE << WHITE << "           TRANSACTIONS           " << RESET << "\n";
        cout << BRIGHT_YELLOW << "═════════════════════════════════════════════════════════════════════════" << RESET << "\n";
        queue<Transaction*> temp = transactions;
        while(!temp.empty()){
            temp.front()->display();
            temp.pop();
        }
        cout << BRIGHT_YELLOW << "═════════════════════════════════════════════════════════════════════════" << RESET << "\n";
    }
    void saveProductsToFile(const string& filename){
        ofstream out(filename);
        if(!out) throw FileOperationException("Cannot open "+filename);
        out<<"ProductID,Name,Price,Stock,Category\n";
        for(auto& p:products) p.second->saveToFile(out);
        out.close();
        cout << BRIGHT_GREEN << "✓ Products saved to " << RESET << CYAN << filename << RESET << endl;
    }
    void saveEmployeesToFile(const string& filename){
        ofstream out(filename);
        if(!out) throw FileOperationException("Cannot open "+filename);
        out<<"EmployeeID,Name,Phone,Position,BaseSalary,TotalSalary\n";
        for(auto e:employees){
            out<<e->getId()<<","<<e->getName()<<","
               <<"PHONE"<<","<<e->getPosition()<<","
               <<fixed<<setprecision(2)<<e->getSalary()<<","
               <<e->calculateSalary()<<"\n";
        }
        out.close();
        cout << BRIGHT_GREEN << "✓ Employees saved to " << RESET << YELLOW << filename << RESET << endl;
    }
    void loadProductsFromFile(const string& filename){
        ifstream in(filename);
        if(!in) throw FileOperationException("Cannot open "+filename);
        string line;
        getline(in,line);
        while(getline(in,line)){
            stringstream ss(line);
            string id,name,price,stock,category;
            getline(ss,id,',');
            getline(ss,name,',');
            getline(ss,price,',');
            getline(ss,stock,',');
            getline(ss,category,',');
            Product* p=new Product(stoi(id),name,stod(price),stoi(stock),category);
            products[p->getId()] = p;
            nextProductId = max(nextProductId,p->getId()+1);
        }
        in.close();
        cout << BRIGHT_GREEN << "✓ Products loaded from " << RESET << CYAN << filename << RESET << endl;
    }
};

void displayMenu(){
    cout << "\n" << BOLD << BRIGHT_CYAN;
    cout << "╔════════════════════════════════════════════╗\n";
    cout << "║                                            ║\n";
    cout << "║     STOX SUPERMARKET MANAGEMENT SYSTEM     ║\n";
    cout << "║                                            ║\n";
    cout << "╚════════════════════════════════════════════╝" << RESET << "\n\n";
    
    cout << BRIGHT_MAGENTA << "   PRODUCTS " << RESET << "\n";
    cout << "   " << CYAN << "1." << RESET << "  Add Product\n";
    cout << "   " << CYAN << "2." << RESET << "  Display Products\n";
    cout << "   " << CYAN << "3." << RESET << "  Update Product Stock\n";
    cout << "   " << CYAN << "4." << RESET << "  Generate Product Barcode\n\n";
    
    cout << BLUE << "   EMPLOYEES" << RESET << "\n";
    cout << "   " << CYAN << "5." << RESET << "  Add Manager\n";
    cout << "   " << CYAN << "6." << RESET << "  Add Cashier\n";
    cout << "   " << CYAN << "7." << RESET << "  Display Employees\n\n";
    
    cout << GREEN << "   TRANSACTIONS" << RESET << "\n";
    cout << "   " << CYAN << "8." << RESET << "  Process Transaction\n";
    cout << "   " << CYAN << "9." << RESET << "  Display Transactions\n\n";
    
    cout << "   " << BG_BLUE <<"FILE OPERATIONS" << RESET << "\n";
    cout << "   " << CYAN << "10." << RESET << " Save Products to File\n";
    cout << "   " << CYAN << "11." << RESET << " Load Products from File\n";
    cout << "   " << CYAN << "12." << RESET << " Save Employees to File\n\n";
    
    cout << "   " << RED << "0." << RESET << "  Exit\n\n";
    cout << YELLOW << "➤ Enter choice: " << RESET;
}

void waitForEnter() {
    cout << "\n" << BRIGHT_BLUE << "Press Enter to continue..." << RESET;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

int main(){
    SupermarketManager manager;
    int choice;
    cout << "\n" << BOLD << BG_GREEN;
    cout << "╔═══════════════════════════════════════╗\n";
    cout << "║   ███████╗████████╗ ██████╗ ██╗  ██╗  ║\n";
    cout << "║   ██╔════╝╚══██╔══╝██╔═══██╗╚██╗██╔╝  ║\n";
    cout << "║   ███████╗   ██║   ██║   ██║ ╚███╔╝   ║\n";
    cout << "║   ╚════██║   ██║   ██║   ██║ ██╔██╗   ║\n";
    cout << "║   ███████║   ██║   ╚██████╔╝██╔╝ ██╗  ║\n";
    cout << "║   ╚══════╝   ╚═╝    ╚═════╝ ╚═╝  ╚═╝  ║\n";
    cout << "╚═══════════════════════════════════════╝" << RESET << "\n\n";
    cout << GREEN << "\nLoading Initializing system with sample data..." << RESET << "\n";
    try{
        manager.addProduct("Milk",3.99,50,"Dairy");
        manager.addProduct("Bread",2.49,100,"Bakery");
        manager.addProduct("Apple",1.99,200,"Fruits");
        manager.addCashier("Kamal Surxanlı","077-543-55-21",2000);
        manager.addManager("Müseyib Teymurov","099-805-49-22",5000,1000);
        cout << BRIGHT_GREEN << "✓ System initialized successfully!\n" << RESET;
    } catch(const exception& e){
        cout << RED << "✗ Error during initialization: " << e.what() << RESET << endl;
    }
    
    while(true){
        displayMenu();
        cin>>choice;
        if(cin.fail()){
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << RED << "✗ Invalid input! Please enter a number." << RESET << "\n";
            continue;
        }
        try{
            switch(choice){
                case 1:{
                    string name,category;
                    double price; int stock;
                    cout << YELLOW << "Product name: " << RESET;
                    cin.ignore(); getline(cin,name);
                    cout << YELLOW << "Price: " << RESET; cin>>price;
                    cout << YELLOW << "Stock: " << RESET; cin>>stock;
                    cout << YELLOW << "Category: " << RESET; cin.ignore(); getline(cin,category);
                    manager.addProduct(name,price,stock,category);
                    break;
                }
                case 2: manager.displayProducts(); break;
                case 3:{
                    int id,qty; 
                    cout << YELLOW << "Product ID: " << RESET; cin>>id;
                    cout << YELLOW << "Quantity to add: " << RESET; cin>>qty;
                    manager.updateProductStock(id,qty);
                    break;
                }
                case 4:{
                    int id; 
                    cout << YELLOW << "Product ID: " << RESET; cin>>id;
                    manager.generateProductBarcode(id);
                    break;
                }
                case 5:{
                    string name,phone; double salary,bonus;
                    cout << YELLOW << "Manager name: " << RESET; cin.ignore(); getline(cin,name);
                    cout << YELLOW << "Phone: " << RESET; getline(cin,phone);
                    cout << YELLOW << "Salary: " << RESET; cin>>salary;
                    cout << YELLOW << "Bonus: " << RESET; cin>>bonus;
                    manager.addManager(name,phone,salary,bonus);
                    break;
                }
                case 6:{
                    string name,phone; double salary;
                    cout << YELLOW << "Cashier name: " << RESET; cin.ignore(); getline(cin,name);
                    cout << YELLOW << "Phone: " << RESET; getline(cin,phone);
                    cout << YELLOW << "Salary: " << RESET; cin>>salary;
                    manager.addCashier(name,phone,salary);
                    break;
                }
                case 7: manager.displayEmployees(); break;
                case 8:{
                    int cashierId; 
                    cout << YELLOW << "Cashier ID: " << RESET; cin>>cashierId;
                    manager.processTransaction(cashierId);
                    break;
                }
                case 9: manager.displayTransactions(); break;
                case 10: manager.saveProductsToFile("products.csv"); break;
                case 11: manager.loadProductsFromFile("products.csv"); break;
                case 12: manager.saveEmployeesToFile("employees.csv"); break;
                case 0: 
                    cout << "\n" << MAGENTA << "✓ Thank you for using STOX system! Goodbye!" << RESET << "\n\n"; 
                    return 0;
                default: 
                    cout << RED << "✗ Invalid choice! Please select a valid option." << RESET << "\n";
            }
        } catch(const exception& e){
            cout << RED << "✗ ERROR: " << e.what() << RESET << endl;
        }
        waitForEnter();
    }
    return 0;
}
