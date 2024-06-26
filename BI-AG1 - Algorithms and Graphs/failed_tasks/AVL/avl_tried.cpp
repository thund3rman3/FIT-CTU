//
// Created by galem on 17.11.2023.
//

#include <iostream>
#include <stdexcept>
#include <string>

class TextEditorBackend {
public:
    struct Node {
        char data;
        int height;
        Node* left;
        Node* right;

        explicit Node(char ch) : data(ch), height(1), left(nullptr), right(nullptr) {}
    };

    Node* root;
    size_t totalSize;
    size_t totalLines;

    // ... (AVL tree implementation methods)

    int getHeight(Node* node) {
        if (node == nullptr) return 0;
        return node->height;
    }

    int getBalanceFactor(Node* node) {
        if (node == nullptr) return 0;
        return getHeight(node->left) - getHeight(node->right);
    }

    Node* rotateRight(Node* y) {
        Node* x = y->left;
        Node* T2 = x->right;

        x->right = y;
        y->left = T2;

        y->height = 1 + std::max(getHeight(y->left), getHeight(y->right));
        x->height = 1 + std::max(getHeight(x->left), getHeight(x->right));

        return x;
    }

    Node* rotateLeft(Node* x) {
        Node* y = x->right;
        Node* T2 = y->left;

        y->left = x;
        x->right = T2;

        x->height = 1 + std::max(getHeight(x->left), getHeight(x->right));
        y->height = 1 + std::max(getHeight(y->left), getHeight(y->right));

        return y;
    }

