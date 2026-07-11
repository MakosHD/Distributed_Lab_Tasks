#include <iostream>
#include <string>
#include <stack>
#include <cstdlib>
#include <queue>
#include <list>

#define NUMBER_OF_TRAPS 5
bool DRAW_PATH = false;

void print_error(const std::string &error) {
	std::cerr << error << std::endl;
	exit(-1);
}

enum e_cell {
	Entrance,
	Exit,
	Road,
	Wall,
	Trap,
	Treasure
};

struct s_cell {
	e_cell type = Wall;
	bool visited = false;
	bool part_of_solution = false;
	int x = 0;
	int y = 0;
	int distance = -1;
};

struct s_maze {
	int width = 3;
	int height = 3;
	s_cell** grid;
	s_cell* entrance = nullptr;
	s_cell* exit = nullptr;
};

s_cell& get_random_cell(s_maze &maze) {
	return maze.grid[rand() % (maze.height)][rand() % (maze.width)];
}

s_cell& get_random_border_cell(s_maze &maze, s_cell **interior_cell_out) {
	// find cell on boarder
	int x = 0;
	int y = 0;
	int rooms;
	switch (rand() % 4) {
	case 0: //up
		rooms = (maze.width - 1) / 2;
		x = 1 + 2 * (rand() % rooms); 
		*interior_cell_out = &maze.grid[1][x];
		break;
	case 1: //down
		y = maze.height - 1;
		rooms = (maze.width - 1) / 2;
		x = 1 + 2 * (rand() % rooms); 
		*interior_cell_out = &maze.grid[y - 1][x];
		break;
	case 2: //left
		rooms = (maze.height - 1) / 2;
		y = 1 + 2 * (rand() % rooms); 
		*interior_cell_out = &maze.grid[y][1];
		break;
	default: //right
		rooms = (maze.height - 1) / 2;
		y = 1 + 2 * (rand() % rooms); 
		x = maze.width - 1;
		*interior_cell_out = &maze.grid[y][x - 1];
		break;
	}
	return maze.grid[y][x];
}

void print_maze(s_maze &maze) {
	for (int i = 0; i < maze.height; i++) {
		for (int j = 0; j < maze.width; j++){
			char buffer;
			const char *color = "\x1b[0m";
			switch (maze.grid[i][j].type) {
			case Entrance:
				buffer = 'E';
				color = "\x1b[44m";
				break;
			case Exit:
				buffer = 'X';
				color = "\x1b[42m";
				break;
			case Road:
				buffer = ' ';
				color = "\x1b[40m";
				break;
			case Wall:
				buffer = '#';
				color = "\x1b[47m";
				break;
			case Trap:
				buffer = 'T';
				color = "\x1b[41m";
				break;
			case Treasure:
				buffer = 'G';
				color = "\x1b[43m";
				break;
			default:
				print_error("SOMETHING WRONG WITH e_cell!!KELFJELW;GHW;AJGHL");
			}
			if (DRAW_PATH && maze.grid[i][j].part_of_solution) {
				color = "\x1b[44m";
			}
			std::cout << color << buffer << "\x1b[0m";
		}
		std::cout << std::endl;
	}
}

bool check_cordinates(s_maze& maze, int x, int y) {
	return ((x > 0 && x < maze.width - 1) && 
			(y > 0 && y < maze.height - 1));
}

void find_path(s_maze &maze, s_cell &start, s_cell &finish) {
	if (&start == &finish){
		start.part_of_solution = true;
		return;
	}

	std::queue<s_cell*> queue;
	s_cell* current;
	s_cell* next;

	int distance;
	start.distance = 0;
	queue.push(&start);
	
	int dx[4] = {1,0,-1,0};
	int dy[4] = {0,1,0,-1};
	
	while (!queue.empty()) {
		current = queue.front();
		queue.pop();
		distance = current->distance;

		for (int i = 0; i < 4; i++) {
			if (!check_cordinates(maze, current->x + dx[i], current->y + dy[i]))
				continue;
			next = &maze.grid[current->y + dy[i]][current->x + dx[i]];
			if (next == &finish) {
				finish.distance = distance + 1;
				goto foundFinish;
			}
			if (next->type == Road
				&& next->distance == -1) {
				next->distance = distance + 1;
				queue.push(next);
			}
		}
	}
foundFinish:
	current = &finish;
	while (current->distance != 0) {
		current->part_of_solution = true;
		for (int i = 0; i < 4; i++) {
			if (!check_cordinates(maze, current->x + dx[i], current->y + dy[i]))
				continue;
			next = &maze.grid[current->y + dy[i]][current->x + dx[i]];
			if (next->distance == current->distance -1) {
				current = next;
				break;
			}
		}
	}
	current->part_of_solution = true;
	for (int i = 0; i < maze.height; i++){
		for (int j = 0; j < maze.width; j++) {
			maze.grid[i][j].distance = -1;
		}
	}
}

