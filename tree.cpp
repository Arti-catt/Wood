#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include "colors.h"

typedef int elem_t;

#define TREE_DEBUG

#define FOLDER_NAME     "DumpPictures"
#define DUMP_FILE       "TreeDump.html"
#define DOT_FILE_NAME   "Tree.dot"

#ifdef TREE_DEBUG
    #define DEBUG(...) __VA_ARGS__
#else
    #define DEBUG(...)
#endif

FILE *dump_file = fopen(DUMP_FILE, "wa");

#define LOCAL_PARS , const char *function, const char *file, int line
#define LOCAL_ARGS , __FUNCTION__, __FILE__, __LINE__

#define GONDON(condition)                                                               \
    if (!(condition)) {                                                                 \
        fprintf(stderr, "Function: %s, file: %s:%d\nGONDON %sHELP%s YOU\n",             \
            __FUNCTION__, __FILE__, __LINE__, RED, RES);                                \
        abort();                                                                        \
    }

#define LEN_COMMAND 100

enum Parametres {
    BEFORE = 0,
    AFTER  = 1,
    BASE   = 2,
};

struct Tree_node_t {
    elem_t       value;
    Tree_node_t *left;
    Tree_node_t *right;
    Tree_node_t *parent;
    ssize_t      root_dist;
};

struct Forest {
    Tree_node_t *root;

    size_t count_pict;
    char command[LEN_COMMAND];
    char picture_name[LEN_COMMAND];
    int err;
};

void TreeInit(Forest *tree, elem_t value);
Tree_node_t *NewTNT();
void LinkRight(Forest *tree, Tree_node_t *parent, elem_t value);
void LinkLeft(Forest *tree, Tree_node_t *parent, elem_t value);
void Link(Tree_node_t *parent, Tree_node_t *node, elem_t value);
void DeleteTNT(Forest *tree, Tree_node_t *node);
void TreeDestroy(Forest *tree);

void TreePrint(Forest *tree, const Tree_node_t *node);
void TreeRecPrint(Forest *tree, const Tree_node_t *node);

void DumpTNT();     // TODO:
void TreeVerify();  // TODO:

void CommandsCreator(Forest *tree);

void Chitalochka(Forest *tree, Tree_node_t *parent, Tree_node_t *node, FILE *read_file);

void Dump(Forest *tree, Tree_node_t *node, int mode LOCAL_PARS);
void DumpParametres(int mode);

void DumpPicture(Forest *tree, Tree_node_t *node);
void WriteGeneralPart(FILE *graph_file);
void NodesCreator(Forest *tree, Tree_node_t *node, FILE *graph_file, size_t *index);
void CreateLeftNode(Forest *tree, Tree_node_t *node, FILE *graph_file, size_t *index);
void CreateRightNode(Forest *tree, Tree_node_t *node, FILE *graph_file, size_t *index);

void AddBackgroundPicture(FILE *file_html);

void CommandsPreparation(Forest *tree);
void ClearNameEnd(char *graph_name, int symbol);

int main()
{
    Forest tree = {};
    TreeInit(&tree, 50);
    // LinkRight(&tree, tree.root, 10);
    // LinkLeft(&tree, tree.root, 20);
    // LinkRight(&tree, tree.root->left, 10);
    // LinkRight(&tree, tree.root->right, 10);
    // LinkLeft(&tree, tree.root->left, 20);
    // LinkLeft(&tree, tree.root->right, 20);
    FILE *read_file = fopen("in.txt", "r");
    GONDON(read_file)

    Chitalochka(&tree, NULL, tree.root, read_file);
    Dump(&tree, tree.root, BASE LOCAL_ARGS);

    TreePrint(&tree, tree.root);
    TreeDestroy(&tree);
    return 0;
}

void TreeInit(Forest *tree, elem_t value)
{
    GONDON(tree)

    tree->root = NewTNT();
    tree->root->value = value;

    CommandsCreator(tree);
}

Tree_node_t *NewTNT()
{
    Tree_node_t *node = (Tree_node_t *) calloc(1, sizeof(Tree_node_t));
    GONDON(node)

    node->value  = 0;
    node->left   = NULL;
    node->right  = NULL;
    node->parent = NULL;
    node->root_dist = 0;

    return node;
}

