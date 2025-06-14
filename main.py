from abc import ABC, abstractmethod
from typing import Optional

# --- Account & Card Classes ---

class BankAccount:
    def __init__(self, balance: int):
        self.balance = balance

    def get_balance(self) -> int:
        return self.balance

    def update_balance(self, amount: int) -> None:
        self.balance += amount


class ATMCard:
    def __init__(self, cvv: int, pin: int, account: BankAccount):
        self.cvv = cvv
        self.pin = pin
        self.account = account

    def get_pin(self) -> int:
        return self.pin

    def get_account(self) -> BankAccount:
        return self.account


# --- ATM State Pattern ---

class ATMState(ABC):
    @abstractmethod
    def insert_card(self, atm: 'ATM', card: ATMCard) -> None: pass

    @abstractmethod
    def authenticate_pin(self, atm: 'ATM', pin: int) -> None: pass

    @abstractmethod
    def check_balance(self, atm: 'ATM') -> None: pass

    @abstractmethod
    def withdraw_cash(self, atm: 'ATM', amount: int) -> None: pass

    @abstractmethod
    def eject_card(self, atm: 'ATM') -> None: pass


class IdleState(ATMState):
    def insert_card(self, atm: 'ATM', card: ATMCard) -> None:
        print("Card inserted")
        atm.card = card
        atm.set_state(atm.has_card_state)

    def authenticate_pin(self, atm: 'ATM', pin: int) -> None:
        print("Please insert card first")

    def check_balance(self, atm: 'ATM') -> None:
        print("Please insert card first")

    def withdraw_cash(self, atm: 'ATM', amount: int) -> None:
        print("Please insert card first")

    def eject_card(self, atm: 'ATM') -> None:
        print("Please insert card first")


class HasCardState(ATMState):
    def insert_card(self, atm: 'ATM', card: ATMCard) -> None:
        print("Card already inserted")

    def authenticate_pin(self, atm: 'ATM', pin: int) -> None:
        if atm.card and atm.card.get_pin() == pin:
            print("PIN authenticated")
            atm.set_state(atm.cash_withdraw_state)
        else:
            print("Invalid PIN")

    def check_balance(self, atm: 'ATM') -> None:
        print("Please authenticate PIN first")

    def withdraw_cash(self, atm: 'ATM', amount: int) -> None:
        print("Please authenticate PIN first")

    def eject_card(self, atm: 'ATM') -> None:
        print("Card ejected")
        atm.card = None
        atm.set_state(atm.idle_state)


class CashWithdrawState(ATMState):
    def insert_card(self, atm: 'ATM', card: ATMCard) -> None:
        print("Card already inserted")

    def authenticate_pin(self, atm: 'ATM', pin: int) -> None:
        print("Already authenticated")

    def check_balance(self, atm: 'ATM') -> None:
        acc = atm.card.get_account()
        print(f"Balance: Rs. {acc.get_balance()}")

    def withdraw_cash(self, atm: 'ATM', amount: int) -> None:
        acc = atm.card.get_account()
        if amount > acc.get_balance():
            print("Insufficient funds in account")
        elif amount > atm.total_cash:
            print("Insufficient cash in ATM")
        else:
            atm.cash_handler.handle(amount, atm)
            acc.update_balance(-amount)
            atm.total_cash -= amount
            print("Please collect your cash")
            atm.set_state(atm.idle_state if atm.total_cash > 0 else atm.out_of_cash_state)

    def eject_card(self, atm: 'ATM') -> None:
        print("Card ejected")
        atm.card = None
        atm.set_state(atm.idle_state)


class OutOfCashState(ATMState):
    def insert_card(self, atm: 'ATM', card: ATMCard) -> None:
        print("ATM out of service")

    def authenticate_pin(self, atm: 'ATM', pin: int) -> None:
        print("ATM out of service")

    def check_balance(self, atm: 'ATM') -> None:
        print("ATM out of service")

    def withdraw_cash(self, atm: 'ATM', amount: int) -> None:
        print("ATM out of service")

    def eject_card(self, atm: 'ATM') -> None:
        print("ATM out of service")


# --- Cash Handler Chain ---

class CashHandler(ABC):
    def __init__(self, next_handler: Optional['CashHandler'] = None):
        self.next_handler = next_handler

    @abstractmethod
    def handle(self, amount: int, atm: 'ATM') -> None: pass


class CashHandler2K(CashHandler):
    def handle(self, amount: int, atm: 'ATM') -> None:
        notes = min(amount // 2000, atm.notes_2k)
        if notes:
            print(f"Dispensing {notes} notes of 2000")
        atm.notes_2k -= notes
        amount -= notes * 2000
        if self.next_handler:
            self.next_handler.handle(amount, atm)


class CashHandler500(CashHandler):
    def handle(self, amount: int, atm: 'ATM') -> None:
        notes = min(amount // 500, atm.notes_500)
        if notes:
            print(f"Dispensing {notes} notes of 500")
        atm.notes_500 -= notes
        amount -= notes * 500
        if self.next_handler:
            self.next_handler.handle(amount, atm)


class CashHandler100(CashHandler):
    def handle(self, amount: int, atm: 'ATM') -> None:
        notes = min(amount // 100, atm.notes_100)
        if notes:
            print(f"Dispensing {notes} notes of 100")
        atm.notes_100 -= notes
        amount -= notes * 100
        if amount:
            print(f"Cannot dispense Rs.{amount} due to note unavailability")


# --- ATM Core ---

class ATM:
    def __init__(self, total_cash: int, notes_2k: int, notes_500: int, notes_100: int):
        self.total_cash = total_cash
        self.notes_2k = notes_2k
        self.notes_500 = notes_500
        self.notes_100 = notes_100
        self.card: Optional[ATMCard] = None

        self.idle_state = IdleState()
        self.has_card_state = HasCardState()
        self.cash_withdraw_state = CashWithdrawState()
        self.out_of_cash_state = OutOfCashState()

        self.cash_handler = CashHandler2K(CashHandler500(CashHandler100()))

        self.state: ATMState = self.idle_state if self.total_cash > 0 else self.out_of_cash_state

    def set_state(self, state: ATMState) -> None:
        self.state = state

    def insert_card(self, card: ATMCard) -> None:
        self.state.insert_card(self, card)

    def enter_pin(self, pin: int) -> None:
        self.state.authenticate_pin(self, pin)

    def check_balance(self) -> None:
        self.state.check_balance(self)

    def withdraw_cash(self, amount: int) -> None:
        self.state.withdraw_cash(self, amount)

    def eject_card(self) -> None:
        self.state.eject_card(self)


# --- Simulation ---
if __name__ == '__main__':
    account = BankAccount(50000)
    card = ATMCard(cvv=123, pin=1111, account=account)

    atm = ATM(total_cash=50000, notes_2k=10, notes_500=10, notes_100=10)

    atm.insert_card(card)
    atm.enter_pin(1111)
    atm.check_balance()
    atm.withdraw_cash(4700)
    atm.eject_card()
