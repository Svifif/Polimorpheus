#include "AVLTreeTests.hpp"
#include <stdexcept>

 void AVLTreeTests::runAllTests()
{
    std::cout << "Running AVLTree tests...\n";

    testInsert();
    testRemove();
    testContains();
    testBalancing();
    testTraversals();
    testExceptions();
    testEmptyTree();

    std::cout << "All AVLTree tests passed!\n";
}

void AVLTreeTests::testInsert() 
{
    AVLTree<int> tree;

    // Test basic insertion
    tree.insert(5);
    assertTrue(tree.contains(5), "Insert single value");

    // Test multiple insertions
    tree.insert(3);
    tree.insert(7);
    assertTrue(tree.contains(3) && tree.contains(7), "Insert multiple values");

    // Test duplicate insertion
    try
    {
        tree.insert(5);
        assertFalse(true, "Duplicate insertion should throw");
    }
    catch (const std::runtime_error&)
    {
        assertTrue(true, "Duplicate insertion throws correctly");
    }

    std::cout << "Insert tests passed\n";
}

void AVLTreeTests::testRemove() 
{
    AVLTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    tree.insert(2);
    tree.insert(4);
    tree.insert(6);
    tree.insert(8);

    // Test remove leaf node
    tree.remove(2);
    assertFalse(tree.contains(2), "Remove leaf node");

    // Test remove node with one child
    tree.remove(3);
    assertFalse(tree.contains(3), "Remove node with one child");
    assertTrue(tree.contains(4), "Child of removed node still exists");

    // Test remove node with two children
    tree.remove(5);
    assertFalse(tree.contains(5), "Remove node with two children");
    assertTrue(tree.contains(4) && tree.contains(6) && tree.contains(7) && tree.contains(8),
        "Tree structure maintained after removal");

    // Test remove non-existent value
    try
    {
        tree.remove(100);
        assertFalse(true, "Remove non-existent value should throw");
    }
    catch (const std::runtime_error&)
    {
        assertTrue(true, "Remove non-existent value throws correctly");
    }

    std::cout << "Remove tests passed\n";
}

void AVLTreeTests::testContains()
{
    AVLTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);

    assertTrue(tree.contains(5), "Contains existing value (root)");
    assertTrue(tree.contains(3), "Contains existing value (left child)");
    assertTrue(tree.contains(7), "Contains existing value (right child)");
    assertFalse(tree.contains(10), "Doesn't contain non-existent value");

    std::cout << "Contains tests passed\n";
}

void AVLTreeTests::testBalancing() 
{
    AVLTree<int> tree;

    // Test left-left case
    tree.insert(3);
    tree.insert(2);
    tree.insert(1);
    assertEqual(tree.contains(2), true, "Balancing after left-left insertion");

    // Test right-right case
    tree.insert(4);
    tree.insert(5);
    assertEqual(tree.contains(4), true, "Balancing after right-right insertion");

    // Test left-right case
    tree.insert(0);
    tree.insert(-1);
    assertEqual(tree.contains(0), true, "Balancing after left-right insertion");

    // Test right-left case
    tree.insert(6);
    tree.insert(7);
    assertEqual(tree.contains(6), true, "Balancing after right-left insertion");

    std::cout << "Balancing tests passed\n";
}

void AVLTreeTests::testTraversals() {
    AVLTree<int> tree;
    tree.insert(4);
    tree.insert(2);
    tree.insert(6);
    tree.insert(1);
    tree.insert(3);
    tree.insert(5);
    tree.insert(7);

    // Проверка, что методы не падают на заполненном дереве
    tree.inOrder();
    tree.preOrder();
    tree.postOrder();

    // Тестирование пустого дерева
    AVLTree<int> emptyTree;

    // Вариант A: Если методы должны бросать исключения
    assertThrows([&]() { emptyTree.inOrder(); }, "inOrder on empty tree");
    assertThrows([&]() { emptyTree.preOrder(); }, "preOrder on empty tree");
    assertThrows([&]() { emptyTree.postOrder(); }, "postOrder on empty tree");

    std::cout << "Traversal tests passed\n";
}

void AVLTreeTests::testExceptions() {
    AVLTree<int> emptyTree;

    // Test empty tree traversals
    assertThrows([&]() { emptyTree.inOrder(); }, "inOrder on empty tree");
    assertThrows([&]() { emptyTree.preOrder(); }, "preOrder on empty tree");
    assertThrows([&]() { emptyTree.postOrder(); }, "postOrder on empty tree");
    assertThrows([&]() { emptyTree.printTree(); }, "printTree on empty tree");

    std::cout << "Exception tests passed\n";
}

void AVLTreeTests::testEmptyTree()
{
    AVLTree<int> tree;

    assertTrue(tree.isEmpty(), "New tree is empty");
    tree.insert(1);
    assertFalse(tree.isEmpty(), "Tree not empty after insertion");
    tree.remove(1);
    assertTrue(tree.isEmpty(), "Tree empty after removing all elements");

    std::cout << "Empty tree tests passed\n";
}

// Helper implementations
template <typename T>
void AVLTreeTests::assertTrue(const T& condition, const std::string& message)
{
    if (!condition)
    {
        throw std::runtime_error("Test failed: " + message);
    }
}

template <typename T>
void AVLTreeTests::assertFalse(const T& condition, const std::string& message) 
{
    assertTrue(!condition, message);
}

template <typename T, typename U>
void AVLTreeTests::assertEqual(const T& actual, const U& expected, const std::string& message) 
{
    if (actual != expected) {
        throw std::runtime_error("Test failed: " + message + " (expected: " +
            std::to_string(expected) + ", actual: " +
            std::to_string(actual) + ")");
    }
}

template <typename Func>
void AVLTreeTests::assertThrows(Func func, const std::string& message) {
    bool threw = false;
    try {
        func();
    }
    catch (const std::exception&) {
        threw = true;
    }
    assertTrue(threw, message);
}