void LinkLeft(Forest *tree, Tree_node_t *parent, elem_t value)
{
    GONDON(parent)

    Dump(tree, tree->root, BEFORE LOCAL_ARGS);

    Tree_node_t *node = NewTNT();
    parent->left = node;
    Link(parent, node, value);

    Dump(tree, tree->root, AFTER LOCAL_ARGS);
}

void LinkRight(Forest *tree, Tree_node_t *parent, elem_t value)
{
    GONDON(parent)

    Dump(tree, tree->root, BEFORE LOCAL_ARGS);

    Tree_node_t *node = NewTNT();
    parent->right = node;
    Link(parent, node, value);

    Dump(tree, tree->root, AFTER LOCAL_ARGS);
}

void Link(Tree_node_t *parent, Tree_node_t *node, elem_t value)
{
    GONDON(parent)

    node->value = value;
    node->parent = parent;
    node->root_dist = parent->root_dist + 1;
}

void DeleteTNT(Forest *tree, Tree_node_t *node)
{
    GONDON(node)

    if (node->left) {
        DeleteTNT(tree, node->left);
    }
    if (node->right) {
        DeleteTNT(tree, node->right);
    }

    memset(node, 0, sizeof(Tree_node_t));
    free(node);
    // if (tree->root) Dump(tree, tree->root, AFTER LOCAL_ARGS);
}

void TreePrint(Forest *tree, const Tree_node_t *node)
{
    GONDON(tree)
    GONDON(node)

    TreeRecPrint(tree, node);
    printf("\n");
}

void TreeRecPrint(Forest *tree, const Tree_node_t *node)
{
    GONDON(tree)
    GONDON(node)

    printf("(");
    printf("%d", node->value);
    if (node->left)
        TreeRecPrint(tree, node->left);
    else
        printf(" # ");

    if (node->right)
        TreeRecPrint(tree, node->right);
    else
        printf(" # ");

    printf(")");
}

void TreeDestroy(Forest *tree)
{
    GONDON(tree)

    Dump(tree, tree->root, BASE LOCAL_ARGS);

    DeleteTNT(tree, tree->root);

    memset(tree, 0, sizeof(Forest));
    tree = NULL;
    fclose(dump_file);
}

void Chitalochka(Forest *tree, Tree_node_t *parent, Tree_node_t *node, FILE *read_file)
{
    GONDON(tree)

    char symbol = 0;
    
    fscanf(read_file, "%c", &symbol);
    // if (symbol == '\n' || symbol == EOF) {
        // ungetc(symbol, read_file);
        // return;
    // }
    if (symbol == '(') {
        node->parent = parent;
    }
    // TODO: think about int
    fscanf(read_file, "%d", &node->value);
    printf("HERE: %d\n", node->value);
    if (parent != NULL) printf("parent: %p\n left: %p, right: %p;\n\n", parent, parent->left, parent->right);
    fscanf(read_file, "%c", &symbol);
    if (symbol == ')') {
        return;
    }
    if (symbol == '(') {
        ungetc(symbol, read_file);
        node->left = NewTNT();
        Chitalochka(tree, node, node->left, read_file);
    }
    // while (fscanf(read_file, "%c", &symbol) && symbol != ')' && symbol != '(') {}
    fscanf(read_file, "%c", &symbol);
    if (symbol == '(') {
        ungetc(symbol, read_file);
        node->right = NewTNT();
        Chitalochka(tree, node, node->right, read_file);
    }
    // fscanf(read_file, "%cil %cil ", &symbol, &symbol);
    // printf("%d\n", check);
}

////////////////////////////////////////////////////////////////////////////

void Dump(Forest *tree, Tree_node_t *node, int mode LOCAL_PARS)     // Work with HTML file
{
    GONDON(tree)
    GONDON(node)
    AddBackgroundPicture(dump_file);

    fprintf(dump_file,
        "<h1>Dumping_Tree<img src=\"https://magnotek.ru/wp-content/uploads/2018/07/g0R5.gif\" width=\"20\"></h1>\n"
        "<p>"
        "<pre style=\"font-size: 24px;\">"
        "<span style=\"color: gold;\">File: %s, line: %d\n",
        file, line
    );
    fprintf(dump_file, "Function: <span style=\"font-weight: bold;\">%s</span>", function);
    DumpParametres(mode);
    
    DumpPicture(tree, node);
    // ErrorShredder(node);
    fprintf(dump_file,
        "\n"
        "<span style=\"font-weight: bold;\">__GRAPHIC_DUMP__</span>\n"
        "</pre>"
        "</p>\n"
        "<div style=\"text-align: center;\"><img src=\"/home/solo/Documents/GitHub/Wood/%s/%s\"></div>\n",
        FOLDER_NAME, tree->picture_name
    );
    fprintf(dump_file, "\n");
}

