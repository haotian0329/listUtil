#include "link_list.h"
#include <memory.h>
#include <malloc.h>

void dlink_init (PDLINK pList)
{
	memset(pList, 0, sizeof(DLINK));
}
/**
 * void dlink_simple_destroy (PDLINK pList)
 * 销毁整个列表，直接对节点数据进行free操作
 */
void dlink_simple_destroy (PDLINK pList)
{
	PLIST_NODE p,q;
	p = dlink_get_head(pList);
	while (p)
	{
		q = dlink_get_next(p);
		free(p->pdata);
		free(p);
		p = q;
	}
	memset(pList, 0, sizeof(DLINK));
}

/**
 * void dlink_destroy (PDLINK pList, DLINKWORKFUN destroy_fun, void * param)
 * 销毁整个列表，使用用户提供的函数对节点数据进行清除，用户提供的函数的返回值没有作用。
 */
void dlink_destroy (PDLINK pList, DLINKWORKFUN destroy_fun, void * param)
{
	PLIST_NODE p,q;
	p = dlink_get_head(pList);
	while (p)
	{
		q = dlink_get_next(p);
		if (destroy_fun) destroy_fun(p->pdata, param);
		free(p);
		p = q;
	}
	memset(pList, 0, sizeof(DLINK));
}

void dlink_add_head(PDLINK pList, void * pData)
{
	PLIST_NODE p = (PLIST_NODE) malloc(sizeof (LIST_NODE));
	p->pdata = pData;
	p->next = pList->head;
	p->prior = NULL;

	if (p->next)
		p->next->prior = p;
	else
		pList->tail = p;

	pList->head = p;
	pList->count++;
}

void dlink_add_tail(PDLINK pList, void * pData)
{
	PLIST_NODE p = (PLIST_NODE) malloc(sizeof (LIST_NODE));
	p->pdata = pData;
	p->next = NULL;
	p->prior = pList->tail;

	if (p->prior)
		p->prior->next = p;
	else
		pList->head = p;

	pList->tail = p;
	pList->count++;
}

void dlink_cat_head(PDLINK pToList, PDLINK pFromList)
{
	PLIST_NODE p;
	if (pFromList->count != 0)
	{
		p = pToList->head;
		pToList->head = pFromList->head;
		pFromList->tail->next = p;
		if (p)
			p->prior = pFromList->tail;
		else
			pToList->tail = pFromList->tail;
	}
	pToList->count += pFromList->count;
	dlink_init(pFromList);
}
void dlink_cat_tail(PDLINK pToList, PDLINK pFromList)
{
	PLIST_NODE p;
	if (pFromList->count != 0)
	{
		p = pToList->tail;
		pToList->tail = pFromList->tail;
		pFromList->head->prior = p;
		if (p)
			p->next = pFromList->head;
		else
			pToList->head = pFromList->head;
	}
	pToList->count += pFromList->count;
	dlink_init(pFromList);
}

int dlink_traverse(PDLINK pList, DLINKWORKFUN visit_fun, void * param)
{
	PLIST_NODE p = dlink_get_head(pList);
	int rtn = 0;
	while (p)
	{
		if ((rtn = visit_fun(p->pdata, param)) != 0)
			break;
		p = dlink_get_next(p);
	}
	return rtn;
}

int dlink_traverse_back(PDLINK pList, DLINKWORKFUN visit_fun, void * param)
{
	PLIST_NODE p = dlink_get_tail(pList);
	int rtn = 0;
	while (p)
	{
		if ((rtn = visit_fun(p->pdata, param)) != 0)
			break;
		p = dlink_get_prior(p);
	}
	return rtn;
}

/**
 * 根据cmp_fun的返回值进行链表排序。
 * int cmp_fun(void * pd1, void * pd2, void * param)
 * 返回正数表示pd1大于pd2，负数表示pd1小于pd2，0表示相等。
 * 最终链表按“升序”排列
 * 选择排序法
 */
void dlink_sort(PDLINK pList, DLINKCMPFUN cmp_fun, void * param)
{
	PLIST_NODE pTail, pNow, pMin;
	void * pt;
	if (pList->count < 2 || cmp_fun == NULL)
		return;
	pTail = pList->head;
	pNow = pList->tail;
	while (pTail != pList->tail)
	{
		pNow = pTail->next;
		pMin = pTail;
		while(pNow)
		{
			if (cmp_fun(pMin->pdata, pNow->pdata, param) > 0)
				pMin = pNow;
			pNow = dlink_get_next(pNow);
		}
		pt = pTail->pdata;
		pTail->pdata = pMin->pdata;
		pMin->pdata = pt;
		pTail = dlink_get_next(pTail);
	}
}

/**
 * 删掉重复元素。
 */
