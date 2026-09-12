// header files
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <cmath>
#include <new>
#include <utility>

using namespace std;

// ---------- Constants ----------
const double MIN_BALANCE = 500.0;

// ---------- Custom exception for EOF on stdin ----------
class InputClosedException : public std::runtime_error {
public:
    InputClosedException() : std::runtime_error("Input stream closed") {}
};

// ---------- Money formatter (safe against NaN/Inf) ----------
string formatMoney(double amount) {
    if (!std::isfinite(amount)) return "0.00";
    stringstream ss;
    ss << fixed << setprecision(2) << amount;
    return ss.str();
}

// ---------- Account class ----------
class Account {
public:
    int accNo;
    string name;
    double balance;
    string pin;
    vector<string> history;

    Account() : accNo(0), balance(0.0) {}

    void create(int no, const string& n, double bal, const string& p) {
        accNo = no;
        name = n;
        balance = bal;
        pin = p;
        history.clear();
        history.push_back("Account created with balance " + formatMoney(balance));
    }

    void addHistory(const string& entry) {
        history.push_back(entry);
    }

    // Build the history string BEFORE mutating balance, so a bad_alloc
    // cannot leave the account half-updated.
    bool deposit(double amount) {
        if (!std::isfinite(amount) || amount <= 0) return false;

        double newBal = balance + amount;
        if (!std::isfinite(newBal)) return false;

        string entry = "Deposited " + formatMoney(amount) +
                       ", New Balance: " + formatMoney(newBal);

        history.reserve(history.size() + 1);   // may throw -> no state change
        balance = newBal;
        history.push_back(std::move(entry));
        return true;
    }

    bool withdraw(double amount) {
        if (!std::isfinite(amount) || amount <= 0) return false;
        if (balance - amount < MIN_BALANCE) return false;

        double newBal = balance - amount;

        string entry = "Withdrawn " + formatMoney(amount) +
                       ", New Balance: " + formatMoney(newBal);

        history.reserve(history.size() + 1);
        balance = newBal;
        history.push_back(std::move(entry));
        return true;
    }

    bool transfer(Account& to, double amount) {
        if (!std::isfinite(amount) || amount <= 0) return false;
        if (balance - amount < MIN_BALANCE) return false;

        double newFrom = balance - amount;
        double newTo   = to.balance + amount;
        if (!std::isfinite(newFrom) || !std::isfinite(newTo)) return false;

        string entryFrom = "Transferred " + formatMoney(amount) +
                           " to A/C " + to_string(to.accNo) +
                           ", New Balance: " + formatMoney(newFrom);
        string entryTo   = "Received " + formatMoney(amount) +
                           " from A/C " + to_string(accNo) +
                           ", New Balance: " + formatMoney(newTo);

        history.reserve(history.size() + 1);
        to.history.reserve(to.history.size() + 1);

        balance = newFrom;
        to.balance = newTo;
        history.push_back(std::move(entryFrom));
        to.history.push_back(std::move(entryTo));
        return true;
    }

    void display() const {
        cout << left << setw(15) << "Account No" << ": " << accNo << '\n';
        cout << left << setw(15) << "Name"       << ": " << name  << '\n';
        cout << left << setw(15) << "Balance"    << ": " << formatMoney(balance) << '\n';
    }

    void displayHistory() const {
        if (history.empty()) {
            cout << "No transactions found.\n";
            return;
        }
        cout << "\nTransaction History for A/C " << accNo << ":\n";
        for (size_t i = 0; i < history.size(); ++i) {
            cout << (i + 1) << ". " << history[i] << '\n';
        }
    }
};

vector<Account> accounts;

// ---------- Input helpers ----------
void checkStream() {
    if (cin.eof()) throw InputClosedException();
}

int readInt(const string& prompt) {
    while (true) {
        checkStream();
        cout << prompt;
        int value;
        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        if (cin.eof()) throw InputClosedException();
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input! Please enter a whole number.\n";
    }
}

double readDouble(const string& prompt) {
    while (true) {
        checkStream();
        cout << prompt;
        double value;
        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (!std::isfinite(value)) {
                cout << "Invalid amount (nan/inf not allowed)!\n";
                continue;
            }
            return value;
        }
        if (cin.eof()) throw InputClosedException();
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input! Please enter a numeric amount.\n";
    }
}

string readLine(const string& prompt) {
    checkStream();
    cout << prompt;
    string value;
    if (!getline(cin, value)) {
        if (cin.eof()) throw InputClosedException();
        cin.clear();
        return "";
    }
    return value;
}

