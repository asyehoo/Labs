#include <GL/glut.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <limits>
#include <string>
#include <sstream>

using namespace std;

const int maxSize = 20;
const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 700;
const float NODE_RADIUS = 20.0f;
const float PI = 3.14159265358979323846f;
const int INF = numeric_limits<int>::max();

void drawText(float x, float y, const string& text) {
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
}

class Graph {
private:
    vector<int> vertList;
    int adjMatrix[maxSize][maxSize];
    int nextVertexId;

public:
    Graph() : nextVertexId(1) {
        for (int i = 0; i < maxSize; ++i) {
            for (int j = 0; j < maxSize; ++j) {
                adjMatrix[i][j] = (i == j) ? 0 : INF;
            }
        }
    }

    int GetVertPos(int vertex) const {
        for (size_t i = 0; i < vertList.size(); ++i) {
            if (vertList[i] == vertex) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    bool IsEmpty() const {
        return vertList.empty();
    }

    bool IsFull() const {
        return vertList.size() == maxSize;
    }

    size_t GetAmountVerts() const {
        return vertList.size();
    }

    void InsertVertex(int vertex) {
        if (!IsFull()) {
            vertList.push_back(vertex);
            if (vertex >= nextVertexId) {
                nextVertexId = vertex + 1;
            }
        }
    }

    int AddVertexAtPosition(float x, float y) {
        if (!IsFull()) {
            int newId = nextVertexId++;
            vertList.push_back(newId);
            return newId;
        }
        return -1;
    }

    void InsertEdge(int vertex1, int vertex2, int weight) {
        int vertPos1 = GetVertPos(vertex1);
        int vertPos2 = GetVertPos(vertex2);
        if (vertPos1 != -1 && vertPos2 != -1) {
            adjMatrix[vertPos1][vertPos2] = weight;
            adjMatrix[vertPos2][vertPos1] = weight;
        }
    }

    void UpdateEdgeWeight(int vertex1, int vertex2, int newWeight) {
        int vertPos1 = GetVertPos(vertex1);
        int vertPos2 = GetVertPos(vertex2);
        if (vertPos1 != -1 && vertPos2 != -1) {
            adjMatrix[vertPos1][vertPos2] = newWeight;
            adjMatrix[vertPos2][vertPos1] = newWeight;
        }
    }

    void RemoveEdge(int vertex1, int vertex2) {
        int vertPos1 = GetVertPos(vertex1);
        int vertPos2 = GetVertPos(vertex2);
        if (vertPos1 != -1 && vertPos2 != -1) {
            adjMatrix[vertPos1][vertPos2] = INF;
            adjMatrix[vertPos2][vertPos1] = INF;
        }
    }

    int GetWeight(int vertex1, int vertex2) const {
        int vertPos1 = GetVertPos(vertex1);
        int vertPos2 = GetVertPos(vertex2);
        if (vertPos1 != -1 && vertPos2 != -1) {
            return adjMatrix[vertPos1][vertPos2];
        }
        return INF;
    }

    void RemoveVertex(int vertexId) {
        int pos = GetVertPos(vertexId);
        if (pos != -1) {
            vertList.erase(vertList.begin() + pos);

            for (int i = pos; i < static_cast<int>(vertList.size()); ++i) {
                for (int j = 0; j < maxSize; ++j) {
                    adjMatrix[i][j] = adjMatrix[i + 1][j];
                }
            }

            for (int j = pos; j < static_cast<int>(vertList.size()); ++j) {
                for (int i = 0; i < maxSize; ++i) {
                    adjMatrix[i][j] = adjMatrix[i][j + 1];
                }
            }

            for (int i = 0; i < maxSize; ++i) {
                adjMatrix[static_cast<int>(vertList.size())][i] = INF;
                adjMatrix[i][static_cast<int>(vertList.size())] = INF;
            }
        }
    }

    void Print() const {
        cout << "Матрица смежности:\n   ";
        for (int v : vertList) cout << v << " ";
        cout << endl;
        for (size_t i = 0; i < vertList.size(); ++i) {
            cout << vertList[i] << ": ";
            for (size_t j = 0; j < vertList.size(); ++j) {
                if (adjMatrix[i][j] == INF) cout << "INF ";
                else cout << adjMatrix[i][j] << " ";
            }
            cout << endl;
        }
    }

    pair<vector<int>, int> SolveTSP() {
        vector<int> path;
        int min_path = INF;

        if (vertList.size() < 2) return { path, 0 };

        vector<bool> visited(vertList.size(), false);
        vector<int> current_path;
        current_path.push_back(0);
        visited[0] = true;

        TSPRec(0, 1, 0, visited, current_path, path, min_path);

        vector<int> result_path;
        for (int idx : path) {
            result_path.push_back(vertList[idx]);
        }
        result_path.push_back(vertList[0]);

        return { result_path, min_path };
    }

private:
    void TSPRec(int current_pos, int count, int current_cost,
        vector<bool>& visited, vector<int>& current_path,
        vector<int>& final_path, int& final_cost) {
        if (count == vertList.size()) {
            int return_cost = adjMatrix[current_path.back()][current_path[0]];
            if (return_cost != INF) {
                int total_cost = current_cost + return_cost;
                if (total_cost < final_cost) {
                    final_cost = total_cost;
                    final_path = current_path;
                }
            }
            return;
        }

        for (size_t i = 0; i < vertList.size(); ++i) {
            if (!visited[i] && adjMatrix[current_pos][i] != INF) {
                int new_cost = current_cost + adjMatrix[current_pos][i];
                if (new_cost < final_cost) {
                    visited[i] = true;
                    current_path.push_back(i);

                    TSPRec(i, count + 1, new_cost, visited, current_path, final_path, final_cost);

                    visited[i] = false;
                    current_path.pop_back();
                }
            }
        }
    }

public:
    const vector<int>& getVertices() const { return vertList; }
    const int(&getAdjMatrix() const)[maxSize][maxSize]{ return adjMatrix; }
};

class GraphVisualizer {
private:
    Graph graph;
    int selectedNode;
    bool showWeights;
    bool edgeCreationMode;
    int edgeStartNode;
    bool weightInputMode;
    int inputWeight;
    vector<pair<float, float>> vertexPositions;
    bool showTSP;
    vector<int> tspPath;
    int tspCost;

    int findNodeAt(int x, int y) const {
        for (size_t i = 0; i < vertexPositions.size(); ++i) {
            float dx = vertexPositions[i].first - static_cast<float>(x);
            float dy = vertexPositions[i].second - static_cast<float>(y);
            if (dx * dx + dy * dy <= NODE_RADIUS * NODE_RADIUS) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    void arrangeVertices() {
        vertexPositions.clear();
        const auto& vertices = graph.getVertices();
        float centerX = static_cast<float>(WINDOW_WIDTH) / 2.0f;
        float centerY = static_cast<float>(WINDOW_HEIGHT) / 2.0f;
        float radius = static_cast<float>(min(WINDOW_WIDTH, WINDOW_HEIGHT)) * 0.4f;

        for (size_t i = 0; i < vertices.size(); ++i) {
            float angle = 2.0f * PI * static_cast<float>(i) / static_cast<float>(vertices.size());
            float x = centerX + radius * cos(angle);
            float y = centerY + radius * sin(angle);
            vertexPositions.emplace_back(x, y);
        }
    }

public:
    GraphVisualizer() : selectedNode(-1), showWeights(true),
        edgeCreationMode(false), edgeStartNode(-1),
        weightInputMode(false), inputWeight(1),
        showTSP(false), tspCost(0) {
        // Инициализация тестового графа
        for (int i = 1; i <= 7; i++) {
            graph.InsertVertex(i);
        }
        // Матрица смежности из примера
        int tempMatrix[7][7] = {
            {0, 6, 29, 10, 59, 34, 13},
            {6, 0, 23, 4, 53, 28, 7},
            {23, 21, 0, 20, 31, 6, 17},
            {8, 3, 20, 0, 50, 25, 4},
            {43, 40, 22, 38, 0, 26, 47},
            {26, 24, 4, 22, 18, 0, 22},
            {10, 5, 17, 4, 35, 19, 0}
        };

        for (int i = 0; i < 7; ++i) {
            for (int j = 0; j < 7; ++j) {
                if (tempMatrix[i][j] != 0) {
                    graph.InsertEdge(i + 1, j + 1, tempMatrix[i][j]);
                }
            }
        }

        arrangeVertices();
    }

    void draw() {
        glClear(GL_COLOR_BUFFER_BIT);

        // Отрисовка рёбер
        const auto& vertices = graph.getVertices();
        const auto& adjMatrix = graph.getAdjMatrix();

        for (size_t i = 0; i < vertices.size(); ++i) {
            for (size_t j = i + 1; j < vertices.size(); ++j) {
                if (adjMatrix[i][j] != INF) {
                    float x1 = vertexPositions[i].first;
                    float y1 = vertexPositions[i].second;
                    float x2 = vertexPositions[j].first;
                    float y2 = vertexPositions[j].second;

                    glColor3f(0.5f, 0.5f, 0.5f);
                    glLineWidth(1.0f);
                    glBegin(GL_LINES);
                    glVertex2f(x1, y1);
                    glVertex2f(x2, y2);
                    glEnd();

                    if (showWeights) {
                        float midX = (x1 + x2) / 2.0f;
                        float midY = (y1 + y2) / 2.0f;
                        glColor3f(0.0f, 0.0f, 0.0f);
                        drawText(midX, midY, to_string(adjMatrix[i][j]));
                    }
                }
            }
        }

        // Отрисовка пути коммивояжера
        if (showTSP && !tspPath.empty()) {
            glColor3f(0.0f, 1.0f, 0.0f);
            glLineWidth(3.0f);
            glBegin(GL_LINE_LOOP);
            for (int v : tspPath) {
                int pos = graph.GetVertPos(v);
                if (pos != -1) {
                    glVertex2f(vertexPositions[pos].first, vertexPositions[pos].second);
                }
            }
            glEnd();
            glLineWidth(1.0f);
        }

        // Отрисовка вершин
        for (size_t i = 0; i < vertices.size(); ++i) {
            float x = vertexPositions[i].first;
            float y = vertexPositions[i].second;

            glColor3f(i == static_cast<size_t>(selectedNode) ? 1.0f : 0.0f, 0.0f, 0.0f);
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(x, y);
            for (int j = 0; j <= 360; j += 10) {
                float a = static_cast<float>(j) * PI / 180.0f;
                glVertex2f(x + NODE_RADIUS * cos(a), y + NODE_RADIUS * sin(a));
            }
            glEnd();

            glColor3f(1.0f, 1.0f, 1.0f);
            string text = to_string(vertices[i]);
            drawText(x - (text.length() > 1 ? 7.0f : 5.0f), y + 5.0f, text);
        }

        drawInfoText();
        glutSwapBuffers();
    }

    void drawInfoText() {
        glColor3f(0.0f, 0.0f, 0.0f);

        drawText(10.0f, 20.0f, "=== Grah control ===");
        drawText(10.0f, 40.0f, "ЛКМ - creat/chois");
        drawText(10.0f, 60.0f, "ПКМ - peak");
        drawText(10.0f, 80.0f, "E - creeat");
        drawText(10.0f, 100.0f, "D - delete");
        drawText(10.0f, 120.0f, "W - change the edge weight");
        drawText(10.0f, 140.0f, "T - traveling salesman problem");
        drawText(10.0f, 160.0f, "P - print matrix");
        drawText(10.0f, 180.0f, "ESC - cancellation");

        if (showTSP) {
            stringstream ss;
            ss << "Оптимальный маршрут: ";
            for (int v : tspPath) ss << v << " ";
            ss << " (стоимость: " << tspCost << ")";
            drawText(10.0f, 220.0f, ss.str());
        }
        string modeText;
        if (weightInputMode) {
            modeText = "Режим: Ввод веса (" + to_string(inputWeight) + ") - Enter/ESC";
        }
        else if (edgeCreationMode) {
            modeText = "Режим: Создание ребра";
            if (edgeStartNode != -1) {
                modeText += " (начало: " + to_string(graph.getVertices()[static_cast<size_t>(edgeStartNode)]) + ")";
            }
        }
        else {
            modeText = "Режим: Выбор вершин";
            if (selectedNode != -1) {
                modeText += " (выбрана: " + to_string(graph.getVertices()[static_cast<size_t>(selectedNode)]) + ")";
            }
        }
        drawText(10.0f, 250.0f, modeText);
    }

    void handleMouseClick(int button, int state, int x, int y) {
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            if (weightInputMode) return;

            int clickedNode = findNodeAt(x, y);

            if (edgeCreationMode) {
                if (clickedNode != -1) {
                    if (edgeStartNode == -1) {
                        edgeStartNode = clickedNode;
                    }
                    else if (clickedNode != edgeStartNode) {
                        weightInputMode = true;
                        inputWeight = graph.GetWeight(
                            graph.getVertices()[static_cast<size_t>(edgeStartNode)],
                            graph.getVertices()[static_cast<size_t>(clickedNode)]);
                        if (inputWeight == INF) inputWeight = 1;
                    }
                }
                else if (edgeStartNode == -1) {
                    int newId = graph.AddVertexAtPosition(static_cast<float>(x), static_cast<float>(y));
                    if (newId != -1) {
                        vertexPositions.emplace_back(static_cast<float>(x), static_cast<float>(y));
                        arrangeVertices();
                    }
                }
            }
            else {
                if (clickedNode == -1) {
                    int newId = graph.AddVertexAtPosition(static_cast<float>(x), static_cast<float>(y));
                    if (newId != -1) {
                        vertexPositions.emplace_back(static_cast<float>(x), static_cast<float>(y));
                        arrangeVertices();
                    }
                }
                else {
                    selectedNode = clickedNode;
                }
            }
        }
        else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
            selectedNode = findNodeAt(x, y);
        }

        glutPostRedisplay();
    }

    void handleKeyboard(unsigned char key, int x, int y) {
        if (weightInputMode) {
            if (key >= '0' && key <= '9') {
                inputWeight = inputWeight * 10 + (key - '0');
                if (inputWeight > 999) inputWeight = 999;
            }
            else if (key == 8) { // Backspace
                inputWeight /= 10;
                if (inputWeight < 1) inputWeight = 1;
            }
            else if (key == 13) { // Enter
                if (edgeStartNode != -1 && selectedNode != -1 && edgeStartNode != selectedNode) {
                    graph.InsertEdge(
                        graph.getVertices()[static_cast<size_t>(edgeStartNode)],
                        graph.getVertices()[static_cast<size_t>(selectedNode)],
                        inputWeight);
                }
                weightInputMode = false;
                edgeStartNode = -1;
            }
            else if (key == 27) { // ESC
                weightInputMode = false;
                edgeStartNode = -1;
            }
            glutPostRedisplay();
            return;
        }
        switch (key) {
        case 'e': case 'E':
            edgeCreationMode = !edgeCreationMode;
            edgeStartNode = -1;
            break;
        case 'd': case 'D':
            if (selectedNode != -1) {
                graph.RemoveVertex(graph.getVertices()[static_cast<size_t>(selectedNode)]);
                vertexPositions.erase(vertexPositions.begin() + selectedNode);
                selectedNode = -1;
                arrangeVertices();
            }
            break;
        case 'w': case 'W':
            if (selectedNode != -1) {
                weightInputMode = true;
                inputWeight = 1;
            }
            break;
        case 't': case 'T': {
            auto result = graph.SolveTSP();
            tspPath = result.first;
            tspCost = result.second;
            showTSP = true;
            cout << "Оптимальный маршрут: ";
            for (int v : tspPath) cout << v << " ";
            cout << "\nСтоимость: " << tspCost << endl;
        }
                break;
        case 'p': case 'P':
            graph.Print();
            break;
        case 27: // ESC
            edgeCreationMode = false;
            weightInputMode = false;
            edgeStartNode = -1;
            showTSP = false;
            break;
        }

        glutPostRedisplay();
    }
};

GraphVisualizer visualizer;

void display() {
    visualizer.draw();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, h, 0);
    glMatrixMode(GL_MODELVIEW);
}

void mouse(int button, int state, int x, int y) {
    visualizer.handleMouseClick(button, state, x, y);
}

void keyboard(unsigned char key, int x, int y) {
    visualizer.handleKeyboard(key, x, y);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Задача коммивояжера - метод ветвей и границ");

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}