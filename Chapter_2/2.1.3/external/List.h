#ifndef List_h
#define List_h

#include <stdlib.h>

//LIST node that comprosises each element of a list
struct LIST_NODE;
typedef struct LIST_NODE
{
    struct LIST_NODE *Next;          //Pointer to next element in list
    struct LIST_NODE *Previous;      //Pointer to previous element in list
    void *Value;                     //Pointer to anything to store in each node
}   LIST_NODE;

//LIST is made up of multiple nodes
typedef struct LIST
{
    int Count;                       //Count of how many nodes are in the list
    LIST_NODE *First;                //Position of first node in the list
    LIST_NODE *Last;                 //Position of last node in the list
}   LIST;

LIST *list_Create();
void list_Destroy(LIST *InputList);
void list_Clear(LIST *InputList);
void list_ClearAndDestroy(LIST *InputList);

#define list_Count(A) ((A)->Count)
#define list_First(A) ((A)->First != NULL ? (A)->First->Value : NULL)
#define list_Last(A) ((A)->Last != NULL ? (A)->Last->Value : NULL)

void list_Push(LIST *InputList, void *InputValue);
void *list_Pop(LIST *InputList);

void list_Unshift(LIST *InputList, void *InputValue);
void *list_Shift(LIST *InputList);

void *list_Remove(LIST *InputList, LIST_NODE *InputNode);

#define list_ForEach(L, S, M, V) LIST_NODE *_node = nullptr;\
LIST_NODE *V = nullptr;\
for(V = _node = L->S; _node != nullptr; V = _node = _node->M)

#endif