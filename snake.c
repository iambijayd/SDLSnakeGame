#include "snake.h"
#include "snakefunc.h"
int main(int argc, char *argv[])
{
    srand(time(NULL));
    int isRunning = 1;
    init();
    scoreAsset();
    soundAsset();
    updateScore();
    createSnakeBody();
    generateFood();
    int desiredDelta = 1000 / FPS;
    Mix_VolumeMusic(20);
    Mix_PlayMusic(gameMusic, -1);
    while (isRunning)
    {
        int start = SDL_GetTicks();
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                isRunning = 0;
            }
            else if (e.type == SDL_KEYDOWN)
            {
                checkButtonPress(e.key.keysym.scancode);
            }
        }
        SDL_SetRenderDrawColor(renderer, 39, 55, 70, 0);
        SDL_RenderClear(renderer);
        checkGameState();
        SDL_RenderPresent(renderer);
        int currDelta = SDL_GetTicks() - start;
        if (currDelta < desiredDelta)
        {
            SDL_Delay(desiredDelta - currDelta);
        }
    }
    dispose();
}
int init(void)
{
    SDL_Surface *surface = NULL;
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        return 0;
    }
    window = SDL_CreateWindow("SNAKE", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (window == NULL)
    {
        return 0;
    }
    surface = IMG_Load("./resources/icon.png");
    SDL_SetWindowIcon(window, surface);
    SDL_FreeSurface(surface);
    Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    renderer = SDL_CreateRenderer(window, -1, render_flags);
    if (renderer == NULL)
    {
        return 0;
    }
    if (!IMG_Init(IMG_INIT_PNG))
    {
        printf("Image Initialization Failed %s", IMG_GetError());
        return 0;
    }
    if (TTF_Init() == -1)
    {
        printf("Font Initialization Failed %s", TTF_GetError());
        return 0;
    }
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    return 1;
}
void dispose(void)
{
    IMG_Quit();
    TTF_Quit();
    deleteSnakeBody();
    SDL_DestroyTexture(scoreTextTex);
    SDL_DestroyTexture(scoreNumTex);
    Mix_CloseAudio();
    Mix_FreeChunk(gameOver);
    Mix_FreeChunk(dirChange);
    Mix_FreeChunk(foodEaten);
    Mix_FreeMusic(gameMusic);
    Mix_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}
