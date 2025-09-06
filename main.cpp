#include <SFML/Graphics.hpp>
#include <iostream>
using namespace std;
using namespace sf;

struct Box
{
    int x;
    int y;
    bool isEmpty;
    int parentRow;
    int parentCol;
    int row;
    int col;
    bool isVisited;
    bool isPath;
};

vector<vector<Box>> boxList(40);

Box start;
Box finish;


class Algorithms
{

public:
    string applyBFS(Text txt, Clock clock, string time, RenderWindow &win, RectangleShape &box, int boxSize, bool isStartSelected, bool isFinishSelected)
    {

        int rows = boxList.size();
        int cols = boxList[0].size();
        queue<Box> q;
        // vector<vector<bool>> visited(rows, vector<bool>(cols, false));

        int nextRows[] = {1, -1, 0, 0};
        int nextCols[] = {0, 0, 1, -1};

        q.push(start);
        start.isVisited = true;

        bool isFound = false;

        while (!q.empty())
        {
            Box current = q.front();
            q.pop();



            if (current.row == finish.row && current.col == finish.col)
            {
                finish = current;
                isFound = true;
                break;
            }

            for (int i = 0; i < 4; i++)
            {
                int nextRow = current.row + nextRows[i];
                int nextCol = current.col + nextCols[i];

                if (nextRow < 0 || nextRow >= rows || nextCol < 0 || nextCol >= cols) {
                    continue;
                }
                
                if (boxList[nextRow][nextCol].isEmpty && !boxList[nextRow][nextCol].isVisited)
                {
                    boxList[nextRow][nextCol].isVisited = true;
                    boxList[nextRow][nextCol].parentRow = current.row;
                    boxList[nextRow][nextCol].parentCol = current.col;
                    q.push(boxList[nextRow][nextCol]);

                    win.clear();
                    for (int r = 0; r < 40; r++)
                    {
                        for (int c = 0; c < 50; c++)
                        {
                            box.setPosition(Vector2f(boxList[r][c].x, boxList[r][c].y));
                            if (boxList[r][c].x == start.x && boxList[r][c].y == start.y && isStartSelected)
                            {
                                box.setFillColor(Color::Green);
                                win.draw(box);
                                continue;
                            }
                            else if (boxList[r][c].x == finish.x && boxList[r][c].y == finish.y && isFinishSelected)
                            {
                                box.setFillColor(Color::Red);
                                win.draw(box);
                                continue;
                            }
                            if (!boxList[r][c].isEmpty)
                            {
                                box.setFillColor(Color(50, 73, 97));
                                win.draw(box);
                            }
                            else if (boxList[r][c].isVisited)
                            {
                                box.setFillColor(Color(75, 156, 237));
                                win.draw(box);
                            }
                            else
                            {
                                box.setFillColor(Color::White);
                                win.draw(box);
                            }
                        }
                    }
                    float timeCount = clock.getElapsedTime().asSeconds();
                    time = to_string(timeCount);
                    txt.setString(time);
                    txt.setPosition(Vector2f(boxSize * 60 + 25, boxSize * 20 + 15));
                    win.draw(txt);
                    win.display();
                    sf::sleep(sf::milliseconds(2)); 
                }
            }
        }

        vector<Box> path;
        boxList[start.row][start.col].parentRow = -1;
        boxList[start.row][start.col].parentCol = -1;
        if (isFound)
        {
            Box current = finish;
            while (current.parentRow != -1 && path.size() < 2000)
            {
                path.push_back(current);
                boxList[current.row][current.col].isPath = true;
                current = boxList[current.parentRow][current.parentCol];
                cout << current.parentRow << " | " << current.parentCol << endl;
                win.clear();
                for (int r = 0; r < 40; r++)
                {
                    for (int c = 0; c < 50; c++)
                    {
                        box.setPosition(Vector2f(boxList[r][c].x, boxList[r][c].y));
                        if (boxList[r][c].x == start.x && boxList[r][c].y == start.y && isStartSelected)
                        {
                            box.setFillColor(Color::Green);
                            win.draw(box);
                            continue;
                        }
                        else if (boxList[r][c].x == finish.x && boxList[r][c].y == finish.y && isFinishSelected)
                        {
                            box.setFillColor(Color::Red);
                            win.draw(box);
                            continue;
                        }
                        if (!boxList[r][c].isEmpty)
                        {
                            box.setFillColor(Color(50, 73, 97));
                            win.draw(box);
                        }
                        else if (boxList[r][c].isVisited && boxList[r][c].isPath)
                        {
                            box.setFillColor(Color::Yellow);
                            win.draw(box);
                        }
                        else if (boxList[r][c].isVisited)
                        {
                            box.setFillColor(Color(75, 156, 237));
                            win.draw(box);
                        }
                        else
                        {
                            box.setFillColor(Color::White);
                            win.draw(box);
                        }
                    }
                }
                txt.setString(time);
                txt.setPosition(Vector2f(boxSize * 60 + 25, boxSize * 20 + 15));
                win.draw(txt);
                win.display();
                sf::sleep(sf::milliseconds(100)); // Slower delay for path visualization
            }
            path.push_back(start);
        }
        return time;
    }
};