char readChar(const string& prompt) {
    while (true) {
        checkStream();
        cout << prompt;
        char c;
        if (cin >> c) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return c;
        }
        if (cin.eof()) throw InputClosedException();
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input!\n";
    }
}

string readPin(const string& prompt) {
    while (true) {
        checkStream();
        cout << prompt;
        string p;
        if (!getline(cin, p)) {
            if (cin.eof()) throw InputClosedException();
            cin.clear();
            continue;
        }
        if (p.length() != 4) {
            cout << "PIN must be exactly 4 digits.\n";
            continue;
        }
        bool ok = true;
        for (char c : p) {
            if (!isdigit(static_cast<unsigned char>(c))) { ok = false; break; }
        }
        if (!ok) {
            cout << "PIN must contain digits only.\n";
            continue;
        }
        return p;
    }
}

// ---------- Validation helpers ----------
bool isValidName(const string& name) {
    if (name.empty()) return false;
    if (name.find('|') != string::npos) return false;

    bool hasLetter = false;
    for (char c : name) {
        if (isalpha(static_cast<unsigned char>(c))) hasLetter = true;
        else if (c != ' ') return false;
    }
    return hasLetter;
}

int findAccount(int accNo) {
    for (size_t i = 0; i < accounts.size(); ++i) {
        if (accounts[i].accNo == accNo) return static_cast<int>(i);
    }
    return -1;
}

bool verifyPin(int idx) {
    if (idx < 0 || idx >= static_cast<int>(accounts.size())) return false;
    string entered = readPin("Enter PIN: ");
    if (accounts[idx].pin == entered) return true;
    cout << "Incorrect PIN!\n";
    return false;
}

string toLower(const string& s) {
    string result = s;
    transform(result.begin(), result.end(), result.begin(),
              [](unsigned char c) { return static_cast<char>(tolower(c)); });
    return result;
}

// ---------- File operations ----------
bool saveToFile(const string& filename) noexcept {
    try {
        ofstream out(filename, ios::trunc);
        if (!out) return false;

        for (const auto& acc : accounts) {
            out << acc.accNo << '|'
                << acc.name << '|'
                << setprecision(15) << acc.balance << '|'
                << acc.pin << '\n';
            if (!out.good()) return false;
        }
        out.flush();
        return out.good();
    } catch (...) {
        return false;
    }
}

bool loadFromFile(const string& filename) noexcept {
    try {
        ifstream in(filename);
        if (!in) return false;

        vector<Account> temp;
        string line;

        while (getline(in, line)) {
            // Strip Windows-style '\r' so the last field parses correctly
            if (!line.empty() && line.back() == '\r') line.pop_back();
            if (line.empty()) continue;

            stringstream ss(line);
            string accNoStr, name, balStr, pin;

            if (!getline(ss, accNoStr, '|')) continue;
            if (!getline(ss, name,     '|')) continue;
            if (!getline(ss, balStr,   '|')) continue;
            if (!getline(ss, pin,      '|')) continue;

            int accNo = 0;
            double balance = 0.0;

            try {
                size_t pos = 0;
                accNo = stoi(accNoStr, &pos);
                if (pos != accNoStr.size()) continue;

                pos = 0;
                balance = stod(balStr, &pos);
                if (pos != balStr.size()) continue;
            }
            catch (const std::invalid_argument&) { continue; }
            catch (const std::out_of_range&)     { continue; }
            catch (...)                          { continue; }

            if (accNo <= 0) continue;
            if (!std::isfinite(balance)) continue;
            if (balance < MIN_BALANCE) continue;

            if (pin.length() != 4) continue;
            bool pinOk = true;
            for (char c : pin) {
                if (!isdigit(static_cast<unsigned char>(c))) { pinOk = false; break; }
            }
            if (!pinOk) continue;

            if (!isValidName(name)) continue;

            bool dup = false;
            for (const auto& a : temp) {
                if (a.accNo == accNo) { dup = true; break; }
            }
            if (dup) continue;

            Account acc;
            acc.accNo   = accNo;
            acc.name    = name;
            acc.balance = balance;
            acc.pin     = pin;
            temp.push_back(std::move(acc));
        }

        // Don't wipe existing accounts if the file had nothing usable
        if (temp.empty()) return false;

        accounts = std::move(temp);
        return true;
    } catch (...) {
        return false;
    }
}

