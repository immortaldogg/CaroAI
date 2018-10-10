#include "GameplayScene.h"
#include "SimpleAudioEngine.h"
#include "ui/CocosGUI.h"
#include <cmath>
#include "Coor.h"
#include <vector>
#include "HomeScene.h"
#include <ctime>
#include <iostream>
#include <fstream>
#include "Helper.h"

USING_NS_CC;

void Gameplay::initAIMove() {
	if (recordPlay(boardSize / 2, boardSize / 2, 0)) {
		float m = boardSize / 2;
		drawPlay(m * cellSize + marginX, m* cellSize + marginY, 0);
		turn++;
	};
}

void Gameplay::resetGame() {
	turn = 0;
	game_over = false;
	coor_stack.clear();
	move_stack.clear();
	play_stack->removeAllChildren();
	for (int i = 0; i < boardSize; i++)
		for (int j = 0; j < boardSize; j++) {
			board[i][j] = -1;
		}
	if (mode == 1) initAIMove();
	announcer->setVisible(false);
	reminder->setVisible(false);
}

Scene* Gameplay::createScene()
{
    return Gameplay::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool Gameplay::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	// background image
	auto bg = Sprite::create("bg_brown.jpg");
	bg->setAnchorPoint(Vec2(0.5, 0.5));
	bg->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	bg->setOpacity(125);

	this->addChild(bg, -1);

	// play stack
	play_stack = Node::create();

	// draw board
	auto draw = DrawNode::create();
	draw->setContentSize(Size(cellSize * boardSize, cellSize * boardSize));

	draw->drawSolidRect(Vec2(0, cellSize * boardSize), Vec2(cellSize * boardSize, 0), Color4F::WHITE); // background
	draw->setColor(Color3B::WHITE);
	for (int i = 0; i <= boardSize; i++) {
		//draw->drawSegment(Point(cellSize * i, 0), Point(cellSize*i, cellSize * boardSize), 1, Color4F::BLUE);
		draw->drawSegment(Point(cellSize * i, 0), Point(cellSize*i, cellSize * boardSize), 1, Color4F(244, 185, 66, 1));
		/*draw->drawSegment(Point(0, cellSize * i), Point(cellSize * boardSize, cellSize * i), 1, Color4F::BLUE);*/
		draw->drawSegment(Point(0, cellSize * i), Point(cellSize * boardSize, cellSize * i), 1, Color4F(244, 185, 66, 1));
	}
	draw->setAnchorPoint(Vec2(0.5f, 0.5f));
	draw->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
	marginX = visibleSize.width / 2 - draw->getContentSize().width / 2;
	marginY = visibleSize.height / 2 - draw->getContentSize().height / 2;
	this->addChild(draw);

	// announcer	
	announcer = Sprite::create();
	announcer->setAnchorPoint(Vec2(0.5, 0.5));
	announcer->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
	announcer->setVisible(false);
	this->addChild(announcer, 3);

	// reminder
	reminder = Label::createWithTTF(TTFConfig("fonts/arial.ttf", 38), "Click Reset to play again!");
	reminder->setColor(Color3B::BLACK);
	reminder->setAnchorPoint(Vec2(0.5f, 0.5f));
	reminder->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 - announcer->getContentSize().height - reminder->getContentSize().height));
	reminder->setVisible(false);
	this->addChild(reminder, 3);

	// reset button
	//auto reset_btn = ui::Button::create();
	auto reset_btn = ui::Button::create("ResetNormal.png", "ResetSelected.png");
	reset_btn->addClickEventListener([&](Ref* sender) {
		log("reset pressed");
		resetGame();
	});
	reset_btn->setScale((float)cellSize * 3.5 / (float)BTN_SPRITE_WIDTH);
	reset_btn->setAnchorPoint(Vec2(0.0, 1.0));
	reset_btn->setPosition(Vec2(visibleSize.width / 2 - draw->getContentSize().width / 2, visibleSize.height / 2 - draw->getContentSize().height / 2 - DrawMarginBtm));
	this->addChild(reset_btn);

	// back to main menu button
	auto back_btn = ui::Button::create("BackNormal.png", "BackSelected.png");
	back_btn->addClickEventListener([&](Ref* sender) {
		Director::getInstance()->replaceScene(Home::create());
	});
	back_btn->setScale((float)cellSize * 3.5 / (float)BTN_SPRITE_WIDTH);
	back_btn->setAnchorPoint(Vec2(0.0, 1.0));
	back_btn->setPosition(Vec2(visibleSize.width / 2 - draw->getContentSize().width / 2 + cellSize * 4, visibleSize.height / 2 - draw->getContentSize().height / 2 - DrawMarginBtm));
	this->addChild(back_btn);

	// undo button
	undo_btn = ui::Button::create("UndoNormal.png", "UndoSelected.png");
	undo_btn->addClickEventListener([&](Ref* sender) {
		//
		if (game_over) return;
		// if single player
		if (mode == 1) {
			// only viable if user's turn
			if (turn % 2 == 1) {

				// can undo to the first move by human only, the limit
				if (!move_stack.empty() && turn >= 2) {
					if (turn == 2) {
						play_stack->removeChild(move_stack.back(), true); move_stack.popBack();
						board[coor_stack.back().i][coor_stack.back().j] = -1; coor_stack.pop_back();
						turn -= 1;
					}
					else {
						play_stack->removeChild(move_stack.back(), true); move_stack.popBack();
						board[coor_stack.back().i][coor_stack.back().j] = -1; coor_stack.pop_back();

						play_stack->removeChild(move_stack.back(), true); move_stack.popBack();
						board[coor_stack.back().i][coor_stack.back().j] = -1; coor_stack.pop_back();

						// uncheck 2 cells in board
						// minus 2 turn
						turn -= 2;
					}
				}
			}
		}
		// if multiplayer
		else {

			// find the  last play sprite
			// delete sprite
			// find the last play's color
			// uncheck the play
			if (!move_stack.empty()) {
				play_stack->removeChild(move_stack.back(), true); move_stack.popBack();
				board[coor_stack.back().i][coor_stack.back().j] = -1; coor_stack.pop_back();
			}
			// minus 1 turn
			turn -= 1;
		}
	});
	undo_btn->setScale((float)cellSize * 3.5 / (float)BTN_SPRITE_WIDTH);
	undo_btn->setAnchorPoint(Vec2(0.0, 1.0));
	undo_btn->setPosition(Vec2(visibleSize.width / 2 - draw->getContentSize().width / 2 + cellSize * 8, visibleSize.height / 2 - draw->getContentSize().height / 2 - DrawMarginBtm));
	this->addChild(undo_btn);

	// load button
	auto load_btn = ui::Button::create();
	load_btn->setTitleText("Load");
	load_btn->addClickEventListener([&](Ref* sender) {

		resetGame();

		int step = 44;
		std::string savePath = FileUtils::getInstance()->getWritablePath() + "10062018_140657_p.txt";
		std::ifstream inFile(savePath);

		int loop = 0;
		int i_c, j_c, tu;

		while (loop <= step) {
			inFile >> tu >> i_c >> j_c ;
			if (loop > 0) recordAndDraw(i_c, j_c, tu % 2);
			loop++;
		}
		turn = loop;

		inFile.close();
	});
	load_btn->setPosition(Vec2(visibleSize.width - load_btn->getContentSize().width, load_btn->getContentSize().height * 10));
	load_btn->setAnchorPoint(Vec2(0.0, 0.0));
	this->addChild(load_btn);
	load_btn->setVisible(false);

	// play stack
	this->addChild(play_stack, 1);


	// inti value
	mode = UserDefault::getInstance()->getIntegerForKey("Mode");
	resetGame();

	// *************************** DEBUG ********************************
	debug_layer = Node::create();
	debug_layer->setVisible(false);

	// mouse position label
	mouse_position_label = Label::createWithTTF(TTFConfig("fonts/Marker Felt.ttf", 12.0), "mosu cou");
	mouse_position_label->setAnchorPoint(Vec2(0.0f, 0.0f));
	mouse_position_label->setPosition(Vec2(0, visibleSize.height - mouse_position_label->getContentSize().height));
	mouse_position_label->setColor(Color3B::RED);
	//this->addChild(mouse_position_label, 4);
	debug_layer->addChild(mouse_position_label);

	// turn label
	turn_label = Label::createWithTTF(TTFConfig("fonts/arial.ttf", 37.0), "Turn--->");

	if (mode == 2) turn_label->setString("<---Turn");

	turn_label->setAnchorPoint(Vec2(0.5, 0.5));
	turn_label->setPosition(Vec2(visibleSize.width/2, visibleSize.height - turn_label->getContentSize().height));
	turn_label->setColor(Color3B(244, 185, 66));
	this->addChild(turn_label);

	// brands
	auto x_brand = Sprite::create("alpha_x.png");

	x_brand->setAnchorPoint(Vec2(0.0, 0.0));
	x_brand->setScale((float)cellSize * 2 / (float)SPRITE_SIZE);
	x_brand->setPosition(Vec2(visibleSize.width / 2 - draw->getContentSize().width / 2, visibleSize.height / 2 - draw->getContentSize().height / 2 + draw->getContentSize().height));

	auto o_brand = Sprite::create("alpha_o.png");

	o_brand->setAnchorPoint(Vec2(1.0, 0));
	o_brand->setScale((float)cellSize * 2 / (float)SPRITE_SIZE);
	o_brand->setPosition(Vec2(visibleSize.width / 2 - draw->getContentSize().width / 2 + draw->getContentSize().width, visibleSize.height / 2 - draw->getContentSize().height / 2 + draw->getContentSize().height));


	this->addChild(x_brand);
	this->addChild(o_brand);

	// move value drawing
	value_stack = Node::create();
	debug_layer->addChild(value_stack);
	this->addChild(debug_layer, 1);

	// debug button
	toggle_btn = ui::Button::create();
	toggle_btn->setTitleText("Toogle AI's value map");
	toggle_btn->addClickEventListener([&](Ref* sender) {

		if (debug_layer->isVisible()) {
			debug_layer->setVisible(false);
		}
		else {
			debug_layer->setVisible(true);
		}

	});
	toggle_btn->setPosition(Vec2(visibleSize.width/2 - toggle_btn->getContentSize().width/2, toggle_btn->getContentSize().height));

	toggle_btn->setAnchorPoint(Vec2(0.0, 1.0));
	toggle_btn->setPosition(Vec2(visibleSize.width / 2 - draw->getContentSize().width / 2 + cellSize * 12, visibleSize.height / 2 - draw->getContentSize().height / 2 - DrawMarginBtm));

	this->addChild(toggle_btn);



	// coor
	Node* column_row_display = Node::create();
	for (int i = 0; i < boardSize; i++) {
		auto row = Label::createWithTTF(TTFConfig("fonts/arial.ttf", 14), std::to_string(i));
		row->setPosition(Vec2(marginX + cellSize * i + cellSize / 2, marginY + boardSize * cellSize + row->getContentSize().height));
		row->setColor(Color3B::RED);
		/*row->setScale((float)cellSize / row->getContentSize().width);*/
		auto col = Label::createWithTTF(TTFConfig("fonts/arial.ttf", 14), std::to_string(i));
		col->setPosition(Vec2(marginX - col->getContentSize().width, marginY + (boardSize - i - 1) * cellSize + cellSize / 2));
		col->setColor(Color3B::RED);
		/*col->setScale((float)cellSize / col->getContentSize().width);*/

		column_row_display->addChild(row);
		column_row_display->addChild(col);
	}
	/*this->addChild(column_row_display, 0);*/
	debug_layer->addChild(column_row_display, 0);
	// *************************** DEBUG ********************************

    return true;
}

