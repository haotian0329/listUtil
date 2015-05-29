/*
 * link_list.c
 *
 *  Created on: 2013-10-2
 *      Author: zgf
 */

#ifndef LINK_LIST_H_
#define LINK_LIST_H_

#include <sys/types.h>

//定义一个节点单元结构体，双向的
typedef struct st_LIST_NODE {
	struct st_LIST_NODE * next;
	struct st_LIST_NODE * prior;
	void * pdata;
} LIST_NODE, *PLIST_NODE;

//定义双向链表结构体
typedef struct st_DLINK_LIST {
	PLIST_NODE head;
	PLIST_NODE tail;
	u_int count;
} DLINK, *PDLINK;

//定义回调函数类型
typedef int (*DLINKWORKFUN)(void * pdata, void * param);

//链表初始化
void dlink_init(PDLINK pList);

//销毁整个链表，直接对节点数据进行free操作
void dlink_simple_destroy(PDLINK pList);

//销毁整个链表，使用用户提供的回调函数destroy_fun对节点数据进行相应的清除操作
void dlink_destroy(PDLINK pList, DLINKWORKFUN destroy_fun, void * param);

/**
 *从链表头或者尾部插入新节点，节点数据pData由用户保存空间，不应在
 *未删除节点的情况下释放pData的空间。
 */
void dlink_add_head(PDLINK pList, void * pData);
void dlink_add_tail(PDLINK pList, void * pData);

/**
 * 链接后，pFromList将被清空
 */
void dlink_cat_head(PDLINK pToList, PDLINK pFromList);
void dlink_cat_tail(PDLINK pToList, PDLINK pFromList);

/**
 * visit_fun返回0值，表示继续，非0值则中断并返回该值
 */
int dlink_traverse(PDLINK pList, DLINKWORKFUN visit_fun, void * param);
int dlink_traverse_back(PDLINK pList, DLINKWORKFUN visit_fun, void * param);

/* find_fun 返回非0值，表示匹配成功*/
PLIST_NODE dlink_find(PDLINK pList, DLINKWORKFUN find_fun, void * param);

/*删除特定节点，并根据回调函数destroy_fun对该节点数据做相应操作*/
void dlink_del_node(PDLINK pList, PLIST_NODE pToDelete,
		DLINKWORKFUN destroy_fun, void * param);
#define dlink_del_tail(pList, destroy_fun, param) dlink_del_node(pList, (pList)->tail, destroy_fun, param)
#define dlink_del_head(pList, destroy_fun, param) dlink_del_node(pList, (pList)->head, destroy_fun, param)

/* 删除特定节点，不对该节点数据做任何操作*/
void dlink_simple_del_node(PDLINK pList, PLIST_NODE pToDelete);
#define dlink_simple_del_tail(pList) dlink_simple_del_node(pList, (pList)->tail)
#define dlink_simple_del_head(pList) dlink_simple_del_node(pList, (pList)->head)

PLIST_NODE dlink_get_head(PDLINK pList);
PLIST_NODE dlink_get_tail(PDLINK pList);
PLIST_NODE dlink_get_next(PLIST_NODE pCurrent);
PLIST_NODE dlink_get_prior(PLIST_NODE pCurrent);
u_int dlink_get_count(PDLINK pList);

#endif /* STRUCTURES_H_ */

