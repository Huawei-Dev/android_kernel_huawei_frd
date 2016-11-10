

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "omlist.h"


#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/*****************************************************************************
  3 函数定义
*****************************************************************************/


VOS_VOID OM_InitNodeHead(
    OM_LIST_NODE_STRU                  *pstNode
)
{
    pstNode->pstNext = pstNode;
    pstNode->pstPrev = pstNode;
}


VOS_VOID OM_AddNodeToList(
    OM_LIST_NODE_STRU                  *pstNewNode,
    OM_LIST_NODE_STRU                  *pstPrevNode,
    OM_LIST_NODE_STRU                  *pstNextNode
)
{
    pstPrevNode->pstNext = pstNewNode;
    pstNextNode->pstPrev = pstNewNode;

    pstNewNode->pstNext = pstNextNode;
    pstNewNode->pstPrev = pstPrevNode;
}


VOS_VOID OM_AddNodeToListTail(
    OM_LIST_NODE_STRU                  *pstNewNode,
    OM_LIST_NODE_STRU                  *pstHeadNode
)
{
    OM_AddNodeToList(pstNewNode, pstHeadNode->pstPrev, pstHeadNode);
}


VOS_VOID OM_AddNodeToListHead(
    OM_LIST_NODE_STRU                  *pstNewNode,
    OM_LIST_NODE_STRU                  *pstHeadNode
)
{
    OM_AddNodeToList(pstNewNode, pstHeadNode, pstHeadNode->pstNext);
}


VOS_VOID OM_DelNodeFromList(
    OM_LIST_NODE_STRU                  *pstNode
)
{
    pstNode->pstNext->pstPrev = pstNode->pstPrev;
    pstNode->pstPrev->pstNext = pstNode->pstNext;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */




