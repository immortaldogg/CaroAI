#ifndef __GAMEPLAY_SCENE_H__
#define __GAMEPLAY_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "Coor.h"
#include <vector>
#include <string>

#define MOVE_LIMIT 2
#define boardSize 15
#define cellSize 40
#define SPRITE_SIZE 256
#define maxDepth 7

#define BTN_SPRITE_WIDTH 260
#define BTN_SPRITE_HEIGHT 100
#define DrawMarginBtm 10

USING_NS_CC;


class Gameplay : public cocos2d::Scene
{
public:

	const std::string X_PATH = "alpha_x.png";
	const std::string O_PATH = "alpha_o.png";

	//static const int SPRITE_SIZE = 28;
	//static const int boardSize = 15;
	//static const int cellSize = 40;

	float marginX, marginY;
	
	int turn;
	int mode;
	int board[boardSize + 1][boardSize + 1];

	// UI component
	Label* turn_label;
	ui::Button* undo_btn;
	Sprite* announcer;
	Label* reminder;

	Vector<Sprite*> move_stack;
	std::vector<Coor> coor_stack;
	Node* play_stack;

    static cocos2d::Scene* createScene();
    virtual bool init();
	void onEnter();
	
	// debug
	Node* debug_layer;
	Label* mouse_position_label;
	Node* value_stack;
	ui::Button* toggle_btn;
	void drawValue(float x_pos, float y_pos, int value);
	// debug

	// game events handling
	bool recordPlay(int i, int j, int color);
	void drawPlay(float x_pos, float y_pos, int color);
	void recordAndDraw(int i_coor, int j_coor, int color);
	bool game_over;
	int winner(int the_board[boardSize + 1][boardSize + 1]);
	void resetGame();
	Vec2* snap(float x, float y);
	void announceWinner();

	// AI related field
	const int attackValue[6] = { 0, 15, 30, 90, 200, 500 }; // estimate the attack value move
	const int defendValue[6] = { 0, 10, 28, 84, 190, 500 }; // estimate the defend value move
	int line_4_value(const int v[]);

	// AI methods
	void playAI();
	Coor bestMove(int** valueBoard);
	void initAIMove();
	bool potentialWinMove(int depth, int(&clone_board)[boardSize + 1][boardSize + 1], Coor &potentialMove);

	bool isBadComMove(int depth, int(&clone_board)[boardSize + 1][boardSize + 1], Coor the_move);

	int** calculateValueBoard(int turn, int the_board[boardSize + 1][boardSize + 1], bool draw);

	// actions events
	void onMouseMove(Event *event);
	void onMouseDown(Event *event);
	void onTouchBegan(Touch *touch, Event *event);

	void afterTouch(float x, float y);
    
    // implement the "static create()" method manually
    CREATE_FUNC(Gameplay);
};

#endif // __GAMEPLAY_SCENE_H__
