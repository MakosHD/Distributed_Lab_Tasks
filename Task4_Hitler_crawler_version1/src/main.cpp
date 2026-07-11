#include <cpr/cpr.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>
#include <list>
#include <atomic>
#include <unistd.h>

bool log_flag = false;

struct s_page {
	std::string id;
	int depth = 0;
	s_page *parent = nullptr;
};

std::list<s_page> all_pages;
std::queue<s_page*, std::list<s_page*>> queue;
std::unordered_set<std::string> visited;

std::mutex mutex_queue;
std::condition_variable condition_v;
std::atomic<int> active_tasks = 0;
std::atomic<bool> found = false;

std::string find_id(std::string &string, size_t pos, size_t size_of_path = 6) {
	std::string result;
	auto it = string.begin() + pos + size_of_path;
	while (it != string.end() && *it != '"') {
		result += *it;
		it++;
	}
	return result;
}

void scan_page(s_page *page) {
	if (found)
		return;
	cpr::Response response = cpr::Get(cpr::Url{std::string("https://en.wikipedia.org/wiki/") + page->id});
	if (found)
		return;
	while (response.status_code == 429) {
		if (log_flag)
			std::cout <<"Retry 429:" << page->id << std::endl;
		sleep(0.2);
		response = cpr::Get(cpr::Url{std::string("https://en.wikipedia.org/wiki/") + page->id});
	}
	if (response.status_code == 404) {
		return;
	}
	if (response.text.find("/wiki/Adolf_Hitler") != std::string::npos) {//are we lucky?
		std::vector<std::string> backtracking;
		for (s_page *p = page; p != nullptr; p = p->parent) {
			backtracking.push_back(p->id);
		}
		for (auto it = backtracking.rbegin(); it != backtracking.rend(); ++it) {
			std::cout << "https://en.wikipedia.org/wiki/" << *it << " ->\n";
		}
		std::cout << "https://en.wikipedia.org/wiki/Adolf_Hitler" << std::endl;
		std::lock_guard<std::mutex> lock(mutex_queue);

		found = true;
		while (!queue.empty()){
			queue.pop();
		}
		condition_v.notify_all();
		return;
	}
	if (page->depth >= 6){
		return;
	}

	size_t position = response.text.find("/wiki/");	
	if (log_flag) {
		std::cout << "SCANNED: [" << page->depth << "] "<< response.status_code << " " << response.text.size() << " en.wikipedia.org/wiki/" << page->id << std::endl;
	}
	while (position != std::string::npos) {
		std::string id = find_id(response.text, position);
		if (id != page->id) {
			std::lock_guard<std::mutex> lock(mutex_queue);

			auto [it, inserted] = visited.insert(id);
			if (inserted) {
				all_pages.push_back(s_page{});
				s_page *new_page = &all_pages.back();
				new_page->id = id;
				new_page->depth = page->depth + 1;
				new_page->parent = page;
				queue.push(new_page);
				condition_v.notify_one();
			}
		}
		position = response.text.find("/wiki/",position + 6);	
	}
}

void* worker(void*) {
	while (true) {
		sleep(1);
		s_page *page = nullptr;
		{
			std::unique_lock<std::mutex> lock(mutex_queue);
			condition_v.wait(lock, [] {return found || !queue.empty() || active_tasks == 0;});
			if (found) {
				return nullptr;
			}
			if (queue.empty() && active_tasks == 0) {
				condition_v.notify_all();
				return nullptr;
			}
			if (queue.empty())
				continue;
			page = queue.front();
			queue.pop();
			active_tasks++;
		}
		scan_page(page);

		{
			std::lock_guard<std::mutex> lock(mutex_queue);
			active_tasks--;
			condition_v.notify_all();
		}
	}
	return nullptr;
}

int main(int argc, char** argv) {
	int number_of_threads = 1;
	
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
	std::cin >> first_page_url;
	all_pages.push_back(s_page{});
	s_page *start_page = &all_pages.back();
	start_page->id=find_id(first_page_url, first_page_url.find("/wiki/"));
	std::cout << "Scanning en.wikipedia.org/wiki/" << start_page->id << std::endl;
	queue.push(start_page);
	visited.insert(start_page->id);
	

	std::vector<std::thread> threads;
	for (int i = 0; i < number_of_threads; i++) {
		threads.emplace_back(worker, nullptr);
		sleep(1);
	}
	for (int i = 0; i < number_of_threads; i++) {
		threads[i].join();
	}
	if (!found) {
		std::cout << "Hitler not found" << std::endl;
	}
	return 0;
}