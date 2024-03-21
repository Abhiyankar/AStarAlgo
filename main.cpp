#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <vector>
#include <iostream>
#include <string>
#include <set>
#include <utility>
#include <stack>

char const* convert_button_number_to_string(int button)
{
    char const* result = "NO_BUTTON";
    switch (button)
    {
    case SDL_BUTTON_LEFT:
        result = "left";
        break;

    case SDL_BUTTON_MIDDLE:
        result = "middle";
        break;

    case SDL_BUTTON_RIGHT:
        result = "right";
        break;

    default:
        break;
    }

    return result;
}

struct color
{
    int r;
    int g;
    int b;
    int a;
};

struct block
{
    SDL_Rect rect;
    color col;
    bool isObstacle = false;
    bool isStart = false;
    bool isEnd = false;
};

enum mode
{
    START,
    END,
    OBSTACLE,
    BACKTONORMAL,
    RESET
};

struct cell
{
    int parentI, parentJ;
    double f, g, h;
};

bool pointRectCollision(int x, int y, SDL_Rect* rect)
{
    if (x >= rect->x &&         // right of the left edge AND
        x <= rect->x + rect->w &&    // left of the right edge AND
        y >= rect->y &&         // below the top AND
        y <= rect->y + rect->h)
    {
        return true;
    }
    return false;
}

void draw_rectF(SDL_Rect* rect, struct color col, SDL_Renderer* renderer)
{
    SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, col.a);
    SDL_RenderFillRect(renderer, rect);
}

class AstarPathFinding
{
private:
    int row; 
    int col;
    int destI;
    int destJ;
    int startI;
    int startJ;
    std::vector<std::vector<block>> map;
    std::vector<std::vector<cell>> cellDetails;
    std::stack<std::pair<int, int>> storedPath;
    std::set<std::pair<double, std::pair<int, int> >> openList;

private:
    double calculateHValue(int i, int j)
    {
        // Return using the distance formula
        return ((double)sqrt(
            (i - destI) * (i - destI)
            + (j - destJ) * (j - destJ)));
    }

    bool isUnBlocked(int i, int j)
    {
        // Returns true if the cell is not blocked else false
        if (map[i][j].isObstacle == false)
            return (true);
        else
            return (false);
    }

    bool isValid(int i, int j, int R, int C)
    {
        return (i >= 0) && (i < row) && (j >= 0)
            && (j < col);
    }

    bool isDestination(int i, int j)
    {
        if (i == destI && j == destJ)
            return (true);
        else
            return (false);
    }

    bool hasReachedDestination(int oi, int oj, int i, int j, bool& foundDest)
    {
        if (isDestination(i, j) == true) {
            cellDetails[i][j].parentI = oi;
            cellDetails[i][j].parentJ = oj;
            tracePath();
            foundDest = true;
            return true;
        }
        return false;
    }

    void checkNextBlock(double& gNew, double& hNew, double& fNew, int oi, int oj, int i, int j)
    {
        gNew = cellDetails[oi][oj].g + 1.0;
        hNew = calculateHValue(i, j);
        fNew = gNew + hNew;

        if (cellDetails[i][j].f == FLT_MAX
            || cellDetails[i][j].f > fNew)
        {
            openList.insert(std::make_pair(
                fNew, std::make_pair(i, j)));

            cellDetails[i][j].f = fNew;
            cellDetails[i][j].g = gNew;
            cellDetails[i][j].h = hNew;
            cellDetails[i][j].parentI = oi;
            cellDetails[i][j].parentJ = oj;
        }
    }

    void tracePath()
    {
        int row = destI;
        int col = destJ;

        std::stack<std::pair<int, int>> Path;

        while (!(cellDetails[row][col].parentI == row
            && cellDetails[row][col].parentJ == col)) {
            Path.push(std::make_pair(row, col));
            int temp_row = cellDetails[row][col].parentI;
            int temp_col = cellDetails[row][col].parentJ;
            row = temp_row;
            col = temp_col;
        }

        Path.push(std::make_pair(row, col));
        storedPath = Path;
        /*while (!Path.empty()) {
            std::pair<int, int> p = Path.top();
            Path.pop();
            printf("-> (%d,%d) ", p.first, p.second);
        }*/
        return;
    }

public:

