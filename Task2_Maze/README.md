# 2. Maze(6h20m)

![Image of ready maze](/Task2_Maze/pics/image_1.png)

## Description
This is a console program for generating mazes. Although it is a console program, the average user will find it easy to use because the only thing required of the user is to enter the maze’s dimensions. Mazes can also contain traps (from 0 to 5) and may also contain treasure. If necessary, the program can display a solution to the maze.

## Solution
The mazes are generated using a randomized depth-first search (with a stack) algorithm. This algorithm is fairly simple, but unfortunately it has a limitation: the dimensions of the maze must be odd numbers. To store the maze, a two-dimensional array in dynamic memory is used, which stores `s_cell` structures. These structures hold all the necessary information about a specific cell (type, coordinates, weight, etc.). The breadth-first search algorithm is used to place traps. You might ask why, but the answer is simple: this algorithm is used to find a solution to the maze, and then the traps are placed. This way, we can count how many traps have already been placed on the solution path and ensure that the maze is actually solvable, rather than just a collection of walls and traps. A side effect of this approach is that we have the solution to the maze at the end, so the user can view the solution if they wish.

## How to use
Program parameters are set using command-line arguments. If no arguments are provided, a maze of the minimum size (3x3) will be generated.

![alt text](/Task2_Maze/pics/image_2.png)

To specify the dimensions of the maze, you need to use the `--height={size}` and `--width={size}` arguments

![alt text](/Task2_Maze/pics/image_3.png)

As we can see, the program warned us that the dimensions of the maze must be odd numbers and changed them accordingly (see the solution section for more information).

If necessary, you can add the `--path` argument so that the maze is generated along with the solution 

![alt text](/Task2_Maze/pics/image_4.png)

If you happen to forget something, you can use the `--help` option, which will display information about all possible options as well as a description of each cell label.

![alt text](/Task2_Maze/pics/image_5.png)
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
cd Distributed_Lab_Tasks/Task2_Maze/
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
./task2 
```


## Why my solution is efficient?
- Using DFS and BFS  
These are efficient and simple algorithms suitable for generating and solving mazes. A stack-based variation of the DFS algorithm was used to avoid problems with recursion (for large mazes, recursion can be very inefficient and unstable).

- Using Pointers When Working with Cells  
Instead of copying each cell a thousand times, the program works with pointers for each cell. This way, when sorting, copying, or storing data, we interact only with the pointers rather than with the entire s_cell structures.

- Avoiding Recursion  
At first glance, using recursion seems like a logical way to generate a maze, but if the maze size is too large, the stack may overflow and the program will terminate abnormally. Instead, queues and stacks were used.
