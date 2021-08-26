/*******************************************************************************
 * Copyright (c) 2021, Initialsoft, Inc.
 * All rights reserved.
 *
 * main : [description]
 *
 * filename : main.cc
 * author   : hunt978(hunt978@initialsoft.com)
 * create   : 2021-03-15 03:15:35 UTC
 * modified : 2021-03-17 16:24:15 UTC
\******************************************************************************/

////////////////////////////////////////////////////////////////////////////////
// Headers
//
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
// Typedefs & Constants
//
#define nullptr NULL

////////////////////////////////////////////////////////////////////////////////
// Inner Scope Objects
//
struct Node {
  uint32_t fid;
  uint32_t tid;

  struct Node* prev;
  struct Node* next;
};

uint32_t find(int* parent, uint32_t parent_len, uint32_t id) {
  if (parent[id] == -1) {
    return id;
  }
  return find(parent, parent_len, (uint32_t)parent[id]);
}

void Union(int* parent, uint32_t parent_len, uint32_t x, uint32_t y)
{
    uint32_t xset = find(parent, parent_len, x);
    uint32_t yset = find(parent, parent_len, y);
    parent[xset] = (int)yset;
}

/**
 * @brief      check if cycle exist if new node added to list using union-find set
 *
 *
 * @param      head  The head
 * @param[in]  fid   The fid
 * @param[in]  tid   The tid
 *
 * @return     -1 if not valid, 0 if valid
 */
int check_valid(struct Node* head, uint32_t fid, uint32_t tid) {
  int edge_num = 0;
  uint32_t max_point = fid > tid ? fid : tid;
  struct Node* node = head;
  while (node != nullptr)
  {
    edge_num += 1;
    max_point = max_point > node->tid ? max_point : node->tid;
    max_point = max_point > node->fid ? max_point : node->fid;
    node = node->next;
  }

  int* parent = (int*)malloc(sizeof(int) * (max_point));
  memset(parent, -1, sizeof(int) * (max_point));

  node = head;
  while (node != nullptr)
  {
    uint32_t x = find(parent, max_point, node->fid);
    uint32_t y = find(parent, max_point, node->tid);
    if (x == y) {
      free(parent);
      return -1;
    }
    Union(parent, max_point, x, y);
    node = node->next;
  }

  uint32_t x = find(parent, max_point, fid);
  uint32_t y = find(parent, max_point, tid);
  if (x == y) {
    free(parent);
    return -1;
  }

  free(parent);
  return 0;
}

/**
 * @brief      insert a node into list
 *
 *             there are 4 strains for the container :
 *
 *               1) from id and to id **CANNOT** be the same
 *               2) it should **NOT** hold duplicated node
 *               3) it should **NOT** hold circled link
 *               4) fan-out is allowed while fan-in is **NOT**.
 *
 * @param      head  The head
 * @param[in]  fid   The fid
 * @param[in]  tid   The tid
 *
 * @return     new head
 */
struct Node* insert_node(struct Node* head, uint32_t fid, uint32_t tid)
{
  if (fid == tid) {
    return head;
  }

  struct Node* curr_node = (struct Node*)malloc(sizeof(struct Node));
  memset(curr_node, 0, sizeof(struct Node));
  curr_node->fid = fid;
  curr_node->tid = tid;
  if (head == nullptr) {
    return curr_node;
  }

  struct Node* next_node = head->next;
  struct Node* node = head;

  while (next_node != nullptr)
  {
    if (node->tid == curr_node->tid || next_node->tid == curr_node->tid) {
      free(curr_node);
      return head;
    } else if (node->tid < curr_node->tid && next_node->tid > curr_node->tid) {
      if (0 != check_valid(head, fid, tid)) {
        /* cicle exist if node added to list */
        free(curr_node);
        return head;
      }

      node->next = curr_node;
      curr_node->prev = node;
      curr_node->next = next_node;
      next_node->prev = curr_node;
      return head;
    } else if (node->tid > curr_node->tid) {
      break;
    }
    node = next_node;
    next_node = node->next;
  }

  if (node->tid == curr_node->tid || 0 != check_valid(head, fid, tid)) {
      free(curr_node);
      return head;
  } else if (node->tid < curr_node->tid) {
    curr_node->next = node->next;
    node->next = curr_node;
    curr_node->prev = node;
  } else {
    curr_node->prev = node->prev;
    curr_node->next = node;
    node->prev = curr_node;
  }

  node->next = curr_node;
  curr_node->prev = node;

  return head;
}

/**
 * @brief      remove a node from list
 *
 * @param      head  The head
 * @param[in]  fid   The fid
 * @param[in]  tid   The tid
 *
 * @return     new head
 */
struct Node* remove_node(struct Node* head, uint32_t fid, uint32_t tid)
{
  // \TODO add your code here
  struct Node* node = head;
  struct Node* prev_node = node->prev;
  struct Node* new_head = head;
  while (node != nullptr) {
    if (node->fid == fid && node->tid == tid) {
      struct Node* next_node = node->next;
      if (prev_node == nullptr) {
        new_head = next_node;
        if (next_node != nullptr) {
          next_node->prev = nullptr;
        }
      } else {
        prev_node->next = next_node;
        next_node->prev = prev_node;
      }
      free(node);
      break;
    }
  }

  return new_head;
}

/**
 * @brief      Dumps a list.
 *
 * @param      head  The head
 */
void dump_list(struct Node* head)
{
  printf("---------------------------\n");
  struct Node* node = head;
  while (node) {
    printf("  %d -> %d\n", node->fid, node->tid);
    node = node->next;
  }
}

/**
 * @brief      Does a test.
 */
void do_test()
{
  struct Node* head = nullptr;

  head = insert_node(head, 1, 2);
  dump_list(head);
  head = insert_node(head, 2, 4);
  dump_list(head);
  head = insert_node(head, 2, 3);
  dump_list(head);
  head = insert_node(head, 3, 4);
  dump_list(head);
  head = insert_node(head, 4, 1);
  dump_list(head);
  head = insert_node(head, 1, 2);
  dump_list(head);

  while (head) {
    head = remove_node(head, head->fid, head->tid);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Classes
//

////////////////////////////////////////////////////////////////////////////////
// Functions
//
int main(int argc, char const* argv[])
{
  do_test();

  return 0;
}

////////////////////////////////// EOF /////////////////////////////////////////