    void init(int row, int col, int startI, int startJ, int destI, int destJ, std::vector<std::vector<block>> map)
    {
        std::vector<std::vector<cell>> cellDetailsInit(row, std::vector<cell>(col));
        cellDetails = cellDetailsInit;
        this->row = row;
        this->col = col;
        this->destI = destI;
        this->destJ = destJ;
        this->startI = startI;
        this->startJ = startJ;
        this->map = map;
    }

    std::stack<std::pair<int, int>> getPath()
    {
        return storedPath;
    }

    void aStarSearch()
    {
        std::vector<std::vector<bool>> closedList(row, std::vector<bool>(col, false));

        for (int i = 0; i < row; i++)
        {
            for (int j = 0; j < col; j++)
            {
                cellDetails[i][j].f = FLT_MAX;
                cellDetails[i][j].g = FLT_MAX;
                cellDetails[i][j].h = FLT_MAX;
                cellDetails[i][j].parentI = -1;
                cellDetails[i][j].parentJ = -1;
            }
        }

        int i = startI;
        int j = startJ;
        cellDetails[i][j].f = 0.0;
        cellDetails[i][j].g = 0.0;
        cellDetails[i][j].h = 0.0;
        cellDetails[i][j].parentI = i;
        cellDetails[i][j].parentJ = j;

        openList.insert(std::make_pair(0.0, std::make_pair(i, j)));
        bool foundDest = false;

        while (!openList.empty())
        {
            std::pair<double, std::pair<int, int>> p = *openList.begin();
            openList.erase(openList.begin());


            i = p.second.first;
            j = p.second.second;
            closedList[i][j] = true;

            double gNew, hNew, fNew;

            std::cout << isValid(i - 1, j, row, col) << std::endl;

            if (isValid(i - 1, j, row, col) == true)
            {
                if (hasReachedDestination(i, j, i - 1, j, foundDest))
                {
                    return;
                }

                if (closedList[i - 1][j] == false
                    && isUnBlocked(i - 1, j)
                    == true)
                {
                    checkNextBlock(gNew, hNew, fNew, i, j, i - 1, j);
                }
            }
            if (isValid(i + 1, j, row, col) == true)
            {
                if (hasReachedDestination(i, j, i + 1, j, foundDest))
                {
                    return;
                }
                else if (closedList[i + 1][j] == false
                    && isUnBlocked(i + 1, j)
                    == true)
                {
                    checkNextBlock(gNew, hNew, fNew, i, j, i + 1, j);
                }
            }
            if (isValid(i, j + 1, row, col) == true)
            {
                if (hasReachedDestination(i, j, i, j + 1, foundDest))
                {
                    return;
                }
                else if (closedList[i][j + 1] == false
                    && isUnBlocked(i, j + 1)
                    == true)
                {
                    checkNextBlock(gNew, hNew, fNew, i, j, i, j + 1);
                }
            }
            if (isValid(i, j - 1, row, col) == true)
            {
                if (hasReachedDestination(i, j, i, j - 1, foundDest))
                {
                    return;
                }
                else if (closedList[i][j - 1] == false
                    && isUnBlocked(i, j - 1)
                    == true)
                {
                    checkNextBlock(gNew, hNew, fNew, i, j, i, j - 1);
                }
            }
            if (isValid(i - 1, j + 1, row, col) == true)
            {
                if (hasReachedDestination(i, j, i - 1, j + 1, foundDest))
                {
                    return;
                }
                else if (closedList[i - 1][j + 1] == false
                    && isUnBlocked(i - 1, j + 1)
                    == true)
                {
                    checkNextBlock(gNew, hNew, fNew, i, j, i - 1, j + 1);
                }
            }
            if (isValid(i - 1, j - 1, row, col) == true)
            {
                if (hasReachedDestination(i, j, i - 1, j - 1, foundDest))
                {
                    return;
                }
                else if (closedList[i - 1][j - 1] == false
                    && isUnBlocked(i - 1, j - 1)
                    == true)
                {
                    checkNextBlock(gNew, hNew, fNew, i, j, i - 1, j - 1);
                }
            }
            if (isValid(i + 1, j + 1, row, col) == true)
            {
                if (hasReachedDestination(i, j, i + 1, j + 1, foundDest))
                {
                    return;
                }
                else if (closedList[i + 1][j + 1] == false
                    && isUnBlocked(i + 1, j + 1)
                    == true)
                {
                    checkNextBlock(gNew, hNew, fNew, i, j, i + 1, j + 1);
                }
            }
            if (isValid(i + 1, j - 1, row, col) == true)
            {
                if (hasReachedDestination(i, j, i + 1, j - 1, foundDest))
                {
                    return;
                }
                else if (closedList[i + 1][j - 1] == false
                    && isUnBlocked(i + 1, j - 1)
                    == true)
                {
                    checkNextBlock(gNew, hNew, fNew, i, j, i + 1, j - 1);
                }
            }
        }
        if (foundDest == false)
        {
            std::cout << "Failed to find destination" << std::endl;
        }

        return;
    }
};