// ---------- Menu actions ----------
void createAccount() {
    int no = readInt("Enter new Account Number: ");
    if (no <= 0) {
        cout << "Account number must be positive.\n";
        return;
    }

    if (findAccount(no) != -1) {
        cout << "Account number already exists!\n";
        return;
    }

    string name = readLine("Enter Name: ");
    if (!isValidName(name)) {
        cout << "Invalid name! Use letters and spaces only.\n";
        return;
    }

    double initial = readDouble("Enter Initial Deposit (minimum " +
                                formatMoney(MIN_BALANCE) + "): ");
    if (initial < MIN_BALANCE) {
        cout << "Initial deposit must be at least " << formatMoney(MIN_BALANCE) << ".\n";
        return;
    }

    string pin = readPin("Set a 4-digit PIN: ");

    Account acc;
    acc.create(no, name, initial, pin);

    try {
        accounts.push_back(std::move(acc));
    } catch (const std::bad_alloc&) {
        cout << "Memory error: could not create account.\n";
        return;
    } catch (const std::exception& e) {
        cout << "Error creating account: " << e.what() << '\n';
        return;
    }
    cout << "Account created successfully!\n";
}

void depositMoney() {
    int no = readInt("Enter Account Number: ");
    int idx = findAccount(no);
    if (idx == -1) { cout << "Account not found!\n"; return; }
    if (!verifyPin(idx)) return;

    double amount = readDouble("Enter amount to deposit: ");
    if (accounts[idx].deposit(amount)) {
        cout << "Deposit successful! New Balance: "
             << formatMoney(accounts[idx].balance) << '\n';
    } else {
        cout << "Deposit failed! Amount must be positive and finite.\n";
    }
}

void withdrawMoney() {
    int no = readInt("Enter Account Number: ");
    int idx = findAccount(no);
    if (idx == -1) { cout << "Account not found!\n"; return; }
    if (!verifyPin(idx)) return;

    double amount = readDouble("Enter amount to withdraw: ");
    if (accounts[idx].withdraw(amount)) {
        cout << "Withdrawal successful! New Balance: "
             << formatMoney(accounts[idx].balance) << '\n';
    } else {
        cout << "Withdrawal failed! Check amount or minimum balance ("
             << formatMoney(MIN_BALANCE) << ").\n";
    }
}

void checkBalance() {
    int no = readInt("Enter Account Number: ");
    int idx = findAccount(no);
    if (idx == -1) { cout << "Account not found!\n"; return; }
    if (!verifyPin(idx)) return;
    accounts[idx].display();
}

void transferMoney() {
    int fromNo = readInt("Enter From Account Number: ");
    int fromIdx = findAccount(fromNo);
    if (fromIdx == -1) { cout << "Source account not found!\n"; return; }
    if (!verifyPin(fromIdx)) return;

    int toNo = readInt("Enter To Account Number: ");
    if (toNo == fromNo) { cout << "Cannot transfer to the same account.\n"; return; }

    int toIdx = findAccount(toNo);
    if (toIdx == -1) { cout << "Destination account not found!\n"; return; }

    double amount = readDouble("Enter amount to transfer: ");

    try {
        if (accounts[fromIdx].transfer(accounts[toIdx], amount)) {
            cout << "Transfer successful!\n";
            cout << "Your New Balance: "
                 << formatMoney(accounts[fromIdx].balance) << '\n';
        } else {
            cout << "Transfer failed! Check amount or minimum balance ("
                 << formatMoney(MIN_BALANCE) << ").\n";
        }
    } catch (const std::bad_alloc&) {
        cout << "Memory error during transfer. No changes made.\n";
    } catch (const std::exception& e) {
        cout << "Transfer error: " << e.what() << '\n';
    }
}

void listAccounts() {
    if (accounts.empty()) { cout << "No accounts found.\n"; return; }

    cout << "\n" << left << setw(12) << "Acc No"
                 << setw(25) << "Name"
                 << setw(15) << "Balance" << '\n';
    cout << string(52, '-') << '\n';

    for (const auto& acc : accounts) {
        cout << left << setw(12) << acc.accNo
                     << setw(25) << acc.name
                     << setw(15) << formatMoney(acc.balance) << '\n';
    }
}

void searchAccount() {
    cout << "Search by:\n1. Account Number\n2. Name\n";
    int choice = readInt("Enter choice: ");

    if (choice == 1) {
        int no = readInt("Enter Account Number: ");
        int idx = findAccount(no);
        if (idx == -1) cout << "Account not found!\n";
        else cout << "Account No: " << accounts[idx].accNo
                  << ", Name: " << accounts[idx].name << '\n';
    } else if (choice == 2) {
        string term = readLine("Enter name (or part of name): ");
        string lowerTerm = toLower(term);
        bool found = false;
        for (const auto& acc : accounts) {
            if (toLower(acc.name).find(lowerTerm) != string::npos) {
                cout << "Account No: " << acc.accNo
                     << ", Name: " << acc.name << '\n';
                found = true;
            }
        }
        if (!found) cout << "No matching accounts found.\n";
    } else {
        cout << "Invalid search choice.\n";
    }
}