void deleteSnakeBody(void)
{
    Snake *temp;
    while (snakeHead != NULL)
    {
        temp = snakeHead;
        snakeHead = snakeHead->next;
        free(temp);
    }
}
void checkGameState(void)
{
    if (gameState == GAME)
    {
        // renderScore();
        drawWalls();
        renderSnake();
        moveSnake();
        renderScore();
        renderFood();
        if (isSnakeColliding() == 1) // colliding with itself
        {
            gameState = GAME_OVER;
            Mix_PlayChannel(-1, gameOver, 0);
            Mix_HaltMusic();
        }
        else if (isSnakeColliding() == 2) // colliding with food
        {
            Mix_PlayChannel(-1, foodEaten, 0);
            createSnakeBody();
            generateFood();
            myScore++;
            updateScore();
        }
    }
    else if (gameState == GAME_OVER)
    {
        renderScore();
        drawWalls();
        renderSnake();
        renderFood();
    }
}
void renderScore(void)
{
    SDL_Rect scoreText = {40, 20, 100, 60};
    SDL_Rect scoreNum = {500, 28, 60, 40};
    SDL_RenderCopy(renderer, scoreTextTex, NULL, &scoreText);
    SDL_RenderCopy(renderer, scoreNumTex, NULL, &scoreNum);
}
void scoreAsset(void)
{
    SDL_Surface *surface;
    TTF_Font *font;
    font = TTF_OpenFont("./Fonts/abduction2002.ttf", 32);
    SDL_Color textcolor = {255, 255, 255, 0};
    surface = TTF_RenderText_Solid(font, "Score", textcolor);
    scoreTextTex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    TTF_CloseFont(font);
}
void updateScore(void)
{
    char buffer[4];
    SDL_Surface *surface;
    TTF_Font *font;
    font = TTF_OpenFont("./Fonts/abduction2002.ttf", 32);
    SDL_Color textcolor = {255, 255, 255, 0};
    sprintf(buffer, "%d", myScore);
    surface = TTF_RenderText_Solid(font, buffer, textcolor);
    scoreNumTex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    TTF_CloseFont(font);
}
void drawWalls(void)
{
    SDL_Rect wallleft = {0, 0, SNAKE_SIZE, SCREEN_HEIGHT};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderFillRect(renderer, &wallleft);

    SDL_Rect wallbottom = {0, SCREEN_HEIGHT - SNAKE_SIZE, SCREEN_WIDTH, SNAKE_SIZE};
    // SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
    SDL_RenderFillRect(renderer, &wallbottom);

    SDL_Rect wallright = {SCREEN_WIDTH - SNAKE_SIZE, 0, SNAKE_SIZE, SCREEN_HEIGHT};
    // SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
    SDL_RenderFillRect(renderer, &wallright);

    SDL_Rect walltop = {0, SCREEN_HEIGHT - 540, SCREEN_WIDTH, SNAKE_SIZE};
    // SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
    SDL_RenderFillRect(renderer, &walltop);
}
void createSnakeBody(void)
{
    Snake *newbody;
    newbody = (Snake *)malloc(sizeof(Snake));
    if (snakeHead == NULL)
    {
        newbody->node.y = SCREEN_HEIGHT / 2;
        newbody->node.x = SCREEN_WIDTH / 2;
        newbody->node.w = SNAKE_SIZE;
        newbody->node.h = SNAKE_SIZE;
        newbody->prev = NULL;
        newbody->next = NULL;
        snakeHead = newbody;
        snakeTail = newbody;
    }
    else
    {
        Snake *temp;
        temp = snakeHead;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        newbody->node.h = SNAKE_SIZE;
        newbody->node.w = SNAKE_SIZE;
        newbody->node.x = temp->node.x;
        newbody->node.y = temp->node.y;
        newbody->prev = temp;
        newbody->next = NULL;
        temp->next = newbody;
        snakeTail = newbody;
    }
}
Snake *createBodyNode(void)
{
    Snake *new;
    new = (Snake *)malloc(sizeof(Snake));
    new->node.h = SNAKE_SIZE;
    new->node.w = SNAKE_SIZE;
    switch (currSnakeDir)
    {
    case RIGHT:
        new->node.x = (snakeHead->node.x) + (SNAKE_SIZE);
        new->node.y = snakeHead->node.y;
        break;
    case UP:
        new->node.x = snakeHead->node.x;
        new->node.y = snakeHead->node.y - (SNAKE_SIZE);
        break;
    case LEFT:
        new->node.x = snakeHead->node.x - (SNAKE_SIZE);
        new->node.y = snakeHead->node.y;
        break;
    case DOWN:
        new->node.x = snakeHead->node.x;
        new->node.y = snakeHead->node.y + (SNAKE_SIZE);
        break;
    }
    return new;
}