int main()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow("SDL2 Window",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        800,
        600,
        0);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Event event;

    std::vector<block> row(18);

    std::vector<std::vector<block>> blocks(25, row);

    int initialX = 20;
    int initialY = 20;
    bool hasStart = false;
    bool hasEnd = false;

    mode mode = mode::BACKTONORMAL;

    for (int i = 0; i < blocks.size(); i++)
    {
        for (int j = 0; j < blocks[i].size(); j++)
        {
            block b{ SDL_Rect{initialX + (i * 30), initialY + (j * 30), 25, 25}, color{ 0, 0, 255, 255}, false };
            blocks[i][j] = b;
        }
    }

    int mouseX = 0;
    int mouseY = 0;

    int startI = -1;
    int startJ = -1;

    int destI = -1;
    int destJ = -1;

    bool running = true;
    while (running)
    {
        SDL_RenderClear(renderer);

        SDL_Rect rect{ 0,0,120,240 };
        struct color col { 255, 0, 0, 0 };
        std::string buttonPressed;

        if (mode == mode::RESET)
        {
            for (int i = 0; i < blocks.size(); i++)
            {
                for (int j = 0; j < blocks[i].size(); j++)
                {
                    blocks[i][j].col = color{ 0,0,255,255 };
                    if (blocks[i][j].isObstacle)
                    {
                        blocks[i][j].isObstacle = false;
                    }
                }
            }
            if (hasEnd)
            {
                blocks[destI][destJ].isEnd = false;
                destI = -1;
                destJ = -1;
                hasEnd = false;
            }
            if (hasStart)
            {
                blocks[startI][startJ].isStart = false;
                startI = -1;
                startJ = -1;
                hasStart = false;
            }
            mode = mode::BACKTONORMAL;
        }

        while (SDL_PollEvent(&event) > 0)
        {
            switch (event.type)
            {
            case SDL_QUIT:
                std::cout << "Quitting application\n";
                running = false;
                break;

            case SDL_MOUSEMOTION:
                /*std::cout << "Mouse Position = { "
                    << event.motion.x << " "
                    << event.motion.y << " }\n";
                    */
                mouseX = event.motion.x;
                mouseY = event.motion.y;
                break;

            case SDL_MOUSEBUTTONDOWN:
                buttonPressed = convert_button_number_to_string(event.button.button);
                if (event.button.state == SDL_PRESSED && buttonPressed == "left")
                {
                    for (int i = 0; i < blocks.size(); i++)
                    {
                        for (int j = 0; j < blocks[i].size(); j++)
                        {
                            if (pointRectCollision(mouseX, mouseY, &blocks[i][j].rect))
                            {
                                switch (mode)
                                {
                                case mode::START:
                                    if (!hasStart && !blocks[i][j].isObstacle && !blocks[i][j].isEnd)
                                    {
                                        hasStart = true;
                                        startI = i;
                                        startJ = j;
                                        blocks[i][j].col = color{ 0,255,255,255 };
                                        blocks[i][j].isStart = true;
                                    }
                                    break;
                                case mode::END:
                                    if (!hasEnd && !blocks[i][j].isObstacle && !blocks[i][j].isStart)
                                    {
                                        //std::cout << "is here" << std::endl;
                                        hasEnd = true;
                                        destI = i;
                                        destJ = j;
                                        blocks[i][j].col = color{ 255,0,0,255 };
                                        blocks[i][j].isEnd = true;
                                    }
                                    break;
                                case mode::OBSTACLE:
                                    if (!blocks[i][j].isStart && !blocks[i][j].isEnd)
                                    {
                                        blocks[i][j].col = color{ 255,255,0,255 };
                                        blocks[i][j].isObstacle = true;
                                    }
                                    break;
                                case mode::BACKTONORMAL:
                                    if (blocks[i][j].isEnd)
                                    {
                                        hasEnd = false;
                                        blocks[i][j].isEnd = false;
                                        destI = -1;
                                        destJ = -1;
                                    }
                                    if (blocks[i][j].isStart)
                                    {
                                        hasStart = false;
                                        blocks[i][j].isStart = false;
                                        startI = -1;
                                        startJ = -1;
                                    }
                                    if (blocks[i][j].isObstacle)
                                    {
                                        blocks[i][j].isObstacle = false;
                                    }
                                    blocks[i][j].col = color{ 0,0,255,255 };
                                }
                                break;
                            }
                        }
                    }
                }
                break;
            case SDL_MOUSEBUTTONUP:
                std::cout << convert_button_number_to_string(event.button.button)
                    << " mouse button " << ((event.button.state == SDL_PRESSED) ? "pressed" : "released");
                std::cout << "\n";
                break;

            case SDL_KEYDOWN:
                if (event.key.state == SDL_PRESSED)
                {
                    switch ((char)event.key.keysym.sym)
                    {
                    case 'a':
                        mode = mode::START;
                        std::cout << "START" << std::endl;
                        break;
                    case 's':
                        mode = mode::END;
                        std::cout << "END" << std::endl;
                        break;
                    case 'd':
                        mode = mode::OBSTACLE;
                        std::cout << "OBSTACLE" << std::endl;
                        break;
                    case 'f':
                        mode = mode::BACKTONORMAL;
                        std::cout << "BACKTONORMAL" << std::endl;
                        break;
                    case 'c':
                        if (hasStart && hasEnd)
                        {
                            AstarPathFinding astar;
                            astar.init(blocks.size(), blocks[0].size(), startI, startJ, destI, destJ, blocks);
                            astar.aStarSearch();
                            std::stack<std::pair<int, int>> storedPath = astar.getPath();
                            //aStarSearch(blocks);
                            if (storedPath.size() == 0)
                            {
                                mode = mode::RESET;
                                std::cout << "Path Not Present" << std::endl;
                            }
                            else
                            {
                                storedPath.pop();
                                while (storedPath.size() > 1) {
                                    std::pair<int, int> p = storedPath.top();
                                    storedPath.pop();
                                    //printf("-> (%d,%d) ", p.first, p.second);
                                    blocks[p.first][p.second].col = color{ 255,126,200,255 };
                                }
                            }
                        }
                        break;
                    case 'r':
                        mode = mode::RESET;
                        std::cout << "RESET" << std::endl;
                        break;
                    }
                }
                break;
            case SDL_KEYUP:
                //std::cout << "Key " << (char)event.key.keysym.sym << " "
                //    << ((event.key.state == SDL_PRESSED) ? "pressed" : "released") << "\n";
                break;
            }
        }

        //draw_rectF(&rect, col, renderer);

        for (int i = 0; i < blocks.size(); i++)
        {
            for (int j = 0; j < blocks[i].size(); j++)
            {
                draw_rectF(&blocks[i][j].rect, blocks[i][j].col, renderer);
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}



