/* DSA Assignment 2. Build: gcc -std=c11 -Wall -Wextra -Wpedantic main.c -o hierarchy */
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NODE_COUNT 8
#define MAX_CHILDREN 3

typedef struct Node {
    const char *name;
    struct Node *children[MAX_CHILDREN];
    int child_count;
} Node;

typedef struct {
    int index;
    int comparisons;
} SearchResult;

/* The caller owns the node storage; pointers remain valid for its lifetime. */
static Node *build_hierarchy(Node nodes[NODE_COUNT])
{
    const char *names[NODE_COUNT] = {
        "CEO", "HR", "Finance", "IT", "Development", "Testing", "Frontend", "Backend"
    };
    int i;
    for (i = 0; i < NODE_COUNT; ++i) {
        nodes[i] = (Node){0};
        nodes[i].name = names[i];
    }
    nodes[0].children[0] = &nodes[1];
    nodes[0].children[1] = &nodes[2];
    nodes[0].children[2] = &nodes[3];
    nodes[0].child_count = 3;
    nodes[3].children[0] = &nodes[4];
    nodes[3].children[1] = &nodes[5];
    nodes[3].child_count = 2;
    nodes[4].children[0] = &nodes[6];
    nodes[4].children[1] = &nodes[7];
    nodes[4].child_count = 2;
    return &nodes[0];
}

/* Array-backed FIFO queue; also retain depth for display and validation.
   Capacity is sufficient for this fixed eight-node hierarchy. */
static int level_order(const Node *root, const Node *order[NODE_COUNT],
                       int depths[NODE_COUNT])
{
    int head = 0, tail = 0;
    if (root == NULL) return 0;
    order[tail] = root;
    depths[tail++] = 0;
    while (head < tail) {
        const Node *current = order[head];
        int depth = depths[head++];
        int i;
        for (i = 0; i < current->child_count; ++i) {
            if (tail == NODE_COUNT) {
                fputs("Hierarchy exceeds queue capacity.\n", stderr);
                exit(EXIT_FAILURE);
            }
            order[tail] = current->children[i];
            depths[tail++] = depth + 1;
        }
    }
    return tail;
}

static int tree_height(const Node *root)
{
    int height = 0, i;
    if (root == NULL) return -1;
    for (i = 0; i < root->child_count; ++i) {
        int candidate = 1 + tree_height(root->children[i]);
        if (candidate > height) height = candidate;
    }
    return height;
}

/* Case-insensitive comparison for the English department names. */
static int compare_names(const char *left, const char *right)
{
    for (;;) {
        int a = tolower((unsigned char)*left);
        int b = tolower((unsigned char)*right);
        if (a != b) return (a > b) - (a < b);
        if (*left == '\0') return 0;
        ++left;
        ++right;
    }
}

/* Insertion sort is simple and sufficient for seven names. */
static void sort_departments(const char *names[], int count)
{
    int i;
    for (i = 1; i < count; ++i) {
        const char *key = names[i];
        int j = i - 1;
        while (j >= 0 && compare_names(names[j], key) > 0) {
            names[j + 1] = names[j];
            --j;
        }
        names[j + 1] = key;
    }
}

static SearchResult linear_search(const char *names[], int count, const char *target)
{
    SearchResult result = {-1, 0};
    int i;
    for (i = 0; i < count; ++i) {
        ++result.comparisons;
        if (compare_names(names[i], target) == 0) {
            result.index = i;
            break;
        }
    }
    return result;
}

/* Precondition: names is sorted using compare_names. */
static SearchResult binary_search(const char *names[], int count, const char *target)
{
    SearchResult result = {-1, 0};
    int low = 0, high = count - 1;
    while (low <= high) {
        int middle = low + (high - low) / 2;
        int relation = compare_names(names[middle], target);
        ++result.comparisons; /* One logical name comparison per midpoint. */
        if (relation == 0) {
            result.index = middle;
            break;
        }
        if (relation < 0) low = middle + 1;
        else high = middle - 1;
    }
    return result;
}

