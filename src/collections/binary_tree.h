/**
 * @file collections/binary_tree.h
 *
 * @section License
 * Copyright (C) 2014-2016, Erik Moqvist
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * This file is part of the Simba project.
 */

#ifndef __COLLECTIONS_BINARY_TREE_H__
#define __COLLECTIONS_BINARY_TREE_H__

#include "simba.h"

struct binary_tree_node_t {
    int key;
    int height;
    struct binary_tree_node_t *left_p;
    struct binary_tree_node_t *right_p;
};

struct binary_tree_t {
    struct binary_tree_node_t *root_p;
};

/**
 * Initialize given binary tree.
 *
 * @param[in] self_p Binary tree.
 *
 * @return zero(0) or negative error code.
 */
int binary_tree_init(struct binary_tree_t *self_p);

/**
 * Insert given node into given binary tree.
 *
 * There can not be two or more nodes in the tree with the same
 * key. This function returns -1 if a node with the same key is
 * already in the binary tree.
 *
 * @param[in] self_p Binary tree to insert the node into.
 * @param[in] node_p Node to insert.
 *
 * @return zero(0) on success, -1 if a node with the same key is
 *         already in the binary tree, otherwise negative error code.
 */
int binary_tree_insert(struct binary_tree_t *self_p,
                       struct binary_tree_node_t *node_p);

/**
 * Delete given node from given binary tree.
 *
 * @param[in] self_p Binary tree to delete the node from.
 * @param[in] key Key of the node to delete.
 *
 * @return zero(0) on success, -1 if the node was not found, otherwise
 *         negative error code.
 */
int binary_tree_delete(struct binary_tree_t *self_p,
                       int key);

/**
 * Search the binary tree for the node with given key.
 *
 * @param[in] self_p Binary tree to search in.
 * @param[in] key Key of the binary tree node to search for.
 *
 * @return Pointer to found node or NULL if a node with given key was
 *         not found in the tree.
 */
struct binary_tree_node_t *
binary_tree_search(struct binary_tree_t *self_p,
                   int key);

/**
 * Print given binary tree.
 *
 * @param[in] self_p Binary tree to print.
 */
void binary_tree_print(struct binary_tree_t *self_p);

#endif
