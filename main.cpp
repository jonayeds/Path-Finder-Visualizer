#include <SFML/Graphics.hpp>
#include <iostream>
using namespace std;
using namespace sf;

int main()
{
    RenderWindow win(VideoMode({1200, 800}), "Path Finder");
    RectangleShape box;
    RectangleShape playBtn;
    int boxSize= 15;
    box.setFillColor(Color::White);
    box.setOutlineColor(Color(235, 235, 235));
    box.setOutlineThickness(2);
    playBtn.setSize(Vector2f(100, 60));
    playBtn.setPosition(Vector2f(boxSize*55, boxSize*2));
    playBtn.setFillColor(Color::Green);
    box.setSize(Vector2f(boxSize, boxSize));

    

    Vector2i mo;

    struct Box
    {
        int x;
        int y;
        bool isEmpty;
    };

    vector<vector<Box>> boxList(40);

    Box start;
    Box finish;
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
                    int x = boxSize * (j+2);
                    int y = boxSize * (i+2);
                    boxList[i].push_back({x, y, true});
                    if (mo.x > x && mo.y > y && mo.x < x + boxSize && mo.y < y + boxSize)
                    {
                        if (Mouse::isButtonPressed(Mouse::Button::Left))
                        {
                            bool isStartNode = boxList[i][j].x == start.x && boxList[i][j].y == start.y;
                            bool isFinishNode = boxList[i][j].x == finish.x && boxList[i][j].y == finish.y;
                            if(!isStartSelected){
                                start = {x,y,true};
                                isStartSelected=true;
                                cout << "Start node selected i:"<< i << " j:"<< j << endl; 
                                continue;
                            }
                            if(!isFinishSelected && !isStartNode){
                                finish = {x,y,true};
                                isFinishSelected=true;
                                cout << "Finish node selected i:"<< i << " j:"<< j << endl;
                                continue;
                            }

                            if(isStartNode || isFinishNode ){
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
                    if(boxList[i][j].x == start.x && boxList[i][j].y == start.y && isStartSelected){
                        box.setFillColor(Color::Green);
                        win.draw(box);
                        continue;
                    }else if(boxList[i][j].x == finish.x && boxList[i][j].y == finish.y && isFinishSelected){
                        box.setFillColor(Color::Red);
                        win.draw(box);
                        continue;
                    }
                    if (!boxList[i][j].isEmpty)
                    {
                        box.setFillColor(Color(50, 73, 97));
                        win.draw(box);
                    }
                    else
                    {
                        box.setFillColor(Color::White);
                        win.draw(box);
                    }
                }
            }

            win.draw(playBtn);

            win.display();
        }
    }

    return 0;
}

// g++ -std=c++17 main.cpp -o sfml-app '\'
//  -I/opt/homebrew/opt/sfml/include '\'
//  -L/opt/homebrew/opt/sfml/lib '\'
// -lsfml-graphics -lsfml-window -lsfml-system && ./sfml-app