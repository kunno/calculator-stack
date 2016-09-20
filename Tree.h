/******************************************************************************
  File Name:    Tree.h
  Description:  This file is part of a multiple files to create a binary tree
                that implements a symbol tree to allow variable assignment to
                the calculator. This one in particular is the driver the one
                that runs the program for the c++ files. 
 *****************************************************************************/
#ifndef TREE_H
#define TREE_H

/* DO NOT CHANGE:  This file is used in evaluation */

#include <fstream>
#include <iostream>
using namespace std;

typedef long offset;

template <class Whatever>
struct TNode;

template <class Whatever>
class Tree {
        friend struct TNode<Whatever>;
        static long cost;
        static int debug_on;
        fstream * fio;
        long occupancy;
        static long operation;
        offset root;
        unsigned long tree_count;
        void ResetRoot (void);

public:
        Tree (const char *);
        ~Tree (void);

        static long GetCost ();
        static long GetOperation ();
        static void IncrementCost ();
        static void IncrementOperation ();
        static void Set_Debug_On (void);
        static void Set_Debug_Off (void);

        unsigned long Insert (Whatever &);
        unsigned long Lookup (Whatever &) const;
        unsigned long Remove (Whatever &);
        ostream & Write (ostream &) const;
};

#include "Tree.c"

#endif
