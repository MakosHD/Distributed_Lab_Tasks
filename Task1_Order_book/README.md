# 1. OrderBook(21h45m)

![Image of interface of Orderbook](/Task1_Order_book/pics/image_1.png)

## Description
This is a GUI application that implements an orderbook. With it, users can easily place buy or sell orders for UAH. Instead of a console window with white lines, users see an intuitive interface and, with just a couple of clicks, can either make a million or lose everything. 

Users can view their balance, lists of active buy and sell orders, and a list of completed or canceled orders. Order creation controls are also available to users. The program has various security measures in place at different levels to ensure that it can accommodate any user.

## Solution
The program was developed using the Qt6 framework, which is entirely responsible for the graphical user interface and user-program interactions. The GUI layout was designed in Qt Widget Designer. Two main classes were developed: ***OrderBook*** and ***Users***.

The ***Users*** class is entirely responsible for managing users, that is, the number of users, their names, their balances, and their order history.

The ***OrderBook*** class depends on the Users class. Its main purpose is to create, store, and process orders. Order processing occurs in a separate thread to avoid blocking order creation.

All classes were designed with multithreading in mind, so multiple users can use these classes simultaneously without worrying about “delays” or “race conditions” (data corruption).

The ***OrderTableModel*** and ***ConsoleReader*** classes were also developed, but they serve as auxiliary classes. 

The ***OrderTableModel*** class acts as an intermediary layer between the *OrderBook* and *QTableView*. It is used to populate tables with information from the *OrderBook.*

***ConsoleReader*** is a small class responsible for reading data from the console and creating new users and corresponding windows for them.

## How to use
When you run the program in a console window, you will be prompted to enter a username. Once you enter it, a new user will be created and the corresponding graphical window will open.

![alt text](/Task1_Order_book/pics/image_2.png)
![alt text](/Task1_Order_book/pics/image_3.png)

Each window has a username at the end (in parentheses) in the title. This allows you to quickly tell which window corresponds to which user. You can then either add another user (by entering a name in the console) or start using the orderbook via the graphical interface (on behalf of the user specified in the title).

### **View current balance**

![alt text](/Task1_Order_book/pics/image_5.png)

In this part of the app, you can view your current balance. At first, you'll have 0 UAH and 0 USD, but you can easily change that (next section).

### **Change User Balance**

![alt text](/Task1_Order_book/pics/image_4.png)

Using this part of the program, you can change a user's balance. By entering positive or negative values, you can add or subtract a specific amount from the balance. Using the combo box, you can select the currency in which the changes will be made

### **Create a new order**

![alt text](/Task1_Order_book/pics/image_6.png)

To create an order, you must use this part of the program. First, you need to specify what you want to do (buy or sell) with hryvnias. After that, you must specify the amount of hryvnias. Next, enter the amount in dollars, but please note that this amount is per HRYVNI, not for the entire order. If you’ve entered all the data correctly (negative values are not allowed, and a ***special sound selected by experts will alert you if you enter one***), the “DO IT” button will become active, and you’ll be able to place your order.

If everything went well, you'll see the following window(next section).

### **Cancel order**

![alt text](/Task1_Order_book/pics/image_7.png)

You can cancel your order while it is still being processed. However, any transactions that have already been completed are non-refundable.

### **Tables of orders**

![alt text](/Task1_Order_book/pics/image_8.png)

Three tables make up most of the program. Each table displays data about orders, specifically who placed them, the quantity, the price, and how much is left. 

The first table (top left) shows all orders to buy hryvnia. They are sorted by price (from highest to lowest).

The second table (top right) shows orders to sell hryvnia. This table is sorted from the lowest to the highest price. 

The last table (bottom) shows the history of orders and their status.

## How to install

**Tested only on Linux!**