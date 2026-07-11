#include "csv.hpp"
#include <iostream>
#include <unordered_map>
#include <unordered_set>


int main(int argc, char** argv) {
	//std::cout << argc << std::endl;
	if (argc != 3) {
		std::cerr << "usage: ./task3 path_to_first_day path_to_second_day";
		return -1;
	}
	std::unordered_map<std::string, std::unordered_set<std::string>> day_1;
	

	for (auto &row: csv::CSVReader(argv[1])) {
		day_1[row["user_id"].get<std::string>()].insert(row["product_id"].get<std::string>());
	}

	std::unordered_set<std::string> result_users;

	for (auto &row: csv::CSVReader(argv[2])) {
		std::string user = row["user_id"].get<std::string>();
		auto it = day_1.find(user);
		if (it != day_1.end()) {
			auto &day_1_products = it->second;
			if (day_1_products.find(row["product_id"].get<std::string>()) == day_1_products.end()) {
				result_users.insert(user);
			}
		}
	}
	for (auto user: result_users) {
		std::cout << user << std::endl;
	}
}