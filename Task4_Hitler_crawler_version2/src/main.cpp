#include <cpr/cpr.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <vector>

#include <list>
#include <unordered_set>
#include <array>
#include <atomic>
#include <nlohmann/json.hpp>
#include <random>

#define MAX_DEPTH 6
using json = nlohmann::json;

bool log_flag = false;

struct s_page {
	std::string title;
	int depth = 0;
	s_page *parent = nullptr;
};

std::list<s_page> all_pages;
std::array<std::vector<s_page*>, MAX_DEPTH> levels;
std::unordered_set<std::string> visited;

std::mutex mutex;
std::condition_variable condition_v;
std::atomic<int> active_tasks = 0;
std::atomic<bool> found = false;

thread_local std::mt19937 rng(std::random_device{}());

void add_to_level(s_page* p) {
	if (p->depth < 0 || p->depth >= MAX_DEPTH)
		return;
	levels[p->depth].push_back(p);
}

bool has_work() {
	for (auto &level : levels) {
		if (!level.empty()) 
			return true;
	}
	return false;
}

s_page* get_random_page() {
	std::uniform_int_distribution<int> level_dist(0, MAX_DEPTH - 1);

	for (int tries = 0; tries < 10; tries++) {
		int level = level_dist(rng);

		if (!levels[level].empty()) {
			auto& vector = levels[level];

			std::uniform_int_distribution<size_t> dist(0, vector.size() - 1);
			size_t page_number = dist(rng);

			s_page* page = vector[page_number];
			
			vector[page_number] = vector.back();
			vector.pop_back();
			return page;
		}
	}
	return nullptr;
}

void hitler_is_found(s_page *page) {
	std::lock_guard<std::mutex> lock(mutex);
	found = true;
	std::map<std::string, std::string> map;
	std::vector<std::string> path;
	for (s_page *p = page; p != nullptr; p = p->parent) {
		map.emplace(p->title, "p->title");
		path.push_back(p->title);
	}
	std::string request_titles = "";
	for (auto it = map.begin(); it != map.end(); it++) {
		request_titles += it->first;
		if (std::next(it) != map.end()) {
			request_titles += '|';
		}
	}
	cpr::Response response;
	do {
		response = cpr::Get(
				cpr::Url{"https://en.wikipedia.org/w/api.php"}, 
				cpr::Parameters{
					{"action", "query"},
					{"titles", request_titles},
					{"prop", "info"},
					{"inprop", "url"},
					{"format", "json"}
				},	
				cpr::Header {
					{"User-Agent", "Sowwy_but_i_need_to_do_it_for_my_intership/1.0 (https://github.com/MakosHD/Distributed_Lab_Tasks) CPR/1.14.2"}
				}
			);
		//std::cout << response.text << std::endl;
		if (response.error.code != cpr::ErrorCode::OK) {
			std::cerr << response.error.message << std::endl;
			return;
		}
		if (response.status_code == 429) {

			if (log_flag) {
				std::cout << "WAIT        " << response.status_code << " Server said to wait " << response.header["retry-after"] << " seconds " << std::endl;
			}
			std::this_thread::sleep_for(std::chrono::seconds(std::stoi(response.header["retry-after"])));
		}
	} while (response.status_code != 200);

	json j = json::parse(response.text);
	for (auto &[id, j_page] : j["query"]["pages"].items()) {
		map[j_page["title"]] = j_page["fullurl"];
	}

	for (auto it = path.rbegin(); it != path.rend(); ++it) {
		std::cout << map[*it]<< " ->\n";
	}

	std::cout << "https://en.wikipedia.org/wiki/Adolf_Hitler" << std::endl;
	condition_v.notify_all();
}

