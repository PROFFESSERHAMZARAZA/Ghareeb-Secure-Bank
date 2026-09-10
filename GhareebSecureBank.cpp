/*
    =====================================================================
     PROJECT   : GHAREEB SECURE BANK - Console Bank Management System
     LANGUAGE  : C++ (OOP)
     STUDENT   : PROFFESSER HAMZA RAZA
     CLASS     : B.S-CYS-2B
     PURPOSE   : Semester Project demonstrating OOP concepts:
                 - Abstraction   (abstract base classes, pure virtual funcs)
                 - Encapsulation (private/protected data + getters/setters)
                 - Inheritance   (Person -> Account -> Savings/Current)
                 - Polymorphism  (virtual functions, base class pointers)
    =====================================================================
    HOW TO RUN (Dev-C++ / Code::Blocks on Windows):
      1. Create a new empty C++ project (or just a new source file).
      2. Paste this whole file in as GhareebSecureBank.cpp
      3. Build & Run.
      4. An "accounts.txt" file will be created automatically next to the
         .exe to store account data permanently between runs.
    =====================================================================
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits>
#include <cstdlib>
#include <cctype>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

using namespace std;

// =====================================================================
//  SECTION 1: CONSOLE COLOR + ANIMATION UTILITY
// =====================================================================
namespace UI {

    // Color codes: works on Windows console (SetConsoleTextAttribute)
    // and falls back to ANSI escape codes elsewhere.
    enum Color {
        BLACK = 0, BLUE = 1, GREEN = 2, CYAN = 3, RED = 4,
        MAGENTA = 5, YELLOW = 6, WHITE = 7, GRAY = 8,
        LBLUE = 9, LGREEN = 10, LCYAN = 11, LRED = 12,
        LMAGENTA = 13, LYELLOW = 14, BRIGHT_WHITE = 15
    };

    void setColor(int color) {
#ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, color);
#else
        // ANSI fallback (used when testing on Linux/Mac)
        static const char* codes[] = {
            "\033[30m","\033[34m","\033[32m","\033[36m","\033[31m",
            "\033[35m","\033[33m","\033[37m","\033[90m","\033[94m",
            "\033[92m","\033[96m","\033[91m","\033[95m","\033[93m","\033[97m"
        };
        cout << codes[color % 16];
#endif
    }

    void resetColor() { setColor(WHITE); }

    // Uses the native Windows Sleep() API (no std::thread dependency,
    // which avoids link errors on MinGW builds that use the win32
    // threading model, as shipped with many Dev-C++ installs).
    void sleepMs(int ms) {
#ifdef _WIN32
        Sleep(ms);
#else
        usleep(ms * 1000);
#endif
    }

    // Typing animation effect
    void typeText(const string &text, int color = WHITE, int delayMs = 12) {
        setColor(color);
        for (size_t i = 0; i < text.size(); i++) {
            cout << text[i] << flush;
            sleepMs(delayMs);
        }
        cout << endl;
        resetColor();
    }

    // Simple animated loading bar
    void loadingBar(const string &label, int color = CYAN) {
        setColor(color);
        cout << label;
        cout.flush();
        for (int i = 0; i < 20; i++) {
            sleepMs(35);
            cout << "#" << flush;
        }
        cout << "  Done!\n";
        resetColor();
    }

    void line(char ch = '=', int len = 70, int color = CYAN) {
        setColor(color);
        cout << string(len, ch) << "\n";
        resetColor();
    }

    // Discards leftover characters up to (and including) the next newline.
    // Call this immediately after any `cin >> x;` that will be followed by
    // getline(), OR right before pause(), so pause() always starts clean.
    void flushLine() {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    void pause() {
        cout << "\nPress Enter to continue...";
        cin.get();  // buffer is guaranteed clean at this point, so this
                    // waits for exactly one real Enter press from the user
    }

    void clearScreen() {
#ifdef _WIN32
        system("cls");
#else
        cout << "\n";
#endif
    }
}

// =====================================================================
//  SECTION 2: PERSON  (Abstract base class -> Abstraction)
// =====================================================================
class Person {
protected:
    string name;
    string cnic;
    string phone;
    int age;

public:
    Person() : age(0) {}
    Person(string n, string c, string p, int a)
        : name(n), cnic(c), phone(p), age(a) {}

    virtual ~Person() {}

    // ---- Encapsulation: controlled access via getters/setters ----
    string getName()  const { return name; }
    string getCNIC()  const { return cnic; }
    string getPhone() const { return phone; }
    int    getAge()   const { return age; }

    void setName(string n)  { name = n; }
    void setPhone(string p) { phone = p; }

    // ---- Abstraction: every derived class must define this ----
    virtual void displayInfo() const = 0;
};

// =====================================================================
//  SECTION 3: ACCOUNT (Abstract, inherits Person -> Inheritance)
// =====================================================================
class Account : public Person {
protected:
    string accountNumber;
    string password;
    double balance;
    string accountType;

public:
    Account() : balance(0) {}
    Account(string n, string c, string p, int a,
            string accNo, string pass, double bal, string type)
        : Person(n, c, p, a), accountNumber(accNo), password(pass),
          balance(bal), accountType(type) {}

    virtual ~Account() {}

    string getAccountNumber() const { return accountNumber; }
    string getAccountType()   const { return accountType; }
    string getPassword()      const { return password; }
    double getBalance()       const { return balance; }

    void setPassword(string p) { password = p; }
    bool checkPassword(const string &p) const { return password == p; }

    void deposit(double amount) { balance += amount; }

    // ---- Polymorphism: each account type withdraws differently ----
    virtual bool withdraw(double amount) = 0;

    // ---- Polymorphism: each account type earns interest differently
    virtual double calculateInterest() const = 0;

    // ---- Overridden from Person (Abstraction/Polymorphism) ----
    void displayInfo() const {
        UI::line('-', 60, UI::CYAN);
        UI::setColor(UI::LYELLOW);
        cout << " Account Number : " << accountNumber << "\n";
        UI::setColor(UI::LGREEN);
        cout << " Account Type   : " << accountType << "\n";
        UI::setColor(UI::WHITE);
        cout << " Holder Name    : " << name << "\n";
        cout << " CNIC           : " << cnic << "\n";
        cout << " Phone Number   : " << phone << "\n";
        cout << " Age            : " << age << "\n";
        UI::setColor(UI::LMAGENTA);
        cout << " Balance        : Rs. " << fixed << setprecision(2) << balance << "\n";
        UI::resetColor();
        UI::line('-', 60, UI::CYAN);
    }

    // For saving to file: "Type|Name|CNIC|Phone|Age|AccNo|Password|Balance"
    virtual string serialize() const {
        ostringstream oss;
        oss << accountType << "|" << name << "|" << cnic << "|" << phone << "|"
            << age << "|" << accountNumber << "|" << password << "|" << balance;
        return oss.str();
    }
};

// =====================================================================
//  SECTION 4: SAVINGS ACCOUNT (concrete, inherits Account)
// =====================================================================
class SavingsAccount : public Account {
private:
    double interestRate;      // 5% annual interest
    double minimumBalance;    // must keep Rs.1000 minimum

public:
    SavingsAccount() : interestRate(0.05), minimumBalance(1000.0) {}
    SavingsAccount(string n, string c, string p, int a,
                    string accNo, string pass, double bal)
        : Account(n, c, p, a, accNo, pass, bal, "Savings"),
          interestRate(0.05), minimumBalance(1000.0) {}

    bool withdraw(double amount) {
        if (amount <= 0) return false;
        if (balance - amount < minimumBalance) return false; // rule: keep min balance
        balance -= amount;
        return true;
    }

    double calculateInterest() const {
        return balance * interestRate;
    }
};

// =====================================================================
//  SECTION 5: CURRENT ACCOUNT (concrete, inherits Account)
// =====================================================================
class CurrentAccount : public Account {
private:
    double overdraftLimit; // allowed to go negative up to this

public:
    CurrentAccount() : overdraftLimit(50000.0) {}
    CurrentAccount(string n, string c, string p, int a,
                    string accNo, string pass, double bal)
        : Account(n, c, p, a, accNo, pass, bal, "Current"),
          overdraftLimit(50000.0) {}

    bool withdraw(double amount) {
        if (amount <= 0) return false;
        if (balance - amount < -overdraftLimit) return false; // overdraft rule
        balance -= amount;
        return true;
    }

    double calculateInterest() const {
        return 0.0; // current accounts earn no interest
    }
};

// =====================================================================
//  SECTION 6: BANK (Manager class - composition, file I/O)
// =====================================================================
class Bank {
private:
    vector<Account*> accounts;
    string fileName;
    int nextAccNo;

    string generateAccountNumber() {
        ostringstream oss;
        oss << "GSB" << nextAccNo++;
        return oss.str();
    }

    // ---------------- Validation helpers ----------------
    bool isAllDigits(const string &s) {
        if (s.empty()) return false;
        for (size_t i = 0; i < s.size(); i++) if (!isdigit((unsigned char)s[i])) return false;
        return true;
    }

    bool validCNIC(const string &cnic) {
        // Accept 13 digit CNIC, with or without dashes (13 digits total)
        string digitsOnly;
        for (size_t i = 0; i < cnic.size(); i++) if (isdigit((unsigned char)cnic[i])) digitsOnly += cnic[i];
        return digitsOnly.length() == 13;
    }

    bool validPhone(const string &phone) {
        string digitsOnly;
        for (size_t i = 0; i < phone.size(); i++) if (isdigit((unsigned char)phone[i])) digitsOnly += phone[i];
        return digitsOnly.length() >= 10 && digitsOnly.length() <= 11;
    }

public:
    Bank() : fileName("accounts.txt"), nextAccNo(1001) { loadFromFile(); }

    ~Bank() {
        saveToFile();
        for (size_t i = 0; i < accounts.size(); i++) delete accounts[i];
    }

    // ---------------- File Persistence ----------------
    void loadFromFile() {
        ifstream fin(fileName.c_str());
        if (!fin.is_open()) return;

        string line;
        int maxSeen = 1000;
        while (getline(fin, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string type, name, cnic, phone, ageStr, accNo, pass, balStr;
            getline(ss, type, '|');
            getline(ss, name, '|');
            getline(ss, cnic, '|');
            getline(ss, phone, '|');
            getline(ss, ageStr, '|');
            getline(ss, accNo, '|');
            getline(ss, pass, '|');
            getline(ss, balStr, '|');

            int age = atoi(ageStr.c_str());
            double bal = atof(balStr.c_str());

            Account* acc = NULL;
            if (type == "Savings")
                acc = new SavingsAccount(name, cnic, phone, age, accNo, pass, bal);
            else if (type == "Current")
                acc = new CurrentAccount(name, cnic, phone, age, accNo, pass, bal);

            if (acc) {
                accounts.push_back(acc);
                string numPart = accNo.substr(3); // strip "GSB"
                if (!numPart.empty() && isAllDigits(numPart)) {
                    int n = atoi(numPart.c_str());
                    if (n >= maxSeen) maxSeen = n + 1;
                }
            }
        }
        nextAccNo = maxSeen;
        fin.close();
    }

    void saveToFile() {
        ofstream fout(fileName.c_str(), ios::trunc);
        for (size_t i = 0; i < accounts.size(); i++) {
            fout << accounts[i]->serialize() << "\n";
        }
        fout.close();
    }

    // ---------------- Core Banking Operations ----------------
    void createAccount() {
        UI::clearScreen();
        UI::line('=', 60, UI::LYELLOW);
        UI::typeText("           CREATE A NEW ACCOUNT", UI::LYELLOW, 8);
        UI::line('=', 60, UI::LYELLOW);

        string name, cnic, phone, pass, typeChoice;
        int age;

        UI::setColor(UI::WHITE);
        cout << "Enter Full Name        : ";
        getline(cin, name);

        cout << "Enter CNIC (13 digits)  : ";
        getline(cin, cnic);
        if (!validCNIC(cnic)) {
            UI::setColor(UI::LRED);
            cout << "\nInvalid CNIC! CNIC must contain 13 digits.\n";
            UI::resetColor();
            UI::pause();
            return;
        }

        cout << "Enter Phone Number      : ";
        getline(cin, phone);
        if (!validPhone(phone)) {
            UI::setColor(UI::LRED);
            cout << "\nInvalid Phone Number! Must be 10-11 digits.\n";
            UI::resetColor();
            UI::pause();
            return;
        }

        cout << "Enter Age               : ";
        cin >> age;
        UI::flushLine();
        if (age <= 18) {
            UI::setColor(UI::LRED);
            cout << "\nAge must be greater than 18 to open an account!\n";
            UI::resetColor();
            UI::pause();
            return;
        }

        cout << "Set Account Password    : ";
        getline(cin, pass);

        cout << "\nSelect Account Type:\n";
        cout << " 1. Savings Account (5% interest, Rs.1000 min balance)\n";
        cout << " 2. Current Account (overdraft up to Rs.50,000)\n";
        cout << "Enter choice (1/2)      : ";
        getline(cin, typeChoice);

        double openingBalance = 0;
        cout << "Enter Opening Deposit   : ";
        cin >> openingBalance;
        UI::flushLine();

        string accNo = generateAccountNumber();
        Account* acc = NULL;

        if (typeChoice == "2")
            acc = new CurrentAccount(name, cnic, phone, age, accNo, pass, openingBalance);
        else
            acc = new SavingsAccount(name, cnic, phone, age, accNo, pass, openingBalance);

        accounts.push_back(acc);
        saveToFile();

        UI::loadingBar("\nCreating your account ", UI::LGREEN);
        UI::setColor(UI::LGREEN);
        cout << "\nAccount created successfully!\n";
        cout << "Your Account Number is: " << accNo << "  (please note it down)\n";
        UI::resetColor();
        UI::pause();
    }

    void viewAllAccounts() {
        UI::clearScreen();
        UI::line('=', 70, UI::LCYAN);
        UI::typeText("               ALL BANK ACCOUNTS", UI::LCYAN, 8);
        UI::line('=', 70, UI::LCYAN);

        if (accounts.empty()) {
            UI::setColor(UI::LRED);
            cout << "\nNo accounts found in the bank yet.\n";
            UI::resetColor();
            UI::pause();
            return;
        }

        UI::setColor(UI::LYELLOW);
        cout << left << setw(12) << "AccNo" << setw(20) << "Name"
             << setw(10) << "Type" << setw(15) << "Balance" << "\n";
        UI::line('-', 70, UI::GRAY);

        for (size_t i = 0; i < accounts.size(); i++) {
            Account* a = accounts[i];
            UI::setColor(UI::WHITE);
            cout << left << setw(12) << a->getAccountNumber()
                 << setw(20) << a->getName()
                 << setw(10) << a->getAccountType()
                 << "Rs. " << fixed << setprecision(2) << a->getBalance() << "\n";
        }
        UI::resetColor();
        UI::pause();
    }

    Account* findAccount(const string &accNo) {
        for (size_t i = 0; i < accounts.size(); i++)
            if (accounts[i]->getAccountNumber() == accNo) return accounts[i];
        return NULL;
    }

    void displayFullAccount() {
        UI::clearScreen();
        UI::line('=', 60, UI::LMAGENTA);
        UI::typeText("            DISPLAY FULL ACCOUNT", UI::LMAGENTA, 8);
        UI::line('=', 60, UI::LMAGENTA);

        string accNo, pass;
        cout << "Enter Account Number : ";
        getline(cin, accNo);
        cout << "Enter Password       : ";
        getline(cin, pass);

        Account* acc = findAccount(accNo);
        if (!acc || !acc->checkPassword(pass)) {
            UI::setColor(UI::LRED);
            cout << "\nAccount not found or incorrect password!\n";
            UI::resetColor();
            UI::pause();
            return;
        }

        cout << "\n";
        acc->displayInfo();
        UI::setColor(UI::LCYAN);
        cout << " Est. Annual Interest : Rs. " << fixed << setprecision(2)
             << acc->calculateInterest() << "\n";
        UI::resetColor();
        UI::pause();
    }

    void depositMoney() {
        UI::clearScreen();
        UI::line('=', 60, UI::LGREEN);
        UI::typeText("               DEPOSIT MONEY", UI::LGREEN, 8);
        UI::line('=', 60, UI::LGREEN);

        string accNo;
        cout << "Enter Account Number : ";
        getline(cin, accNo);

        Account* acc = findAccount(accNo);
        if (!acc) {
            UI::setColor(UI::LRED);
            cout << "\nAccount not found!\n";
            UI::resetColor();
            UI::pause();
            return;
        }

        double amount;
        cout << "Enter Amount to Deposit : ";
        cin >> amount;
        UI::flushLine();

        if (amount <= 0) {
            UI::setColor(UI::LRED);
            cout << "\nInvalid amount!\n";
            UI::resetColor();
            UI::pause();
            return;
        }

        acc->deposit(amount);
        saveToFile();

        UI::loadingBar("\nProcessing deposit ", UI::LGREEN);
        UI::setColor(UI::LGREEN);
        cout << "Deposit successful! New Balance: Rs. "
             << fixed << setprecision(2) << acc->getBalance() << "\n";
        UI::resetColor();
        UI::pause();
    }

    void withdrawMoney() {
        UI::clearScreen();
        UI::line('=', 60, UI::LRED);
        UI::typeText("               WITHDRAW MONEY", UI::LRED, 8);
        UI::line('=', 60, UI::LRED);

        string accNo, pass;
        cout << "Enter Account Number : ";
        getline(cin, accNo);
        cout << "Enter Password       : ";
        getline(cin, pass);

        Account* acc = findAccount(accNo);
        if (!acc || !acc->checkPassword(pass)) {
            UI::setColor(UI::LRED);
            cout << "\nAccount not found or incorrect password!\n";
            UI::resetColor();
            UI::pause();
            return;
        }

        double amount;
        cout << "Enter Amount to Withdraw : ";
        cin >> amount;
        UI::flushLine();

        // Polymorphic call: SavingsAccount and CurrentAccount enforce
        // their own withdrawal rules here.
        if (acc->withdraw(amount)) {
            saveToFile();
            UI::loadingBar("\nProcessing withdrawal ", UI::LYELLOW);
            UI::setColor(UI::LGREEN);
            cout << "Withdrawal successful! New Balance: Rs. "
                 << fixed << setprecision(2) << acc->getBalance() << "\n";
        } else {
            UI::setColor(UI::LRED);
            cout << "\nWithdrawal failed! Check amount / account limit rules.\n";
        }
        UI::resetColor();
        UI::pause();
    }

    void changePassword() {
        UI::clearScreen();
        UI::line('=', 60, UI::LBLUE);
        UI::typeText("              CHANGE PASSWORD", UI::LBLUE, 8);
        UI::line('=', 60, UI::LBLUE);

        string accNo, oldPass, newPass;
        cout << "Enter Account Number  : ";
        getline(cin, accNo);
        cout << "Enter Current Password: ";
        getline(cin, oldPass);

        Account* acc = findAccount(accNo);
        if (!acc || !acc->checkPassword(oldPass)) {
            UI::setColor(UI::LRED);
            cout << "\nAccount not found or incorrect password!\n";
            UI::resetColor();
            UI::pause();
            return;
        }

        cout << "Enter New Password    : ";
        getline(cin, newPass);
        acc->setPassword(newPass);
        saveToFile();

        UI::setColor(UI::LGREEN);
        cout << "\nPassword changed successfully!\n";
        UI::resetColor();
        UI::pause();
    }
};

// =====================================================================
//  SECTION 7: INTRO BANNER
// =====================================================================
void showIntro() {
    UI::clearScreen();
    UI::loadingBar("Loading Ghareeb Secure Bank ", UI::LCYAN);
    UI::sleepMs(200);
    UI::clearScreen();

    UI::line('=', 70, UI::LCYAN);
    UI::typeText("               THE BANK OF PROFFESSER", UI::LYELLOW, 10);
    UI::line('=', 70, UI::LCYAN);
    cout << "\n";
    UI::typeText("        G H A R E E B   S E C U R E   B A N K", UI::LGREEN, 12);
    cout << "\n";
    UI::line('-', 70, UI::GRAY);
    UI::setColor(UI::LMAGENTA);
    cout << "               PROJECT CREATOR: PROFFESSER HAMZA RAZA\n";
    UI::resetColor();
    UI::line('-', 70, UI::GRAY);
}

void showMenu() {
    cout << "\n";
    UI::line('-', 40, UI::LCYAN);
    UI::setColor(UI::LCYAN);
    cout << "            BANK MENU\n";
    UI::line('-', 40, UI::LCYAN);
    UI::setColor(UI::WHITE);
    cout << "1. Create Account\n";
    cout << "2. View All Accounts\n";
    cout << "3. Display Full Account\n";
    cout << "4. Deposit Money\n";
    cout << "5. Withdraw Money\n";
    cout << "6. Change Password\n";
    cout << "7. Exit\n";
    UI::resetColor();
    cout << "Enter Your Choice: ";
}

// =====================================================================
//  SECTION 8: MAIN
// =====================================================================
int main() {
    showIntro();
    Bank bank;

    int choice;
    do {
        showMenu();
        cin >> choice;

        if (cin.eof()) {
            // Input stream closed unexpectedly - exit gracefully instead of looping forever
            choice = 7;
            break;
        }
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = 0;
        } else {
            UI::flushLine(); // discard trailing newline left after reading choice
        }

        UI::clearScreen();
        switch (choice) {
            case 1: bank.createAccount();     break;
            case 2: bank.viewAllAccounts();   break;
            case 3: bank.displayFullAccount();break;
            case 4: bank.depositMoney();      break;
            case 5: bank.withdrawMoney();     break;
            case 6: bank.changePassword();    break;
            case 7:
                UI::setColor(UI::LYELLOW);
                UI::typeText("\nThank you for banking with Ghareeb Secure Bank!", UI::LYELLOW, 10);
                UI::resetColor();
                break;
            default:
                UI::setColor(UI::LRED);
                cout << "Invalid choice! Please try again.\n";
                UI::resetColor();
                UI::pause();
        }
    } while (choice != 7);

    return 0;
}
