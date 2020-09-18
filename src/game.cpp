#include "game.h"
#include <ncurses.h>
#include <iostream>

Game::Game()
	: quit(false)
	, moves(0)
	, win(false)
	, game_over(false)
	, generator(random_device())
	, dist(std::uniform_int_distribution<int>(1, 6))
{
	init_ncurses();
	init_blocks();
	run();
}

Game::~Game()
{
	endwin();

	if (!err.empty()) {
		std::cout << err << std::endl;
	}
}

void Game::check_terminal()
{
	int h, w;

	if (!has_colors()) {
		err = "Your terminal does not support color.";
		quit = true;
	}

	getmaxyx(stdscr, h, w);
	if (h < min_height || w < min_width) {
		err = "Your terminal is too small, you need at least " +
		      std::to_string(min_width) + " x " +
		      std::to_string(min_height) + ".";
		quit = true;
	}
}

void Game::init_ncurses()
{
	initscr();
	clear();
	keypad(stdscr, TRUE);
	cbreak();
	noecho();
	curs_set(0);
	mousemask(BUTTON1_PRESSED, NULL);

	check_terminal();

	start_color();
	init_pair(1, COLOR_YELLOW, COLOR_YELLOW);
	init_pair(2, COLOR_CYAN, COLOR_CYAN);
	init_pair(3, COLOR_RED, COLOR_RED);
	init_pair(4, COLOR_MAGENTA, COLOR_MAGENTA);
	init_pair(5, COLOR_GREEN, COLOR_GREEN);
	init_pair(6, COLOR_WHITE, COLOR_WHITE);
}

void Game::init_blocks()
{
	for (int i = 0; i < num_colors; i++) {
		Block block;
		block.rect.x = menu_xpos;
		block.rect.y = 1 + 3 * (i + 1);
		block.rect.w = 3;
		block.rect.h = 2;
		block.color = i + 1;
		menu_blocks.push_back(block);
	}

	for (int x = 0; x < board_size; x++) {
		for (int y = 0; y < board_size; y++) {
			Block block;
			block.rect.x = x * 4;
			block.rect.y = y * 2;
			block.rect.w = 4;
			block.rect.h = 2;
			block.color = dist(generator);
			board_blocks.push_back(block);
		}
	}
}

void Game::new_game()
{
	for (auto &i : board_blocks) {
		i.color = dist(generator);
	}
	fill_color = board_blocks[0].color;

	moves = 0;
	win = false;
	game_over = false;
}

void Game::flood_fill(int i, int j, int old_c, int new_c)
{
	if (board_blocks[j * board_size + i].color != old_c || new_c == old_c) {
		return;
	}

	board_blocks[j * board_size + i].color = new_c;

	if (j + 1 < board_size) {
		flood_fill(i, j + 1, old_c, new_c);
	}
	if (j - 1 >= 0) {
		flood_fill(i, j - 1, old_c, new_c);
	}
	if (i - 1 >= 0) {
		flood_fill(i - 1, j, old_c, new_c);
	}
	if (i + 1 < board_size) {
		flood_fill(i + 1, j, old_c, new_c);
	}

	return;
}

bool Game::check_winning()
{
	for (const auto &i : board_blocks) {
		if (i.color != board_blocks[0].color) {
			return false;
		}
	}
	return true;
}

bool Game::point_in_rect(const Rect &rect, int y, int x)
{
	if ((rect.x <= x && x < rect.x + rect.w) &&
	    (rect.y <= y && y < rect.y + rect.h)) {
		return true;
	}
	return false;
}

bool Game::block_clicked(int y, int x)
{
	for (const auto &i : menu_blocks) {
		if (point_in_rect(i.rect, y, x)) {
			fill_color = i.color;
			return true;
		}
	}

	for (const auto &i : board_blocks) {
		if (point_in_rect(i.rect, y, x)) {
			fill_color = i.color;
			return true;
		}
	}

	return false;
}

void Game::draw_block(const Block &block)
{
	attron(COLOR_PAIR(block.color));

	for (int i = 0; i < block.rect.h; i++) {
		mvhline(block.rect.y + i, block.rect.x, 0, block.rect.w);
	}

	attroff(COLOR_PAIR(block.color));
}

void Game::draw_board()
{
	for (const auto &i : board_blocks) {
		draw_block(i);
	}
}

void Game::draw_menu()
{
	mvprintw(0, menu_xpos, "New : Space");
	mvprintw(2, menu_xpos, "Exit : Q");

	for (int i = 0; i < num_colors; i++) {
		draw_block(menu_blocks[i]);
		int y = menu_blocks[i].rect.y;
		int w = menu_blocks[i].rect.w;
		int h = menu_blocks[i].rect.h;
		mvprintw(y + h - 1, menu_xpos + w, " : %d", i + 1);
	}

	mvprintw(23, menu_xpos, "%d", moves);
	printw(" / %d", max_moves);

	if (win) {
		attron(A_BLINK);
		mvprintw(26, menu_xpos - 1, "* You win *");
		attroff(A_BLINK);
	}

	if (!win && moves == max_moves) {
		attron(A_BLINK);
		mvprintw(26, menu_xpos - 2, "* You loose *");
		attroff(A_BLINK);
	}
}

void Game::run()
{
	new_game();

	while (!quit) {
		clear();

		flood_fill(0, 0, board_blocks[0].color, fill_color);
		win = check_winning();

		if (win || (!win && moves == max_moves)) {
			game_over = 1;
		}

		draw_board();
		draw_menu();
		refresh();

		int ch = wgetch(stdscr);

		if (ch == 'q' || ch =='Q') {
			quit = true;
		}

		if (ch == ' ') {
			new_game();
		}

		if (game_over) {
			continue;
		}

		switch (ch) {
		case KEY_MOUSE:
			MEVENT event;
			if (getmouse(&event) == OK) {
				if (block_clicked(event.y, event.x)) {
					moves++;
				}
			}
			break;
		case '1':
			fill_color = 1;
			moves++;
			break;
		case '2':
			fill_color = 2;
			moves++;
			break;
		case '3':
			fill_color = 3;
			moves++;
			break;
		case '4':
			fill_color = 4;
			moves++;
			break;
		case '5':
			fill_color = 5;
			moves++;
			break;
		case '6':
			fill_color = 6;
			moves++;
			break;
		default:
			break;
		}
	}
}
