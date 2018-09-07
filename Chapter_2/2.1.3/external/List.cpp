#include "../../misc/includes/debug.h"
#include "../external/List.h"
#include <stdio.h>

LIST *list_Create()
{
    return (LIST *)calloc(1, sizeof(LIST));
}

void list_Destroy(LIST *InputList)
{
    list_ForEach(InputList, First, Next, Current)
    {
        if(Current->Previous)
        {
            free(Current->Previous);
        }
    }

    free(InputList->Last);
    free(InputList);
}

void list_Clear(LIST *InputList)
{
    list_ForEach(InputList, First, Next, Current)
    {
        free(Current->Value);
    }
}

void list_ClearAndDestroy(LIST *InputList)
{
    list_Clear(InputList);
    list_Destroy(InputList);
}

void list_Push(LIST *InputList, void *InputValue)
{
    LIST_NODE *Node = (LIST_NODE *) calloc(1, sizeof(LIST_NODE));
    debug_CheckMemory(Node);

    Node->Value = InputValue;

    if(InputList->Last == nullptr)
    {
        InputList->First = Node;
        InputList->Last = Node;
    }
    else
    {
        InputList->Last->Next = Node;
        Node->Previous = InputList->Last;
        InputList->Last = Node;
    }

    InputList->Count++;

error:

    return;
}

void *list_Pop(LIST *InputList)
{
    LIST_NODE *Node = InputList->Last;
    return Node != nullptr ? list_Remove(InputList, Node) : nullptr; 
}

void list_Unshift(LIST *InputList, void *InputValue)
{
    LIST_NODE *Node = (LIST_NODE *) calloc(1, sizeof(LIST_NODE));
    debug_CheckMemory(Node);

    Node->Value = InputValue;

    if(InputList->Last == nullptr)
    {
        InputList->First = Node;
        InputList->Last = Node;
    }
    else
    {
        Node->Next = InputList->First;
        InputList->First->Previous = Node;
        InputList->First = Node;
    }

    InputList->Count++;

error:

    return;
}

void *list_Shift(LIST *InputList)
{
    LIST_NODE *Node = InputList->First;
    return Node != nullptr ? list_Remove(InputList, Node) : nullptr;
}

void *list_Remove(LIST *InputList, LIST_NODE *InputNode)
{
    void *Result = nullptr;

    debug_CheckError(InputList->First && InputList->Last, "List is empty");
    debug_CheckError(InputNode, "Node can't be null");

    if(InputNode == InputList->First && InputNode == InputList->Last)
    {
        InputList->First = nullptr;
        InputList->Last = nullptr;
    }

    else if(InputNode == InputList->First)
    {
        InputList->First = InputNode->Next;
        debug_CheckError(InputList->First != nullptr, "Invalid list, first element is null");
        InputList->First->Previous = nullptr;
    }

    else if(InputNode == InputList->Last)
    {
        InputList->Last = InputNode->Previous;
        debug_CheckError(InputList->Last != nullptr, "Invalid list, next element is null");
        InputList->Last->Next = nullptr;
    }

    else
    {
        LIST_NODE *After = InputNode->Next;
        LIST_NODE *Before = InputNode->Previous;
        After->Previous = Before;
        Before->Next = After;
    }

    InputList->Count--;
    Result = InputNode->Value;
    free(InputNode);

error:

    return Result;
}


