#include <GL/glut.h>
#include <vector>
#include <algorithm>
#include <string>

const int WINDOW_WIDTH = 1100;
const int WINDOW_HEIGHT = 800;
const int UI_HEIGHT = 150;
const int BUTTON_HEIGHT = 40;
const int BUTTON_START_Y = 110;
const int TRAVERSAL_Y = 600;

class BinaryTree {
private:
    struct TreeNode {
        int data;
        TreeNode* left;
        TreeNode* right;
        int x, y;
        bool isMin;

        TreeNode(int val) : data(val), left(nullptr), right(nullptr),
            x(0), y(0), isMin(false) {}
    };

    TreeNode* root;
    std::vector<int> traversalResult;
    int currentStep;
    bool isTraversing;
    int treeX, treeY;

    void deleteTree(TreeNode* node) {
        if (node) {
            deleteTree(node->left);
            deleteTree(node->right);
            delete node;
        }
    }

    void setPositions(TreeNode* node, int x, int y, int level) {
        if (!node) return;
        node->x = x;
        node->y = y;
        int offset = 300 / (level + 1);
        setPositions(node->left, x - offset, y + 80, level + 1);
        setPositions(node->right, x + offset, y + 80, level + 1);
    }

    void preOrder(TreeNode* node) {
        if (!node) return;
        traversalResult.push_back(node->data);
        preOrder(node->left);
        preOrder(node->right);
    }

    void inOrder(TreeNode* node) {
        if (!node) return;
        inOrder(node->left);
        traversalResult.push_back(node->data);
        inOrder(node->right);
    }

    void postOrder(TreeNode* node) {
        if (!node) return;
        postOrder(node->left);
        postOrder(node->right);
        traversalResult.push_back(node->data);
    }

    void collectElements(TreeNode* node, std::vector<int>& elements) {
        if (!node) return;
        collectElements(node->left, elements);
        elements.push_back(node->data);
        collectElements(node->right, elements);
    }

    TreeNode* buildBalanced(std::vector<int>& elements, int start, int end) {
        if (start > end) return nullptr;
        int mid = (start + end) / 2;
        TreeNode* node = new TreeNode(elements[mid]);
        node->left = buildBalanced(elements, start, mid - 1);
        node->right = buildBalanced(elements, mid + 1, end);
        return node;
    }

    TreeNode* findMinNode(TreeNode* node) {
        while (node && node->left) node = node->left;
        return node;
    }

    TreeNode* removeNode(TreeNode* node, int value) {
        if (!node) return nullptr;

        if (value < node->data) {
            node->left = removeNode(node->left, value);
        }
        else if (value > node->data) {
            node->right = removeNode(node->right, value);
        }
        else {
            if (!node->left) {
                TreeNode* temp = node->right;
                delete node;
                return temp;
            }
            else if (!node->right) {
                TreeNode* temp = node->left;
                delete node;
                return temp;
            }

            TreeNode* temp = findMinNode(node->right);
            node->data = temp->data;
            node->right = removeNode(node->right, temp->data);
        }
        return node;
    }

public:
    BinaryTree() : root(nullptr), currentStep(0), isTraversing(false),
        treeX(400), treeY(150) {}

    ~BinaryTree() {
        deleteTree(root);
    }

    void insert(int value) {
        if (!root) {
            root = new TreeNode(value);
            return;
        }

        TreeNode* current = root;
        while (true) {
            if (value < current->data) {
                if (!current->left) {
                    current->left = new TreeNode(value);
                    break;
                }
                current = current->left;
            }
            else {
                if (!current->right) {
                    current->right = new TreeNode(value);
                    break;
                }
                current = current->right;
            }
        }
    }

    void remove(int value) {
        root = removeNode(root, value);
    }

    void drawNode(TreeNode* node) {
        if (!node) return;

        // Рисуем связи
        if (node->left) {
            glColor3f(1, 1, 1);
            glBegin(GL_LINES);
            glVertex2i(node->x, node->y);
            glVertex2i(node->left->x, node->left->y);
            glEnd();
        }
        if (node->right) {
            glColor3f(1, 1, 1);
            glBegin(GL_LINES);
            glVertex2i(node->x, node->y);
            glVertex2i(node->right->x, node->right->y);
            glEnd();
        }

        // Рисуем узел
        if (node->isMin) glColor3f(0.0f, 1.0f, 0.0f); // Зеленый для минимального
        else glColor3f(0.4f, 0.7f, 1.0f); // Синий для обычных

        glBegin(GL_POLYGON);
        for (int i = 0; i < 360; i++) {
            float angle = i * 3.14159f / 180;
            glVertex2f(node->x + 20 * cos(angle), node->y + 20 * sin(angle));
        }
        glEnd();
        // Текст
        glColor3f(0, 0, 0);
        glRasterPos2i(node->x - (node->data < 10 ? 5 : 10), node->y - 5);
        std::string num = std::to_string(node->data);
        for (char c : num) glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
    }

    void drawTree() {
        if (root) {
            setPositions(root, treeX, treeY, 1);
            drawTreeNodes(root);
        }
    }

    void drawTreeNodes(TreeNode* node) {
        if (!node) return;
        drawNode(node);
        drawTreeNodes(node->left);
        drawTreeNodes(node->right);
    }

    void startTraversal(const std::string& type) {
        traversalResult.clear();
        currentStep = 0;

        if (type == "PreOrder") preOrder(root);
        else if (type == "InOrder") inOrder(root);
        else if (type == "PostOrder") postOrder(root);

        isTraversing = true;
    }

