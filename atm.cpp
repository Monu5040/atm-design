#include <bits/stdc++.h>
using namespace std;

class BankAccount {
private:
    int balance;
public:
    BankAccount(int balance) : balance(balance) {}

    int getCurrentBalance() {
        return balance;
    }

    void updateBalance(int amount) {
        balance += amount;
    }
};

class ATMCard {
private:
    int cvv;
    int pin;
    shared_ptr<BankAccount> b_acc;
public:
    ATMCard(int cvv, int pin, shared_ptr<BankAccount> b_acc) : cvv(cvv), pin(pin), b_acc(b_acc) {}

    int getPin() {
        return pin;
    }

    shared_ptr<BankAccount> getBankAccount() {
        return b_acc;
    }
};

class User {
private:
    string name;
    shared_ptr<ATMCard> card;
public:
    User(string name, shared_ptr<ATMCard> card) : name(name), card(card) {}

    shared_ptr<ATMCard> getCard() {
        return card;
    }
};

class ATM;

class ATMState {
public:
    virtual void insertCard(ATM*) = 0;
    virtual void authenticatePin(ATM*, int) = 0;
    virtual void checkBalance(ATM*) = 0;
    virtual void cashWithdraw(ATM*, int) = 0;
    virtual void ejectCard(ATM*) = 0;
    virtual void outOfCash(ATM*) = 0;
    virtual ~ATMState() {}
};

class IdleState : public ATMState {
public:
    void insertCard(ATM* atm) override;

    void authenticatePin(ATM* atm, int pin) override {
        cout << "Please insert Card" << endl;
    }

    void checkBalance(ATM* atm) override {
        cout << "Please insert Card" << endl;
    }

    void cashWithdraw(ATM* atm, int amount) override {
        cout << "Please insert card" << endl;
    }

    void ejectCard(ATM* atm) override {
        cout << "Please insert Card" << endl;
    }

    void outOfCash(ATM* atm) override {
        cout << "Please insert card" << endl;
    }
};

class HasCardState : public ATMState {
public:
    void insertCard(ATM* atm) override {
        cout << "Card inserted" << endl;
        // change card state
    }

    void authenticatePin(ATM* atm, int pin) override;

    void checkBalance(ATM* atm) override {
        cout << "Please authenticate pin" << endl;
    }

    void cashWithdraw(ATM* atm, int amount) override {
        cout << "Please authenticate pin" << endl;
    }

    void ejectCard(ATM* atm) override {
        cout << "Card ejected" << endl;
    }

    void outOfCash(ATM* atm) override {
        cout << "Please authenticate pin" << endl;
    }
};

class CashWithdrawState : public ATMState {
public:
    void insertCard(ATM* atm) override {
        cout << "Only cash withdraw" << endl;
    }

    void authenticatePin(ATM* atm, int pin) override {
        cout << "Only cash withdraw" << endl;
    }

    void checkBalance(ATM* atm) override {
        cout << "Only cash withdraw" << endl;
    }

    void cashWithdraw(ATM* atm, int amount) override;

    void ejectCard(ATM* atm) override {
        cout << "Card ejected" << endl;
    }

    void outOfCash(ATM* atm) override {
        cout << "Cash withdraw state" << endl;
    }
};

class OutOfCashState : public ATMState {
public:
    void insertCard(ATM* atm) override {
        cout << "Out of Cash" << endl;
    }

    void authenticatePin(ATM* atm, int pin) override {
        cout << "Out of Cash" << endl;
    }

    void checkBalance(ATM* atm) override {
        cout << "Out of Cash" << endl;
    }

    void cashWithdraw(ATM* atm, int amount) override {
        cout << "Out of Cash" << endl;
    }

    void ejectCard(ATM* atm) override {
        cout << "Out of Cash" << endl;
    }

    void outOfCash(ATM* atm) override {
        cout << "Out of Cash" << endl;
    }
};

class CashWithdrawHandler;
class CashWithdrawHandler2K;
class CashWithdrawHandler500;
class CashWithdrawHandler100;  // Added semicolon

class ATM {
private:
    shared_ptr<User> user;
    shared_ptr<ATMState> curr_state;
    shared_ptr<ATMState> idle_state;
    shared_ptr<ATMState> has_card_state;
    shared_ptr<ATMState> cash_withdraw_state;
    shared_ptr<ATMState> out_of_cash_state;
    int notes_2k;
    int notes_500;
    int notes_100;
    int machine_balance;

public:
    ATM(shared_ptr<User> user, int amount, int notes_2k, int notes_500, int notes_100) :
        user(user), machine_balance(amount), notes_2k(notes_2k), notes_500(notes_500), notes_100(notes_100) {
        idle_state = make_shared<IdleState>();
        has_card_state = make_shared<HasCardState>();
        cash_withdraw_state = make_shared<CashWithdrawState>();
        out_of_cash_state = make_shared<OutOfCashState>();

        if (machine_balance < 0) {
            cout << "Out of cash" << endl;
            curr_state = out_of_cash_state;
        } else {
            curr_state = idle_state;
        }
    }

