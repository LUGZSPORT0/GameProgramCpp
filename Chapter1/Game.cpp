
#include "Game.h"
#include "SDL_image.h"
#include <iostream>
const int thickness = 15;
const float paddleH = 100.0f;

Game::Game()
	:mWindow(nullptr)
	, mRenderer(nullptr)
	, mIsRunning(true)
	, mTicksCount(0)
	, mPaddleDir(0)
	, nPaddleDir(0)
	, playerOneScore(0)
	, playerTwoScore(0)
	, gameOver(false)
{

}

bool Game::Initialize() // First step is to ensure successful initialization of key game functions
{
	// Initialize SDL
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	// Create an SDL Window
	mWindow = SDL_CreateWindow(
		"Game Programming in C++ (Chapter 1)", // Window title
		100,	// Top left x-coordinate of window
		100,	// Top left y-coordinate of window
		1024,	// Width of window
		768,	// Height of window
		0		// Flags (0 for no flags set) windowed version
	);

	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}

	mRenderer = SDL_CreateRenderer(
		mWindow, // Window to create renderer for
		-1,		 // Usually -1
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}

	/* Initialize the TTF library */
	if (TTF_Init() < 0) {
		SDL_Log("Couldn't initialize TTF: %s\n", SDL_GetError());
		SDL_Quit();
		return(2);
	}

	ptsize = DEFAULT_PTSIZE;
	font = TTF_OpenFont("Assets\\Fonts\\jersey-m54-font\\JerseyM54-aLX9.ttf", ptsize);
	if (font == NULL) {
		SDL_Log("Couldn't load %d pt font from %s: %s\n",
			ptsize, "Assets\\Fonts\\jersey-m54-font\\JerseyM54-aLX9.ttf", SDL_GetError());
		cleanup(2);
	}

	rendermethod = TextRenderSolid;
	renderstyle = TTF_STYLE_NORMAL;
	rendertype = RENDER_LATIN1;
	outline = 0;
	hinting = TTF_HINTING_NORMAL;
	kerning = 1;
	// Default is black and white
	forecol = &black;
	backcol = &white;

	TTF_SetFontStyle(font, renderstyle);
	TTF_SetFontOutline(font, outline);
	TTF_SetFontKerning(font, kerning);
	TTF_SetFontHinting(font, hinting);
	message = DEFAULT_TEXT;
	text = TTF_RenderText_Solid(font, message, *backcol);

	mPaddlePos.x = 10.0f;
	mPaddlePos.y = 768.0f / 2.0f;
	nPaddlePos.x = 1024.0f - thickness - 10.0f; // start at 1024 account for thickness and adjust by 10 to match first paddle
	nPaddlePos.y = 768.0f / 2.0f;

	//mBallPos.x = 1024.0f/2.0f;
	//mBallPos.y = 768.0f/2.0f;

	Ball ball1;
	ball1.velocity.x = -200.0f;
	ball1.velocity.y = 235.0f;
	ball1.position.x = 1024.0f / 2.0f;
	ball1.position.y = 768.0f / 2.0f;

	Ball ball2;
	ball2.velocity.x = 200.0f;
	ball2.velocity.y = 235.0f;
	ball2.position.x = 1024.0f / 2.0f;
	ball2.position.y = 768.0f / 2.0f;

	Ball ball3;
	ball3.velocity.x = 200.0f;
	ball3.velocity.y = -235.0f;
	ball3.position.x = 1024.0f / 2.0f;
	ball3.position.y = 768.0f / 2.0f;

	ball.push_back(ball1);
	//ball.push_back(ball2);
	//ball.push_back(ball3);

}

void Game::cleanup(int exitcode)
{
	TTF_Quit();
	SDL_Quit();
	exit(exitcode);
}

void Game::ProcessInput()
{
	SDL_Event event;
	// While there are still events in the queue
	while (SDL_PollEvent(&event)) // takes in a pointer
	{
		switch (event.type)
		{
		case SDL_QUIT:
			mIsRunning = false;
			break;
		}
	}

	// Get state of keyboard
	const Uint8* state = SDL_GetKeyboardState(NULL);
	// if escape is pressed, aso end loop
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}

	// Update paddle direction based on W/S keys
	mPaddleDir = 0;
	if (state[SDL_SCANCODE_W])
	{
		mPaddleDir -= 1;
	}
	if (state[SDL_SCANCODE_S])
	{
		mPaddleDir += 1;
	}

	// Update second paddle direction based on I/K keys
	nPaddleDir = 0;
	if (state[SDL_SCANCODE_I])
	{
		nPaddleDir -= 1;
	}
	if (state[SDL_SCANCODE_K])
	{
		nPaddleDir += 1;
	}
	if (gameOver && state[SDL_SCANCODE_R])
	{
		playerOneScore = 0;
		playerTwoScore = 0;
		gameOver = false;
	}
}

void PaddlePosition(Vector2& pos, int& dir, float& deltaTime)
{
	if (dir != 0)
	{
		pos.y += dir * 300.0f * deltaTime;
		// Make sure paddle doesn't move off screen!
		if (pos.y < paddleH / 2.0f + thickness)
		{
			pos.y = paddleH / 2.0f + thickness;
		}
		else if (pos.y > (768.0f - paddleH / 2.0f - thickness))
		{
			pos.y = 768.0f - paddleH / 2.0f - thickness;
		}
	}
}

