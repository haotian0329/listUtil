/*
 * link_list.c
 *
 *  Created on: 2013-10-2
 *      Author: zgf
 */
#include "link_list.h"
#include <string.h>
#include <stdlib.h>

/**
 * 初始化链表，全部置0
 */
void dlink_init(PDLINK pList) {
	bzero(pList, sizeof(DLINK));
}

/**
 * 销毁整个列表，直接对节点数据进行free操作
 */
void dlink_simple_destroy(PDLINK pList) {
	PLIST_NODE p, q;
	p = dlink_get_head(pList);
	while (p) {
		q = dlink_get_next(p);
		free(p->pdata);
		free(p);
		p = q;
	}
	bzero(pList, sizeof(DLINK));
}

/**
 * 销毁整个列表，使用用户提供的函数对节点数据进行清除，用户提供的函数的返回值没有作用
 */
void dlink_destroy(PDLINK pList, DLINKWORKFUN destroy_fun, void * param) {
	PLIST_NODE p, q;
	p = dlink_get_head(pList);
	while (p) {
		q = dlink_get_next(p);
		if (destroy_fun != NULL)
			destroy_fun(p->pdata, param);
		free(p);
		p = q;
	}
	bzero(pList, sizeof(DLINK));
}

/**
 * 从链表头部插入新节点
 */
void dlink_add_head(PDLINK pList, void * pData) {
	PLIST_NODE p = (PLIST_NODE) malloc(sizeof(LIST_NODE));
	p->pdata = pData;
	p->next = pList->head;
	p->prior = NULL;

	if (p->next) //链表不为空
		p->next->prior = p;
	else
		//链表为空
		pList->tail = p;

	pList->head = p;
	pList->count++;
}

/**
 * 从链表尾部插入新节点
 */
void dlink_add_tail(PDLINK pList, void * pData) {
	PLIST_NODE p = (PLIST_NODE) malloc(sizeof(LIST_NODE));
	p->pdata = pData;
	p->next = NULL;
	p->prior = pList->tail;

	if (p->prior) //链表不为空
		p->prior->next = p;
	else
		//链表为空
		pList->head = p;

	pList->tail = p;
	pList->count++;
}

/**
 * 头部链接后，pFromList将被清空
 */
void dlink_cat_head(PDLINK pToList, PDLINK pFromList) {
	PLIST_NODE p;
	if (pFromList->count != 0) { //pFromList链表不为空
		p = pToList->head;
		pToList->head = pFromList->head;
		pFromList->tail->next = p;
		if (p) //pToList链表不为空
			p->prior = pFromList->tail;
		else
			pToList->tail = pFromList->tail;
	}
	pToList->count += pFromList->count;
	dlink_init(pFromList);
}

/**
 * 尾部链接后，pFromList将被清空
 */
void dlink_cat_tail(PDLINK pToList, PDLINK pFromList) {
	PLIST_NODE p;
	if (pFromList->count != 0) {
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

/**
 * 从头部遍历所有节点，根据回调函数visit_fun对每个节点做相应的操作
 * visit_fun返回0值，表示继续，非0值则中断并返回该值
 */
int dlink_traverse(PDLINK pList, DLINKWORKFUN visit_fun, void * param) {
	PLIST_NODE p = dlink_get_head(pList);
	int rtn = 0;
	while (p) {
		if ((rtn = visit_fun(p->pdata, param)) != 0)
			break;
		p = dlink_get_next(p);
	}
	return rtn;
}

/**
 * 从尾部遍历所有节点，根据回调函数visit_fun对每个节点做相应的操作
 * visit_fun返回0值，表示继续，非0值则中断并返回该值
 */
int dlink_traverse_back(PDLINK pList, DLINKWORKFUN visit_fun, void * param) {
	PLIST_NODE p = dlink_get_tail(pList);
	int rtn = 0;
	while (p) {
		if ((rtn = visit_fun(p->pdata, param)) != 0)
			break;
		p = dlink_get_prior(p);
	}
	return rtn;
}

/*
 * 根据回调函数find_fun寻找满足条件的节点
 * find_fun 返回非0值，表示匹配成功
 */
PLIST_NODE dlink_find(PDLINK pList, DLINKWORKFUN find_fun, void * param) {
	PLIST_NODE p = dlink_get_head(pList);
	while (p) {
		if (find_fun(p->pdata, param))
			return p;
		p = dlink_get_next(p);
	}
	return NULL;
}

/**
 * 删除特定节点，并根据回调函数destroy_fun对该节点数据做相应操作
 */
void dlink_del_node(PDLINK pList, PLIST_NODE pToDelete,
		DLINKWORKFUN destroy_fun, void * param) {
	if (pList->head == pToDelete) { //头节点
		pList->head = pToDelete->next;
		if (pList->head)
			pList->head->prior = NULL;
	} else if (pList->tail == pToDelete) { //尾节点
		pList->tail = pToDelete->prior;
		if (pList->tail)
			pList->tail->next = NULL;
	} else { //中间节点
		pToDelete->prior->next = pToDelete->next;
		pToDelete->next->prior = pToDelete->prior;
	}

	if (destroy_fun != NULL)
		destroy_fun(pToDelete->pdata, param);
	free(pToDelete);
	pList->count--;
}

/**
 * 删除特定节点，不对该节点数据做任何操作
 */
void dlink_simple_del_node(PDLINK pList, PLIST_NODE pToDelete) {
	if (pList->head == pToDelete) {
		pList->head = pToDelete->next;
		if (pList->head)
			pList->head->prior = NULL;
	} else if (pList->tail == pToDelete) {
		pList->tail = pToDelete->prior;
		if (pList->tail)
			pList->tail->next = NULL;
	} else {
		pToDelete->prior->next = pToDelete->next;
		pToDelete->next->prior = pToDelete->prior;
	}

	free(pToDelete->pdata);
	free(pToDelete);
	pList->count--;
}

/**
 * 获取头部节点
 */
PLIST_NODE dlink_get_head(PDLINK pList) {
	return pList->head;
}

/**
 * 获取尾部节点
 */
PLIST_NODE dlink_get_tail(PDLINK pList) {
	return pList->tail;
}

/**
 * 获取节点数
 */
u_int dlink_get_count(PDLINK pList) {
	return pList->count;
}

/**
 * 获取下一节点
 */
PLIST_NODE dlink_get_next(PLIST_NODE pCurrent) {
	return pCurrent->next;
}

/**
 * 获取上一节点
 */
PLIST_NODE dlink_get_prior(PLIST_NODE pCurrent) {
	return pCurrent->prior;
}