    shared_ptr<ATMState> getIdleState() { return idle_state; }
    shared_ptr<ATMState> getHasCardState() { return has_card_state; }
    shared_ptr<ATMState> getCashWithdrawState() { return cash_withdraw_state; }
    shared_ptr<User> getUser() { return user; }
    int get2KNotes() { return notes_2k; }
    int get500Notes() { return notes_500; }
    int get100Notes() { return notes_100; }

    void setState(shared_ptr<ATMState> new_state) {
        curr_state = new_state;
    }

    void insertCardATM(shared_ptr<ATMCard> card) {
        curr_state->insertCard(this);
    }

    void enterPinATM(int pin) {
        curr_state->authenticatePin(this, pin);
    }

    void cashWithdrawATM(int amount) {
        if (amount > machine_balance) {
            cout << "Insufficient cash in machine" << endl;
        } else {
            curr_state->cashWithdraw(this, amount);
        }
    }

    void cashHandler(int amount) ;
};

class CashWithdrawHandler {
private:
    CashWithdrawHandler* next_handler;
protected:
    ATM* atm;
public:
    CashWithdrawHandler(ATM* atm, CashWithdrawHandler* next_handler) :
        atm(atm), next_handler(next_handler) {}

    virtual void withdrawNotes(int amount) {
        if (next_handler != nullptr) {
            next_handler->withdrawNotes(amount);
        }
    }
};

class CashWithdrawHandler2K : public CashWithdrawHandler {
public:
    CashWithdrawHandler2K(ATM* atm, CashWithdrawHandler* next_handler) :
        CashWithdrawHandler(atm, next_handler) {}

    void withdrawNotes(int amount) override {
        int count = min(amount / 2000, atm->get2KNotes());
        int dispenseAmount = count * 2000;
        if (count > 0) {
            cout << "Dispensing " << count << " notes of 2000" << endl;
            amount -= dispenseAmount;
        }
        CashWithdrawHandler::withdrawNotes(amount);
    }
};

class CashWithdrawHandler500 : public CashWithdrawHandler {
public:
    CashWithdrawHandler500(ATM* atm, CashWithdrawHandler* next_handler) :
        CashWithdrawHandler(atm, next_handler) {}

    void withdrawNotes(int amount) override {
        int count = min(amount / 500, atm->get500Notes());
        int dispenseAmount = count * 500;
        if (count > 0) {
            cout << "Dispensing " << count << " notes of 500" << endl;
            amount -= dispenseAmount;
        }
        CashWithdrawHandler::withdrawNotes(amount);
    }
};

class CashWithdrawHandler100 : public CashWithdrawHandler {
public:
    CashWithdrawHandler100(ATM* atm, CashWithdrawHandler* next_handler) :
        CashWithdrawHandler(atm, next_handler) {}

    void withdrawNotes(int amount) override {
        int count = min(amount / 100, atm->get100Notes());
        int dispenseAmount = count * 100;
        if (count > 0) {
            cout << "Dispensing " << count << " notes of 100" << endl;
            amount -= dispenseAmount;
        }
        CashWithdrawHandler::withdrawNotes(amount);
    }
};

void ATM::cashHandler(int amount) {
        CashWithdrawHandler* cash_handler = new CashWithdrawHandler2K(this,new CashWithdrawHandler500(this,new CashWithdrawHandler100(this, nullptr)));
        cash_handler->withdrawNotes(amount);
    }
// State transitions
void IdleState::insertCard(ATM* atm) {
    cout << "Card inserted" << endl;
    atm->setState(atm->getHasCardState());
}

void HasCardState::authenticatePin(ATM* atm, int pin) {
    if (pin == atm->getUser()->getCard()->getPin()) {
        cout << "PIN authenticated" << endl;
        atm->setState(atm->getCashWithdrawState());
    } else {
        cout << "Invalid PIN" << endl;
    }
}

void CashWithdrawState::cashWithdraw(ATM* atm, int amount) {
    atm->cashHandler(amount);
}

// Main function for simulation
int main() {
    auto bank_acc = make_shared<BankAccount>(50000);
    auto atm_card = make_shared<ATMCard>(123, 1111, bank_acc);
    auto user = make_shared<User>("John", atm_card);
    ATM atm(user, 50000, 10, 10, 10);

    atm.insertCardATM(atm_card);
    atm.enterPinATM(1111);
    atm.cashWithdrawATM(4700);

    return 0;
}
