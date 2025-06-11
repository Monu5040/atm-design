from abc import ABC, abstractmethod
from typing import Optional


# --- Domain Classes ---

class BankAccount:
    def __init__(self, balance: int) -> None:
        self._balance = balance

    def get_balance(self) -> int:
        return self._balance

    def update_balance(self, amount: int) -> None:
        self._balance += amount


class ATMCard:
    def __init__(self, cvv: int, pin: int, bank_account: BankAccount) -> None:
        self._cvv = cvv
        self._pin = pin
        self._bank_account = bank_account

    def get_pin(self) -> int:
        return self._pin

    def get_account(self) -> BankAccount:
        return self._bank_account


class User:
    def __init__(self, name: str, card: ATMCard) -> None:
        self.name = name
        self._card = card

    def get_card(self) -> ATMCard:
        return self._card


# --- State Pattern ---

class ATM:  # forward declaration for type hinting
    ...


class ATMState(ABC):
    @abstractmethod
    def insert_card(self, atm: "ATM") -> None: pass

    @abstractmethod
    def authenticate_pin(self, atm: "ATM", pin: int) -> None: pass

    @abstractmethod
    def check_balance(self, atm: "ATM") -> None: pass

    @abstractmethod
    def cash_withdraw(self, atm: "ATM", amount: int) -> None: pass

    @abstractmethod
    def eject_card(self, atm: "ATM") -> None: pass


class IdleState(ATMState):
    def insert_card(self, atm: "ATM") -> None:
        print("Card inserted")
        atm.set_state(atm.has_card_state)

    def authenticate_pin(self, atm: "ATM", pin: int) -> None:
        print("Please insert card first")

    def check_balance(self, atm: "ATM") -> None:
        print("Please insert card first")

    def cash_withdraw(self, atm: "ATM", amount: int) -> None:
        print("Please insert card first")

    def eject_card(self, atm: "ATM") -> None:
        print("Please insert card first")


class HasCardState(ATMState):
    def insert_card(self, atm: "ATM") -> None:
        print("Card already inserted")

    def authenticate_pin(self, atm: "ATM", pin: int) -> None:
        if pin == atm.user.get_card().get_pin():
            print("PIN authenticated")
            atm.set_state(atm.cash_withdraw_state)
        else:
            print("Invalid PIN")

    def check_balance(self, atm: "ATM") -> None:
        print("Authenticate PIN first")

    def cash_withdraw(self, atm: "ATM", amount: int) -> None:
        print("Authenticate PIN first")

    def eject_card(self, atm: "ATM") -> None:
        print("Card ejected")
        atm.set_state(atm.idle_state)


class CashWithdrawState(ATMState):
    def insert_card(self, atm: "ATM") -> None:
        print("Card already inserted")

    def authenticate_pin(self, atm: "ATM", pin: int) -> None:
        print("PIN already authenticated")

    def check_balance(self, atm: "ATM") -> None:
        balance = atm.user.get_card().get_account().get_balance()
        print(f"Account balance: ₹{balance}")

    def cash_withdraw(self, atm: "ATM", amount: int) -> None:
        if amount > atm.machine_balance:
            print("Insufficient cash in ATM")
        elif amount > atm.user.get_card().get_account().get_balance():
            print("Insufficient balance in account")
        else:
            atm.user.get_card().get_account().update_balance(-amount)
            atm.machine_balance -= amount
            print(f"Withdrawing ₹{amount}")
            atm.handle_cash_dispense(amount)

    def eject_card(self, atm: "ATM") -> None:
        print("Card ejected")
        atm.set_state(atm.idle_state)


class OutOfCashState(ATMState):
    def insert_card(self, atm: "ATM") -> None:
        print("ATM out of cash")

    def authenticate_pin(self, atm: "ATM", pin: int) -> None:
        print("ATM out of cash")

    def check_balance(self, atm: "ATM") -> None:
        print("ATM out of cash")

    def cash_withdraw(self, atm: "ATM", amount: int) -> None:
        print("ATM out of cash")

    def eject_card(self, atm: "ATM") -> None:
        print("ATM out of cash")


# --- Chain of Responsibility ---

class CashWithdrawHandler(ABC):
    def __init__(self, atm: "ATM", next_handler: Optional["CashWithdrawHandler"] = None) -> None:
        self.atm = atm
        self.next_handler = next_handler

    @abstractmethod
    def withdraw(self, amount: int) -> None: pass


class CashWithdrawHandler2K(CashWithdrawHandler):
    def withdraw(self, amount: int) -> None:
        count = min(amount // 2000, self.atm.notes_2k)
        if count > 0:
            print(f"Dispensing {count} notes of ₹2000")
            self.atm.notes_2k -= count
        amount -= count * 2000
        if self.next_handler:
            self.next_handler.withdraw(amount)


class CashWithdrawHandler500(CashWithdrawHandler):
    def withdraw(self, amount: int) -> None:
        count = min(amount // 500, self.atm.notes_500)
        if count > 0:
            print(f"Dispensing {count} notes of ₹500")
            self.atm.notes_500 -= count
        amount -= count * 500
        if self.next_handler:
            self.next_handler.withdraw(amount)


class CashWithdrawHandler100(CashWithdrawHandler):
    def withdraw(self, amount: int) -> None:
        count = min(amount // 100, self.atm.notes_100)
        if count > 0:
            print(f"Dispensing {count} notes of ₹100")
            self.atm.notes_100 -= count
        amount -= count * 100
        if amount > 0:
            print("Cannot dispense remaining amount")


# --- ATM Class ---

class ATM:
    def __init__(self, user: User, total_cash: int, notes_2k: int, notes_500: int, notes_100: int) -> None:
        self.user = user
        self.machine_balance = total_cash
        self.notes_2k = notes_2k
        self.notes_500 = notes_500
        self.notes_100 = notes_100

        self.idle_state: ATMState = IdleState()
        self.has_card_state: ATMState = HasCardState()
        self.cash_withdraw_state: ATMState = CashWithdrawState()
        self.out_of_cash_state: ATMState = OutOfCashState()
        self.curr_state: ATMState = self.idle_state if total_cash > 0 else self.out_of_cash_state

    def set_state(self, state: ATMState) -> None:
        self.curr_state = state

    def insert_card(self) -> None:
        self.curr_state.insert_card(self)

    def enter_pin(self, pin: int) -> None:
        self.curr_state.authenticate_pin(self, pin)

    def check_balance(self) -> None:
        self.curr_state.check_balance(self)

    def withdraw_cash(self, amount: int) -> None:
        self.curr_state.cash_withdraw(self, amount)

    def eject_card(self) -> None:
        self.curr_state.eject_card(self)

    def handle_cash_dispense(self, amount: int) -> None:
        chain = CashWithdrawHandler2K(self, CashWithdrawHandler500(self, CashWithdrawHandler100(self)))
        chain.withdraw(amount)


# --- Example Usage ---

if __name__ == "__main__":
    account = BankAccount(50000)
    card = ATMCard(cvv=123, pin=1111, bank_account=account)
    user = User("John", card)

    atm = ATM(user, total_cash=50000, notes_2k=10, notes_500=10, notes_100=10)

    atm.insert_card()
    atm.enter_pin(1111)
    atm.check_balance()
    atm.withdraw_cash(4700)
    atm.eject_card()
