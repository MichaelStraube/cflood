#ifndef GAME_H
#define GAME_H

#include <vector>
#include <string>
#include <random>

struct Rect {
	int x;
	int y;
	int w;
	int h;
};

struct Block {
	Rect rect;
	int color;
};

class Game {
public:
	Game();
	~Game();

	bool quit;

private:
	void check_terminal();
	void init_ncurses();
	void init_blocks();
	void new_game();
	void flood_fill(int i, int j, int old_c, int new_c);
	bool check_winning();
	bool point_in_rect(const Rect &rect, int y, int x);
	bool block_clicked(int y, int x);
	void draw_block(const Block &block);
	void draw_board();
	void draw_menu();
	void run();

	static constexpr int min_width = 80;
	static constexpr int min_height = 28;
	static constexpr int menu_xpos = 64;
	static constexpr int board_size = 14;
	static constexpr int num_colors = 6;
	static constexpr int max_moves = 25;
	int moves;
	bool win;
	bool game_over;
	int fill_color;
	std::vector<Block> menu_blocks;
	std::vector<Block> board_blocks;
	std::string err;
	std::random_device random_device;
	std::mt19937 generator;
	std::uniform_int_distribution<int> dist;
};

#endif // GAME_H