void dlink_del_dup(PDLINK pList, DLINKCMPFUN cmp_fun, void * cmp_param,
		DLINKWORKFUN destroy_fun, void * des_param)
{
	PLIST_NODE p, q, t;
	if (pList->count < 2 || cmp_fun == NULL)
		return;
	p = pList->head;
	while(p != pList->tail)
	{
		q = p->next;
		while(q)
		{
			if (cmp_fun(p->pdata, q->pdata, cmp_param) == 0)
			{
				t = q;
				q = dlink_get_next(q);
				dlink_del_node(pList, t, destroy_fun, des_param);
			}
			else
				q = dlink_get_next(q);
		}
		p = dlink_get_next(p);
	}
}

/**
 * 删掉重复元素，销毁函数直接free掉pdata。
 */
void dlink_simple_del_dup(PDLINK pList, DLINKCMPFUN cmp_fun, void * param)
{
	PLIST_NODE p, q, t;
	if (pList->count < 2 || cmp_fun == NULL)
		return;
	p = pList->head;
	while(p != pList->tail)
	{
		q = p->next;
		while(q)
		{
			if (cmp_fun(p->pdata, q->pdata, param) == 0)
			{
				t = q;
				q = dlink_get_next(q);
				dlink_simple_del_node(pList, t);
			}
			else
				q = dlink_get_next(q);
		}
		p = dlink_get_next(p);
	}
}



/*
 * dlink_find
 * find_fun 返回非0值，表示匹配成功。
 */
PLIST_NODE dlink_find(PDLINK pList, DLINKWORKFUN find_fun, void * param)
{
	PLIST_NODE p = dlink_get_head(pList);
	while (p)
	{
		if (find_fun(p->pdata, param))
			return p;
		p = dlink_get_next(p);
	}
	return NULL;
}

void dlink_del_node(PDLINK pList, PLIST_NODE pToDelete, DLINKWORKFUN destroy_fun, void * param)
{
	if (pList->head == pToDelete)
	{
		pList->head = pToDelete->next;
		if (pList->head)
			pList->head->prior = NULL;
	}
	else if (pList->tail == pToDelete)
	{
		pList->tail = pToDelete->prior;
		if (pList->tail)
			pList->tail->next = NULL;
	}
	else
	{
		pToDelete->prior->next = pToDelete->next;
		pToDelete->next->prior = pToDelete->prior;
	}

	if (destroy_fun) destroy_fun(pToDelete->pdata, param);
	free(pToDelete);
	pList->count--;
}


void dlink_simple_del_node(PDLINK pList, PLIST_NODE pToDelete)
{
	if (pList->head == pToDelete)
	{
		pList->head = pToDelete->next;
		if (pList->head)
			pList->head->prior = NULL;
	}
	else if (pList->tail == pToDelete)
	{
		pList->tail = pToDelete->prior;
		if (pList->tail)
			pList->tail->next = NULL;
	}
	else
	{
		pToDelete->prior->next = pToDelete->next;
		pToDelete->next->prior = pToDelete->prior;
	}

	free(pToDelete->pdata);
	free(pToDelete);
	pList->count--;
}



PLIST_NODE dlink_get_head(PDLINK pList)
{
	return pList->head;
}

PLIST_NODE dlink_get_tail(PDLINK pList)
{
	return pList->tail;
}

uint32_t dlink_get_count(PDLINK pList)
{
	return pList->count;
}

PLIST_NODE dlink_get_next(PLIST_NODE pCurrent)
{
	return pCurrent->next;
}
PLIST_NODE dlink_get_prior(PLIST_NODE pCurrent)
{
	return pCurrent->prior;
}

/**
 * 移动pToMove节点到pPosition节点位置的前方或后方
 * nFront为1表明移动到前方，为0表明移动到后方。
 */
void dlink_move_node(PDLINK pList, PLIST_NODE pToMove, PLIST_NODE pPosition, int nFront)
{
	PLIST_NODE pPrior = NULL;
	PLIST_NODE pNext = NULL;
	if (pList == NULL || pToMove == NULL || pPosition == NULL ||
			pToMove == pPosition)
		return;
	if (nFront == 1 && pToMove->next == pPosition)
		return;

	if (nFront == 0 && pToMove->prior == pPosition)
		return;

	pPrior = pToMove->prior;
	pNext = pToMove->next;

	//1.从当前列表摘除要移动的节点
	if (pPrior == NULL) //要移动头节点
	{
		pNext->prior = NULL;
		pList->head = pNext;
	}
	else if (pNext == NULL) //要移动尾节点
	{
		pPrior->next = NULL;
		pList->tail = pPrior;
	}
	else
	{
		pPrior->next = pNext;
		pNext->prior = pPrior;
	}

	//2.将节点插入目标位置
	if (nFront == 1)
	{
		pPrior = pPosition->prior;
		pToMove->next = pPosition;
		pToMove->prior = pPrior;
		pPosition->prior = pToMove;

		if (pPrior == NULL)
			pList->head = pToMove;
		else
			pPrior->next = pToMove;
	}
	else
	{
		pNext = pPosition->next;
		pToMove->prior = pPosition;
		pToMove->next = pNext;
		pPosition->next = pToMove;

		if (pNext == NULL)
			pList->tail = pToMove;
		else
			pNext->prior = pToMove;
	}
}



