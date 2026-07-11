#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <algorithm>

int read_number(std::string question) {
	std::string string;
	int number = 0;
	while (true) {
		try {
			std::cout << question;
			std::cin >> string;
			number = std::stoi(string);
			break;
		} catch (...) {
			std::cout << "Something went wrong, try again\n";
		}
	}
	return number;
}

int main() {
	int c = read_number("How much money do you have? : ");
	int n = read_number("How many laptops can you buy? : ");
	int number_of_laptops = read_number("How many laptops are on the market? : ");
	std::vector<int> prices(number_of_laptops);
	std::vector<int> gains(number_of_laptops);

	std::cout << "Enter prices of each laptop in such way: 0 1 3 12 ... \n";
	for (int i = 0; i < number_of_laptops; i++) {
		prices[i] = read_number("");
	}

	std::cout << "Enter gains of each laptop in such way: 0 1 3 12 ... \n";
	for (int i = 0; i < number_of_laptops; i++) {
		gains[i] = read_number("");
	}

	std::vector<std::pair<int,int>> jobs(number_of_laptops);

	for (int i = 0; i < number_of_laptops; i++) {
		jobs[i] = {prices[i],gains[i]};
	}

	std::sort(jobs.begin(),jobs.end());
	int i = 0;
	std::priority_queue<int> max_gain;
	
	while(n--) {
		while (i < number_of_laptops && jobs[i].first <= c) {
			max_gain.push(jobs[i].second);
			i++;
		}
		if (max_gain.empty())
			break;
		c += max_gain.top();
		max_gain.pop();
	}
	std::cout << "At the end of the summer you will have such capital: " << c << std::endl;
	
}