void scan_page(s_page *page) {
	if (found)
		return;

	std::string plcontinue;
	cpr::Parameters params{
			{"action", "query"},
			{"titles", page->title},
			{"prop", "links"},
			{"pllimit", "max"},
			{"format", "json"}
		};
		do {
		
		if (!plcontinue.empty())
			params.Add({"plcontinue", plcontinue});
		std::this_thread::sleep_for(std::chrono::milliseconds(200));

		auto response = cpr::Get(
			cpr::Url{"https://en.wikipedia.org/w/api.php"}, 
			params,			
			cpr::Header {
				{"User-Agent", "Sowwy_but_i_need_to_do_it_for_my_intership/1.0 (https://github.com/MakosHD/Distributed_Lab_Tasks) CRP/1.14.2"}
			}
		);
		if (response.error.code != cpr::ErrorCode::OK) {
			std::cerr << response.error.message << std::endl;
			return;
		}
		if (found)
			return;
		if (response.status_code == 429) {
			
			if (log_flag) {
				// for (auto &[key,value] : response.header) {
				// 	std::cout << key << ":" << value << std::endl;
				// }
				std::cout << "WAIT     [" << page->depth << "] " << response.status_code << " Server said to wait " << response.header["retry-after"] << " seconds " << page->title << std::endl;
			}
			std::this_thread::sleep_for(std::chrono::seconds(std::stoi(response.header["retry-after"])));
			continue;
		}

		if (log_flag) {
			std::lock_guard<std::mutex> lock(mutex);
			std::cout << "SCANNED: [" << page->depth << "] "<< response.status_code << " " << response.text.size() << " " << page->title << std::endl;
			//std::cout << response.text << std::endl;
		}

		json j = json::parse(response.text);

		if (j["query"]["pages"].contains("-1")) {
			std::cerr << "NOT FOUND: " << page->title << std::endl;
			return;
		}

		if (j["query"].contains("normalized")) {
			page->title = j["query"]["normalized"][0]["to"];
		}

		auto j_pages = j["query"]["pages"];
		
		for (auto &[id, j_page] : j_pages.items()) {
			if (!j_page.contains("links"))
				continue;

			for (auto &link : j_page["links"]) {
				std::string title = link["title"];
				if (title == "Adolf Hitler") {
					hitler_is_found(page);
					return;
				}
				if (page->depth >= MAX_DEPTH - 1){
					continue;
				}
				std::lock_guard<std::mutex> lock(mutex);
				auto [it, inserted] = visited.insert(title);
				if (inserted) {
					all_pages.push_back(s_page{});
					s_page *new_page = &all_pages.back();
					new_page->title = title;
					new_page->depth = page->depth + 1;
					new_page->parent = page;
					add_to_level(new_page);
					condition_v.notify_one();
				}
			}
		}

		if (j.contains("continue")) {
			plcontinue = j["continue"]["plcontinue"].get<std::string>();
		} else {
			plcontinue.clear();
		} 
	 } while (!plcontinue.empty());
}

void* worker(void*) {
	while (true) {
		s_page *page = nullptr;
		{
			std::unique_lock<std::mutex> lock(mutex);
			// for (auto &level : levels) {
			// 	if (!level.empty()) 
			// 		std::cout << true << std::endl;
			// 	else
			// 		std::cout << false << std::endl;
			// }
			condition_v.wait(lock, [] {
				if (found)
					return true;
				for (auto &level : levels) {
					if (!level.empty()) 
						return true;
				}
				if (active_tasks == 0)
					return true;
			return false;
			});
			if (found || (active_tasks == 0 && !has_work())) {
				condition_v.notify_all();
				return nullptr;
			}
			page = get_random_page();
		}

		if (page) {
			active_tasks++;
			scan_page(page);
			active_tasks--;
			condition_v.notify_all();
		}
	}
	return nullptr;
}

int main(int argc, char** argv) {
	int number_of_threads = 4;
	
	for (int i = 1; i < argc; i++) {
		std::string arg = argv[i];

		if (arg.rfind("--threads=", 0) == 0) {
			try {
				number_of_threads = std::stoi(arg.substr(10));
			} 
			catch (...) {
				std::cerr << "Invalid value for --threads" << std::endl;
				return -1;
			}
		} else if (arg == "--log") {
			log_flag = true;
		} else if (arg == "--help") {
			std::cout << std::string("usage: ") + argv[0] + " --threads={number of threads to use} --log" << std::endl;
			return 0;
		} else {
			std::cerr << "Unknown argument: " << arg << std::endl;
				return -1;
		}
	}

	std::cout << "Enter the first Wiki-page: ";
	std::string first_page_url;
	std::getline(std::cin,first_page_url);
	//std::cout << first_page_url << std::endl;
	{
		std::lock_guard<std::mutex> lock(mutex);
		all_pages.push_back(s_page{});

		s_page *start_page = &all_pages.back();
		start_page->title = first_page_url;
		visited.insert(start_page->title);
		add_to_level(start_page);
		std::cout << "Scanning " << start_page->title << " page" << std::endl;
	}


	std::vector<std::thread> threads;
	for (int i = 0; i < number_of_threads; i++) {
		if (found)
			break;
		threads.emplace_back(worker, nullptr);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	for (int i = 0; i < threads.size(); i++) {
		threads[i].join();
	}
	if (!found) {
		std::cout << "Hitler not found" << std::endl;
	}
	return 0;
}
