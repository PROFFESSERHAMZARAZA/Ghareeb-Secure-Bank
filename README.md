<div align="center">

# 🏦 Ghareeb Secure Bank

### A Console-Based Bank Management System in C++ | OOP Semester Project

[![Language](https://img.shields.io/badge/Language-C%2B%2B-00599C?style=flat-square&logo=c%2B%2B)](https://isocpp.org/)
[![Standard](https://img.shields.io/badge/Standard-C%2B%2B11%2F98-blue?style=flat-square)](https://en.cppreference.com/)
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-lightgrey?style=flat-square)](#-getting-started)
[![License](https://img.shields.io/badge/License-MIT-green?style=flat-square)](LICENSE)
[![Status](https://img.shields.io/badge/Status-Completed-brightgreen?style=flat-square)]()

*A fully working, file-persistent bank management system built to demonstrate the four pillars of Object-Oriented Programming — Abstraction, Encapsulation, Inheritance, and Polymorphism — in a colorful, animated console interface.*

[Features](#-features) •
[Screenshots](#-screenshots) •
[Class Diagram](#-class-diagram--architecture) •
[OOP Concepts](#-oop-concepts-explained) •
[Getting Started](#-getting-started) •
[Project Structure](#-project-structure)

</div>

---

## 📖 Overview

**Ghareeb Secure Bank** is a console-based banking system written in C++ for a Cyber Security semester project. It simulates real banking operations — account creation, deposits, withdrawals, balance inquiries, and password management — while enforcing real-world validation rules (a valid 13-digit CNIC, a valid phone number, and a minimum age of 18 for every new account holder).

The project was built specifically to demonstrate a clean, practical application of Object-Oriented Programming in C++, backed by permanent file-based storage so account data survives between runs.

> 📄 A full written report covering the project's objectives, class diagram, and OOP concept breakdown is available at [`docs/Project_Report.docx`](docs/Project_Report.docx).

---

## ✨ Features

- 🔐 **Secure account creation** — validates CNIC (13 digits), phone number (10–11 digits), and age (must be > 18)
- 💰 **Two account types** — Savings (5% annual interest, Rs. 1,000 minimum balance) and Current (Rs. 50,000 overdraft limit)
- 📋 **Full banking menu** — create account, view all accounts, display full account details, deposit, withdraw, change password
- 💾 **Persistent storage** — every account is saved to `accounts.txt` and reloaded automatically on the next run
- 🎨 **Colorful animated console UI** — typing animation, loading bars, and color-coded menus/messages
- 🧱 **Clean OOP architecture** — abstract base classes, inheritance hierarchy, encapsulated data, and polymorphic behavior throughout
- ✅ **Portable code** — compiles cleanly on both Windows (Dev-C++ / Code::Blocks) and Linux/macOS (g++), C++98/C++11 compatible

---

## 🖼️ Screenshots

<div align="center">

**Main Menu**

<img src="docs/screenshots/01_main_menu.png" width="620" alt="Main Menu">

**Creating a New Account**

<img src="docs/screenshots/02_create_account.png" width="620" alt="Create Account">

**Viewing All Accounts**

<img src="docs/screenshots/03_view_accounts.png" width="620" alt="View All Accounts">

</div>

---

## 🧩 Class Diagram & Architecture

<div align="center">
<img src="docs/class_diagram.png" width="620" alt="Class Diagram">
</div>

The system is built around a clean **three-level inheritance hierarchy**:

```
Person (abstract)
   └── Account (abstract)
          ├── SavingsAccount
          └── CurrentAccount

Bank  →  manages a vector<Account*>  (composition)
```

| Class | Role |
|---|---|
| `Person` | Abstract base class holding identity data (`name`, `cnic`, `phone`, `age`) |
| `Account` | Abstract class holding banking data (`accountNumber`, `balance`, `password`) and declaring the operations every account must support |
| `SavingsAccount` | Concrete class — 5% annual interest, enforces a Rs. 1,000 minimum balance |
| `CurrentAccount` | Concrete class — no interest, allows overdraft up to Rs. 50,000 |
| `Bank` | Manages the collection of accounts, handles all menu operations and file persistence |

---

## 🎯 OOP Concepts Explained

Every one of the four core Object-Oriented Programming principles is deliberately and concretely implemented in this project:

### 1️⃣ Abstraction
`Person` and `Account` are **abstract classes** — they declare pure virtual functions and can never be instantiated directly.

```cpp
class Account : public Person {
public:
    virtual bool withdraw(double amount) = 0;      // must be implemented
    virtual double calculateInterest() const = 0;   // by every derived class
};
```

### 2️⃣ Encapsulation
All sensitive data (`password`, `balance`, `cnic`, etc.) is kept `private`/`protected` and only reachable through controlled public methods.

```cpp
private:
    string password;
public:
    bool checkPassword(const string &p) const { return password == p; }
    void setPassword(string p) { password = p; }
    // password can never be read directly from outside the class
```

### 3️⃣ Inheritance
`Account` inherits from `Person`, and `SavingsAccount` / `CurrentAccount` both inherit from `Account`, reusing and extending shared behavior at every level.

```cpp
class Person          { /* name, cnic, phone, age            */ };
class Account          : public Person  { /* balance, password ... */ };
class SavingsAccount   : public Account { /* interestRate           */ };
class CurrentAccount   : public Account { /* overdraftLimit         */ };
```

### 4️⃣ Polymorphism
`withdraw()` and `calculateInterest()` behave differently for each account type, and the correct version is selected **automatically at runtime** through a base-class pointer.

```cpp
bool SavingsAccount::withdraw(double amount) {
    if (balance - amount < minimumBalance) return false;
    balance -= amount; return true;
}

bool CurrentAccount::withdraw(double amount) {
    if (balance - amount < -overdraftLimit) return false;
    balance -= amount; return true;
}

// Bank.cpp — the Bank class never needs to know the exact derived type:
Account* acc = findAccount(accNo);
acc->withdraw(amount);   // correct rule applied automatically
```

---

## 📋 Account Opening Rules

| Rule | Requirement |
|---|---|
| CNIC | Must contain exactly 13 digits (dashes optional) |
| Phone Number | Must contain 10–11 digits |
| Age | Must be strictly greater than 18 |

If any rule fails, account creation is rejected with a clear error message.

---

## 🚀 Getting Started

### Option A — Windows (Dev-C++ / Code::Blocks)

1. Download or clone this repository.
2. Open **`GhareebSecureBank.cpp`** in Dev-C++ or Code::Blocks.
3. Click **Compile & Run** (F11 in Dev-C++).
4. An `accounts.txt` file is created automatically next to the `.exe` to store account data permanently.

### Option B — Linux / macOS (g++)

```bash
git clone https://github.com/<your-username>/Ghareeb-Secure-Bank.git
cd Ghareeb-Secure-Bank
g++ -std=c++11 -o GhareebSecureBank GhareebSecureBank.cpp
./GhareebSecureBank
```

> The code is written to be portable: console colors use the Windows API (`windows.h`) when compiled on Windows, and fall back to ANSI escape codes elsewhere, so it builds cleanly on any platform.

---

## 📁 Project Structure

```
Ghareeb-Secure-Bank/
├── GhareebSecureBank.cpp     # Complete source code (single file)
├── docs/
│   ├── class_diagram.png     # Class hierarchy diagram
│   ├── Project_Report.docx   # Full semester project report
│   └── screenshots/          # Console UI screenshots
├── LICENSE
└── README.md
```

---

## 🛠️ Tech Stack

- **Language:** C++ (C++98 / C++11 compatible)
- **Paradigm:** Object-Oriented Programming
- **Storage:** Flat-file persistence (`accounts.txt`)
- **Compiler:** GCC / MinGW (Dev-C++, Code::Blocks, or any standard g++ toolchain)

---

## 🔮 Future Enhancements

- [ ] Encrypt passwords instead of storing them as plain text
- [ ] Add a transaction history log per account
- [ ] Add admin login and account deletion
- [ ] Migrate storage from flat file to SQLite
- [ ] Build a GUI version using Qt

---

## 👤 Author

**PROFFESSER HAMZA RAZA**
BS Cyber Security — Air University, Kamra Campus
Class: B.S-CYS-2B

---

## 📜 License

This project is licensed under the [MIT License](LICENSE) — free to use, modify, and distribute for learning purposes.

---

<div align="center">

*⭐ If you found this project helpful, consider giving it a star!*

</div>
