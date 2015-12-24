#ifndef LINK_LIST_H_
#define LINK_LIST_H_

#include <stdint.h>
typedef struct st_LIST_NODE
{
	struct st_LIST_NODE * next;
	struct st_LIST_NODE * prior;
	void * pdata;
} LIST_NODE, * PLIST_NODE;

typedef struct st_DLINK_LIST
{
	PLIST_NODE head;
	PLIST_NODE tail;
	uint32_t count;
} DLINK, * PDLINK;

typedef int (*DLINKWORKFUN)(void * pdata, void * param);
typedef int (*DLINKCMPFUN)(void * pd1, void * pd2, void * param);

void dlink_init (PDLINK pList);


void dlink_simple_destroy (PDLINK pList);

void dlink_destroy (PDLINK pList, DLINKWORKFUN destroy_fun, void * param);
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


void dlink_del_node(PDLINK pList, PLIST_NODE pToDelete, DLINKWORKFUN destroy_fun, void * param);
#define dlink_del_tail(pList, destroy_fun, param) dlink_del_node(pList, (pList)->tail, destroy_fun, param)
#define dlink_del_head(pList, destroy_fun, param) dlink_del_node(pList, (pList)->head, destroy_fun, param)

void dlink_simple_del_node(PDLINK pList, PLIST_NODE pToDelete);

#define dlink_simple_del_tail(pList) dlink_simple_del_node(pList, (pList)->tail)
#define dlink_simple_del_head(pList) dlink_simple_del_node(pList, (pList)->head)

/**
 * visit_fun返回0值，表示继续，非0值则中断并返回该值
 */
int dlink_traverse(PDLINK pList, DLINKWORKFUN visit_fun, void * param);
int dlink_traverse_back(PDLINK pList, DLINKWORKFUN visit_fun, void * param);

void dlink_sort(PDLINK pList, DLINKCMPFUN cmp_fun, void * param);
void dlink_del_dup(PDLINK pList, DLINKCMPFUN cmp_fun, void * cmp_param,
		DLINKWORKFUN destroy_fun, void * des_param);
void dlink_simple_del_dup(PDLINK pList, DLINKCMPFUN cmp_fun, void * param);

PLIST_NODE dlink_find(PDLINK pList, DLINKWORKFUN find_fun, void * param);

PLIST_NODE dlink_get_head(PDLINK pList);
PLIST_NODE dlink_get_tail(PDLINK pList);
PLIST_NODE dlink_get_next(PLIST_NODE pCurrent);
PLIST_NODE dlink_get_prior(PLIST_NODE pCurrent);
uint32_t dlink_get_count(PDLINK pList);


void dlink_move_node(PDLINK pList, PLIST_NODE pToMove, PLIST_NODE pPosition, int nFront);
#define dlink_move_to_head(pList, pToMove) dlink_move_node((pList), (pToMove), dlink_get_head((pList)), 1)
#define dlink_move_to_tail(pList, pToMove) dlink_move_node((pList), (pToMove), dlink_get_tail((pList)), 0)

#endif /* STRUCTURES_H_ */