//depth-first serach
void generate_maze(s_maze &maze) {
	std::stack<s_cell*> stack;
	s_cell *generator_start;
	maze.entrance = &get_random_border_cell(maze, &generator_start);
	maze.entrance->type = Entrance;
	generator_start->visited = true;
	generator_start->type = Road;

	stack.push(generator_start);

	while(!stack.empty()) {
		s_cell *current_cell = stack.top();
		stack.pop();
	
		//check neigbors cells
		int dx[4] = {2,0,-2,0};
		int dy[4] = {0,2,0,-2};

		s_cell* avalible_cells[4] = {nullptr,nullptr,nullptr,nullptr};
		int number_of_avalible_cells = 0;
		for (int i = 0; i < 4; i++)	{
			int nx = current_cell->x + dx[i];
			int ny =  current_cell->y + dy[i];
			if (!check_cordinates(maze, nx, ny))
				continue;
			if (maze.grid[ny][nx].visited)
				continue;
			avalible_cells[number_of_avalible_cells] = &maze.grid[ny][nx];
			number_of_avalible_cells++;
		}
		if (number_of_avalible_cells == 0){
			continue; //dead end
		}
		
		//chose random next cell
		stack.push(current_cell);
		s_cell *next_cell = avalible_cells[rand() % number_of_avalible_cells];
		
		//remove wall between chosen and current cell
		maze.grid[(current_cell->y + next_cell->y) / 2][(current_cell->x + next_cell->x) / 2].type = Road;
		next_cell->visited = true;
		next_cell->type = Road;
		stack.push(next_cell);
	}
	s_cell *treasure_cell = nullptr;
	if (rand() % 2) {
		do {
			treasure_cell = &get_random_cell(maze);
		} while (treasure_cell->type != Road);

		treasure_cell->type=Treasure;	
	}

	s_cell *exit_cell = nullptr;
	s_cell *exit_inside_cell = nullptr;
	do {
		exit_cell = &get_random_border_cell(maze, &exit_inside_cell);
	} while (!(exit_inside_cell->type == Road || exit_inside_cell->type == Treasure) || exit_cell->type == Entrance);
	exit_cell->type = Exit;
	if (treasure_cell) {
		find_path(maze, *generator_start, *treasure_cell);
		find_path(maze, *treasure_cell, *exit_inside_cell);	
	} else {
		find_path(maze, *generator_start, *exit_inside_cell);
	}
	int number_of_traps = rand() % NUMBER_OF_TRAPS;
	int protection_of_many_atemps = 0;
	for (int i = 0; i < number_of_traps; i++) {
		s_cell *trap = nullptr;
		int number_of_traps_on_path = 0;
		do {
			trap = &get_random_cell(maze);
			protection_of_many_atemps++;
			if (protection_of_many_atemps > 10)
				break;
		} while ((trap->part_of_solution && number_of_traps_on_path >= 2) || trap->type != Road);
		if (protection_of_many_atemps > 10)
			break;
		if (trap->part_of_solution)
			number_of_traps_on_path++;
		trap->type=Trap;
		
	}
}

int main(int argc, char* argv[]) {
	s_maze maze;

	for (int i = 1; i < argc; i++) {
		// std::cout << "HAI!" << std::endl;
		// std::cout << argc << std::endl;
		std::string arg = argv[i];

		if (arg.rfind("--width=", 0) == 0) {
			try {
				maze.width = std::stoi(arg.substr(8)); 
			} 
			catch (...) {
				std::cerr << "Invalid value for --width" << std::endl;
				return -1;
			}
		} else if (arg.rfind("--height=", 0) == 0) {
			try {
				maze.height = std::stoi(arg.substr(9)); 
			} 
			catch (...) {
				std::cerr << "Invalid value for --height" << std::endl;
				return -1;
			}
		} else if (arg == "--path") {
			DRAW_PATH = true;
		} else if (arg == "--help") {
			std::cout << std::string("usage: ") + argv[0] + " --width={3} --height={3} --path --help" << std::endl;
			return 0;
		} else {
			std::cerr << "Unknown argument: " << arg << std::endl;
				return -1;
		}
	}

	srand(time(0));
	if (maze.width < 3 || maze.height < 3)
		print_error("The dimensions of the maze are too small. The minimum dimensions are 3x3");
	if (!(maze.width % 2) || !(maze.height % 2)) {
		std::cout << "The numbers must be odd. The dimensions of the maze have been changed to ";
		if (!(maze.width % 2))
			maze.width++;
		if (!(maze.height % 2))
			maze.height++;
		std::cout << maze.width << "x" << maze.height << std::endl;
	}

	maze.grid = new s_cell* [maze.height];
	for (int i = 0; i < maze.height; i++){
		maze.grid[i] = new s_cell[maze.width];
		for (int j = 0; j < maze.width; j++) {
			maze.grid[i][j].x = j;
			maze.grid[i][j].y = i;
		}
	}
	generate_maze(maze);

	print_maze(maze);

	//clear memory
	for (int i = 0; i < maze.height; i++) {
		delete[] maze.grid[i];
	}
	delete[] maze.grid;
}