void updateName() {
    int no = readInt("Enter Account Number: ");
    int idx = findAccount(no);
    if (idx == -1) { cout << "Account not found!\n"; return; }
    if (!verifyPin(idx)) return;

    string newName = readLine("Enter new name: ");
    if (!isValidName(newName)) {
        cout << "Invalid name! Use letters and spaces only.\n";
        return;
    }

    try {
        string entry = "Name updated to " + newName;
        accounts[idx].history.reserve(accounts[idx].history.size() + 1);
        accounts[idx].name = newName;
        accounts[idx].history.push_back(std::move(entry));
        cout << "Name updated successfully!\n";
    } catch (const std::bad_alloc&) {
        cout << "Memory error updating name.\n";
    }
}

void closeAccount() {
    int no = readInt("Enter Account Number to close: ");
    int idx = findAccount(no);
    if (idx == -1) { cout << "Account not found!\n"; return; }
    if (!verifyPin(idx)) return;

    char confirm = readChar("Are you sure you want to close A/C " +
                            to_string(no) + "? (y/n): ");
    if (confirm != 'y' && confirm != 'Y') {
        cout << "Close cancelled.\n";
        return;
    }

    double returned = accounts[idx].balance;
    try {
        accounts.erase(accounts.begin() + idx);
    } catch (const std::exception& e) {
        cout << "Could not close account: " << e.what() << '\n';
        return;
    }
    cout << "Account closed. Returned balance: "
         << formatMoney(returned) << '\n';
}

void viewHistory() {
    int no = readInt("Enter Account Number: ");
    int idx = findAccount(no);
    if (idx == -1) { cout << "Account not found!\n"; return; }
    if (!verifyPin(idx)) return;
    accounts[idx].displayHistory();
}

// ---------- Main ----------
int main() {
    try {
        loadFromFile("accounts.txt");   // silent load; ignore failure

        int choice = 0;
        do {
            cout << "\n===== Bank Management System =====\n";
            cout << "1.  Create Account\n";
            cout << "2.  Deposit\n";
            cout << "3.  Withdraw\n";
            cout << "4.  Check Balance\n";
            cout << "5.  Transfer Money\n";
            cout << "6.  List All Accounts\n";
            cout << "7.  Search Account\n";
            cout << "8.  Update Account Name\n";
            cout << "9.  Close Account\n";
            cout << "10. View Transaction History\n";
            cout << "11. Save Accounts to File\n";
            cout << "12. Load Accounts from File\n";
            cout << "13. Exit\n";

            choice = readInt("Enter your choice: ");

            try {
                switch (choice) {
                    case 1:  createAccount();    break;
                    case 2:  depositMoney();     break;
                    case 3:  withdrawMoney();    break;
                    case 4:  checkBalance();     break;
                    case 5:  transferMoney();    break;
                    case 6:  listAccounts();     break;
                    case 7:  searchAccount();    break;
                    case 8:  updateName();       break;
                    case 9:  closeAccount();     break;
                    case 10: viewHistory();      break;
                    case 11:
                        cout << (saveToFile("accounts.txt")
                                 ? "Accounts saved to accounts.txt\n"
                                 : "Error saving file!\n");
                        break;
                    case 12:
                        cout << (loadFromFile("accounts.txt")
                                 ? "Accounts loaded from accounts.txt\n"
                                 : "No accounts loaded (file missing, empty, or invalid).\n");
                        break;
                    case 13:
                        cout << "Exiting... Thank you!\n";
                        break;
                    default:
                        cout << "Invalid choice! Please try again.\n";
                }
            }
            catch (const InputClosedException&) {
                throw;   // bubble up to main so we save & exit cleanly
            }
            catch (const std::bad_alloc&) {
                cout << "Memory error. Operation aborted.\n";
            }
            catch (const std::exception& e) {
                cout << "Operation error: " << e.what() << '\n';
            }
            catch (...) {
                cout << "Unknown error during operation.\n";
            }

        } while (choice != 13);
    }
    catch (const InputClosedException&) {
        cout << "\nInput stream ended. Saving and exiting...\n";
        saveToFile("accounts.txt");
        return 0;
    }
    catch (const std::bad_alloc&) {
        cerr << "Fatal: out of memory.\n";
        return 1;
    }
    catch (const std::exception& e) {
        cerr << "Fatal error: " << e.what() << '\n';
        return 1;
    }
    catch (...) {
        cerr << "Unknown fatal error.\n";
        return 1;
    }
    return 0;
}