#include <stdio.h>
#include <stdlib.h>

#define MAX_LEVEL 8

typedef struct skiplistNode
{
        int score;
        struct skiplistLevel
        {
                struct skiplistNode *forward;
        }level[];
}skiplistNode;

typedef struct skiplist
{
        struct skiplistNode *header;
        int level;
}skiplist;

skiplistNode* CreateNode(int level, int score);
skiplist* CreateList();
int RandomLevel();
skiplistNode* InsertNode(skiplist *sl, int score);
int SearchNode(skiplist *sl, int score);
void DeleteNode(skiplist *sl, skiplistNode *node, skiplistNode **update);
int Delete(skiplist *sl, int score);
void FreeNode(skiplistNode *node);
void Free(skiplist *sl);
void Print(skiplist *sl);

skiplistNode* CreateNode(int level, int score)
{
        skiplistNode *sn = (skiplistNode*)malloc(sizeof(skiplistNode) + level * sizeof(struct skiplistLevel));
        sn->score = score;
        return sn;
}

skiplist* CreateList()
{
        int i;
        skiplist *sl = (skiplist*)malloc(sizeof(skiplist));
        sl->header = CreateNode(MAX_LEVEL, 0);
        for(i = 0; i < MAX_LEVEL; ++i)
        {
                sl->header->level[i].forward = NULL;
        }
        sl->level = 1;
        return sl;
}

int RandomLevel()
{
        int level = 1;
        while((rand() & 0xFFFF) < (0.5 * 0xFFFF))
        {
                ++level;
        }
        return level < MAX_LEVEL ? level : MAX_LEVEL;
}

skiplistNode* InsertNode(skiplist *sl, int score)
{
        skiplistNode *update[MAX_LEVEL];
        skiplistNode *node;
        int i, level;

        node = sl->header;
        for(i = sl->level - 1; i >= 0; --i)
        {
                while(NULL != node->level[i].forward && node->level[i].forward->score < score)
                {
                        node = node->level[i].forward;
                }
                update[i] = node;
        }

        level = RandomLevel();
        if(level > sl->level)
        {
                for(i = sl->level; i < level; ++i)
                {
                        update[i] = sl->header;
                }
                sl->level = level;
        }

        node = CreateNode(level, score);
        for(i = 0; i < level; ++i)
        {
                node->level[i].forward = update[i]->level[i].forward;
                update[i]->level[i].forward = node;
        }

        return node;
}

int SearchNode(skiplist *sl, int score)
{
        skiplistNode *node = sl->header;
        int i;

        for(i = sl->level - 1; i >= 0; --i)
        {
                while(NULL != node->level[i].forward && node->level[i].forward->score < score)
                {
                        node = node->level[i].forward;
                }
        }

        node = node->level[0].forward;
        if(NULL != node && node->score == score)
        {
                printf("Found %d\n", score);
                return 1;
        }
        else
        {
                printf("Not Found %d\n", score);
                return 0;
        }
}

void DeleteNode(skiplist *sl, skiplistNode *node, skiplistNode **update)
{
        int i;

        for(i = 0; i < sl->level; ++i)
        {
                if(update[i]->level[i].forward == node)
                {
                        update[i]->level[i].forward = node->level[i].forward;
                }
        }

        while(sl->level > 1 && sl->header->level[sl->level-1].forward == NULL)
        {
                --sl->level;
        }
}

int Delete(skiplist *sl, int score)
{
        skiplistNode *node = sl->header;
        skiplistNode *update[MAX_LEVEL];
        int i;

        for(i = sl->level-1; i >= 0; --i)
        {
                while(NULL != node->level[i].forward && node->level[i].forward->score < score)
                {
                        node = node->level[i].forward;
                }
                update[i] = node;
        }

        node = node->level[0].forward;
        if(NULL != node && node->score == score)
        {
                DeleteNode(sl, node, update);
                FreeNode(node);
                return 1;
        }
        else
        {
                return 0;
        }
}

void FreeNode(skiplistNode *node)
{
        free(node);
}

void Free(skiplist *sl)
{
        skiplistNode *node = sl->header->level[0].forward;
        skiplistNode *next;

        free(sl->header);

        while(node)
        {
                next = node->level[0].forward;
                FreeNode(node);
                node = next;
        }

        free(sl);
}

void Print(skiplist *sl)
{
        skiplistNode *node;
        int i;

        for(i = 0; i < sl->level; ++i)
        {
                printf("Level[%d]: ", i);
                node = sl->header->level[i].forward;
                while(node)
                {
                        printf("%d -> ", node->score);
                        node = node->level[i].forward;
                }
                printf("NULL\n");
        }
}

int main()
{
        srand((unsigned int)time(NULL));
        int count = 20;
        int i;

        printf("=== Init SkipList ===\n");
        skiplist *sl = CreateList();
        for(i = 0; i < count; ++i)
        {
                InsertNode(sl, i);
        }
        Print(sl);

        printf("=== Search SkipList ===\n");
        for(i = 0; i < count; ++i)
        {
                int value = rand() % (count + 10);
                SearchNode(sl, value);
        }

        printf("=== Delete SkipList ===\n");
        for(i = 0; i < count + 10; i+=2)
        {
                printf("Delete[%d] : %s\n", i , Delete(sl, i) ? "SUCCESS" : "NOT FOUND");
        }
        Print(sl);

        Free(sl);
        sl = NULL;

        return 0;

}