void Gameplay::onEnter() {
	Scene::onEnter();

	auto _mouseListener = EventListenerMouse::create();
	_mouseListener->onMouseMove = CC_CALLBACK_1(Gameplay::onMouseMove, this);
	_mouseListener->onMouseDown = CC_CALLBACK_1(Gameplay::onMouseDown, this);


	auto _touchListener = EventListenerTouchOneByOne::create();

	// trigger when you push down
	_touchListener->onTouchBegan = [=](Touch* touch, Event* event) {
		// your code

		afterTouch(touch->getLocation().x, touch->getLocation().y);

		return true; // if you are consuming it
	};

	// Add listener
	_eventDispatcher->addEventListenerWithSceneGraphPriority(_touchListener, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(_mouseListener, this);

	//AI made move first if the game mode is vs AI
	if (mode == 1) initAIMove();
}

void Gameplay::onMouseMove(Event *event) {
	EventMouse* e = (EventMouse*)event;
	if (mouse_position_label != nullptr && e != nullptr) 
		mouse_position_label->setString("Mouse coords -> " + std::to_string(e->getCursorX()) + ":" + std::to_string(e->getCursorY()));
}

Vec2* Gameplay::snap(float x, float y) {
	// check if click outside board
	if (x < marginX || x > boardSize * cellSize + marginX || y < marginY || y > boardSize * cellSize + marginY) {
		return NULL;
	}

	// check if not clicked on the border of cell
	if ( (int)(x - marginX) % cellSize == 0 || (int)(y - marginY) % cellSize == 0 
		|| (int)(x - marginX + 1) % cellSize == 0 || (int)(y - marginY + 1) % cellSize == 0 ) {

		return NULL;
	}

	float xx = x - ((int)(x - marginX) % cellSize);
	float yy = y - ((int)(y - marginY) % cellSize);

	Vec2 res = Vec2(xx, yy);

	return &res;
}

void Gameplay::announceWinner() {
	if (mode == 1) {
		if (winner(board) == 0) {
			announcer->setTexture("AnnouncerAIWins.png");
		}
		else {
			announcer->setTexture("AnnouncerYouWins.png");
		}
	}
	else {
		if (winner(board) == 0) {
			announcer->setTexture("AnnouncerP1Wins.png");
		}
		else {
			announcer->setTexture("AnnouncerP2Wins.png");
		}
	}
	announcer->setVisible(true);
	reminder->setVisible(true);

	// write replay of the game
	std::time_t t = std::time(0);
	std::tm* now = std::localtime(&t);
	int year = now->tm_year + 1900;
	int month = now->tm_mon + 1;
	int day = now->tm_mday;
	int hour = now->tm_hour;
	int min = now->tm_min;
	int sec = now->tm_sec;

	std::string name = Helper::format2(month) + Helper::format2(day) + Helper::format2(year) + "_" + Helper::format2(hour) + Helper::format2(min) + Helper::format2(sec) + ".txt";
	std::string filePath = FileUtils::sharedFileUtils()->getWritablePath() + name;

	std::ofstream myfile(filePath);

	myfile.flush();
	for (int i = 0; i < coor_stack.size(); i++) {
		myfile << std::to_string(i) << " " << std::to_string(coor_stack[i].i) << " " << std::to_string(coor_stack[i].j) << std::endl;
	}

	myfile.close();
}


void Gameplay::onMouseDown(Event *event) {
	EventMouse* e = (EventMouse*)event;

	afterTouch(e->getCursorX(), e->getCursorY());

	//log("Clicked: %f:%f", e->getCursorX(), e->getCursorY());	
	//Vec2* coor = snap(e->getCursorX(), e->getCursorY());
	//if (coor == nullptr) {
	//	log("not a play");
	//	return;
	//};
	//Vec2 tmp = *coor;
	////log("success play at: %f:%f", tmp.x, tmp.y);/*
	////log("success play at: %f:%f", tmp.x, tmp.y);*/
	//int x_coor = (int)(tmp.x - marginX) / cellSize;
	//int j_coor = x_coor;
	//int y_coor = (int)(tmp.y - marginY) / cellSize;
	//int i_coor = boardSize - 1 - y_coor;


	//if (mode == 1 && turn % 2 == 0) return; //if computer turn, not allowed to play	

	//// record on "board" array
	//// if record successfully then draw the srpite
	//if (recordPlay(i_coor, j_coor, turn % 2)) {
	//	drawPlay(tmp.x, tmp.y, turn % 2);
	//	turn++;

	//	//debug
	//	if (turn % 2 == 0) turn_label->setString("<---Turn"); else turn_label->setString("Turn--->");

	//	// check if there is line of 5 same color sprites
	//	if (winner(board) != -1) {
	//		log("Game over");

	//		announceWinner();
	//		turn++;
	//		game_over = true;
	//		/*resetGame();*/
	//		return;
	//	}

	//	if (mode == 1) {
	//		playAI();
	//	}
	//};
}

void Gameplay::afterTouch(float x, float y) {
	if (game_over) return;
	log("Clicked: %f:%f", x, y);
	Vec2* coor = snap(x, y);
	if (coor == nullptr) {
		log("not a play");
		return;
	};
	Vec2 tmp = *coor;
	//log("success play at: %f:%f", tmp.x, tmp.y);/*
	//log("success play at: %f:%f", tmp.x, tmp.y);*/
	int x_coor = (int)(tmp.x - marginX) / cellSize;
	int j_coor = x_coor;
	int y_coor = (int)(tmp.y - marginY) / cellSize;
	int i_coor = boardSize - 1 - y_coor;


	if (mode == 1 && turn % 2 == 0) return; //if computer turn, not allowed to play	

	// record on "board" array
	// if record successfully then draw the srpite
	if (recordPlay(i_coor, j_coor, turn % 2)) {
		drawPlay(tmp.x, tmp.y, turn % 2);
		turn++;

		//debug
		if (turn % 2 == 0) turn_label->setString("<---Turn"); else turn_label->setString("Turn--->");

		// check if there is line of 5 same color sprites
		if (winner(board) != -1) {
			log("Game over");

			announceWinner();
			turn++;
			game_over = true;
			/*resetGame();*/
			return;
		}

		if (mode == 1) {
			playAI();
		}
	};
}

int Gameplay::winner(int the_board[boardSize + 1][boardSize + 1]) {
	char c = 'u';
	for (int i = 0; i < boardSize; i++)
		for (int j = 0; j < boardSize; j++) {
			if (the_board[i][j] != -1) {
				int currColor = the_board[i][j];
				int line;
				int best = 1;
				// check 4 directions for line up same color of 5
				
				// horizontal
				line = 1;
				for (int k = 1; k <= 4; k++) {
					if (j + k < boardSize) {
						if (the_board[i][j + k] == currColor) line++;
					}
				};
				if (line > best) {
					best = line; c = '--';
				}

				// vertical
				line = 1;
				for (int k = 1; k <= 4; k++) {
					if (i + k < boardSize) {
						if (the_board[i + k][j] == currColor) line++;
					}
				};
				if (line > best) {
					best = line; c = '|';
				}

				// topLeft->btmRight
				line = 1;
				for (int k = 1; k <= 4; k++) {
					if (j + k < boardSize && i + k < boardSize) {
						if (the_board[i + k][j + k] == currColor) line++;
					}
				};
				if (line > best) {
					best = line; c = '\\';
				}

				// topRight -> btmLeft
				line = 1;
				for (int k = 1; k <= 4; k++) {
					if (j - k >= 0 && i + k < boardSize) {
						if (the_board[i + k][j - k] == currColor) line++;
					}
				};
				if (line > best) {
					best = line; c = '/';
				}

				if (best == 5) {
					log("Detected game over at: %d:%d with direction of %c", i, j, c);
					log("Winner: %d", currColor);
					return currColor;
				}
			}
		}
	return -1;
}

bool Gameplay::recordPlay(int i, int j, int color) {	
	//if (board[boardSize - 1 - y_coor][x_coor] != 0) {
	if (board[i][j] != -1) {
		log("Cell occupied");
		return false;
	}
	else {
		board[i][j] = color;

		// add to coor_stack
		coor_stack.push_back(Coor(i, j));
		return true;
	}
}

void Gameplay::drawPlay(float x_pos, float y_pos, int color) {
	// create sprite
	auto play = Sprite::create();
	if (color == 0) play->setTexture(X_PATH); else play->setTexture(O_PATH);
	if (play == nullptr) {
		problemLoading("X/O sprite missing");
		return;
	}

	// reposition sprite
	play->setPosition(Vec2(x_pos + 1, y_pos + 1));
	play->setAnchorPoint(Vec2(0, 0));

	// set scale
	play->setScale((float)(cellSize - 2) / SPRITE_SIZE);

	// add to parent for reseting board purpose
	play_stack->addChild(play, 1);

	// add to move_stack
	move_stack.pushBack(play);
}

void Gameplay::recordAndDraw(int i_coor, int j_coor, int color) {
	if (recordPlay(i_coor, j_coor, color)) /*drawPlay(padding + j_coor * cellSize, padding + (boardSize - i_coor - 1) * cellSize, color);*/
	drawPlay(marginX + j_coor * cellSize, marginY + (boardSize - i_coor - 1) * cellSize, color);
}

int** Gameplay::calculateValueBoard(int color, int the_board[boardSize+1][boardSize+1], bool draw) {
	int** valueBoard = 0;
	valueBoard = new int*[boardSize];

	for (int h = 0; h < boardSize; h++) {
		valueBoard[h] = new int[boardSize];
		for (int w = 0; w < boardSize; w++) {
			valueBoard[h][w] = 0;
		}
	}

	int current_color = color;

	// horizontal
	for (int h = 0; h < boardSize - 4; h++) {
		for (int w = 0; w < boardSize; w++) {

			int com_cells = 0, player_cells = 0;

			for (int add = 0; add < 5; add++) {
				com_cells += the_board[h + add][w] == 0 ? 1 : 0;
				player_cells += the_board[h + add][w] == 1 ? 1 : 0;
			}
			// count the number of same color cell in a line

			if (com_cells * player_cells == 0 && com_cells != player_cells) {
				// if all the cells have the same color, we do the calculating

				for (int add = 0; add < 5; add++) {
					if (the_board[h + add][w] == -1) {
						// calculate free cell only
						
						int add_up = 0;
						if (current_color == 1) { // calculate for player
							// if all cells belongs to players
							if (com_cells == 0)  add_up = attackValue[player_cells];
							// if all celss belongs to computer
							else add_up = defendValue[com_cells];
						}
						else { // calculate for computer
							// if all cells belongs to player
							if (com_cells == 0) add_up = defendValue[player_cells];
							// if all cells belongs to computer
							else add_up = attackValue[com_cells];
						}
						valueBoard[h + add][w] += add_up;
						// if there are already 4 of the cells of the same color, double that value
						if (com_cells == 4 || player_cells == 4) valueBoard[h + add][w] *= 2;
					}
				}
			}
			
		}
	}
	// vertical
	for (int h = 0; h < boardSize; h++) {
		for (int w = 0; w < boardSize - 4; w++) {

			int com_cells = 0, player_cells = 0;

			for (int add = 0; add < 5; add++) {
				com_cells += the_board[h][w + add] == 0 ? 1 : 0;
				player_cells += the_board[h][w + add] == 1 ? 1 : 0;
			}
			// count the number of same color cell in a line

			if (com_cells * player_cells == 0 && com_cells != player_cells) {
				// if all the cells have the same color, we do the calculating

				for (int add = 0; add < 5; add++) {
					if (the_board[h][w + add] == -1) {
						// calculate free cell only

						int add_up = 0;
						if (current_color == 1) { // calculate for player
							// if all cells belongs to players
							if (com_cells == 0)  add_up = attackValue[player_cells];
							// if all celss belongs to computer
							else add_up = defendValue[com_cells];
						}
						else { // calculate for computer
							// if all cells belongs to player
							if (com_cells == 0) add_up = defendValue[player_cells];
							// if all cells belongs to computer
							else add_up = attackValue[com_cells];
						}
						valueBoard[h][w + add] += add_up;
						// if there are already 4 of the cells of the same color, double that value
						if (com_cells == 4 || player_cells == 4) valueBoard[h][w + add] *= 2;
					}
				}
			}
		}
	}

	// topr->btml
	for (int h = 0; h < boardSize - 4; h++) {
		for (int w = 0; w < boardSize - 4; w++) {

			int com_cells = 0, player_cells = 0;

			for (int add = 0; add < 5; add++) {
				com_cells += the_board[h + add][w + add] == 0 ? 1 : 0;
				player_cells += the_board[h + add][w + add] == 1 ? 1 : 0;
			}
			// count the number of same color cell in a line

			if (com_cells * player_cells == 0 && com_cells != player_cells) {
				// if all the cells have the same color, we do the calculating

				for (int add = 0; add < 5; add++) {
					if (the_board[h + add][w + add] == -1) {
						// calculate free cell only

						int add_up = 0;
						if (current_color == 1) { // calculate for player
							// if all cells belongs to players
							if (com_cells == 0)  add_up = attackValue[player_cells];
							// if all celss belongs to computer
							else add_up = defendValue[com_cells];
						}
						else { // calculate for computer
							// if all cells belongs to player
							if (com_cells == 0) add_up = defendValue[player_cells];
							// if all cells belongs to computer
							else add_up = attackValue[com_cells];
						}
						valueBoard[h + add][w + add] += add_up;
						// if there are already 4 of the cells of the same color, double that value
						if (com_cells == 4 || player_cells == 4) valueBoard[h + add][w + add] *= 2;
					}
				}
			}
		}
	}

	// topl->btmr
	for (int h = 0; h < boardSize - 4; h++) {
		for (int w = boardSize - 1; w >= 4; w--) {

			int com_cells = 0, player_cells = 0;

			for (int add = 0; add < 5; add++) {
				com_cells += the_board[h + add][w - add] == 0 ? 1 : 0;
				player_cells += the_board[h + add][w - add] == 1 ? 1 : 0;
			}
			// count the number of same color cell in a line

			if (com_cells * player_cells == 0 && com_cells != player_cells) {
				// if all the cells have the same color, we do the calculating

				for (int add = 0; add < 5; add++) {
					if (the_board[h + add][w - add] == -1) {
						// calculate free cell only

						int add_up = 0;
						if (current_color == 1) { // calculate for player
							// if all cells belongs to players
							if (com_cells == 0)  add_up = attackValue[player_cells];
							// if all celss belongs to computer
							else add_up = defendValue[com_cells];
						}
						else { // calculate for computer
							// if all cells belongs to player
							if (com_cells == 0) add_up = defendValue[player_cells];
							// if all cells belongs to computer
							else add_up = attackValue[com_cells];
						}
						valueBoard[h + add][w - add] += add_up;
						// if there are already 4 of the cells of the same color, double that value
						if (com_cells == 4 || player_cells == 4) valueBoard[h + add][w - add] *= 2;
					}
				}
			}
		}
	}


	// debug review value board
	if (draw) {
		value_stack->removeAllChildren();
		for (int h = 0; h < boardSize; h++) {
			for (int w = 0; w < boardSize; w++) {
				if (valueBoard[h][w] > 0) {
					drawValue(marginX + w * cellSize, marginY + (boardSize - h - 1) * cellSize, valueBoard[h][w]);
				}
			}
		}
	}
	// debug review value board
	return valueBoard;
}

void Gameplay::drawValue(float x_pos, float y_pos, int value) {
	Label* valueLabel = Label::createWithTTF(TTFConfig("fonts/arial.ttf", 10), std::to_string(value));
	valueLabel->setPosition(Vec2(x_pos + 1, y_pos + 1));
	valueLabel->setAnchorPoint(Vec2(0.0, 0.0));
	valueLabel->setScale((float)(cellSize) / valueLabel->getContentSize().width);

	valueLabel->setColor(Color3B::BLACK);

	value_stack->addChild(valueLabel);
}

Coor Gameplay::bestMove(int** valueBoard) {
	int max = 0, i, j;
	for (int h = 0; h < boardSize; h++)
		for (int w = 0; w < boardSize; w++) {
			if (valueBoard[h][w] > max) {
				max = valueBoard[h][w];
				i = h;
				j = w;
			}
		}
	return Coor(i, j);
}

void Gameplay::playAI() {

	int clone_board[boardSize + 1][boardSize + 1];
	std::copy(&board[0][0], &board[0][0] + (boardSize + 1) * (boardSize + 1), &clone_board[0][0]);

	int** com_value_board = calculateValueBoard(0, clone_board, true);


	Coor winning_move;
	int moves_watched = 1;
	while (moves_watched <= 3) {

		Coor this_move = bestMove(com_value_board);
		if (!isBadComMove(0, clone_board, this_move) || com_value_board[this_move.i][this_move.j] >= line_4_value(defendValue)) {
			winning_move.i = this_move.i;
			winning_move.j = this_move.j;
			break;
		}
		com_value_board[this_move.i][this_move.j] = 0;
		moves_watched++;
	}
	


	int i, j;
	if (moves_watched > 3) {
		Coor give_up_move = bestMove(com_value_board);
		i = give_up_move.i;
		j = give_up_move.j;
	}
	else {
		i = winning_move.i, j = winning_move.j;
	}

	for (int i = 0; i < boardSize; i++)
		delete[] com_value_board[i];
	delete[] com_value_board;

	/*Coor winning_move2;
	if (potentialWinMove(0, clone_board, winning_move2)) {
		winning_move = winning_move2;
	}
	else {
		winning_move = bestMove(com_value_board);
	};*/


	float delay = 1.5f;
	auto delayAction = DelayTime::create(delay);  // For 2 Seconds of Delay
	auto funcCallback = CallFunc::create([=]() {
		
		int i = winning_move.i, j = winning_move.j;
		if (recordPlay(i, j, 0)) drawPlay(marginX + j * cellSize, marginY + (boardSize - i - 1) * cellSize, 0);
		if (winner(board) != -1) {
			log("Game over");
			/*resetGame();*/
			announceWinner();
			turn++;
			game_over = true;
			return;
		}
		turn++;
		if (turn % 2 == 0) turn_label->setString("<---Turn"); else turn_label->setString("Turn--->");
	});
	this->runAction(Sequence::create(delayAction, funcCallback, NULL));



	// try the move and predict user move based on move's value, with depth pre-defined
	// if the move makes human won => drop the move
	// if the move makes AI won => make the move

	// if all 3 moves doesn't lead to winning, take the highest value move

	// this is where algorithm for, calculate the move for AI
	/*while ((recordPlay(i_coor, j_coor, turn) == false) && i_coor < boardSize && j_coor < boardSize) {
		i_coor++;
		j_coor++;
	}
	if (i_coor < boardSize && j_coor < boardSize) {
		drawPlay(j_coor * cellSize + padding, (boardSize - i_coor -1) * cellSize + padding, turn);
		turn++;
	}*/

	// UNCOMMENT THIS AFTER TESTING
	// check game over after ai made move
	/*if (winner() != -1) {
		log("Game over");
		resetGame();
		return;
	}
	turn++;
	delete[] value_board;*/
}

bool Gameplay::isBadComMove(int depth, int(&clone_board)[boardSize + 1][boardSize + 1], Coor the_move) {
	if (depth > maxDepth) return false;

	//bool bad = false;

	int** com_value_board = calculateValueBoard(0, clone_board, 0);

	int bad_move_count = 0;

	for (int c = 1; c <= 3; c++) {
		Coor com_move;
		if (depth == 0) 
			com_move = Coor(the_move.i, the_move.j);
		else 
			com_move = bestMove(com_value_board);

		clone_board[com_move.i][com_move.j] = 0;
		if (winner(clone_board) == 0) {

			for (int i = 0; i < boardSize; i++)
				delete[] com_value_board[i];
			delete[] com_value_board;

			return false;
			break;
		}
		else {
			int** plr_value_board = calculateValueBoard(1, clone_board, 0);

			int plr_win_move = 0;

			for (int p = 1; p <= 3; p++) {
				Coor plr_move = bestMove(plr_value_board);
				clone_board[plr_move.i][plr_move.j] = 1;

				if (winner(clone_board) == 1) {
					plr_win_move++;
				}
				else {
					plr_win_move += isBadComMove(depth + 1, clone_board, the_move) ? 1 : 0;
				}

				clone_board[plr_move.i][plr_move.j] = -1;
				plr_value_board[plr_move.i][plr_move.j] = 0;
			}

			if (plr_win_move == 3) {
				bad_move_count++;
			}

			for (int i = 0; i < boardSize; i++)
				delete[] plr_value_board[i];
			delete[] plr_value_board;
		}
		clone_board[com_move.i][com_move.j] = -1;

		if (depth == 0) break; 
		else {
			com_value_board[com_move.i][com_move.j] = 0;
		}
	}

	


	for (int i = 0; i < boardSize; i++)
		delete[] com_value_board[i];
	delete[] com_value_board;
	
	if (bad_move_count == 3) return true;  
	else return false;
}

bool Gameplay::potentialWinMove(int depth, int(&clone_board)[boardSize + 1][boardSize + 1], Coor &potentialMove) {


	if (depth > maxDepth) return false;

	bool potential = false;

	int** com_value_board = calculateValueBoard(0, clone_board, 0);
	for (int c = 1; c <= 3; c++) {
		Coor com_move = bestMove(com_value_board);
		if (depth == 0) {
			potentialMove.i = com_move.i;
			potentialMove.j = com_move.j;

		}

		clone_board[com_move.i][com_move.j] = 0;
		if (winner(clone_board) == 0) {
			potential = true;
			break;
		}
		else {

			int** plr_value_board = calculateValueBoard(1, clone_board, 0);
			for (int p = 1; p <= 3; p++) {
				Coor plr_move = bestMove(plr_value_board);
				clone_board[plr_move.i][plr_move.j] = 1;

				if (winner(clone_board) == 1) {
					potential = potential && false;
					break;
				}
				else {
					potential = potential && potentialWinMove(depth + 1, clone_board, potentialMove);
				}

				clone_board[plr_move.i][plr_move.j] = -1;

			}

			for (int i = 0; i < boardSize; i++)
				delete[] plr_value_board[i];
			delete[] plr_value_board;
		}
		clone_board[com_move.i][com_move.j] = -1;
	}

	for (int i = 0; i < boardSize; i++)
		delete[] com_value_board[i];
	delete[] com_value_board;
	return potential;

























	//if (depth > maxDepth) {
	//	return false;
	//}

	//bool potential = true;

	//int** com_value_board = calculateValueBoard(0, clone_board, false);

	//int com_potential_moves_use = 0;
	//while (com_potential_moves_use <= MOVE_LIMIT) {
	//	Coor* the_move = bestMove(com_value_board);

	//	if (depth == 0) potentialMove = Coor(the_move->i, the_move->j);

	//	clone_board[the_move->i][the_move->j] = 0;
	//	if (winner(clone_board) == 0) {			
	//		//delete the_move;
	//		for (int i = 0; i < boardSize; i++)
	//			delete[] com_value_board[i];
	//		delete[] com_value_board;

	//		return true;
	//	}
	//	int** player_value_board = calculateValueBoard(1, clone_board, false);

	//	int player_potential_move_use = 0;
	//	while (player_potential_move_use <= MOVE_LIMIT) {
	//		Coor* player_move = bestMove(player_value_board);
	//		clone_board[player_move->i][player_move->j] = 1;
	//		if (winner(clone_board) == 1) {
	//			//delete the_move;
	//			/*for (int i = 0; i < boardSize; i++)
	//				delete[] com_value_board[i];
	//			delete[] com_value_board;*/
	//			//delete player_move;
	//			/*for (int i = 0; i < boardSize; i++)
	//				delete[] player_value_board[i];
	//			delete[] player_value_board;
	//			return false;*/
	//			potential = potential && false;
	//		}

	//		potential = potential && potentialWinMove(depth + 1, clone_board, potentialMove);

	//		clone_board[player_move->i][player_move->j] = -1;
	//		player_potential_move_use++;


	//		player_value_board[the_move->i][the_move->j] = 0;
	//		/*delete player_move;*/
	//	}

	//	clone_board[the_move->i][the_move->j] = -1;
	//	com_potential_moves_use++;
	//	com_value_board[the_move->i][the_move->j] = 0;

	//	for (int i = 0; i < boardSize; i++)
	//		delete[] player_value_board[i];
	//	delete[] player_value_board;
	//}

	//for (int i = 0; i < boardSize; i++)
	//	delete[] com_value_board[i];
	//delete[] com_value_board;

	//return potential;
}

int Gameplay::line_4_value(const int v[]) {
	return (v[1] + v[2] + v[3] + v[4])*2;
}