static void run_tests(Node *root, const Node *order[], const int depths[],
                      int count, const char *departments[])
{
    const char *expected[] = {
        "CEO", "HR", "Finance", "IT", "Development", "Testing", "Frontend", "Backend"
    };
    const char *sorted[] = {"Backend", "Development", "Finance", "Frontend", "HR", "IT", "Testing"};
    const int expected_depths[] = {0, 1, 1, 1, 2, 2, 3, 3};
    const char *targets[] = {"Backend", "HR", "Testing", "Marketing"};
    const int indices[] = {0, 4, 6, -1};
    const int linear_counts[] = {1, 5, 7, 7};
    const char *single[] = {"HR"};
    int i;
    assert(count == NODE_COUNT);
    assert(root->child_count == 3);
    assert(root->children[2]->child_count == 2);
    assert(root->children[2]->children[0]->child_count == 2);
    assert(tree_height(root) == 3);
    assert(tree_height(root->children[0]) == 0);
    for (i = 0; i < count; ++i) {
        assert(strcmp(order[i]->name, expected[i]) == 0);
        assert(depths[i] == expected_depths[i]);
    }
    for (i = 0; i < count - 1; ++i) {
        assert(strcmp(departments[i], sorted[i]) == 0);
        assert(linear_search(departments, count - 1, sorted[i]).index == i);
        assert(binary_search(departments, count - 1, sorted[i]).index == i);
    }
    for (i = 0; i < 4; ++i) {
        SearchResult a = linear_search(departments, count - 1, targets[i]);
        SearchResult b = binary_search(departments, count - 1, targets[i]);
        assert(a.index == indices[i] && b.index == indices[i]);
        assert(a.comparisons == linear_counts[i] && b.comparisons == 3);
    }
    assert(linear_search(single, 1, "hr").index == 0);
    assert(binary_search(single, 1, "hr").index == 0);
    assert(binary_search(single, 1, "IT").index == -1);
    assert(linear_search(single, 1, "IT").index == -1);
    assert(linear_search(NULL, 0, "HR").comparisons == 0);
    assert(binary_search(NULL, 0, "HR").comparisons == 0);
    assert(linear_search(NULL, 0, "HR").index == -1);
    assert(binary_search(NULL, 0, "HR").index == -1);
    assert(binary_search(departments, count - 1, "AAA").index == -1);
    assert(binary_search(departments, count - 1, "ZZZ").index == -1);
    assert(binary_search(departments, count - 1, "CEO").index == -1);
    puts("All C self-tests passed.");
}

int main(int argc, char *argv[])
{
    Node nodes[NODE_COUNT];
    Node *root = build_hierarchy(nodes);
    const Node *order[NODE_COUNT];
    int depths[NODE_COUNT];
    const char *departments[NODE_COUNT - 1];
    const char *targets[] = {"Backend", "HR", "Testing", "Marketing"};
    int count = level_order(root, order, depths);
    int i;
    for (i = 1; i < count; ++i) departments[i - 1] = order[i]->name;
    sort_departments(departments, count - 1);
    if (argc == 2 && strcmp(argv[1], "--test") == 0) {
        run_tests(root, order, depths, count, departments);
        return EXIT_SUCCESS;
    }
    if (argc != 1) {
        fprintf(stderr, "Usage: %s [--test]\n", argv[0]);
        return EXIT_FAILURE;
    }
    puts("ORGANISATIONAL HIERARCHY - LEVEL-ORDER TRAVERSAL");
    for (i = 0; i < count; ++i) {
        if (i == 0 || depths[i] != depths[i - 1]) {
            if (i != 0) putchar('\n');
            printf("Level %d: %s", depths[i], order[i]->name);
        } else printf(" -> %s", order[i]->name);
    }
    puts("\n(Arrows within a level indicate visit order, not reporting links.)");
    printf("\nTree height: %d edges (%d levels)\n", tree_height(root), tree_height(root) + 1);
    printf("\nSorted departments: ");
    for (i = 0; i < count - 1; ++i) printf("%s%s", i ? ", " : "", departments[i]);
    puts("\nCEO is a role, so it is excluded from the department index.");
    puts("\nSEARCH COMPARISONS");
    printf("%-14s%-12s%8s%8s\n", "Target", "Result", "Linear", "Binary");
    for (i = 0; i < 4; ++i) {
        SearchResult a = linear_search(departments, count - 1, targets[i]);
        SearchResult b = binary_search(departments, count - 1, targets[i]);
        printf("%-14s%-12s%8d%8d\n", targets[i], a.index >= 0 ? "Found" : "Not found",
               a.comparisons, b.comparisons);
    }
    puts("\nOne comparison means comparing the target with one list entry.");
    return EXIT_SUCCESS;
}
