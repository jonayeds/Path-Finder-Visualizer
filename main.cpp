#include <SFML/Graphics.hpp>
#include <iostream>
#include <stack>
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
    bool isExplored;
};

vector<vector<Box>> boxList(40);

Box start;
Box finish;

string bfsLength = "0";
string dijkstraLength = "0";
string dfsLength = "0";

void drawWindow(RectangleShape box, bool isStartSelected, bool isFinishSelected, RenderWindow &win, string time, Text txt, int boxSize, string algo)
{
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
            else if (boxList[r][c].isExplored)
            {
                box.setFillColor(Color::Cyan);
            }
            else
            {
                box.setFillColor(Color::White);
                win.draw(box);
            }
        }
    }
    txt.setString(algo + " :");
    txt.setPosition(Vector2f(boxSize * 60 - 40, boxSize * 20 + 15));
    win.draw(txt);
    txt.setString(time);
    if (algo == "bfs")
    {
        txt.setPosition(Vector2f(boxSize * 60 + 25, boxSize * 20 + 15));
    }
    else
    {
        txt.setPosition(Vector2f(boxSize * 61 + 65, boxSize * 20 + 15));
    }
    win.draw(txt);
}

class Algorithms
{

public:
    string applyBFS(Text txt, Clock clock, string time, RenderWindow &win, RectangleShape &box, int boxSize, bool isStartSelected, bool isFinishSelected)
    {

        int rows = boxList.size();
        int cols = boxList[0].size();
        queue<Box> q;

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

                if (nextRow < 0 || nextRow >= rows || nextCol < 0 || nextCol >= cols)
                {
                    continue;
                }

                if (boxList[nextRow][nextCol].isEmpty && !boxList[nextRow][nextCol].isVisited)
                {
                    boxList[nextRow][nextCol].isVisited = true;
                    boxList[nextRow][nextCol].parentRow = current.row;
                    boxList[nextRow][nextCol].parentCol = current.col;
                    q.push(boxList[nextRow][nextCol]);

                    // Update time during BFS execution
                    float timeCount = clock.getElapsedTime().asSeconds();
                    time = to_string(timeCount);

                    win.clear();
                    drawWindow(box, isStartSelected, isFinishSelected, win, time, txt, boxSize, "BFS");
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

                win.clear();
                drawWindow(box, isStartSelected, isFinishSelected, win, time, txt, boxSize, "BFS");
                win.display();
                sf::sleep(sf::milliseconds(100)); // Slower delay for path visualization
            }
            path.push_back(start);
            bfsLength = to_string(path.size() - 1);
        }else{
            bfsLength = "N/A";
        }
        return time;
    }

