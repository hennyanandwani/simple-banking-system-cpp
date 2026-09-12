#include <iostream>
#include <string>
using namespace std;

class Account {
public:
    int accNo;
    string name;
    float balance;
    static const int MIN_BALANCE = 500;

    void create(int no, string n, float initialDeposit) {
        accNo = no;
        name = n;
        balance = initialDeposit;
    }

    bool deposit(float amount) {
        if (amount <= 0) return false;
        balance += amount;
        return true;
    }

    bool withdraw(float amount) {
        if (amount <= 0 || (balance - amount) < MIN_BALANCE) return false;
        balance -= amount;
        return true;
    }

    void display() {
        cout << "Account No : " << accNo << endl;
        cout << "Name       : " << name << endl;
        cout << "Balance    : " << (long)(balance * 100 + 0.5) / 100.0 << endl;
    }
};

Account acc[100];
int count = 0;

// returns index of account, or -1 if not found
int findAccount(int no) {
    for (int i = 0; i < count; i++) {
        if (acc[i].accNo == no) return i;
    }
    return -1;
}

int main() {
    int choice, no;
    string name;
    float amount;

    do {
        cout << "\n----- Bank Management System -----\n";
        cout << "1. Create Account\n";
        cout << "2. Deposit\n";
        cout << "3. Withdraw\n";
        cout << "4. Check Balance\n";
        cout << "5. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1) {
            cout << "Enter Account Number: ";
            cin >> no;

            if (findAccount(no) != -1) {
                cout << "Account Number already exists!\n";
                continue;
            }

            cout << "Enter Name: ";
            cin >> name;

            bool validName = true;
            for (int i = 0; i < name.length(); i++) {
                if (!((name[i] >= 'a' && name[i] <= 'z') || (name[i] >= 'A' && name[i] <= 'Z'))) {
                    validName = false;
                    break;
                }
            }
            if (!validName) {
                cout << "Name should only contain letters!\n";
                continue;
            }

            cout << "Enter Initial Deposit (minimum 500): ";
            cin >> amount;

            if (amount < 500) {
                cout << "Initial deposit must be at least 500. Account not created.\n";
                continue;
            }

            acc[count].create(no, name, amount);
            count++;
            cout << "Account Created Successfully!\n";
        }
        else if (choice == 2 || choice == 3) {
            cout << "Enter Account Number: ";
            cin >> no;
            int idx = findAccount(no);

            if (idx == -1) {
                cout << "Account not found!\n";
                continue;
            }

            cout << (choice == 2 ? "Enter Amount to Deposit: " : "Enter Amount to Withdraw: ");
            cin >> amount;

            bool success = (choice == 2) ? acc[idx].deposit(amount) : acc[idx].withdraw(amount);

            if (success) {
                cout << (choice == 2 ? "Amount Deposited Successfully!\n" : "Amount Withdrawn Successfully!\n");
                cout << "New Balance: " << (long)(acc[idx].balance * 100 + 0.5) / 100.0 << endl;
            } else {
                cout << "Transaction failed! Check amount or minimum balance (500).\n";
            }
        }
        else if (choice == 4) {
            cout << "Enter Account Number: ";
            cin >> no;
            int idx = findAccount(no);

            if (idx == -1) cout << "Account not found!\n";
            else acc[idx].display();
        }
        else if (choice == 5) {
            cout << "Exiting... Thank You!\n";
        }
        else {
            cout << "Invalid Choice! Please try again.\n";
        }

    } while (choice != 5);

    return 0;
}