int main()
{
    RenderWindow win(VideoMode({1200, 800}), "Path Finder");
    Image icon;
    if (icon.loadFromFile("/Users/sajjad/University/SFML/Path-Finder/images/icon.jpg"))
    {
        win.setIcon(icon);
    }

    Algorithms *algo = new Algorithms();

    RectangleShape box;
    RectangleShape button;
    int boxSize = 15;
    box.setFillColor(Color::White);
    box.setOutlineColor(Color(235, 235, 235));
    box.setOutlineThickness(2);
    button.setSize(Vector2f(100, 60));
    button.setFillColor(Color(50, 73, 97));
    box.setSize(Vector2f(boxSize, boxSize));

    Font fnt;
    Text txt(fnt);
    if (fnt.openFromFile("/Users/sajjad/University/SFML/Path-Finder/fonts/primary.ttf"))
    {
        txt.setCharacterSize(24);
        txt.setOutlineThickness(1);
        txt.setFillColor(Color::White);
        txt.setOutlineColor(Color::White);
    }

    Vector2i mo;

    bool isStartSelected = false;
    bool isFinishSelected = false;

    // Initialize the boxList once
    for (int i = 0; i < 40; i++)
    {
        boxList[i].clear(); // Clear any existing data
        for (int j = 0; j < 50; j++)
        {
            int x = boxSize * (j + 2);
            int y = boxSize * (i + 2);
            boxList[i].push_back({x, y, true, -1, -1, i, j, false, false});
        }
    }

    string time = "0";
    while (win.isOpen())
    {
        mo = Mouse::getPosition(win);

        while (const optional event = win.pollEvent())
        {
            if (event->is<Event::Closed>() || Keyboard::isKeyPressed(Keyboard::Key::Escape))
            {
                win.close();
            }
            win.clear();
            
            // Handle mouse interactions with existing boxes
            for (int i = 0; i < 40; i++)
            {
                for (int j = 0; j < 50; j++)
                {
                    if (mo.x > boxList[i][j].x && mo.y > boxList[i][j].y && 
                        mo.x < boxList[i][j].x + boxSize && mo.y < boxList[i][j].y + boxSize)
                    {
                        if (Mouse::isButtonPressed(Mouse::Button::Left))
                        {
                            bool isStartNode = boxList[i][j].x == start.x && boxList[i][j].y == start.y;
                            bool isFinishNode = boxList[i][j].x == finish.x && boxList[i][j].y == finish.y;
                            if (!isStartSelected)
                            {
                                start = boxList[i][j];
                                isStartSelected = true;
                                cout << "Start node selected i:" << i << " j:" << j << endl;
                                continue;
                            }
                            if (!isFinishSelected && !isStartNode)
                            {
                                finish = boxList[i][j];
                                isFinishSelected = true;
                                cout << "Finish node selected i:" << i << " j:" << j << endl;
                                continue;
                            }

                            if (isStartNode || isFinishNode)
                            {
                                continue;
                            }
                            boxList[i][j].isEmpty = false;
                        }
                    }
                }
            }

            for (int i = 0; i < 40; i++)
            {
                for (int j = 0; j < 50; j++)
                {
                    box.setPosition(Vector2f(boxList[i][j].x, boxList[i][j].y));
                    if (boxList[i][j].x == start.x && boxList[i][j].y == start.y && isStartSelected)
                    {
                        box.setFillColor(Color::Green);
                        win.draw(box);
                        continue;
                    }
                    else if (boxList[i][j].x == finish.x && boxList[i][j].y == finish.y && isFinishSelected)
                    {
                        box.setFillColor(Color::Red);
                        win.draw(box);
                        continue;
                    }
                    if (!boxList[i][j].isEmpty)
                    {
                        box.setFillColor(Color(50, 73, 97));
                        win.draw(box);
                    }
                    else if (boxList[i][j].isVisited && boxList[i][j].isPath)
                    {
                        box.setFillColor(Color::Yellow);
                        win.draw(box);
                    }
                    else if (boxList[i][j].isVisited)
                    {
                        box.setFillColor(Color(75, 156, 237));
                        win.draw(box);
                    }
                    else
                    {
                        box.setFillColor(Color::White);
                        win.draw(box);
                    }
                }
            }

            // Reset Button
            if (mo.x >= boxSize * 55 && mo.y >= boxSize * 2 && mo.x <= (boxSize * 55 + 100) && mo.y <= (boxSize * 2 + 60))
            {
                if (Mouse::isButtonPressed(Mouse::Button::Left))
                {
                    for (int i = 0; i < boxList.size(); i++)
                    {
                        for (int j = 0; j < boxList[i].size(); j++)
                        {
                            boxList[i][j].isEmpty = true;
                            boxList[i][j].isVisited = false;
                            boxList[i][j].isPath = false;
                            start = {};
                            finish = {};
                            isStartSelected = false;
                            isFinishSelected = false;
                        }
                    }
                }
            }

            Clock clock;
            // play button
            if (mo.x >= boxSize * 65 && mo.y >= boxSize * 2 && mo.x <= (boxSize * 65 + 100) && mo.y <= (boxSize * 2 + 60))
            {
                if (Mouse::isButtonPressed(Mouse::Button::Left))
                {
                    time = algo->applyBFS(txt, clock, time, win, box, boxSize, isStartSelected, isFinishSelected);
                }
            }

            button.setPosition(Vector2f(boxSize * 55, boxSize * 2));
            txt.setPosition(Vector2f(boxSize * 55 + 25, boxSize * 2 + 15));
            txt.setString("Reset");
            win.draw(button);
            win.draw(txt);

            button.setPosition(Vector2f(boxSize * 65, boxSize * 2));
            txt.setPosition(Vector2f(boxSize * 65 + 25, boxSize * 2 + 15));
            txt.setString("Play");
            win.draw(button);
            win.draw(txt);

            txt.setString(time);
            txt.setPosition(Vector2f(boxSize * 60 + 25, boxSize * 20 + 15));
            win.draw(txt);

            win.display();
        }
    }

    return 0;
}

// g++ -std=c++17 main.cpp -o sfml-app '\'
//  -I/opt/homebrew/opt/sfml/include '\'
//  -L/opt/homebrew/opt/sfml/lib '\'
// -lsfml-graphics -lsfml-window -lsfml-system && ./sfml-app