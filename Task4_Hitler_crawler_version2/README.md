# 4. Hitlew crawler version 2 (5h05m)

![Image of interface of hitlew crawler](/Task4_Hitler_crawler_version2/pics/image_1.png)

## Description
A console program for finding a path to Hitler on Wikipedia! With it, users can quickly find a connection between their favorite Wikipedia page and an “outstanding historical figure.”

## Solution
The code is based on [version 1](/Task4_Hitler_crawler_version1/README.md) but with significant improvements which made it possible to achieve the desired result. The main difference is the use of the Wikipedia API. Thanks to it, speed and efficiency have increased significantly. Now, instead of manually scanning every page, the API provides a ready-made list of links found on any page.

The program's structure has also changed. There is a single global `list all_pages`. Its main purpose is to store all pages, whether they have been scanned or not. Next, there is an array called `levels` that stores vectors of pointers to pages. Its main purpose is to store pointers to pages that still need to be scanned. The pages in this array are sorted by their depths. `unsorted_set visited` stores the names of all the pages that have been scanned (we don't want to create a loop where two pages link to each other). 

Now, when a new thread is launched, it randomly selects one page from the levels and begins scanning it. Randomness is necessary here because if we were to scan page by page, level by level, it could take a very, very long time (each page has approximately 0 to 700 links, and with each level, the total number of pages increases dramatically). Randomization helps speed up the overall process, because Hitler is usually not on the first or second level, but more often on levels 3–5. 

This implementation uses the Wikipedia API, and according to Wikipedia’s rules, the program can send 200 requests per minute. This number is much better than the 10 requests per minute in version 1. Experiments have shown that 4 threads are enough to quickly find Hitler without spending most of the time waiting for a minute to pass (so that the request limits reset). This implementation also correctly handles various status codes; when it receives a 429 status code, it waits the appropriate number of seconds.

## How to use
![alt text](/Task4_Hitler_crawler_version2/pics/image_3.png)

When you launch the program, it asks you to enter the title of the first page from which you want to start the search. The page title is the general title of the page, which is written in a larger font, as shown in the photo

![alt text](/Task4_Hitler_crawler_version2/pics/image_2.png)

Once you've entered the name, the program will start searching for the path, and after a while it will find the correct one. However, because randomness is used, there is no guarantee that the path will be the same every time when starting from the same page. 

### **Arguments**
If you use the `--help` argument, a list of all possible arguments will be displayed 

![alt text](/Task4_Hitler_crawler_version2/pics/image_4.png)

`--thread={4}` is used to specify the number of threads to run. Usually, 4 is enough

`--log` argument is responsible for logging; if provided, it will display which pages were scanned and their results 

![alt text](/Task4_Hitler_crawler_version2/pics/image_5.png)  
![alt text](/Task4_Hitler_crawler_version2/pics/image_6.png)

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
sudo apt-get install git cmake build-essential libcurl4-openssl-dev libssl-dev
```

After successful installation, you need to clone this repository

```
git clone https://github.com/MakosHD/Distributed_Lab_Tasks.git
```

Go to the repository and create a folder for the build

```
cd Distributed_Lab_Tasks/Task4_Hitler_crawler_version2/
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
./task4
```

## Why my solution is efficient?

- Each page is processed only once  
The `unsorted_set visited` is used to keep track of which pages have been scanned. This helps avoid situations where two pages link to each other, which could create an infinite loop. 

- Using of threads  
Using threads has significantly sped up the overall search process. Usually, when we make requests, we spend most of the time simply waiting for a response, but with threads, we send multiple requests simultaneously, which increases the overall speed. You can also easily adjust the number of threads as needed, but the main thing is to avoid any restrictions imposed by Wikipedia.

- Interaction Between Threads  
A quick-stop mechanism was developed, that is, if “hitler” is found, all threads will be notified and will terminate as quickly as possible.

- Working with pointers
Instead of working with s_page structures, the entire search algorithm works with pointers, which speeds up the process and reduces the load on memory

- A pinch of randomness
The program doesn't scan every link in sequence; that would take a very long time because Hitler is usually found at levels 3–5 rather than 0–3. If the scanning were done sequentially, it would take a long time. Randomization helps scan both the initial pages for adding branches and deep pages, which speeds up the search.