#pragma once
#include "SDL.h"
#include "SDL_ttf.h"
#include <vector>
#include <unordered_map>
#include <string>

#define DEFAULT_PTSIZE  18
#define DEFAULT_TEXT    "PONG.2024"
#define WIDTH   1024
#define HEIGHT  768

#define TTF_SHOWFONT_USAGE \
"Usage: %s [-solid] [-shaded] [-blended] [-wrapped] [-utf8|-unicode] [-b] [-i] [-u] [-s] [-outline size] " \
"[-hintlight|-hintmono|-hintnone] [-nokerning] [-wrap] [-fgcol r,g,b,a] [-bgcol r,g,b,a] <font>.ttf [ptsize] [text]\n"

typedef enum
{
	TextRenderSolid,
	TextRenderShaded,
	TextRenderBlended
} TextRenderMethod;


// Vector2 struct just stores x/y coordinates
struct Vector2
{
	float x;
	float y;
};

struct Ball
{
	Vector2 position;
	Vector2 velocity;
};

class Game
{
public:
	Game();
	// Initialize the Game
	bool Initialize();
	// Runs the game loop until the game is over
	void RunLoop();
	// Shutdown the game
	void Shutdown();
private:
	// Helper funcitons for the game loop
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	void cleanup(int exitcode);
	// Window created by SDL
	SDL_Window* mWindow;
	// Renderer for 2D drawing
	SDL_Renderer* mRenderer;
	// Number of ticks since start of game
	Uint32 mTicksCount;
	// Game should continue to run
	bool mIsRunning;
	bool gameOver;
	// Pong specific
	// Direction of paddle
	int mPaddleDir;
	// Position of paddle
	Vector2 mPaddlePos;
	// Direction of second paddle
	int nPaddleDir;
	// Positon of second paddle
	Vector2 nPaddlePos;
	//Position of ball
	Vector2 mBallPos;
	// Velocity of ball
	Vector2 mBallVel;
	// vector of balls
	std::vector<Ball> ball;
	int playerOneScore;
	int playerTwoScore;
	SDL_Texture* GetTexture(const std::string& fileName);
	std::unordered_map<std::string, SDL_Texture*> mTextures;
	int mTexWidth, mTexHeight;

	void resetBallPosition(Ball& ball);

	typedef struct {
		SDL_Texture* caption;
		SDL_Rect captionRect;
		SDL_Texture* message;
		SDL_Rect messageRect;
	} Scene;

	// text properties
	TTF_Font* font;
	SDL_Surface* text = NULL;
	SDL_Surface* winner = NULL;
	int ptsize;
	int i, done;
	SDL_Color white = { 0xFF, 0xFF, 0xFF, 0 };
	SDL_Color black = { 0x00, 0x00, 0x00, 0 };
	SDL_Color* forecol;
	SDL_Color* backcol;
	SDL_Event* event;
	Scene scene;
	Scene end;
	TextRenderMethod rendermethod;
	int renderstyle;
	int outline;
	int hinting;
	int kerning;
	int wrap;

	enum {
		RENDER_LATIN1,
		RENDER_UTF8,
		RENDER_UNICODE
	} rendertype;

	const char* message, string[128] = DEFAULT_TEXT;
};