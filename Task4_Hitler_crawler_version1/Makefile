NAME = task_4

CXX = g++

CXXFLAGS = -Wall
CPPFLAGS = -Iinc -std=c++17 -Ilibs/cpr/include


SRC_DIR = src
OBJ_DIR = obj

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(OBJS) -Lcpr/build/lib -lcpr -lcurl -lssl -lcrypto -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR)

uninstall: clean
	rm -rf $(NAME)

reinstall : uninstall all

.PHONY: all clean uninstall reinstall
