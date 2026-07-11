# 1. Maze(30m)


## Description
A console program that takes two CSV files (representing different days and containing user_name, order_id, and timestamp) and outputs all users who meet the following criteria:
- Visited some pages on both days
- On the second day, visited a page that this user had not visited on the first day

## Solution
This program uses the `csv` library to work with CSV files. First, the program reads all records from the first day and stores them in `unsorted_map`. Next, the program begins reading the second day’s data, but now, for each user, it checks whether they visited two identical pages and whether there is one unique page. If the conditions are met, we count that user as a match.

## How to use
The program is very easy to use. All you need to do is pass two arguments: the paths to the CSV files
![alt text](/Task3_Website_analytics/pics/image_1.png)

This repository includes two sample files to make it easier to test the program's functionality.
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
cd Distributed_Lab_Tasks/Task3_Website_analytics/
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
./task3 
```


## Why my solution is efficient?
