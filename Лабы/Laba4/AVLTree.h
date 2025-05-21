#include <iostream>
#include <algorithm>
#include <stdexcept>

// Template class for AVL Tree (self-balancing binary search tree)
template <typename T>
class AVLTree {
private:
    // Node structure for AVL Tree
    struct Node
    {
        T value;         // Data stored in node
        int height;      // Height of the node (longest path to leaf)
        Node* left;     // Pointer to left child
        Node* right;    // Pointer to right child

        // Node constructor
        Node(const T& val) : value(val), height(1), left(nullptr), right(nullptr) {}
    };

    Node* root; // Root node of the tree

    // Utility functions

    // Get height of a node (returns 0 for nullptr)
    int height(Node* node) const
    {
        return node ? node->height : 0;
    }

    // Calculate balance factor (difference between left and right subtree heights)
    int balanceFactor(Node* node) const
    {
        return node ? height(node->left) - height(node->right) : 0;
    }

    // Update node height based on
    // children's heights
    void updateHeight(Node* node)
    {
        if (node)
        {
            node->height = 1 + std::max(height(node->left), height(node->right));
        }
    }

    // Tree rotation operations

    // Right rotation around node y
    Node* rotateRight(Node* y)
    {
        if (!y || !y->left) throw std::runtime_error("Invalid right rotation");

        Node* x = y->left;
        y->left = x->right;
        x->right = y;

        updateHeight(y);
        updateHeight(x);

        return x; // New root after rotation
    }

    // Left rotation around node x
    Node* rotateLeft(Node* x)
    {
        if (!x || !x->right) throw std::runtime_error("Invalid left rotation");

        Node* y = x->right;
        x->right = y->left;
        y->left = x;

        updateHeight(x);
        updateHeight(y);

        return y; // New root after rotation
    }

    // Balance the tree at given node
    Node* balance(Node* node)
    {
        if (!node) return nullptr;

        updateHeight(node);
        int bf = balanceFactor(node);

        // Left-heavy case
        if (bf > 1)
        {
            // Left-Right case - needs double rotation
            if (balanceFactor(node->left) < 0)
            {
                node->left = rotateLeft(node->left);
            }
            return rotateRight(node);
        }

        // Right-heavy case
        if (bf < -1)
        {
            // Right-Left case - needs double rotation
            if (balanceFactor(node->right) > 0)
            {
                node->right = rotateRight(node->right);
            }
            return rotateLeft(node);
        }

        // Already balanced
        return node;
    }

    // Insert a value into the subtree rooted at 'node'
    Node* insert(Node* node, const T& value)
    {
        if (!node)
        {
            return new Node(value); // Create new node at insertion point
        }

        // Recursive insertion
        if (value < node->value)
        {
            node->left = insert(node->left, value);
        }
        else if (value > node->value)
        {
            node->right = insert(node->right, value);
        }
        else
        {
            // Duplicate values not allowed
            throw std::runtime_error("Duplicate value not allowed in AVL tree");
        }

        // Balance the tree after insertion
        return balance(node);
    }

    // Find node with minimum value in subtree
    Node* findMin(Node* node) const
    {
        if (!node) throw std::runtime_error("Cannot find minimum in empty subtree");

        while (node && node->left)
        {
            node = node->left;
        }
        return node;
    }

    // Remove a value from subtree
    Node* remove(Node* node, const T& value)
    {
        if (!node) throw std::runtime_error("Value not found in AVL tree");

        // Search for node to delete
        if (value < node->value)
        {
            node->left = remove(node->left, value);
        }
        else if (value > node->value)
        {
            node->right = remove(node->right, value);
        }
        else {
            // Node found - perform deletion

            // Case 1: Node has 0 or 1 child
            if (!node->left || !node->right)
            {
                Node* temp = node->left ? node->left : node->right;

                // No children
                if (!temp)
                {
                    temp = node;
                    node = nullptr;
                }
                else
                {
                    // One child - copy its data
                    *node = *temp;
                }
                delete temp;
            }
            else
            {
                // Case 2: Node has 2 children
                // Find smallest in right subtree (in-order successor)
                Node* minRight = findMin(node->right);
                node->value = minRight->value; // Copy successor's value
                node->right = remove(node->right, minRight->value); // Remove duplicate
            }
        }

        if (!node) return nullptr; // Tree became empty
        return balance(node); // Balance after deletion
    }

    // Check if value exists in subtree
    bool contains(Node* node, const T& value) const
    {
        if (!node) return false;
        if (value < node->value) return contains(node->left, value);
        if (value > node->value) return contains(node->right, value);
        return true; // Value found
    }

    // Tree traversal methods

    // In-order traversal (left-root-right) - produces sorted output
    void inOrder(Node* node) const
    {
        if (!node) return;
        inOrder(node->left);
        std::cout << node->value << " ";
        inOrder(node->right);
    }

    // Pre-order traversal (root-left-right)
    void preOrder(Node* node) const
    {
        if (!node) return;
        std::cout << node->value << " ";
        preOrder(node->left);
        preOrder(node->right);
    }

    // Post-order traversal (left-right-root)
    void postOrder(Node* node) const
    {
        if (!node) return;
        postOrder(node->left);
        postOrder(node->right);
        std::cout << node->value << " ";
    }

    // Recursively delete all nodes
    void clear(Node* node)
    {
        if (node) {
            clear(node->left);
            clear(node->right);
            delete node;
        }
    }

public:
    // Constructor - creates empty tree
    AVLTree() : root(nullptr) {}

    // Destructor - frees all memory
    ~AVLTree()
    {
        clear(root);
    }

    // Disable copying (for simplicity)
    AVLTree(const AVLTree&) = delete;
    AVLTree& operator=(const AVLTree&) = delete;

    // Public interface

    // Insert value into tree
    void insert(const T& value)
    {
        root = insert(root, value);
    }

    // Remove value from tree
    void remove(const T& value)
    {
        root = remove(root, value);
    }

    // Check if value exists in tree
    bool contains(const T& value) const
    {
        return contains(root, value);
    }

    // Check if tree is empty
    bool isEmpty() const
    {
        return !root;
    }

    // Public traversal methods

    void inOrder() const
    {
        if (isEmpty()) throw std::runtime_error("Tree is empty");
        inOrder(root);
        std::cout << std::endl;
    }

    void preOrder() const
    {
        if (isEmpty()) throw std::runtime_error("Tree is empty");
        preOrder(root);
        std::cout << std::endl;
    }

    void postOrder() const
    {
        if (isEmpty()) throw std::runtime_error("Tree is empty");
        postOrder(root);
        std::cout << std::endl;
    }

    // Print tree structure (for debugging)
    void printTree(Node* node, int indent = 0) const
    {
        if (!node) return;
        // Print right subtree first (appears at top)
        printTree(node->right, indent + 4);
        // Indent according to node depth
        std::cout << std::string(indent, ' ') << node->value << " (h:" << node->height << ")" << std::endl;
        // Print left subtree
        printTree(node->left, indent + 4);
    }

    // Public method to print tree
    void printTree() const
    {
        if (isEmpty()) throw std::runtime_error("Tree is empty");
        printTree(root);
    }
};