void DumpParametres(int mode)
{
    switch(mode) {
        case BEFORE:
            fprintf(dump_file, "(BEFORE)");
            break;
        case AFTER:
            fprintf(dump_file, "(AFTER)");
            break;
        default:
            break;
    }
    fprintf(dump_file, "</span>\n");
}

void DumpPicture(Forest *tree, Tree_node_t *node)     // Work with DOT file
{
    GONDON(node)

    static int open_check = mkdir(FOLDER_NAME, 0777);

    FILE *graph_file = fopen(DOT_FILE_NAME, "w");
    GONDON(graph_file)
    // Write in the html file
    WriteGeneralPart(graph_file);
    size_t index = 0;
    NodesCreator(tree, node, graph_file, &index);
    fprintf(graph_file, "}");

    fclose(graph_file);

    CommandsPreparation(tree);

    system(tree->command);

    // while (getchar() != '\n') {};
}

void WriteGeneralPart(FILE *graph_file)
{
    GONDON(graph_file)

    fprintf(graph_file,
        "digraph G {\n"
        "\tsize=\"18,14\";\n"
        "\tgraph [nodesep = 1.0];\n"
        "\tbgcolor = \"black\";\n"  // transparent
        "\trankdir = TB;\n"
        "\tnode [shape = Mrecord, color = darkred, style = filled, fillcolor = pink];\n"
        "\tedge [color = \"green\", penwidth = 2];\n"
    );
    fprintf(graph_file, "\n");
}

void NodesCreator(Forest *tree, Tree_node_t *node, FILE *graph_file, size_t *index)
{
    GONDON(tree)
    GONDON(node)
    GONDON(graph_file)
    if (*index == 0) {
        fprintf(graph_file, "\tnode%p [label = \"{{%d} | {%p} | {left | %p} | {right | %p} | {parent | %p} | {root_dist | %zd}}\"];\n",
                node, node->value, node, node->left, node->right, node->parent, node->root_dist);
        fprintf(graph_file, "\t{rank = %zd; node%p;}\n", node->root_dist, node);
    }
    if (node->left) {
        CreateLeftNode(tree, node, graph_file, index);
        NodesCreator(tree, node->left, graph_file, index);
    }
    if (node->right) {
        CreateRightNode(tree, node, graph_file, index);
        NodesCreator(tree, node->right, graph_file, index);
    }
    return;
}

void CreateLeftNode(Forest *tree, Tree_node_t *node, FILE *graph_file, size_t *index)
{
    GONDON(tree)
    GONDON(node)
    GONDON(graph_file)

    fprintf(graph_file, 
        "\tnode%p [label = \"{{%d} | {%p} | {left | %p} | {right | %p} | {parent | %p} | {root_dist | %zd}}\"];\n"
        "\t{rank = %zd; node%p;}\n"
        "\tnode%p -> node%p;\n",
        node->left, node->left->value, node, node->left->left, node->left->right, node->left->parent, node->left->root_dist,
        node->left->root_dist, node->left, node, node->left);
    ++(*index);
    fprintf(graph_file,
        "\tnode%zu [style = invis];\n"
        "\t{rank = %zd; node%zu;}\n"
        "\tnode%p -> node%zu [color = \"transparent\"];\n",
        *index, node->left->root_dist, *index, node, *index);
}
void CreateRightNode(Forest *tree, Tree_node_t *node, FILE *graph_file, size_t *index)
{
    GONDON(tree)
    GONDON(node)
    GONDON(graph_file)

    ++(*index);
    fprintf(graph_file,
        "\tnode%zu [style = invis];\n"
        "\t{rank = %zd; node%zu;}\n"
        "\tnode%p -> node%zu [color = \"transparent\"];\n",
        *index, node->right->root_dist, *index, node, *index);
    fprintf(graph_file,
        "\tnode%p [label = \"{{%d} | {%p} | {left | %p} | {right | %p} | {parent | %p} | {root_dist | %zd}}\"];\n"
        "\t{rank = %zd; node%p;}\n"
        "\tnode%p -> node%p;\n",
        node->right, node->right->value, node, node->right->left, node->right->right, node->right->parent, node->right->root_dist,
        node->right->root_dist, node->right, node, node->right);
}

