#include <iostream>
#include <string>
using namespace std;

class Account {
private:
    int accNo;
    string name;
    float balance;

public:
    void createAccount(int no, string n, float initialDeposit) {
        accNo = no;
        name = n;
        balance = initialDeposit;
    }

    void deposit(float amount) {
        balance = balance + amount;
    }

    bool withdraw(float amount) {
        if (amount > balance) {
            return false;   // insufficient balance
        }
        balance = balance - amount;
        return true;
    }

    void display() {
        cout << "Account No : " << accNo << endl;
        cout << "Name       : " << name << endl;
        cout << "Balance    : " << balance << endl;
    }

    int getAccNo() {
        return accNo;
    }
};

int main() {
    Account acc[100];
    int count = 0;
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

        switch (choice) {
        case 1:
            cout << "Enter Account Number: ";
            cin >> no;
            cout << "Enter Name: ";
            cin >> name;
            cout << "Enter Initial Deposit: ";
            cin >> amount;
            acc[count].createAccount(no, name, amount);
            count++;
            cout << "Account Created Successfully!\n";
            break;

        case 2:
            cout << "Enter Account Number: ";
            cin >> no;
            for (int i = 0; i < count; i++) {
                if (acc[i].getAccNo() == no) {
                    cout << "Enter Amount to Deposit: ";
                    cin >> amount;
                    acc[i].deposit(amount);
                    cout << "Amount Deposited Successfully!\n";
                }
            }
            break;

        case 3:
            cout << "Enter Account Number: ";
            cin >> no;
            for (int i = 0; i < count; i++) {
                if (acc[i].getAccNo() == no) {
                    cout << "Enter Amount to Withdraw: ";
                    cin >> amount;
                    if (acc[i].withdraw(amount))
                        cout << "Amount Withdrawn Successfully!\n";
                    else
                        cout << "Insufficient Balance!\n";
                }
            }
            break;

        case 4:
            cout << "Enter Account Number: ";
            cin >> no;
            for (int i = 0; i < count; i++) {
                if (acc[i].getAccNo() == no) {
                    acc[i].display();
                }
            }
            break;

        case 5:
            cout << "Exiting... Thank You!\n";
            break;

        default:
            cout << "Invalid Choice! Please try again.\n";
        }

    } while (choice != 5);

    return 0;
}