    Node* insert(Node* node, char ch, size_t& index) {
        if (node == nullptr) {
            totalSize++;
            if (ch == '\n') totalLines++;
            return new Node(ch);
        }

        if (index < size_t(getCharCount(node->left))) {
            node->left = insert(node->left, ch, index);
        } else if (index == size_t(getCharCount(node->left))) {
            totalSize++;
            if (ch == '\n') totalLines++;
            else index++;
            node->right = insert(node->right, ch, index);
        } else {
            totalSize++;
            if (ch == '\n') totalLines++;
            index -= size_t(getCharCount(node->left)) + 1;
            node->right = insert(node->right, ch, index);
        }

        node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));

        int balance = getBalanceFactor(node);

        // Left Left
        if (balance > 1 && index < size_t(getCharCount(node->left)))
            return rotateRight(node);

        // Right Right
        if (balance < -1 && index >= size_t(getCharCount(node->left)))
            return rotateLeft(node);

        // Left Right
        if (balance > 1 && index >= size_t(getCharCount(node->left))) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }

        // Right Left
        if (balance < -1 && index < size_t(getCharCount(node->left))) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    size_t getCharCount(Node* node) const {
        if (node == nullptr) return 0;
        return 1 + getCharCount(node->left) + getCharCount(node->right);
    }

    // ... (other AVL tree helper methods)

    char at(Node* node, size_t& i) const {
        size_t leftCount = size_t(getCharCount(node->left));

        if (i < leftCount) {
            return at(node->left, i);
        } else if (i == leftCount) {
            return node->data;
        } else {
            i -= leftCount + 1;
            return at(node->right, i);
        }
    }

    void edit(Node* node, size_t& i, char c) {
        size_t leftCount = size_t(getCharCount(node->left));

        if (i < leftCount) {
            edit(node->left, i, c);
        } else if (i == leftCount) {
            node->data = c;
        } else {
            i -= leftCount + 1;
            edit(node->right, i, c);
        }
    }

    size_t char_to_line(Node* node, size_t i) const {
        size_t leftCount = size_t(getCharCount(node->left));

        if (i < leftCount) {
            return char_to_line(node->left, i);
        } else if (i == leftCount) {
            return totalLines - getLineCount(node->right);
        } else {
            i -= leftCount + 1;
            return char_to_line(node->right, i);
        }
    }

    size_t getLineCount(Node* node) const {
        if (node == nullptr) return 0;
        return 1 + getLineCount(node->left) + getLineCount(node->right);
    }

    Node* findMin(Node* node) {
        while (node->left != nullptr)
            node = node->left;
        return node;
    }

    void erase(Node*& node, size_t i) {
        size_t leftCount = size_t(getCharCount(node->left));

        if (i < leftCount) {
            erase(node->left, i);
        } else if (i == leftCount) {
            totalSize--;
            if (node->data == '\n') totalLines--;

            if (node->left == nullptr) {
                Node* temp = node->right;
                delete node;
                node = temp;
            } else if (node->right == nullptr) {
                Node* temp = node->left;
                delete node;
                node = temp;
            } else {
                Node* temp = findMin(node->right);
                node->data = temp->data;
                erase(node->right, 0);
            }
        } else {
            i -= leftCount + 1;
            erase(node->right, i);
        }

        if (node != nullptr) {
            node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));

            int balance = getBalanceFactor(node);

            // Left Left
            if (balance > 1 && getBalanceFactor(node->left) >= 0)
                node = rotateRight(node);

            // Left Right
            if (balance > 1 && getBalanceFactor(node->left) < 0) {
                node->left = rotateLeft(node->left);
                node = rotateRight(node);
            }

            // Right Right
            if (balance < -1 && getBalanceFactor(node->right) <= 0)
                node = rotateLeft(node);

            // Right Left
            if (balance < -1 && getBalanceFactor(node->right) > 0) {
                node->right = rotateRight(node->right);
                node = rotateLeft(node);
            }
        }
    }


    size_t line_start(Node* node, size_t r) const {
        size_t leftLineCount = size_t(getLineCount(node->left));

        if (r < leftLineCount) {
            return line_start(node->left, r);
        } else if (r == leftLineCount) {
            return totalSize - getCharCount(node->right);
        } else {
            r -= leftLineCount + 1;
            return line_start(node->right, r);
        }
    }

    size_t line_length(Node* node, size_t r) const {
        size_t leftLineCount = size_t(getLineCount(node->left));

        if (r < leftLineCount) {
            return line_length(node->left, r);
        } else if (r == leftLineCount) {
            return getCharCount(node);
        } else {
            r -= leftLineCount + 1;
            return line_length(node->right, r);
        }
    }

    explicit TextEditorBackend(const std::string& text) : root(nullptr), totalSize(0), totalLines(1) {
        size_t index = 0;
        for (char ch : text) {
            insert( index, ch);
        }
    }

    size_t size() const {
        return totalSize;
    }

    size_t lines() const {
        return totalLines;
    }

    char at(size_t i) const {
        if (i >= totalSize) {
            throw std::out_of_range("Index out of range");
        }

        return at(root, i);
    }

    void edit(size_t i, char c) {
        if (i >= totalSize) {
            throw std::out_of_range("Index out of range");
        }

        edit(root, i, c);
    }

    void insert(size_t i, char c) {
        if (i > totalSize) {
            throw std::out_of_range("Index out of range");
        }

        insert(root, c, i);
    }

    void erase(size_t i) {
        if (i >= totalSize) {
            throw std::out_of_range("Index out of range");
        }

        erase(root, i);
    }

    size_t line_start(size_t r) const {
        if (r >= totalLines) {
            throw std::out_of_range("Index out of range");
        }

        return line_start(root, r);
    }

    size_t line_length(size_t r) const {
        if (r >= totalLines) {
            throw std::out_of_range("Index out of range");
        }

        return line_length(root, r);
    }

    size_t char_to_line(size_t i) const {
        if (i >= totalSize) {
            throw std::out_of_range("Index out of range");
        }
        return char_to_line(root, i);
    }



};

int main() {
    // Example usage
    TextEditorBackend editor("Hello, World!\nThis is a text editor.");

    std::cout << "Size: " << editor.size() << std::endl;
    std::cout << "Lines: " << editor.lines() << std::endl;
    std::cout << "Character at index 7: " << editor.at(7) << std::endl;
    std::cout << "Line start of line 1: " << editor.line_start(1) << std::endl;
    std::cout << "Line length of line 1: " << editor.line_length(1) << std::endl;
    std::cout << "Character to line of index 16: " << editor.char_to_line(16)<<std::endl;
}