void CommandsCreator(Forest *tree)
{
    GONDON(tree)

    strncpy(tree->command, "/usr/bin/dot -Tpng ", strlen("/usr/bin/dot -Tpng ") + 1);
    strncat(tree->command, DOT_FILE_NAME, strlen(DOT_FILE_NAME) + 1);
    strncat(tree->command, " -o ", 5);
    strncat(tree->command, FOLDER_NAME, strlen(FOLDER_NAME) + 1);
    strncat(tree->command, "/", 2);
}

void CommandsPreparation(Forest *tree)
{
    GONDON(tree)

    ClearNameEnd(tree->command, '/');
    snprintf(tree->picture_name, LEN_COMMAND, "graph%zu.png", tree->count_pict);
    strncat(tree->command, tree->picture_name, strlen(tree->picture_name));

    tree->count_pict++;
}

void ClearNameEnd(char *graph_name, int symbol)
{
    if (graph_name == NULL)
        return;
    char *letter = strrchr(graph_name, symbol);
    letter++;

    while (*letter != '\0')
        *(letter++) = 0;
}

void AddBackgroundPicture(FILE *file_html)
{
    GONDON(file_html)
    
    static int non_first = false;
    if (non_first)
        return;

    fprintf(file_html,
        "<body style =\"background-color: black; color: white;\">\n"
        // "background-image: url(\'/home/solo/Documents/GitHub/Doubly-Linked-List/kaneki_bubble_small.jpg\'); "
        // "background-size: cover; "
        // "background-repeat: no-repeat;"
        // "background-position: center; "
        // "background-attachment: fixed; "
        // "color: white;\">\n"
    );
    non_first = true;
}
// 
// void DumpStructure(Tree_node_t *node)
// {
    // GONDON(node)
// 
    // fprintf(dump_file,
        // "\nstruct <span style=\"color: gold;\">ListData<%p></span> {\n"
        // "\tcapacity = %zu;\n"
        // "\tsize = %zu;\n\n"
        // "\t<span style=\"color: purple; font-weight: bold;\">tail</span> = %zd;\n"
        // "\t<span style=\"color: crimson; font-weight: bold;\">head</span> = %zu;\n\n"
        // "\t<span style=\"color: darkorange; font-weight: bold;\">free</span> = %zd;\n"
        // "\t<span style=\"color: red;\">err</span>  = %b;\n"
        // "}\n",
        // node->list, node->capacity, node->size, node->tail,
        // node->head, node->free, node->err
    // );
// }
// 
// void DumpTable(Tree_node_t *node)
// {
    // GONDON(node)
    // if (node->err == NON_ERR)
        // return;
    // fprintf(dump_file,
        // "\n<table border =\"1\" style=\"width: 20%%; font-size: 22px;\">\n"
        // "<tr style=\"font-weight: bold;\"> <td>Index</td><td>Value</td><td>Next</td><td>Prev</td> </tr>\n"
    // );
    // for (ssize_t i = 0; i < node->capacity; i++) {
        // if (i == node->head)
            // fprintf(dump_file, "<tr style=\"background-color: crimson;\"> ");
        // else if (i == node->tail)
            // fprintf(dump_file, "<tr style=\"background-color: purple;\"> ");
        // else if (i == node->free)
            // fprintf(dump_file, "<tr style=\"background-color: darkorange;\"> ");
        // else
            // fprintf(dump_file, "<tr> ");
        // fprintf(dump_file,
            // "<td>%zd</td>"
            // "<td>%d</td>"
            // "<td>%zd</td>"
            // "<td>%zd</td> </tr>\n",
            // i, LIST[i].value, LIST[i].next, LIST[i].prev
        // );
    // }
    // fprintf(dump_file, "</table>\n");
// }
// 