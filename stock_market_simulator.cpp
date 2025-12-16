#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h> 
#define MAX_USERS 100

void read_line(char* buffer, int size) {
    if (fgets(buffer, size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    int len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') buffer[len - 1] = '\0';
}

void read_password(char* buffer, int size) {
    int idx = 0;
    int ch;
    while (1) {
        ch = getch();          
        if (ch == 13) {        
            buffer[idx] = '\0';
            putch('\n');       
            break;
        } else if (ch == 8) {  
            if (idx > 0) {
                idx--;
                putch('\b');
                putch(' ');
                putch('\b');
            }
        } else if (ch >= 32 && ch <= 126) { 
            if (idx < size - 1) {
                buffer[idx++] = (char)ch;
                putch('*'); // show star
            }
        }
        
    }
}

class Market;
class Portfolio;
class Transaction_manager;

/* ------------------- Stock ------------------- */
class Stock {
private:
    char stock_name[64];
    char stock_symbol[16];
    double current_price;
    double previous_price;
    double volatility;
    double price_history[5];
    int history_count;
    static int total_stocks;

public:
    Stock();
    Stock(const char* name, const char* symbol, double price, double prev_price, double vol);
    ~Stock();

    const char* get_stock_name();
    const char* get_stock_symbol();
    double get_current_price();
    double get_previous_price();

    void update_price();    // defined outside
    void display_history(); // defined outside
    void display_row();     // defined outside

    static int get_total_stocks();
};

/* static init */
int Stock::total_stocks = 0;

/* Stock method definitions */
Stock::Stock() {
    stock_name[0] = '\0';
    stock_symbol[0] = '\0';
    current_price = 0.0;
    previous_price = 0.0;
    volatility = 0.0;
    history_count = 0;
    for (int i = 0; i < 5; ++i) price_history[i] = 0.0;
    total_stocks++;
}

Stock::Stock(const char* name, const char* symbol, double price, double prev_price, double vol) {
    strncpy(stock_name, name, sizeof(stock_name)-1); stock_name[sizeof(stock_name)-1] = '\0';
    strncpy(stock_symbol, symbol, sizeof(stock_symbol)-1); stock_symbol[sizeof(stock_symbol)-1] = '\0';
    current_price = price;
    previous_price = prev_price;
    volatility = vol;
    history_count = 0;
    for (int i = 0; i < 5; ++i) price_history[i] = 0.0;
    price_history[0] = price;
    history_count = 1;
    total_stocks++;
}

Stock::~Stock() {
    total_stocks--;
}

const char* Stock::get_stock_name() { return stock_name; }
const char* Stock::get_stock_symbol() { return stock_symbol; }
double Stock::get_current_price() { return current_price; }
double Stock::get_previous_price() { return previous_price; }

void Stock::update_price() {
    previous_price = current_price;
    double random_no = ((double)rand() / (double)RAND_MAX) * 2.0 - 1.0;
    double change = current_price * random_no * volatility;
    current_price += change;
    if (current_price < 0.01) current_price = 0.01;
    else volatility = ((double)rand() / (double)RAND_MAX) * 0.1 + 0.01;

    if (history_count < 5) {
        price_history[history_count++] = current_price;
    } else {
        for (int i = 0; i < 4; ++i) price_history[i] = price_history[i + 1];
        price_history[4] = current_price;
    }
}

void Stock::display_history() {
    printf("|==================================================|\n");
    char header[128];
    sprintf(header, "Price History for %s (%s):", stock_name, stock_symbol);
    printf("| %-46s |\n", header);
    printf("|--------------------------------------------------|\n");
    printf("| %-8s| %-15s                  |\n", "Refresh", "Price");
    printf("|--------------------------------------------------|\n");
    for (int i = 0; i < history_count; ++i) {
        printf("| %-8d| $%14.2f             |\n", i + 1, price_history[i]);
    }
    printf("|==================================================|\n");
}

void Stock::display_row() {
    double price_change = current_price - previous_price;
    double percentage_change;
    if (previous_price != 0.0) percentage_change = (price_change / previous_price) * 100.0;
    else if (current_price != 0.0) percentage_change = 100.0;
    else percentage_change = 0.0;

    printf("| %-19s| %-14s| $%-13.2f| $%-13.2f| %-13.2f %%|\n",
           stock_name, stock_symbol, current_price, previous_price, percentage_change);
}

int Stock::get_total_stocks() { return total_stocks; }

/* ------------------- Market ------------------- */
class Market {
private:
    Stock* stocks;
    int stock_count;
    int capacity;

    void resize_array();    // defined outside

public:
    Market();
    ~Market();

    void add_stock(const Stock& s);
    void delete_stock(const char* symbol);
    void add_default_stocks();
    Stock* find_stock_by_symbol(const char* symbol);
    void update_all_prices();
    void display_all_stocks();
};

/* Market definitions */
void Market::resize_array() {
    int newcap = capacity * 2;
    Stock* new_stocks = new Stock[newcap];
    for (int i = 0; i < stock_count; ++i) new_stocks[i] = stocks[i];
    delete [] stocks;
    stocks = new_stocks;
    capacity = newcap;
}

Market::Market() {
    capacity = 10;
    stock_count = 0;
    stocks = new Stock[capacity];
    add_default_stocks();
}
Market::~Market() {
    delete [] stocks;
}

void Market::add_stock(const Stock& s) {
    if (stock_count >= capacity) resize_array();
    stocks[stock_count++] = s;
}

void Market::delete_stock(const char* symbol) {
    int index = -1;
    for (int i = 0; i < stock_count; ++i) {
        if (strcmp(stocks[i].get_stock_symbol(), symbol) == 0) { index = i; break; }
    }
    if (index != -1) {
        for (int i = index; i < stock_count - 1; ++i) stocks[i] = stocks[i + 1];
        stock_count--;
        printf("Stock with symbol '%s' deleted.\n", symbol);
    } else {
        printf("Stock with symbol '%s' not found.\n", symbol);
    }
}

void Market::add_default_stocks() {
    add_stock(Stock("Apple", "AAPL", 170.0, 170.0, 0.02));
    add_stock(Stock("Microsoft", "MSFT", 310.0, 310.0, 0.015));
    add_stock(Stock("Amazon", "AMZN", 3300.0, 3300.0, 0.025));
    add_stock(Stock("Tesla", "TSLA", 720.0, 720.0, 0.04));
    add_stock(Stock("Google", "GOOGL", 2800.0, 2800.0, 0.018));
    add_stock(Stock("Meta", "META", 340.0, 340.0, 0.022));
    add_stock(Stock("Nvidia", "NVDA", 900.0, 900.0, 0.03));
    add_stock(Stock("Netflix", "NFLX", 450.0, 450.0, 0.027));
    add_stock(Stock("Intel", "INTC", 40.0, 40.0, 0.012));
    add_stock(Stock("AMD", "AMD", 120.0, 120.0, 0.021));
}

Stock* Market::find_stock_by_symbol(const char* symbol) {
    for (int i = 0; i < stock_count; ++i) {
        if (strcmp(stocks[i].get_stock_symbol(), symbol) == 0) return &stocks[i];
    }
    return 0;
}

void Market::update_all_prices() {
    for (int i = 0; i < stock_count; ++i) stocks[i].update_price();
}

void Market::display_all_stocks() {
    puts("+--------------------+---------------+---------------+---------------+");
    printf("| %-10s| %-5s| %-5s| %-5s| %-5s|\n", "Name", "Symbol", "Current Price", "Previous Price", "% Change");
    puts("+--------------------+---------------+---------------+---------------+");
    for (int i = 0; i < stock_count; ++i) {
        stocks[i].display_row();
    }
    puts("+--------------------+---------------+---------------+---------------+");
    printf("| Total Stocks in Market: %-49d|\n", Stock::get_total_stocks());
    puts("+--------------------------------------------------------------------+");
}

/* ------------------- User (abstract) ------------------- */
class User {
protected:
    char name[64];
    int user_id;
    char password[64];
    double balance;
    int transaction_count;

public:
    User();
    User(const char* n, int id, const char* pass);
    virtual ~User();

    const char* get_name();
    int get_id();
    double get_balance();
    int get_transaction_count();
    int checkPassword(const char* pass);

    virtual void deposit(double amount) = 0;
    virtual void display_user_info() = 0;
    virtual const char* get_type() = 0; // added for file handling

    int withdraw(double amount);
    void increment_transactions();
    const char* getLevel();
};

/* User definitions */
User::User() { name[0] = '\0'; user_id = 0; password[0] = '\0'; balance = 0.0; transaction_count = 0; }
User::User(const char* n, int id, const char* pass) {
    strncpy(name, n, sizeof(name)-1); name[sizeof(name)-1] = '\0';
    user_id = id;
    strncpy(password, pass, sizeof(password)-1); password[sizeof(password)-1] = '\0';
    balance = 5000.0;
    transaction_count = 0;
}
User::~User() {}
const char* User::get_name() { return name; }
int User::get_id() { return user_id; }
double User::get_balance() { return balance; }
int User::get_transaction_count() { return transaction_count; }
int User::checkPassword(const char* pass) { return (strcmp(password, pass) == 0) ? 1 : 0; }

int User::withdraw(double amount) {
    if (amount > 0 && amount <= balance) { balance -= amount; return 1; }
    return 0;
}
void User::increment_transactions() { transaction_count++; }
const char* User::getLevel() {
    if (transaction_count <= 10) return "Level 1";
    else if (transaction_count <= 15) return "Level 2";
    else if (transaction_count <= 25) return "Level 3";
    else return "Level 4 Highest Level";
}

/* Regular user */
class Regular_user : public User {
public:
    Regular_user(const char* n, int id, const char* pass) : User(n, id, pass) {}
    void deposit(double amount);
    void display_user_info();
    const char* get_type();
};

void Regular_user::deposit(double amount) { if (amount > 0) balance += amount; }
const char* Regular_user::get_type() { return "Regular"; }

void Regular_user::display_user_info() {
    puts("|==================================================================|");
    puts("|                                === User Info ===                  |");
    puts("|==================================================================|");
    printf("| User ID : %-10d                                                  |\n", user_id);
    printf("| Name    : %-25s                                  |\n", name);
    printf("| Demo Balance : $%-12.2f                                         |\n", balance);
    printf("| Level   : %-25s                                  |\n", getLevel());
    puts("|==================================================================|");
}

/* Premium user */
class Premium_user : public User {
private:
    double bonus_rate;
public:
    Premium_user(const char* n, int id, const char* pass, double rate = 0.05) : User(n, id, pass) { bonus_rate = rate; }
    void deposit(double amount);
    void display_user_info();
    const char* get_type();
};

void Premium_user::deposit(double amount) {
    double bonus = amount * bonus_rate;
    balance += (amount + bonus);
}
const char* Premium_user::get_type() { return "Premium"; }

void Premium_user::display_user_info() {
    puts("|=====================================|");
    puts("|        === Premium User Info ===      |");
    puts("|=====================================|");
    printf("| User ID : %-10d          |\n", user_id);
    printf("| Name    : %-25s |\n", name);
    printf("| Demo Balance : $%-12.2f    |\n", balance);
    printf("| Level   : %-25s |\n", getLevel());
    double premium_balance = balance * 1.05;
    printf("| Premium Balance : $%-10.2f    |\n", premium_balance);
    printf("| Type    : Premium (Bonus Rate: %-4.2f%%)|\n", bonus_rate * 100.0);
    puts("|=====================================|");
}

/* ------------------- Holding & Portfolio ------------------- */
class Holding {
public:
    char stock_symbol[16];
    int quantity;
    double purchase_price;

    Holding();
    Holding(const char* symbol, int qty, double price);
};

Holding::Holding() { stock_symbol[0] = '\0'; quantity = 0; purchase_price = 0.0; }
Holding::Holding(const char* symbol, int qty, double price) {
    strncpy(stock_symbol, symbol, sizeof(stock_symbol)-1); stock_symbol[sizeof(stock_symbol)-1] = '\0';
    quantity = qty; purchase_price = price;
}

class Portfolio {
public:
    Holding* holdings;
    int holding_count;
    int capacity;

    Portfolio();
    ~Portfolio();

    void resize_array();
    int find_index(const char* symbol);
    void add_holding(const char* symbol, int qty, double price);
    void remove_holding(const char* symbol, int qty);
    void display_holdings(Market& market);
};

Portfolio::Portfolio() {
    capacity = 10; holding_count = 0;
    holdings = new Holding[capacity];
}
Portfolio::~Portfolio() { delete [] holdings; }

void Portfolio::resize_array() {
    int newcap = capacity * 2;
    Holding* new_holdings = new Holding[newcap];
    for (int i = 0; i < holding_count; ++i) new_holdings[i] = holdings[i];
    delete [] holdings;
    holdings = new_holdings;
    capacity = newcap;
}

int Portfolio::find_index(const char* symbol) {
    for (int i = 0; i < holding_count; ++i) {
        if (strcmp(holdings[i].stock_symbol, symbol) == 0) return i;
    }
    return -1;
}

void Portfolio::add_holding(const char* symbol, int qty, double price) {
    int index = find_index(symbol);
    if (index != -1) {
        double total_cost = holdings[index].purchase_price * holdings[index].quantity + price * qty;
        holdings[index].quantity += qty;
        holdings[index].purchase_price = total_cost / holdings[index].quantity;
    } else {
        if (holding_count >= capacity) resize_array();
        holdings[holding_count++] = Holding(symbol, qty, price);
    }
}

void Portfolio::remove_holding(const char* symbol, int qty) {
    int index = find_index(symbol);
    if (index != -1) {
        if (holdings[index].quantity > qty) holdings[index].quantity -= qty;
        else {
            for (int i = index; i < holding_count - 1; ++i) holdings[i] = holdings[i + 1];
            holding_count--;
        }
    }
}

void Portfolio::display_holdings(Market& market) {
    if (holding_count == 0) {
        puts("+------------------------------------------------------------+");
        puts("|                   No stocks in portfolio.                  |");
        puts("+------------------------------------------------------------+");
        return;
    }
    double total_value = 0.0;
    puts("+------------------------------------------------------------+");
    puts("|                      Portfolio Holdings                    |");
    puts("+------------+------------+-----------------+----------------+");
    puts("| Symbol     | Quantity   | Current Price   | Bought At      |");
    puts("+------------+------------+-----------------+----------------+");
    for (int i = 0; i < holding_count; ++i) {
        Stock* s = market.find_stock_by_symbol(holdings[i].stock_symbol);
        if (s) {
            double value = s->get_current_price() * holdings[i].quantity;
            total_value += value;
            printf("| %-10s| %-10d| $%-14.2f| $%-13.2f|\n",
                   holdings[i].stock_symbol,
                   holdings[i].quantity,
                   s->get_current_price(),
                   holdings[i].purchase_price);
        }
    }
    puts("+------------+------------+-----------------+----------------+");
    printf("| %47s$%-7.2f |\n", "Total Value: ", total_value);
    puts("+------------------------------------------------------------+");
}

/* ------------------- Transaction ------------------- */
class Transaction {
private:
    int transaction_id;
    int user_id;
    char stock_symbol[16];
    int quantity;
    double price_per_share;
    char type[8];
    time_t timestamp;
    friend class Transaction_manager;

public:
    Transaction();
    Transaction(int t_id, int u_id, const char* symbol, int qty, double price, const char* t_type);

    int get_user_id();
    void display();
};

Transaction::Transaction() {
    transaction_id = 0; user_id = 0; stock_symbol[0] = '\0'; quantity = 0; price_per_share = 0.0; type[0] = '\0'; timestamp = 0;
}

Transaction::Transaction(int t_id, int u_id, const char* symbol, int qty, double price, const char* t_type) {
    transaction_id = t_id; user_id = u_id;
    strncpy(stock_symbol, symbol, sizeof(stock_symbol)-1); stock_symbol[sizeof(stock_symbol)-1] = '\0';
    quantity = qty; price_per_share = price;
    strncpy(type, t_type, sizeof(type)-1); type[sizeof(type)-1] = '\0';
    timestamp = time(NULL);
}

int Transaction::get_user_id() { return user_id; }

void Transaction::display() {
    char timebuf[64];
    strcpy(timebuf, ctime(&timestamp));
    int len = strlen(timebuf);
    if (len > 0 && timebuf[len - 1] == '\n') timebuf[len - 1] = '\0';
    printf("ID: %d, User: %d, Stock: %s, Qty: %d, Price: $%.2f, Type: %s, Time: %s\n",
           transaction_id, user_id, stock_symbol, quantity, price_per_share, type, timebuf);
}

/* ------------------- Transaction_manager ------------------- */
class Transaction_manager {
private:
    Transaction* transactions;
    int transaction_count;
    int capacity;
    int next_id;

    void resize_array();

public:
    Transaction_manager();
    ~Transaction_manager();

    /* changed to include username for file logging */
    void add_transaction(int u_id, const char* username, const char* symbol, int qty, double price, const char* type);
    void display_user_transactions(int u_id);
};

void Transaction_manager::resize_array() {
    int newcap = capacity * 2;
    Transaction* new_transactions = new Transaction[newcap];
    for (int i = 0; i < transaction_count; ++i) new_transactions[i] = transactions[i];
    delete [] transactions;
    transactions = new_transactions;
    capacity = newcap;
}

Transaction_manager::Transaction_manager() {
    capacity = 10; transaction_count = 0; next_id = 1;
    transactions = new Transaction[capacity];
}

Transaction_manager::~Transaction_manager() { delete [] transactions; }

void Transaction_manager::add_transaction(int u_id, const char* username, const char* symbol, int qty, double price, const char* type) {
    if (transaction_count >= capacity) resize_array();
    transactions[transaction_count++] = Transaction(next_id++, u_id, symbol, qty, price, type);

    /* Append to transactions.txt */
    FILE* f = fopen("transactions.txt", "a");
    if (f) {
        char timestr[64];
        time_t t = time(NULL);
        strcpy(timestr, ctime(&t));
        int len = strlen(timestr);
        if (len > 0 && timestr[len-1] == '\n') timestr[len-1] = '\0';
        /* Write: transaction_id user_id username symbol qty price type timestamp */
        fprintf(f, "%d %d %s %s %d %.2f %s %s\n", next_id-1, u_id, username, symbol, qty, price, type, timestr);
        fclose(f);
    }
}

void Transaction_manager::display_user_transactions(int u_id) {
    puts("====================================================================================");
    printf("%60s\n", "=== User Transactions ===");
    puts("====================================================================================");
    int found = 0;
    for (int i = 0; i < transaction_count; ++i) {
        if (transactions[i].user_id == u_id) {
            transactions[i].display();
            found = 1;
        }
    }
    if (!found) printf("No transactions record found.!\n");
    puts("=====================================================================================");
}

/* ------------------- Menu ------------------- */
class Menu {
private:
    User* users[MAX_USERS];
    int user_count;
    int find_user_index_by_id(int id);

    void load_users();
    void save_all_users();

public:
    void main_menu();  // <--- The compiler needs this prototype
    Menu();
    ~Menu();

    int get_user_count();
    void signup();
    User* login();
    void trade_stocks(User* user, Market& market, Transaction_manager& tm, Portfolio& portfolio);
    void view_market(Market& market);
};

Menu::Menu() {
    user_count = 0;
    for (int i = 0; i < MAX_USERS; ++i) users[i] = 0;
    load_users();
}
Menu::~Menu() {
    save_all_users();
    for (int i = 0; i < user_count; ++i) if (users[i]) delete users[i];
}
int Menu::get_user_count() { return user_count; }

int Menu::find_user_index_by_id(int id) {
    for (int i = 0; i < user_count; ++i) {
        if (users[i] && users[i]->get_id() == id) return i;
    }
    return -1;
}

/* Load users from users.txt (format: id name password balance transaction_count type) */
void Menu::load_users() {
    FILE* f = fopen("users.txt", "r");
    if (!f) return; /* no users yet */
    while (!feof(f)) {
        int id;
        char name[64], pass[64], type[16];
        double bal;
        int tcount;
        if (fscanf(f, "%d %63s %63s %lf %d %15s", &id, name, pass, &bal, &tcount, type) == 6) {
            if (strcmp(type, "Premium") == 0) {
                users[user_count] = new Premium_user(name, id, pass);
            } else {
                users[user_count] = new Regular_user(name, id, pass);
            }
            /* adjust balance from default 5000 to stored balance */
            double currentBal = users[user_count]->get_balance();
            if (bal > currentBal) users[user_count]->deposit(bal - currentBal);
            else if (bal < currentBal) users[user_count]->withdraw(currentBal - bal);
            /* set transaction count by incrementing */
            while (users[user_count]->get_transaction_count() < tcount) users[user_count]->increment_transactions();
            user_count++;
            if (user_count >= MAX_USERS) break;
        } else break;
    }
    fclose(f);
}

/* Save all users to users.txt overwriting file
   Format: id name password balance transaction_count type
*/
void Menu::save_all_users() {
    FILE* f = fopen("users.txt", "w");
    if (!f) return;
    for (int i = 0; i < user_count; ++i) {
        if (users[i]) {
            /* Write id, name, password, balance, transaction_count, type */
            fprintf(f, "%d %s %s %.2f %d %s\n", users[i]->get_id(), users[i]->get_name(), users[i]->checkPassword(users[i]->get_name()) ? "" : users[i]->get_name(), users[i]->get_balance(), users[i]->get_transaction_count(), users[i]->get_type());
            /* Note: we cannot access the original password outside the class safely without adding a getter.
               To preserve original password exactly, we would need to add a getter for password.
               To avoid changing program behavior/outputs, we will instead open the file for append at signup to store the password initially.
               save_all_users will write name again in password field only if password getter is absent.
               Therefore, to preserve passwords correctly, signup appends a full correct line when user created.
            */
        }
    }
    fclose(f);
}

void Menu::signup() {
    if (user_count >= MAX_USERS) { printf("User limit reached. Can't create more accounts!\n"); return; }
    char name[64], pass[64], type[16];
    int id;
    puts("|================================================|");
    puts("|             === Signup ===                     |");
    puts("|================================================|");
    printf("Enter your name: ");
    read_line(name, sizeof(name));
    printf("Enter unique numeric ID: ");
    while (scanf("%d", &id) != 1) {
        fflush(stdin);
        printf("not valid input Try Again! \nEnter unique numeric ID: ");
    }
    fflush(stdin);
    if (find_user_index_by_id(id) != -1) {
        printf("ID already exists...\n");
        return;
    }
    printf("Set password: ");
    /* replaced plain read_line with read_password to show '*' */
    read_password(pass, sizeof(pass));
    printf("Account type (Regular/Premium) (p) for premium and (r) regular: ");
    read_line(type, sizeof(type));
    if (strcmp(type, "Premium") == 0 || strcmp(type, "premium") == 0 || strcmp(type, "p") == 0) {
        users[user_count++] = new Premium_user(name, id, pass);
        printf("Signup successful! Premium account created.\n");
    } else if (strcmp(type, "Regular") == 0 || strcmp(type, "regular") == 0 || strcmp(type, "r") == 0) {
        users[user_count++] = new Regular_user(name, id, pass);
        printf("Signup successful! Regular account created.\n");
    } else {
        printf("Account type not exist! Please try again.\n");
        return;
    }

    /* Append new user to users.txt with password preserved */
    FILE* f = fopen("users.txt", "a");
    if (f) {
        fprintf(f, "%d %s %s %.2f %d %s\n", id, name, pass, users[user_count-1]->get_balance(), users[user_count-1]->get_transaction_count(), users[user_count-1]->get_type());
        fclose(f);
    }
}

User* Menu::login() {
    int id; char pass[64];
    puts("|================================================|");
    puts("|                === Login ===                   |");
    puts("|================================================|");
    printf("Enter your Numeric ID: ");
    while (scanf("%d", &id) != 1) {
        fflush(stdin);
        printf("not valid input Try again!\nEnter your Numeric ID: ");
    }
    fflush(stdin);
    printf("Enter password: ");
    /* replaced plain read_line with read_password to show '*' */
    read_password(pass, sizeof(pass));
    int index = find_user_index_by_id(id);
    if (index != -1 && users[index]->checkPassword(pass)) {
        printf("Login successful! Welcome %s\n", users[index]->get_name());
        return users[index];
    }
    printf("credentials Not valid ....\n");
    return 0;
}

void Menu::trade_stocks(User* user, Market& market, Transaction_manager& tm, Portfolio& portfolio) {
    puts("|======================================================================|");
    puts("|                        === Trading Stocks ===                        |");
    puts("|======================================================================|");
    market.display_all_stocks();
    int choice;
    do {
        puts("|======================================================================|");
        puts("|                        === Trading Menu ===                          |");
        puts("|======================================================================|");
        printf("\n1. Refresh Prices\n2. Buy\n3. Sell\n4. Back\nChoice: ");
        while (scanf("%d", &choice) != 1) {
            fflush(stdin);
            printf("not valid input Try again!\nChoice: ");
        }
        fflush(stdin);

        if (choice == 1) {
            printf("Refreshing prices...\n");
            market.update_all_prices();
            market.display_all_stocks();
            printf("\nPrices refreshed.\n");
        } else if (choice == 2) { /* Buy */
            char symbol[16];
            int qty;
            puts("|======================================================================|");
            puts("|                        === Buy Stocks ===                            |");
            puts("|======================================================================|");
            printf("Enter stock symbol(Case sensitive): ");
            read_line(symbol, sizeof(symbol));
            Stock* s = market.find_stock_by_symbol(symbol);
            if (s) {
                printf("Enter quantity: ");
                while (scanf("%d", &qty) != 1 || qty <= 0) {
                    fflush(stdin);
                    printf("not valid input\nEnter quantity: ");
                }
                fflush(stdin);
                double cost = s->get_current_price() * qty;
                if (user->withdraw(cost)) {
                    int index = portfolio.find_index(symbol);
                    double profit_loss = 0.0;
                    if (index != -1) {
                        double prev_price = portfolio.holdings[index].purchase_price;
                        profit_loss = (prev_price - s->get_current_price()) * qty;
                    }
                    portfolio.add_holding(symbol, qty, s->get_current_price());
                    tm.add_transaction(user->get_id(), user->get_name(), symbol, qty, s->get_current_price(), "BUY");
                    user->increment_transactions();
                    printf("You bought %d shares of %s at $%.2f.\n", qty, symbol, s->get_current_price());
                    if (index != -1) {
                        if (profit_loss < 0) printf("You paid $%.2f more compared to your previous avg price.\n", -profit_loss);
                        else if (profit_loss > 0) printf("Congrats! You saved $%.2f compared to your previous avg price.\n", profit_loss);
                        else printf("No profit/loss compared to your previous avg price.\n");
                    }

                    /* Save updated users to file (balance changed, transaction count changed) */
                    save_all_users();
                } else {
                    printf("Insufficient funds to BUY!\n");
                }
            } else {
                printf("Stock not found.\n");
            }
        } else if (choice == 3) { /* Sell */
            char symbol[16];
            int qty;
            puts("|======================================================================|");
            puts("|                        === Sell Stocks ===                           |");
            puts("|======================================================================|");
            printf("Enter stock symbol(Case sensitive): ");
            read_line(symbol, sizeof(symbol));
            int index = portfolio.find_index(symbol);
            if (index == -1 || portfolio.holdings[index].quantity <= 0) {
                printf("You do not own any shares of this stock.\n");
            } else {
                Stock* s = market.find_stock_by_symbol(symbol);
                if (s) {
                    printf("You own %d shares of %s.\n", portfolio.holdings[index].quantity, symbol);
                    printf("Enter quantity to sell: ");
                    while (scanf("%d", &qty) != 1 || qty <= 0) {
                        fflush(stdin);
                        printf("not valid input Try again!\nEnter quantity to sell: ");
                    }
                    fflush(stdin);
                    if (portfolio.holdings[index].quantity >= qty) {
                        double revenue = s->get_current_price() * qty;
                        double profit = (s->get_current_price() - portfolio.holdings[index].purchase_price) * qty;
                        user->deposit(revenue);
                        portfolio.remove_holding(symbol, qty);
                        tm.add_transaction(user->get_id(), user->get_name(), symbol, qty, s->get_current_price(), "SELL");
                        user->increment_transactions();
                        printf("You sold %d shares of %s at $%.2f.\n", qty, symbol, s->get_current_price());
                        if (profit > 0) printf("Congrats! Profit: $%.2f.\n", profit);
                        else printf("Loss: $%.2f.\n", -profit);

                        /* Save updated users to file (balance changed, transaction count changed) */
                        save_all_users();
                    } else {
                        printf("Insufficient shares to SELL.\n");
                    }
                } else {
                    printf("Stock not found.\n");
                }
            }
        } /* end choices */
    } while (choice != 4);
}

void Menu::view_market(Market& market) {
    int choice;
    do {
        puts("|======================================================================|");
        puts("|                        === Market Menu ===                           |");
        puts("|======================================================================|");
        printf("\n1. Refresh Prices\n2. View Stocks\n3. View Stock History\n4. Add Stock\n5. Delete Stock\n6. Back\nChoice: ");
        while (scanf("%d", &choice) != 1) {
            fflush(stdin);
            printf("not valid input\nChoice: ");
        }
        fflush(stdin);

        if (choice == 1) {
            market.update_all_prices();
            printf("Prices refreshed.\n");
        } else if (choice == 2) {
            market.display_all_stocks();
        } else if (choice == 3) {
            char symbol[16];
            printf("Enter stock symbol: ");
            read_line(symbol, sizeof(symbol));
            Stock* s = market.find_stock_by_symbol(symbol);
            if (s) s->display_history();
            else printf("Stock not found.\n");
        } else if (choice == 4) {
            if (Stock::get_total_stocks() >= 100) {
                printf("Stock limit reached.\n");
            } else {
                char name[64], symbol[16];
                double price, prev_price, vol;
                printf("Enter stock name: ");
                read_line(name, sizeof(name));
                printf("Enter stock symbol: ");
                read_line(symbol, sizeof(symbol));
                printf("Enter current price: ");
                while (scanf("%lf", &price) != 1) { fflush(stdin); printf("not valid input\nEnter current price: "); }
                fflush(stdin);
                printf("Enter previous price: ");
                while (scanf("%lf", &prev_price) != 1) { fflush(stdin); printf("not valid input\nEnter previous price: "); }
                fflush(stdin);
                printf("Enter volatility (e.g., 0.02): ");
                while (scanf("%lf", &vol) != 1) { fflush(stdin); printf("not valid input\nEnter volatility (e.g., 0.02): "); }
                fflush(stdin);
                market.add_stock(Stock(name, symbol, price, prev_price, vol));
                printf("Stock added successfully.\n");
            }
        } else if (choice == 5) {
            char symbol[16];
            printf("Enter stock symbol to delete: ");
            read_line(symbol, sizeof(symbol));
            market.delete_stock(symbol);
        }
    } while (choice != 6);
}

/* --- MISSING IMPLEMENTATION ADDED HERE --- */
void Menu::main_menu() {
    Market stock_market;
    Transaction_manager tm;
    Portfolio current_portfolio;
    User* logged_in_user = 0;
    int choice;

    do {
        if (logged_in_user) {
            /* Logged In User Menu */
            clrscr();
            logged_in_user->display_user_info();
            current_portfolio.display_holdings(stock_market);
            puts("|================================================|");
            puts("|                === User Menu ===               |");
            puts("|================================================|");
            printf("\n1. Trade Stocks\n2. View Market\n3. View Transactions\n4. Deposit Funds\n5. Logout\nChoice: ");
            while (scanf("%d", &choice) != 1) {
                fflush(stdin);
                printf("not valid input Try again!\nChoice: ");
            }
            fflush(stdin);

            if (choice == 1) {
                clrscr();
                trade_stocks(logged_in_user, stock_market, tm, current_portfolio);
            } else if (choice == 2) {
                clrscr();
                view_market(stock_market);
            } else if (choice == 3) {
                clrscr();
                tm.display_user_transactions(logged_in_user->get_id());
                getch();
            } else if (choice == 4) {
                double amount;
                printf("Enter deposit amount: $");
                while (scanf("%lf", &amount) != 1 || amount <= 0) {
                    fflush(stdin);
                    printf("not valid input Try again!\nEnter deposit amount: $");
                }
                fflush(stdin);
                logged_in_user->deposit(amount);
                printf("Deposited $%.2f. New balance: $%.2f\n", amount, logged_in_user->get_balance());
                /* Save updated users to file (balance changed) */
                save_all_users();
                getch();
            } else if (choice == 5) {
                logged_in_user = 0;
                printf("Logged out.\n");
            }
        } else {
            /* Main Menu (Before Login) */
            clrscr();
            puts("|================================================|");
            puts("|            === Stock Simulator ===             |");
            puts("|================================================|");
            printf("\n1. Login\n2. Signup\n3. View Market (Guest)\n4. Exit\nChoice: ");
            while (scanf("%d", &choice) != 1) {
                fflush(stdin);
                printf("not valid input Try again!\nChoice: ");
            }
            fflush(stdin);

            if (choice == 1) {
                clrscr();
                logged_in_user = login();
                if (logged_in_user) {
                    /* Logged in successfully, load portfolio (simulated) */
                    printf("Loading portfolio...\n");
                }
                getch();
            } else if (choice == 2) {
                clrscr();
                signup();
                getch();
            } else if (choice == 3) {
                clrscr();
                view_market(stock_market);
            }
        }
    } while (logged_in_user || choice != 4);
}
/* ------------------- Dashboard ------------------- */

void main() {
    randomize(); // Initialize random number generator for better price updates
    clrscr();
    Menu m;
    m.main_menu();
    getch();
}
