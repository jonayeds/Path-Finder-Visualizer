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
    void applyBFS()
    {

        int rows = boxList.size();
        int cols = boxList[0].size();
        queue<Box> q;
        vector<vector<bool>> visited(rows, vector<bool>(cols, false));

        int nextRows[] = {1, -1, 0, 0};
        int nextCols[] = {0, 0, 1, -1};

        q.push(start);
        visited[start.row][start.col] = true;
        boxList[start.row][start.col].parentRow =-1;
        boxList[start.row][start.col].parentCol =-1;
        start.isVisited=true;
        
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

                if (nextRow >= 0 && nextRow < rows && nextCol >= 0 && nextCol < cols && boxList[nextRow][nextCol].isEmpty && !visited[nextRow][nextCol])
                {
                    visited[nextRow][nextCol] = true;
                    boxList[nextRow][nextCol].isVisited = true;
                    boxList[nextRow][nextCol].parentRow = current.row;
                    boxList[nextRow][nextCol].parentCol = current.col;
                    q.push(boxList[nextRow][nextCol]);
                }
            }
        }

        vector<Box> path;
        if(isFound){
            Box current = finish;
            int count =0;
            while(current.parentRow != -1){
                path.push_back(current);
                boxList[current.row][current.col].isPath = true;
                current = boxList[current.parentRow][current.parentCol];
                count++;
            }
            path.push_back(start);   
        }
    }
};

int main()
{
    RenderWindow win(VideoMode({1200, 800}), "Path Finder");
    Image icon;
    if (icon.loadFromFile("./images/icon.jpg"))
    {
        win.setIcon(icon);
    }

    Algorithms* algo = new Algorithms();

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
    if (fnt.openFromFile("./fonts/primary.ttf"))
    {
        txt.setCharacterSize(24);
        txt.setOutlineThickness(1);
        txt.setFillColor(Color::White);
        txt.setOutlineColor(Color::White);
    }

    Vector2i mo;

    bool isStartSelected = false;
    bool isFinishSelected = false;

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
            for (int i = 0; i < 40; i++)
            {

                for (int j = 0; j < 50; j++)
                {
                    int x = boxSize * (j + 2);
                    int y = boxSize * (i + 2);
                    boxList[i].push_back({x, y, true, -1, -1, i, j, false, false});
                    if (mo.x > x && mo.y > y && mo.x < x + boxSize && mo.y < y + boxSize)
                    {
                        if (Mouse::isButtonPressed(Mouse::Button::Left))
                        {
                            bool isStartNode = boxList[i][j].x == start.x && boxList[i][j].y == start.y;
                            bool isFinishNode = boxList[i][j].x == finish.x && boxList[i][j].y == finish.y;
                            if (!isStartSelected)
                            {
                                start = {x, y, true, -1, -1, i, j, false, false};
                                isStartSelected = true;
                                cout << "Start node selected i:" << i << " j:" << j << endl;
                                continue;
                            }
                            if (!isFinishSelected && !isStartNode)
                            {
                                finish = {x, y, true, -1, -1, i, j, false, false};
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
                    }else if(boxList[i][j].isVisited && boxList[i][j].isPath){
                        box.setFillColor(Color::Yellow);
                        win.draw(box);
                    }
                    else if(boxList[i][j].isVisited){
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

            // play button
            if (mo.x >= boxSize * 65 && mo.y >= boxSize * 2 && mo.x <= (boxSize * 65 + 100) && mo.y <= (boxSize * 2 + 60))
            {
                if (Mouse::isButtonPressed(Mouse::Button::Left))
                {
                    algo->applyBFS();
                }
            }


            button.setPosition(Vector2f(boxSize * 55, boxSize * 2));
            txt.setPosition(Vector2f(boxSize * 55 + 25, boxSize * 2 + 15));
            txt.setString("Reset");
            win.draw(button);
            win.draw(txt);
            
            button.setPosition(Vector2f(boxSize * 65 , boxSize * 2));
            txt.setPosition(Vector2f(boxSize * 65 + 25, boxSize * 2 + 15));
            txt.setString("Play");
            win.draw(button);
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