void Game::resetBallPosition(Ball& ball)
{
	ball.velocity.x = -350.0f;
	ball.velocity.y = 195.0f;
	ball.position.x = 1024.0f / 2.0f;
	ball.position.y = 768.0f / 2.0f;
}

void Game::UpdateGame()
{
	// wait until 16ms has elapsed since last frame
	// frame limiting 
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	// Delta time is the difference in ticks from last frame
	// converted to seconds
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;

	// Clamp maximum delta time value
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	// Update tick counts (for next frame)
	mTicksCount = SDL_GetTicks();

	// Update paddle position based on direction
	PaddlePosition(mPaddlePos, mPaddleDir, deltaTime);
	// Update second paddle position based on direction
	PaddlePosition(nPaddlePos, nPaddleDir, deltaTime);

	//// Update ball position based on velocity
	//mBallPos.x += mBallVel.x * deltaTime;
	//mBallPos.y += mBallVel.y * deltaTime;

	for (int i = 0; i < ball.size(); i++)
	{
		ball[i].position.x += ball[i].velocity.x * deltaTime;
		ball[i].position.y += ball[i].velocity.y * deltaTime;

		if (ball[i].position.x > 1024.0f / 1.5f)
		{
			if (nPaddlePos.y != ball[i].position.y && ball[i].position.y < (nPaddlePos.y) && ball[i].velocity.x > 0)
			{
				nPaddleDir -= 1;
				PaddlePosition(nPaddlePos, nPaddleDir, deltaTime);
				nPaddleDir = 0;
			}
			else if (nPaddlePos.y != ball[i].position.y && ball[i].position.y > (nPaddlePos.y) && ball[i].velocity.x > 0)
			{
				nPaddleDir += 1;
				PaddlePosition(nPaddlePos, nPaddleDir, deltaTime);
				nPaddleDir = 0;
			}
		}

		// Bounce if needed
		// Did we intersect with the left paddle?
		float diff = ball[i].position.y - mPaddlePos.y;
		float ndiff = ball[i].position.y - nPaddlePos.y;

		// Take absolute value of the difference
		diff = (diff > 0.0f) ? diff : -diff;
		ndiff = (ndiff > 0.0f) ? ndiff : -ndiff;
		if (
			// Our y-difference is small enough
			diff <= paddleH / 2.0f &&
			// Ball is at the correct x-position
			ball[i].position.x <= 25.0f &&
			// The ball is moving to the left
			ball[i].velocity.x <= 0.0f)
		{
			ball[i].velocity.x *= -1.0f;
		}
		// Did we intersect with the right paddle?
		else if (
			ndiff <= paddleH / 2.0f &&
			ball[i].position.x >= 999.0f &&
			ball[i].velocity.x > 0.0f)
		{
			ball[i].velocity.x *= -1.0f;
		}

		// Did the ball go off the screen? (if so, end game)
		if (ball[i].position.x <= 0.0f)
		{
			playerTwoScore++;
			if (playerTwoScore != 7)
			{
				resetBallPosition(ball[i]);
			}
		}
		if (ball[i].position.x >= 1024.0f)
		{
			playerOneScore++;
			if (playerOneScore != 7)
			{
				resetBallPosition(ball[i]);
			}
		}

		// Did the ball collide with the top wall
		if (ball[i].position.y <= thickness && ball[i].velocity.y < 0.0f)
		{
			//mBallVel.y *= -1;
			ball[i].velocity.y *= -1;
		}
		// Did the ball collide with the bottom wall?
		else if (ball[i].position.y >= (768.0f - thickness) && ball[i].velocity.y > 0.0f)
		{
			ball[i].velocity.y *= -1;
		}
	}
}

