# Hitler crawler version 1

![alt text](/Task4_Hitler_crawler_version1/pics/image_1.png)

> [!WARNING]  
> This version is considered a failure. For the final result, go to [version 2](/Task4_Hitler_crawler_version2/README.md). For this reason, this version does not include instructions on how to install or run it, because the way it works violates Wikipedia's rules.

## Description
This is a console program whose main task is to scan Wiki pages in search of Adolf Hitler! You specify the starting page, and the program tries to find a path from that page to the page about Adolf Hitler.

## Solution
Multithreading is the foundation of this program. Most of the time spent on a request is spent waiting, so using multiple threads can speed up the overall process. There is an `s_page` structure that contains all the necessary information about a page, namely the id (a string that stores the page name), depth, and a pointer to the parent. All pages are stored in a `list`, and a queue is used to determine which page should be scanned next. The `unsorted_set` `visited` is used to keep track of which pages have already been scanned so they aren’t scanned again. All global variables are protected using mutexes and conditional variables.



## Why my solution is efficient?
- This version is unefficient at all  
The main problem is that the program simply scans every wiki page for links that start with “https://en.wikipedia.org/wiki/”. This isn't efficient because it takes a long time to read the entire response, and a lot of junk data gets picked up this way. Another problem is that the program is multithreaded, but Wikipedia has strict rules regarding the rate of requests per minute. In this implementation, the allowed number of requests is 10 per minute, which is very few and will take a long time. Also, the program ignores all errors, including 429, so most requests (after the first 10) are simply ignored.
![alt text](/Task4_Hitler_crawler_version1/pics/image_2.png)