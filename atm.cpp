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
    User(string name,shared_ptr<ATMCard> card) : 
        name(name), card(card) {}

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

class IdleState : public ATMState{
public:
	void insertCard(ATM* atm) override;

	void authenticatePin(ATM* atm, int pin) override {
		cout<<"Please insert Card"<<endl;
	}

	void checkBalance(ATM* atm) override {
		cout<<"Please insert Card"<<endl;
	}

	void cashWithdraw(ATM* atm, int amount) override {
		cout<<"Please insert card"<<endl;
	}

	void ejectCard(ATM* atm) override {
		cout<<"Please insert Card"<<endl;
	}

	void outOfCash(ATM* atm) override {
		cout<<"Please insert card"<<endl;
	}
};

class HasCardState : public ATMState {
public:
	void insertCard(ATM* atm) override {
		cout<<"Card inserted"<<endl;
		//change card state
	}

	void authenticatePin(ATM* atm, int pin) override;

	void checkBalance(ATM* atm) override {
		cout<<"Please authenticate pin"<<endl;
	}

	void cashWithdraw(ATM* atm, int amount) override {
		cout<<"Please authenticate pin"<<endl;
	}

	void ejectCard(ATM* atm) override {
		cout<<"Card ejected"<<endl;
	}

	void outOfCash(ATM* atm) override {
		cout<<"Please authenticate pin"<<endl;
	}
};

class CashWithdrawState : public ATMState {
public:
	void insertCard(ATM* atm) override {
		cout<<"Only cash withdraw"<<endl;
		//change card state
	}

	void authenticatePin(ATM* atm, int pin) override{
		cout<<"Only cash withdraw"<<endl;
	}

	void checkBalance(ATM* atm) override {
		cout<<"Only cash withdraw"<<endl;
	}

	void cashWithdraw(ATM* atm, int amount) override;

	void ejectCard(ATM* atm) override {
		cout<<"Card ejected"<<endl;
	}

	void outOfCash(ATM* atm) override {
		cout<<"Cash withdraw state"<<endl;
	}
};



class OutOfCashState : public ATMState {
public:
	void insertCard(ATM* atm) override {
		cout<<"Out of Cash"<<endl;
		//change card state
	}

	void authenticatePin(ATM* atm, int pin) override {
		cout<<"Out of Cash"<<endl;
	}

	void checkBalance(ATM* atm) override {
		cout<<"Out of Cash"<<endl;
	}

	void cashWithdraw(ATM* atm, int amount) override {
		cout<<"Out of Cash"<<endl;
	}

	void ejectCard(ATM* atm) override {
		cout<<"Out of Cash"<<endl;
	}

	void outOfCash(ATM* atm) override {
		cout<<"Out of Cash"<<endl;
	}
};


class ATM {
private:
	shared_ptr<User> user;
	shared_ptr<ATMState>curr_state;
	shared_ptr<ATMState>idle_state;
	shared_ptr<ATMState>has_card_state;
	shared_ptr<ATMState>cash_withdraw_state;
	shared_ptr<ATMState>out_of_cash_state;

	int machine_balance;

public:
	ATM(shared_ptr<User> user, int amount) :user(user), machine_balance(amount) {
		idle_state = make_shared<IdleState>();
		has_card_state = make_shared<HasCardState>();
		cash_withdraw_state = make_shared<CashWithdrawState>();
		out_of_cash_state = make_shared<OutOfCashState>();


		if(machine_balance < 0) {
			cout<<"Out of cash"<<endl;
			curr_state = out_of_cash_state;
		} else {
			curr_state = idle_state;
		}
	}

	shared_ptr<ATMState> getIdleSate() {return idle_state;}
	shared_ptr<ATMState> getHasCardSate() {return has_card_state;}
	shared_ptr<ATMState> getCashWithdrawSate() {return cash_withdraw_state;}
	shared_ptr<User> getUser() {return user;}

	void setState(shared_ptr<ATMState> new_state) {
		curr_state = new_state;
	}

	void insertCardATM(shared_ptr<ATMCard>card) {
		curr_state->insertCard(this);
	}

	void enterPinATM(int pin) {
		curr_state->authenticatePin(this, pin);
	}

	void cashWithdrawATM(int amount) {
		if(amount > machine_balance) {
			cout<<"Insufficient cash in machine"<<endl;
		} else {
			curr_state->cashWithdraw(this, amount);
		}
	}
	


};

void IdleState::insertCard(ATM* atm) {
	cout<<"Card inserted"<<endl;
	atm->setState(atm->getHasCardSate());
	//change card state
}

void HasCardState::authenticatePin(ATM* atm, int pin) {
	if(pin == atm->getUser()->getCard()->getPin()) {
		cout<<"Pin authenticated"<<endl;
		atm->setState(atm->getCashWithdrawSate());
	}
	
}

void CashWithdrawState::cashWithdraw(ATM* atm, int amount) {
	cout<<"Only cash withdraw"<<endl;
	int balance = atm->getUser()->getCard()->getBankAccount()->getCurrentBalance();
	if(amount<=balance) {
		cout<<"Amount debited : "<<amount<<endl;
		atm->getUser()->getCard()->getBankAccount()->updateBalance(-amount);
	} else {
		cout<<"Insufficient balance"<<endl;
	}
	atm->setState(atm->getHasCardSate());
}


int main() {
	cout<<"ATM Design"<<endl;
	// shared_ptr<BankAccount>b_acc(new BankAccount(100000));
	shared_ptr<BankAccount>b_acc = make_shared<BankAccount>(100000);
	// cout<<b_acc->getCurrentBalance()<<endl;
	shared_ptr<ATMCard>card = make_shared<ATMCard>(233,5040,b_acc);
	// cout<<card->getPin()<<endl;
	shared_ptr<User>user = make_shared<User>("Monu",card);
	shared_ptr<ATM>atm = make_shared<ATM>(user, 500000);
	atm->insertCardATM(card);
	atm->enterPinATM(5040);
	atm->cashWithdrawATM(5000);
}