void moveSnake(void)
{
    Snake *temp = snakeHead;
    if (temp->next == NULL)
    {
        Snake *tempHead = createBodyNode();
        tempHead->next = NULL;
        tempHead->prev = NULL;
        snakeTail = snakeHead = tempHead;
        free(temp);
    }
    else
    {
        Snake *tempTail = snakeTail;
        snakeTail = snakeTail->prev;
        snakeTail->next = NULL;
        Snake *tempHead = createBodyNode();
        tempHead->next = snakeHead;
        tempHead->prev = NULL;
        snakeHead->prev = tempHead;
        snakeHead = tempHead;
        free(tempTail);
    }
    // printf("SnakeHead-> x: %d : Y: %d",snakeHead->node.x,snakeHead->node.y);
}
int isSnakeColliding(void)
{
    int isColliding = 0;

    // colliding with wall
    if (snakeHead->node.x >= (SCREEN_WIDTH - (2 * SNAKE_SIZE - 2)) || snakeHead->node.x <= (SNAKE_SIZE - 2) || snakeHead->node.y >= SCREEN_HEIGHT - (2 * SNAKE_SIZE - 2) || snakeHead->node.y <= SCREEN_HEIGHT - (520 + 2))
        isColliding = 1;

    // self-colliding
    Snake *temp = snakeHead->next;
    while (temp != NULL)
    {
        if (snakeHead->node.x < temp->node.x + SNAKE_SIZE && snakeHead->node.x + SNAKE_SIZE > temp->node.x && snakeHead->node.y < temp->node.y + SNAKE_SIZE && snakeHead->node.y + SNAKE_SIZE > temp->node.y)
            isColliding = 1;
        temp = temp->next;
    }

    // collidding with food
    if (snakeHead->node.x < newFood.food_rec.x + SNAKE_SIZE && snakeHead->node.x + SNAKE_SIZE > newFood.food_rec.x && snakeHead->node.y < newFood.food_rec.y + SNAKE_SIZE && SNAKE_SIZE + snakeHead->node.y > newFood.food_rec.y)
        isColliding = 2;

    return isColliding;
}
void renderSnake(void)
{
    Snake *temp = NULL;
    temp = snakeHead;
    temp = snakeHead->next;
    while (temp != NULL)
    {
        SDL_SetRenderDrawColor(renderer, 32, 17, 245, 0);
        SDL_RenderFillRect(renderer, &temp->node);
        temp = temp->next;
    }
    SDL_SetRenderDrawColor(renderer, 132, 176, 245, 0);
    SDL_RenderFillRect(renderer, &snakeHead->node);
}
void generateFood(void)
{
    const int FOOD_SIZE = SNAKE_SIZE;
    const int lowerY = 6;
    const int upperY = 30;
    const int lowerX = 2;
    const int upperX = 30;
    bool isFoodPlaced = false;
    while (!isFoodPlaced)
    {
        newFood.x = ((rand() % (upperX - lowerX + 1)) + lowerX) * FOOD_SIZE;
        newFood.y = ((rand() % (upperY - lowerY + 1)) + lowerY) * FOOD_SIZE;

        Snake *temp = snakeHead;
        bool isColliding = false;
        while (temp != NULL)
        {
            if (temp->node.x < newFood.x + SNAKE_SIZE && temp->node.x + SNAKE_SIZE > newFood.x && temp->node.y < newFood.y + SNAKE_SIZE && temp->node.y + SNAKE_SIZE > newFood.y)
                isColliding = true;
            temp = temp->next;
        }
        if (!isColliding)
        {
            newFood.food_rec.h = FOOD_SIZE;
            newFood.food_rec.w = FOOD_SIZE;
            newFood.food_rec.x = newFood.x;
            newFood.food_rec.y = newFood.y;
            isFoodPlaced = true;
        }
    }
}
void renderFood(void)
{
    SDL_SetRenderDrawColor(renderer, 238, 75, 43, 0);
    SDL_RenderFillRect(renderer, &newFood.food_rec);
}
void checkButtonPress(int keyPressed)
{
    if (keyPressed == SDL_SCANCODE_UP && currSnakeDir != DOWN)
    {
        currSnakeDir = UP;
        Mix_PlayChannel(-1, dirChange, 0);
    }
    else if (keyPressed == SDL_SCANCODE_RIGHT && currSnakeDir != LEFT)
    {
        currSnakeDir = RIGHT;
        Mix_PlayChannel(-1, dirChange, 0);
    }
    else if (keyPressed == SDL_SCANCODE_LEFT && currSnakeDir != RIGHT)
    {
        currSnakeDir = LEFT;
        Mix_PlayChannel(-1, dirChange, 0);
    }
    else if (keyPressed == SDL_SCANCODE_DOWN && currSnakeDir != UP)
    {
        currSnakeDir = DOWN;
        Mix_PlayChannel(-1, dirChange, 0);
    }
}
void soundAsset(void)
{
    gameMusic = Mix_LoadMUS("./sound/game_music.mp3");
    foodEaten = Mix_LoadWAV("./sound/food.mp3");
    dirChange = Mix_LoadWAV("./sound/move.mp3");
    gameOver = Mix_LoadWAV("./sound/gameover.mp3");
}
