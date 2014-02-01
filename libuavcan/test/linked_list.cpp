/*
 * Copyright (C) 2014 Pavel Kirienko <pavel.kirienko@gmail.com>
 */

#include <gtest/gtest.h>
#include <uavcan/internal/linked_list.hpp>

struct ListItem : uavcan::LinkedListNode<ListItem>
{
    int value;

    ListItem(int value = 0)
    : value(value)
    { }

    struct GreaterThanComparator
    {
        const int compare_with;

        GreaterThanComparator(int compare_with)
        : compare_with(compare_with)
        { }

        bool operator()(const ListItem* item) const
        {
            return item->value > compare_with;
        }
    };

    void insort(uavcan::LinkedListRoot<ListItem>& root)
    {
        root.insertBefore(this, GreaterThanComparator(value));
    }
};

TEST(LinkedList, Basic)
{
    uavcan::LinkedListRoot<ListItem> root;

    /*
     * Insert/remove
     */
    EXPECT_EQ(0, root.length());

    ListItem item1;
    root.insert(&item1);
    root.insert(&item1);         // Insert twice - second will be ignored
    EXPECT_EQ(1, root.length());

    EXPECT_TRUE(root.remove(&item1));
    EXPECT_FALSE(root.remove(&item1));
    EXPECT_EQ(0, root.length());

    ListItem items[3];
    root.insert(&item1);
    root.insert(items + 0);
    root.insert(items + 1);
    root.insert(items + 2);
    EXPECT_EQ(4, root.length());

    /*
     * Order persistence
     */
    items[0].value = 10;
    items[1].value = 11;
    items[2].value = 12;
    const int expected_values[] = {12, 11, 10, 0};
    ListItem* node = root.get();
    for (int i = 0; i < 4; i++)
    {
        EXPECT_EQ(expected_values[i], node->value);
        node = node->getNextListNode();
    }

    EXPECT_TRUE(root.remove(items + 0));
    EXPECT_TRUE(root.remove(items + 2));
    EXPECT_FALSE(root.remove(items + 2));
    EXPECT_EQ(2, root.length());

    const int expected_values2[] = {11, 0};
    node = root.get();
    for (int i = 0; i < 2; i++)
    {
        EXPECT_EQ(expected_values2[i], node->value);
        node = node->getNextListNode();
    }

    root.insert(items + 2);
    EXPECT_EQ(3, root.length());
    EXPECT_EQ(12, root.get()->value);

    /*
     * Emptying
     */
    EXPECT_TRUE(root.remove(&item1));
    EXPECT_FALSE(root.remove(items + 0));
    EXPECT_TRUE(root.remove(items + 1));
    EXPECT_TRUE(root.remove(items + 2));
    EXPECT_EQ(0, root.length());
}

TEST(LinkedList, Sorting)
{
    uavcan::LinkedListRoot<ListItem> root;
    ListItem items[] = {0, 1, 2, 3, 4, 5};

    items[2].insort(root);
    items[3].insort(root);
    items[0].insort(root);
    items[4].insort(root);
    items[1].insort(root);
    items[5].insort(root);

    EXPECT_EQ(6, root.length());

    int prev_val = -100500;
    const ListItem* item = root.get();
    while (item)
    {
        //std::cout << item->value << std::endl;
        EXPECT_LT(prev_val, item->value);
        prev_val = item->value;
        item = item->getNextListNode();
    }
}