public:
    string applyDijkstra(Text txt, Clock clock, string time, RenderWindow &win, RectangleShape &box, int boxSize, bool isStartSelected, bool isFinishSelected)
    {
        struct Compare
        {
            bool operator()(const vector<int> &a, const vector<int> &b)
            {
                return a[0] > b[0]; // Min-heap based on distance
            }
        };

        priority_queue<vector<int>, vector<vector<int>>, Compare> pq;
        vector<vector<int>> distances(boxList.size(), vector<int>(boxList[0].size(), INT_MAX));
        vector<vector<bool>> visited(boxList.size(), vector<bool>(boxList[0].size(), false));

        // Initialize starting node
        distances[start.row][start.col] = 0;
        boxList[start.row][start.col].parentRow = -1; // Set start node parent
        boxList[start.row][start.col].parentCol = -1;
        pq.push({0, start.row, start.col}); // {distance, row, col}

        int nextRows[] = {1, -1, 0, 0};
        int nextCols[] = {0, 0, 1, -1};

        bool isFound = false;

        while (!pq.empty())
        {
            vector<int> current = pq.top();
            pq.pop();

            int currentDist = current[0];
            int currentRow = current[1];
            int currentCol = current[2];

            // Skip if already visited with a better distance
            if (visited[currentRow][currentCol])
            {
                continue;
            }

            // Mark as visited and update visualization
            visited[currentRow][currentCol] = true;
            boxList[currentRow][currentCol].isVisited = true;
            // Don't reset parent information here - we need it for path reconstruction!

            // Check if we reached the finish
            if (currentRow == finish.row && currentCol == finish.col)
            {
                isFound = true;
                break;
            }

            // Explore neighbors
            for (int i = 0; i < 4; i++)
            {
                int nextRow = currentRow + nextRows[i];
                int nextCol = currentCol + nextCols[i];

                // Bounds checking
                if (nextRow < 0 || nextRow >= boxList.size() ||
                    nextCol < 0 || nextCol >= boxList[0].size())
                {
                    continue;
                }

                // Skip if not empty, already visited, or is an obstacle
                if (!boxList[nextRow][nextCol].isEmpty || visited[nextRow][nextCol])
                {
                    continue;
                }

                int newDistance = currentDist + 1; // All edges have weight 1

                // If we found a shorter path to this neighbor
                if (newDistance < distances[nextRow][nextCol])
                {
                    distances[nextRow][nextCol] = newDistance;
                    boxList[nextRow][nextCol].parentRow = currentRow;
                    boxList[nextRow][nextCol].parentCol = currentCol;
                    pq.push({newDistance, nextRow, nextCol});
                }
            }

            // Update time and visualize
            float timeCount = clock.getElapsedTime().asSeconds();
            time = to_string(timeCount);

            win.clear();
            drawWindow(box, isStartSelected, isFinishSelected, win, time, txt, boxSize, "Dijkstra");
            win.display();
            sf::sleep(sf::milliseconds(2)); // Slightly slower than BFS for better visualization
        }

        // Reconstruct path if found
        if (isFound)
        {
            vector<Box> path;
            int currentRow = finish.row;
            int currentCol = finish.col;

            while (boxList[currentRow][currentCol].parentRow != -1 && path.size() < 2000)
            {
                path.push_back(boxList[currentRow][currentCol]);
                boxList[currentRow][currentCol].isPath = true;

                int parentRow = boxList[currentRow][currentCol].parentRow;
                int parentCol = boxList[currentRow][currentCol].parentCol;
                currentRow = parentRow;
                currentCol = parentCol;

                win.clear();
                drawWindow(box, isStartSelected, isFinishSelected, win, time, txt, boxSize, "Dijkstra");
                win.display();
                sf::sleep(sf::milliseconds(100));
            }
            path.push_back(start);
            dijkstraLength = to_string(distances[finish.row][finish.col]);
        }else{
            dijkstraLength = "N/A";
        }

        return time;
    }

public:
    string applyDFS(Text txt, Clock clock, string time, RenderWindow &win, RectangleShape &box, int boxSize, bool isStartSelected, bool isFinishSelected)
    {
        int rows = boxList.size();
        int cols = boxList[0].size();
        stack<Box> s; // Stack for DFS (LIFO - Last In First Out)

        int nextRows[] = {1, -1, 0, 0};
        int nextCols[] = {0, 0, 1, -1};

        s.push(start);
        boxList[start.row][start.col].isVisited = true;
        boxList[start.row][start.col].parentRow = -1;
        boxList[start.row][start.col].parentCol = -1;

        bool isFound = false;

        while (!s.empty() && s.size() < 2000)
        {
            Box current = s.top();
             // Always pop the current node first

            if (current.row == finish.row && current.col == finish.col)
            {
                float finalTimeCount = clock.getElapsedTime().asSeconds();
                time = to_string(finalTimeCount);
                isFound = true;
                finish.parentCol = current.parentCol;
                finish.parentRow = current.parentRow;
                break;
            }

            // Look for an unvisited neighbor
            bool foundNeighbor = false;
            for (int i = 0; i < 4; i++)
            {
                int nextRow = current.row + nextRows[i];
                int nextCol = current.col + nextCols[i];
                
                if (nextRow < 0 || nextRow >= rows || nextCol < 0 || nextCol >= cols)
                {
                    continue;
                }

                if (boxList[nextRow][nextCol].isEmpty && !boxList[nextRow][nextCol].isVisited)
                {
                    boxList[nextRow][nextCol].isVisited = true;
                    boxList[nextRow][nextCol].parentRow = current.row;
                    boxList[nextRow][nextCol].parentCol = current.col;
                    s.push(boxList[nextRow][nextCol]);
                    foundNeighbor = true;
                    
                    // Update time during DFS execution
                    float timeCount = clock.getElapsedTime().asSeconds();
                    time = to_string(timeCount);

                    win.clear();
                    drawWindow(box, isStartSelected, isFinishSelected, win, time, txt, boxSize, "DFS");
                    win.display();
                    sf::sleep(sf::milliseconds(2));
                    break; // Only explore one neighbor at a time for DFS
                }
            }
            
            if (!foundNeighbor)
            {
                cout << "Backtracking from row:" << current.row << " col:" << current.col << " | Stack size: " << s.size() << endl;
                s.pop();
            }
        }

        // Reconstruct path if found
        if (isFound)
        {
            vector<Box> path;
            Box current = finish;
            cout << "Current r:"<< current.row << " | c:" << current.col<< "Parent r:" << current.parentRow << " | c:" << current.parentCol<< endl;
            while (current.parentRow != -1 && path.size() < 2000)
            {
                path.push_back(current);
                boxList[current.row][current.col].isPath = true;
                current = boxList[current.parentRow][current.parentCol];
                win.clear();
                drawWindow(box, isStartSelected, isFinishSelected, win, time, txt, boxSize, "DFS");
                win.display();
                sf::sleep(sf::milliseconds(10));
            }
            path.push_back(start);
            dfsLength = to_string(path.size() - 1);
        }else{
            dfsLength = "N/A";
        }

        return time;
    }
};

