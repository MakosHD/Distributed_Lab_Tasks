# 8. Student's capital (40m)

![Image of interface of student's capital](/Task8_Students_capital/pics/image.png)

## Description
This is a console program that lets you calculate your capital at the end of summer by entering a couple of values

## Solution
Most of the code is dedicated to obtaining values from the user. The solution itself is quite simple. First, we create a vector containing all the laptops along with their prices and gains. First, we sort the laptops by price and start a loop. We begin by adding all the prices of the laptops we can buy to the queue, then we buy the most expensive laptop and add it to our capital. Now we repeat this cycle until either the number of laptops we can buy runs out or the laptops themselves run out. In the end, we get the result in the form of maximum capital

## How to use
It's quite simple to use; all you have to do is launch the program, and it will prompt you to enter all the necessary information.

![Image of interface of student's capital](/Task8_Students_capital/pics/image.png)
## How to install

**Tested only on Linux!**


### Ubuntu:

First, you need to update all repositories and packages

```
sudo apt-get update
sudo apt-get upgrade
```

Next, you need to install all the dependencies required for a successful build

```
sudo apt-get install git cmake build-essential
```

After successful installation, you need to clone this repository

```
git clone https://github.com/MakosHD/Distributed_Lab_Tasks.git
```

Go to the repository and create a folder for the build

```
cd Distributed_Lab_Tasks/Task8_Students_capital/
mkdir build
cd build
```

And the final step is to compile the program

```
cmake ..
cmake --build .
```

If no errors appeared, then congratulations! You've successfully compiled the program and now have an executable file. To run it, enter

```
./task8
```