void Game::GenerateOutput()
{
	// Clear back buffer
	// Set draw color to to black
	SDL_SetRenderDrawColor(
		mRenderer,
		0,		// R
		0,		// G
		0,		// B
		255		// A
	);

	// Clear back buffer with the current draw color
	SDL_RenderClear(mRenderer);

	if (playerOneScore == 7)
	{
		const char* winnerDisplay = "Player 1 wins\nPress R to play again!";
		winner = TTF_RenderText_Blended_Wrapped(font, winnerDisplay, *backcol, 0);
		SDL_Rect t;
		t.x = (float)((WIDTH - winner->w) / 2);
		t.y = 100;
		t.w = winner->w;
		t.h = winner->h;
		SDL_Texture* stuff = SDL_CreateTextureFromSurface(mRenderer, winner);

		SDL_RenderCopy(mRenderer, stuff, NULL, &t);
		gameOver = true;
	}

	if (playerTwoScore == 7)
	{
		const char* winnerDisplay = "Player 2 wins\nPress R to play again!";
		winner = TTF_RenderText_Blended_Wrapped(font, winnerDisplay, *backcol, 0);
		SDL_Rect t;
		t.x = (float)((WIDTH - winner->w) / 2);
		t.y = 100;
		t.w = winner->w;
		t.h = winner->h;
		SDL_Texture* stuff = SDL_CreateTextureFromSurface(mRenderer, winner);

		SDL_RenderCopy(mRenderer, stuff, NULL, &t);
		gameOver = true;
	}

	scene.messageRect.x = (float)((WIDTH - text->w) / 2);
	scene.messageRect.y = (float)((HEIGHT - text->h) / 2);
	scene.messageRect.w = (float)text->w;
	scene.messageRect.h = (float)text->h;
	scene.message = SDL_CreateTextureFromSurface(mRenderer, text);

	SDL_RenderCopy(mRenderer, scene.message, NULL, &scene.messageRect);

	SDL_Texture* fTextures = GetTexture("Assets/digits.png");

	SDL_QueryTexture(fTextures, nullptr, nullptr, &mTexWidth, &mTexHeight);

	SDL_Rect r;
	r.w = mTexWidth / 11;
	r.h = mTexHeight;
	r.x = 100;
	r.y = 100;

	SDL_Rect s;
	s.x = playerOneScore * 21;
	s.y = 0;
	s.w = 21;
	s.h = 32;
	SDL_RenderCopy(mRenderer, fTextures, &s, &r);

	SDL_Rect p2;
	p2.w = mTexWidth / 11;
	p2.h = mTexHeight;
	p2.x = 900;
	p2.y = 100;

	SDL_Rect k;
	k.x = playerTwoScore * 21;
	k.y = 0;
	k.w = 21;
	k.h = 32;
	SDL_RenderCopy(mRenderer, fTextures, &k, &p2);

	// Set next render color (walls)
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);

	// Draw top wall
	SDL_Rect wall{
		0,			// Top left x
		0,			// Top left y
		1024,		// Width
		thickness	// Height
	};
	SDL_RenderFillRect(mRenderer, &wall);

	// Draw bottom wall
	wall.y = 768 - thickness;
	SDL_RenderFillRect(mRenderer, &wall);

	//// Draw right wall ***** Replaced with second paddle
	//wall.x = 1024 - thickness;
	//wall.y = 0;
	//wall.w = thickness;
	//wall.h = 1024;
	//SDL_RenderFillRect(mRenderer, &wall);

	SDL_Rect paddle{
		static_cast<int>(mPaddlePos.x),
		static_cast<int>(mPaddlePos.y - paddleH / 2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &paddle);

	paddle.x = static_cast<int>(nPaddlePos.x);
	paddle.y = static_cast<int>(nPaddlePos.y - paddleH / 2);
	paddle.w = thickness;
	paddle.h = static_cast<int>(paddleH);
	SDL_RenderFillRect(mRenderer, &paddle);

	if (!gameOver)
	{
		for (int i = 0; i < ball.size(); i++)
		{
			SDL_Rect ballRect{
			static_cast<int>(ball[i].position.x - thickness / 2),
			static_cast<int>(ball[i].position.y - thickness / 2),
			thickness,
			thickness
			};
			SDL_RenderFillRect(mRenderer, &ballRect);
		}
	}
	else
	{
		ball[0].position.x = 500;
		ball[0].position.y = 500;

	}

	//SDL_Rect ball{
	//	static_cast<int>(mBallPos.x - thickness / 2),
	//	static_cast<int>(mBallPos.y - thickness / 2),
	//	thickness,
	//	thickness
	//};
	//SDL_RenderFillRect(mRenderer, &ball);

	// Swap the front buffer and back buffer
	SDL_RenderPresent(mRenderer);
}

SDL_Texture* Game::GetTexture(const std::string& fileName)
{
	SDL_Texture* tex = nullptr;

	// Is the texture already in the map
	auto iter = mTextures.find(fileName);
	if (iter != mTextures.end())
		tex = iter->second;
	else
	{
		// Load an image from file
		SDL_Surface* surf = IMG_Load(fileName.c_str());
		if (!surf)
		{
			SDL_Log("Failed to load texture file %s", fileName.c_str());
			return nullptr;
		}

		// Create texture from surface - convers an SDL_Surface to an SDL_Texture
		// Returns a pointer to an SDL_Texture if successful, otherwise nullptr
		tex = SDL_CreateTextureFromSurface(mRenderer, surf);
		SDL_FreeSurface(surf);
		if (!tex)
		{
			SDL_Log("Failed to convert surface to texture for %s", fileName.c_str());
			return nullptr;
		}

		mTextures.emplace(fileName.c_str(), tex);

	}
	return tex;
}

void Game::RunLoop() // Second step
{

	// A game performs the following steps on each frame
	while (mIsRunning)
	{
		// Typically keyboard, mouse, or controller. Can even be GPS information or, data from internet. Dependent on type of game and platform
		ProcessInput();
		// Updating every object in the game
		UpdateGame();
		// Graphics, audio, force feedback
		GenerateOutput();
	}
}

void Game::Shutdown()
{
	SDL_DestroyWindow(mWindow);
	TTF_CloseFont(font);
	SDL_DestroyTexture(scene.caption);
	SDL_DestroyTexture(scene.message);
	SDL_Quit();
}