int main()
{
    RenderWindow win(VideoMode({1300, 800}), "Path Finder");
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
        txt.setCharacterSize(28);
        txt.setOutlineThickness(0.5);
        txt.setLetterSpacing(1.8);
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

    string bfsTime = "0";
    string dijkstraTime = "0";
    string dfsTime = "0";

    string algorithm = "bfs";

    while (win.isOpen())
    {
        mo = Mouse::getPosition(win);

        while (const optional event = win.pollEvent())
        {
            if (event->is<Event::Closed>() || Keyboard::isKeyPressed(Keyboard::Key::Escape))
            {
                win.close();
            }
            else if (event->is<Event::Resized>())
            {
                sf::Vector2f position(0.f, 0.f);
                sf::Vector2f size(win.getSize().x, win.getSize().y);
                sf::FloatRect visibleArea(position, size);
                win.setView(View(visibleArea));
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
                            boxList[i][j].isExplored = false;
                            start = {};
                            finish = {};
                            isStartSelected = false;
                            isFinishSelected = false;
                        }
                    }
                }
            }

            Clock clock; // Move clock creation here, before the button check
            // play button
            if (mo.x >= boxSize * 65 && mo.y >= boxSize * 2 && mo.x <= (boxSize * 65 + 100) && mo.y <= (boxSize * 2 + 60))
            {
                if (Mouse::isButtonPressed(Mouse::Button::Left))
                {
                    if (algorithm == "bfs")
                    {
                        clock.restart();
                        bfsTime = algo->applyBFS(txt, clock, bfsTime, win, box, boxSize, isStartSelected, isFinishSelected);
                    }
                    else if (algorithm == "dijkstra")
                    {
                        clock.restart();
                        dijkstraTime = algo->applyDijkstra(txt, clock, dijkstraTime, win, box, boxSize, isStartSelected, isFinishSelected);
                    }
                    else if (algorithm == "dfs")
                    {
                        clock.restart();
                        dfsTime = algo->applyDFS(txt, clock, dfsTime, win, box, boxSize, isStartSelected, isFinishSelected);
                    }
                }
            }

            button.setFillColor(Color(50, 73, 97));
            button.setSize(Vector2f(100, 60));
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

            // Algorithm selector
            // DFS Selector
            button.setSize(Vector2f(200, 60));
            button.setPosition(Vector2f(boxSize * 57, boxSize * 10));
            txt.setPosition(Vector2f(boxSize * 57 + 25, boxSize * 10 + 15));
            txt.setString("DFS");
            if (algorithm == "dfs")
            {
                button.setFillColor(Color(37, 148, 188));
            }
            else
            {
                button.setFillColor(Color(50, 73, 97));
            }
            win.draw(button);
            win.draw(txt);
            if (mo.x >= boxSize * 57 && mo.y >= boxSize * 10 && mo.x <= (boxSize * 65 + 200) && mo.y <= (boxSize * 10 + 60))
            {
                if (Mouse::isButtonPressed(Mouse::Button::Left))
                {
                    algorithm = "dfs";
                    for (int i = 0; i < boxList.size(); i++)
                    {
                        for (int j = 0; j < boxList[i].size(); j++)
                        {
                            boxList[i][j].isVisited = false;
                            boxList[i][j].isPath = false;
                        }
                    }
                }
            }

            // BFS selector
            button.setPosition(Vector2f(boxSize * 57, boxSize * 15));
            txt.setPosition(Vector2f(boxSize * 57 + 25, boxSize * 15 + 15));
            txt.setString("BFS");
            if (algorithm == "bfs")
            {
                button.setFillColor(Color(37, 148, 188));
            }
            else
            {
                button.setFillColor(Color(50, 73, 97));
            }
            win.draw(button);
            win.draw(txt);
            if (mo.x >= boxSize * 57 && mo.y >= boxSize * 15 && mo.x <= (boxSize * 65 + 200) && mo.y <= (boxSize * 15 + 60))
            {
                if (Mouse::isButtonPressed(Mouse::Button::Left))
                {
                    algorithm = "bfs";
                    for (int i = 0; i < boxList.size(); i++)
                    {
                        for (int j = 0; j < boxList[i].size(); j++)
                        {
                            boxList[i][j].isVisited = false;
                            boxList[i][j].isPath = false;
                        }
                    }
                }
            }

            // Dijkstra Selector
            if (algorithm == "dijkstra")
            {
                button.setFillColor(Color(37, 148, 188));
            }
            else
            {
                button.setFillColor(Color(50, 73, 97));
            }
            button.setPosition(Vector2f(boxSize * 57, boxSize * 20));
            txt.setPosition(Vector2f(boxSize * 57 + 25, boxSize * 20 + 15));
            txt.setString("DIJKSTRA");
            win.draw(button);
            win.draw(txt);
            if (mo.x >= boxSize * 57 && mo.y >= boxSize * 20 && mo.x <= (boxSize * 65 + 200) && mo.y <= (boxSize * 20 + 60))
            {
                if (Mouse::isButtonPressed(Mouse::Button::Left))
                {
                    algorithm = "dijkstra";
                    for (int i = 0; i < boxList.size(); i++)
                    {
                        for (int j = 0; j < boxList[i].size(); j++)
                        {
                            boxList[i][j].isVisited = false;
                            boxList[i][j].isPath = false;
                        }
                    }
                }
            }

            // BFS
            txt.setString("dfs :");
            txt.setPosition(Vector2f(boxSize * 60 - 40, boxSize * 25 + 15));
            win.draw(txt);
            txt.setString(dfsTime);
            txt.setPosition(Vector2f(boxSize * 60 + 25, boxSize * 25 + 15));
            win.draw(txt);
            txt.setString("Length :");
            txt.setPosition(Vector2f(boxSize * 70 + 25, boxSize * 25 + 15));
            win.draw(txt);
            txt.setString(dfsLength);
            txt.setPosition(Vector2f(boxSize * 78 + 25, boxSize * 25 + 15));
            win.draw(txt);

            // DFS
            txt.setString("dfs :");
            txt.setPosition(Vector2f(boxSize * 60 - 40, boxSize * 28 + 15));
            win.draw(txt);
            txt.setString(bfsTime);
            txt.setPosition(Vector2f(boxSize * 60 + 75, boxSize * 28 + 15));
            win.draw(txt);
            txt.setString("Length :");
            txt.setPosition(Vector2f(boxSize * 72 + 25, boxSize * 28 + 15));
            win.draw(txt);
            txt.setString(bfsLength);
            txt.setPosition(Vector2f(boxSize * 78 + 75, boxSize * 28 + 15));
            win.draw(txt);

            // Dijkstra
            txt.setString("Dijkstra :");
            txt.setPosition(Vector2f(boxSize * 60 - 40, boxSize * 31 + 15));
            win.draw(txt);
            txt.setString(dijkstraTime);
            txt.setPosition(Vector2f(boxSize * 60 + 75, boxSize * 31 + 15));
            win.draw(txt);
            txt.setString("Length :");
            txt.setPosition(Vector2f(boxSize * 72 + 25, boxSize * 31 + 15));
            win.draw(txt);
            txt.setString(dijkstraLength);
            txt.setPosition(Vector2f(boxSize * 78 + 75, boxSize * 31 + 15));
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