    void drawTraversalResult() {
        if (!isTraversing || traversalResult.empty()) return;

        glColor3f(1, 1, 1);
        glRasterPos2i(20, TRAVERSAL_Y);
        std::string label = "Результат: ";
        for (char c : label) glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);

        // Выводим уже пройденные элементы
        glColor3f(0, 1, 0); // Зеленый для пройденных
        int x = 120;
        for (int i = 0; i < currentStep && i < traversalResult.size(); i++) {
            glRasterPos2i(x, TRAVERSAL_Y);
            std::string num = std::to_string(traversalResult[i]) + " ";
            for (char c : num) glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
            x += 30;
        }

        // Выводим текущий элемент (если есть)
        if (currentStep < traversalResult.size()) {
            glColor3f(1, 0, 0); // Красный для текущего
            glRasterPos2i(x, TRAVERSAL_Y);
            std::string num = std::to_string(traversalResult[currentStep]);
            for (char c : num) glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
            currentStep++;
        }
        else {
            isTraversing = false;
        }
    }

    void findMin() {
        clearMinFlag(root);
        if (!root) return;

        TreeNode* current = root;
        while (current->left) current = current->left;

        current->isMin = true;
    }

    void balance() {
        std::vector<int> elements;
        collectElements(root, elements);
        deleteTree(root);
        root = buildBalanced(elements, 0, elements.size() - 1);
    }

    void clearMinFlag(TreeNode* node) {
        if (!node) return;
        node->isMin = false;
        clearMinFlag(node->left);
        clearMinFlag(node->right);
    }

    void setPosition(int x, int y) {
        treeX = x;
        treeY = y;
    }
};

BinaryTree tree;
std::string message = "Введите число и нажмите Enter";
std::string traversalType = "";
std::string inputStr = "";

void drawButton(int x, int y, int w, int h, const std::string& text) {
    glColor3f(0.7f, 0.7f, 0.7f);
    glBegin(GL_QUADS);
    glVertex2i(x, y);
    glVertex2i(x + w, y);
    glVertex2i(x + w, y + h);
    glVertex2i(x, y + h);
    glEnd();

    glColor3f(0, 0, 0);
    glRasterPos2i(x + 10, y + h / 2 + 5);
    for (char c : text) glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
}

void drawInterface() {
    // Фон интерфейса
    glColor3f(0.1f, 0.1f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2i(0, 0);
    glVertex2i(WINDOW_WIDTH, 0);
    glVertex2i(WINDOW_WIDTH, UI_HEIGHT);
    glVertex2i(0, UI_HEIGHT);
    glEnd();

    // Текст
    glColor3f(1, 1, 1);
    glRasterPos2i(20, 20);
    for (char c : message) glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);

    glRasterPos2i(20, 50);
    std::string inputText = "Ввод: " + inputStr;
    for (char c : inputText) glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);

    glRasterPos2i(20, 80);
    for (char c : traversalType) glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
    // Кнопки
    drawButton(20, BUTTON_START_Y, 100, BUTTON_HEIGHT, "Add (A)");
    drawButton(140, BUTTON_START_Y, 120, BUTTON_HEIGHT, "PreOrder (P)");
    drawButton(280, BUTTON_START_Y, 120, BUTTON_HEIGHT, "InOrder (I)");
    drawButton(420, BUTTON_START_Y, 120, BUTTON_HEIGHT, "PostOrder (O)");
    drawButton(560, BUTTON_START_Y, 120, BUTTON_HEIGHT, "Find Min (M)");
    drawButton(700, BUTTON_START_Y, 120, BUTTON_HEIGHT, "Balance (B)");
    drawButton(840, BUTTON_START_Y, 120, BUTTON_HEIGHT, "Remove (D)");
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawInterface();

    // Рисуем дерево
    glPushMatrix();
    glTranslatef(0, UI_HEIGHT + 10, 0);
    tree.drawTree();
    glPopMatrix();

    // Рисуем результат обхода
    tree.drawTraversalResult();

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    switch (tolower(key)) {
    case 13: // Enter
    case 'a':
        if (!inputStr.empty()) {
            try {
                int value = std::stoi(inputStr);
                tree.insert(value);
                message = "Добавлено: " + inputStr;
                inputStr = "";
            }
            catch (...) {
                message = "Ошибка ввода числа";
            }
        }
        break;
    case 'd':
        if (!inputStr.empty()) {
            try {
                int value = std::stoi(inputStr);
                tree.remove(value);
                message = "Удалено: " + inputStr;
                inputStr = "";
            }
            catch (...) {
                message = "Ошибка ввода числа";
            }
        }
        break;
    case 'p':
        tree.startTraversal("PreOrder");
        traversalType = "PreOrder Traversal";
        break;
    case 'i':
        tree.startTraversal("InOrder");
        traversalType = "InOrder Traversal";
        break;
    case 'o':
        tree.startTraversal("PostOrder");
        traversalType = "PostOrder Traversal";
        break;
    case 'm':
        tree.findMin();
        message = "Найден минимальный элемент";
        break;
    case 'b':
        tree.balance();
        message = "Дерево сбалансировано";
        break;
    case 8: // Backspace
        if (!inputStr.empty()) inputStr.pop_back();
        break;
    default:
        if (isdigit(key)) inputStr += key;
    }
    glutPostRedisplay();
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, height, 0);
    glMatrixMode(GL_MODELVIEW);
    tree.setPosition(width / 2, 150);
}

void update(int value) {
    glutPostRedisplay();
    glutTimerFunc(500, update, 0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Binary Tree Visualizer");

    glClearColor(0.2f, 0.2f, 0.3f, 1.0f);

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutReshapeFunc(reshape);
    glutTimerFunc(500, update, 0);

    glutMainLoop();
    return 0;
}