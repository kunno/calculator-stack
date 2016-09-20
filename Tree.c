/****************************************************************************** 
  File Name:    Tree.c
  Description:  This file is part of a multiple files to create a binary tree
                that implements a symbol tree to allow variable assignment to
                the calculator. The tree aspect of this file works as a binary
                search tree in which it allows the user to create a pre-order
                tree. Update to this program reads and writes created nodes to
                disk. 
 *****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "Tree.h"

// debug messages
static const char ALLOCATE[] = " - Allocating]\n";
static const char COST_READ[] = "[Cost Increment (Disk Access): Reading ";
static const char COST_WRITE[] = "[Cost Increment (Disk Access): Writing ";
static const char DEALLOCATE[] = " - Deallocating]\n";
static const char TREE[] = "[Tree ";

template <class Whatever>
int Tree<Whatever>::debug_on = 0;

template <class Whatever>
long Tree<Whatever>::cost = 0;

template <class Whatever>
long Tree<Whatever>::operation = 0;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif
// defining null
#ifdef NULL
#undef NULL
#define NULL 0
#endif

#define THRESHOLD 2

template <class Whatever>
ostream & operator << (ostream &, const TNode<Whatever> &);

/******************************************************************************
% Routine Name : TNode class
% File :         Tree.c
% 
% Description :  This struct contains all the necessary functions for
%                allocating, inserting, searching, and deallocating TNodes. It
%                also contains the Tree class within which is defined below. 
%
% Parameters descriptions :
% 
% name               description
% ------------------ ---------------------------------------------------------
% none               N/A
******************************************************************************/
template <class Whatever>
struct  TNode {
/******************************************************************************
% Routine Name : Tree class
% File :         Tree.c
% 
% Description :  This class contains all the necessary functions for
%                allocating, inserting, searching, and deallocating the tree.
%
% Parameters descriptions :
% 
% name               description
% ------------------ ---------------------------------------------------------
% none               N/A
******************************************************************************/
// friends:

// data fields:
	Whatever data;
	long height;
	long balance;
	offset left;
	offset right;
	offset this_position;	// current position

// function fields:
	TNode () : height (0), balance (0), left (0), right (0), 
		this_position (0) {}

	// to declare the working TNode in Tree's Remove
	TNode (Whatever & element) : data (element), height (0), balance (0),
		left (0), right (0), this_position (0) {}
	
	TNode (Whatever &, fstream *, long &);	// to add new node to disk
	TNode (const offset &, fstream *);	// to read node from disk
	
	unsigned long Insert (Whatever &, fstream *, long &, offset &);
	unsigned long Lookup (Whatever &, fstream * ) const;
	void Read (const offset &, fstream *);	// read node from disk
	unsigned long Remove (TNode<Whatever> &, fstream *, long &, offset &,
		long fromSHB = FALSE);
	void ReplaceAndRemoveMin (TNode<Whatever> &, fstream *, offset &);
	void SetHeightAndBalance (fstream *, offset &);
	void Write (fstream *) const;		// update node to disk

	ostream & Write_AllTNodes (ostream &, fstream *) const;
};

template <class Whatever>
unsigned long Tree<Whatever> :: Insert (Whatever & element) {
/******************************************************************************
% Routine Name : Tree<Whatever> :: Insert (public)
% File :         Tree.c
% 
% Description : This function will insert the element in the tree if the root
%               is null and delegates the rest to TNodes insert.
%               If the element cannot be inserted, null will be returned.
%               If the element can be inserted, the element is inserted. 
%               Duplicate insertions will cause the existing element to be
%               deleted, and the duplicate element to take its place. If the
%               element can be inserted true is returned. 
%
% Parameters descriptions :
%  
% name               description
% ------------------ ---------------------------------------------------------
% element            The element to insert.
% <return>           true or false indicating success or failure of insertion 
******************************************************************************/ 
   
    // variable to store success or failure
    long result = 0;

    // if root doesnt exist
    if( occupancy == 0 ){
        // make root node
        TNode<Whatever> writeRootNode( element, fio, occupancy );

    }
    // otherwise delegate insertion to TNodes insert */
    else{
        TNode<Whatever> readRootNode( root, fio ); // read root node
        //pass to tnodes insert 
        result = readRootNode.Insert( element, fio, occupancy, root ); 
        
    }
    IncrementOperation(); // call to increment operation count 
    return result; // returns success or failure 
}

template <class Whatever>
void TNode<Whatever> :: ReplaceAndRemoveMin (TNode<Whatever> & targetTNode, 
	fstream * fio, offset & PositionInParent) {
/******************************************************************************
% Routine Name : TNode :: ReplaceAndRemoveMin (private)
% File :         Tree.c
% 
% Description : This function will be called when removing a TNode with two
%               children, replaces that TNode with the minimum TNode in its
%               right subtree to maintain a balanced tree structure.
%
% Parameters descriptions :
%  
% name               description
% ------------------ ---------------------------------------------------------
% targetTNode        Reference to the TNode to remove that has two children.
% PositionInParent   The reference to the TNode position in the parent
%                    TNode used to get to the current TNode.
% fio                Filestream corresponding to the datafile where the Tree is
%                    stored on disk.
******************************************************************************/ 
    
    // if left node exists 
    if( left ){
        TNode<Whatever> leftNode( left, fio ); 
        // call RARM on targetTNode with left as PIP
        leftNode.ReplaceAndRemoveMin( targetTNode, fio, left );
        // update height and balance
        SetHeightAndBalance( fio, PositionInParent );
    }
    // if left doesnt exist
    else{
        // set PIP to right
        PositionInParent = right;
        // save data in targetTNodes data
        targetTNode.data = data;
    }
}

template <class Whatever>
unsigned long TNode<Whatever> :: Remove (TNode<Whatever> & elementTNode,
	fstream * fio, long & occupancy, offset & PositionInParent,
	long fromSHB) {
/******************************************************************************
% Routine Name : TNode :: Remove (private)
% File :         Tree.c
% 
% Description : This function will remove element in the
%               tree and prevent the lookup of removed element.
%
% Parameters descriptions :
%  
% name               description
% ------------------ ----------------------------------------------------------
% elementTNode       The element needing to be removed
% PositionInParent   The reference to the TNode position in the parent
%                    TNode used to get to the current TNode.
% fromSHB            keeps track of whether or not Remove was called 
%                    from SHAB so that Remove can determine wheather or
%                    not to call SHAB. returns true or false.
% fio                The filestream corresponding to the datafile where the Tree
%                    is stored on the disk.
% occupancy          Reference to the occupancy of the tree.
% <return>           pointer to item existing, or null if not existing
%
******************************************************************************/ 

    // variable to store success or failure 
    long result = 0;

    // if element to remove if found 
    if( data == elementTNode.data){
        // stores complete data to include studentnum 
        elementTNode.data = data; 
        // if node has no children (leaf node)
        if( left == NULL && right == NULL ){
            // assign PIP to null and delete current node 
            PositionInParent = NULL;                 
            result = 1; // success 
            return result;
        }
        // right child exists
        else if( left == NULL && right != NULL ){
            // assign PIP to right and delete current node 
            PositionInParent = right;
            result = 1; // success 
            return result;
        }
        // left child exists
        else if( right == NULL && left != NULL ){
            // assign PIP to left and delete current node 
            PositionInParent = left;
            result = 1; // success 
            return result;
        }
        // two children
        else{
            TNode<Whatever> rightNode( right, fio ); 
            // call RARM on the right child 
            rightNode.ReplaceAndRemoveMin( *this, fio, right );
            result = 1; // success

        }
        // call to write to disk
        Write(fio);
    }
    // if current data is not less than data to be deleted 
    else if( !(data < elementTNode.data) ){
        // if left child exists 
        if( left != NULL ){
            TNode<Whatever> leftNode( left, fio );
            // assign PIP to left 
            // call remove on element to be removed and save result 
            result = leftNode.Remove( elementTNode, fio, occupancy, left, 
                    FALSE );
        }
    }
    // in all other cases differnt from above 
    else{
        // if right child exists
        if( right != NULL ){
            TNode<Whatever> rightNode( right, fio );
            // assign PIP to right 
            // call remove on element to be removed and save result 
            result = rightNode.Remove( elementTNode, fio, occupancy, right, 
                    FALSE );
        }
    }
    // update height and balance of nodes in tree 
    if(!fromSHB && result == 1 )
        SetHeightAndBalance( fio, PositionInParent );
    
    return result; // return success or failure 
}                  
	
template <class Whatever>
unsigned long Tree<Whatever> :: Remove (Whatever & element) {
/******************************************************************************
% Routine Name : Tree :: Remove (public)
% File :         Tree.c
% 
% Description : This function will delegate the removal of element in the
                tree to TNodes remove.
%
% Parameters descriptions :
%  
% name               description
% ------------------ ---------------------------------------------------------
% element            The element needing to be removed
% <return>           pointer to item found, or null if not found
******************************************************************************/

    // variable to store success or failure 
    long result = 0;

    // if tree is not empty 
    if( occupancy != 0 ){
        TNode<Whatever> thisNode( element );
        // creating local node on stack with element and pointer 
        TNode<Whatever> targetNode( root, fio);
        // delegating remove to TNodes remove 
        result = targetNode.Remove( thisNode, fio, occupancy, root, FALSE );
        // restoring data fields of element 
        element = thisNode.data;
        // decrement occupancy if element is removed
        if( result == 1 ){
            occupancy--;
        }
        // call to reset root is last tnode is removed 
        if( occupancy == 0 )
            ResetRoot();
    }
    IncrementOperation(); // call to increment operation 
    return result; // return success or failure 
}

template <class Whatever>
void TNode<Whatever> :: SetHeightAndBalance (fstream * fio,
	offset & PositionInParent) {
/******************************************************************************
% Routine Name : TNode :: SetHeightAndBalance (private)
% File :         Tree.c
% 
% Description : This function will set the height and balance of the nodes in
%               the tree. Constantly updates the height and balance whenever a
%               node is inserted or removed.
%
% Parameters descriptions :
%  
% name               description
% ------------------ ---------------------------------------------------------
% PositionInParent   The reference to the TNode position in the parent
%                    TNode used to get to the current TNode on the disk.
% fio                The filestream corresponding to the datafile where the Tree
%                    is stored on the disk. 
******************************************************************************/ 

    // variables to keep track of left and right height respectively
    long lHeight = -1;
    long rHeight = -1;
    long tempOccup = 0;

    

    // if left node exists 
    if( left != NULL ){
        // creating a local node on RTS
        TNode<Whatever> lNode( left, fio );
        // set variable to lefts height 
        lHeight = lNode.height;
    }
    else
        lHeight = -1; // otherwise reset to -1
    // set variable to rights height if right exists 
    if( right != NULL ){
        // creating a local node on RTS
        TNode<Whatever> rNode( right, fio );
        // set variable to lefts height
        rHeight = rNode.height;
    } 
    else
        rHeight = -1; // otherwise reset to -1
    // if right child is taller 
    if( rHeight >= lHeight ){
        // use right child to find height of parent 
        height = rHeight + 1;
    }
    // if left child is taller 
    else if( rHeight < lHeight ){
        // use left child to find height of parent 
        height = lHeight + 1;
    }
    // calculate balance 
    balance = lHeight - rHeight;
    // check if tree is out of balance 
    if( abs(balance) > THRESHOLD ){
        // creating a local node on RTS 
        TNode<Whatever> thisNode( data );
        // remove unbalanced node 
        Remove( thisNode, fio, tempOccup, PositionInParent , TRUE);
        TNode<Whatever> pipNode( PositionInParent, fio );
        // reinsert with PIP 
        pipNode.Insert( thisNode.data, fio, tempOccup, PositionInParent );
    }
    // write if tree is no longer out of balance
    else
        Write( fio );
}

template <class Whatever>
long Tree <Whatever> :: GetCost () {
/******************************************************************************
% Routine Name : Tree<Whatever> :: IncrementCost
% File :         Tree.c
% 
% Description :  gets the count of cost with each call to insert, lookup, or
%                remove.
% <return> :     Returns a long that contains current cost count
******************************************************************************/ 
    
    // return cost count
    return cost;
}

template <class Whatever>
long Tree <Whatever> :: GetOperation () {
/******************************************************************************
% Routine Name : Tree<Whatever> :: GetOperation
% File :         Tree.c
% 
% Description :  gets the count of operation with each reading and writing to
%                disk
% <return> :     Returns a long that contains current operation count
******************************************************************************/ 
    // return opreation count
    return operation;
}

template <class Whatever>
void Tree <Whatever> :: IncrementCost () {
/******************************************************************************
% Routine Name : Tree<Whatever> :: IncrementCost
% File :         Tree.c
% 
% Description :  Increments count of cost with each call to insert, lookup, or
%                remove.
******************************************************************************/ 

    // increment cost
    cost++;
}

template <class Whatever>
void Tree <Whatever> :: IncrementOperation () {
/******************************************************************************
% Routine Name : Tree<Whatever> :: IncrementOperation
% File :         Tree.c
% 
% Description :  Increments count of operation with each reading and writing to
%                disk
******************************************************************************/ 

    // increment operation count
    operation++;
}

template <class Whatever>
void Tree <Whatever> :: ResetRoot () {
/***************************************************************************
% Routine Name : Tree<Whatever> :: ResetRoot
% File :         Tree.c
% 
% Description :  Resets the root of the disk when the last node is removed from
%                the tree.
***************************************************************************/ 

    fio->seekp( 0, ios :: end ); // seek to the end
    // write root and occupancy
    fio->write( (const char *) &root, sizeof(root) );
    fio->write( (const char *) &occupancy, sizeof(occupancy) );
    root = fio->tellp(); // update root
}    

template <class Whatever>
unsigned long TNode<Whatever> :: Insert (Whatever & element, fstream * fio,
	long & occupancy, offset & PositionInParent) {
/******************************************************************************
% Routine Name : TNode :: Insert (private)
% File :         Tree.c
% 
% Description : This function will insert the element in the tree. If the 
%               element cannot be inserted, null will be returned.
%               If the element can be inserted, the element is inserted. 
%               Duplicate insertions will cause the existing element to be
%               deleted, and the duplicate element to take its place. If the
%               element can be isnerted true is returned.
%
% Parameters descriptions :
%  
% name               description
% ------------------ ---------------------------------------------------------
% element            The element to insert.
% PositionInParent   The reference to the TNode pointer in the parent
%                    TNode used to get to the current TNode.
% fio                The filestream corresponding to the datafile where the tree
%                    is stored on disk.
% occupancy          Reference to the occupancy of the tree.
% <return>           true or false indicating success or failure of insertion 
******************************************************************************/

    // variable to save success or failure 
    long result = 0;

    // duplicate insertion 
    if( data == element ){ 
        // replace existing data with new data 
        data = element;
        result = 1;
        // write to disk 
        Write( fio );
        return result; // retrun success 
    }
    else if( !(data < element) ){
        // go left 
        if( left == NULL ){
            // makes new node on left 
            TNode<Whatever> writeLeftNode( element, fio, occupancy );
            // update current position
            left = writeLeftNode.this_position;
        }
        // if left node already exists
        else{
            //go left again 
            TNode<Whatever> readLeftNode( left, fio );
            //recursive call to insert 
            result = readLeftNode.Insert( element, fio, occupancy, left ); 
        }
    }
    else{
        // go right 
        if( right == NULL ){
            // makes new node on the right 
            TNode<Whatever> writeRightNode( element, fio, occupancy );
            // update current position
            right = writeRightNode.this_position;
        }
        // if right node already exists 
        else{
            // keep going right 
            TNode<Whatever> readRightNode( right, fio );
            //recursive call to insert
            result = readRightNode.Insert( element, fio, occupancy, right );
        }
    }
    // update height and balance  
    SetHeightAndBalance( fio, PositionInParent );
    return result; // returns success or failure
}

template <class Whatever>
unsigned long TNode<Whatever> :: Lookup (Whatever & element, fstream * fio) 
    const {
/******************************************************************************
% Routine Name : TNode :: Lookup (private)
% File :         Tree.c
% 
% Description : This function will locate the location in the
%               tree for the element to be looked up.
%
% Parameters descriptions :
%  
% name               description
% ------------------ ----------------------------------------------------------
% element            The element needing to be looked up.
% fio                Filestream corresponding to the datafile where the tree is
%                    stored on disk.
% <return>           item found, or null if not found.
%
******************************************************************************/

    long result = 0; // variable for storing success or failure

    // if the data matches 
    if( data == element ){
        element = data;
        result = 1;
    }
    // if incoming is greater than data 
    else if( !(data < element) ){
        if( left != NULL ){
            TNode<Whatever> leftNode( left, fio );
            result = leftNode.Lookup( element, fio ); // keep moving left
        }
    }
    else{ // move right if incoming is less than data 
        if( right != NULL ){
            TNode<Whatever> rightNode( right, fio );
            result = rightNode.Lookup( element, fio ); // keep moving right 
        }
    }
    return result; // returns success or failure 
}

template <class Whatever>
unsigned long Tree<Whatever> :: Lookup (Whatever & element) const {
/******************************************************************************
% Routine Name : Tree :: Lookup (public)
% File :         Tree.c
% 
% Description : This function will delegate the look up of element in the
%               tree to TNodes lookup.
%
% Parameters descriptions :
%  
% name               description
% ------------------ ---------------------------------------------------------
% element            The element needing to be looked up
% <return>           item found, or null if not found
******************************************************************************/

    // variable to store success or failure 
    long result = 0;

    // if root node exists 
    if( occupancy != 0 ){
        TNode<Whatever> rootNode( root, fio );
        // delegate to TNodes lookup */
        result = rootNode.Lookup( element, fio );
    }

    IncrementOperation(); // call to increment operation count 
    return result; // returns success or failure 
}

template <class Whatever>
void TNode<Whatever> :: Read (const offset & position, fstream * fio) {
/***************************************************************************
% Routine Name : TNode<Whatever> :: Read
% File :         Tree.c
% 
% Description :  TNode method to read from disk.
% Parameters descriptions :
%  
% name               description
% ------------------ ---------------------------------------------------------
% position            The offset in the datafile corresponding to the position
%                     of the TNode we wish to read into memory.
% fio                 The filestream corresponding to the datafile where the
%                     tree is stored on disk.   
***************************************************************************/ 

    Tree <Whatever> :: IncrementCost(); //call to increment cost
    fio->seekg( position ); // seek the position to read from
    fio->read( (char *) this, sizeof(TNode<Whatever>) ); // read node
    //debug message
    if(Tree<Whatever> :: debug_on)
        cerr << COST_READ << (const char *) data << "]" << endl;
    
}

template <class Whatever>
TNode<Whatever> :: TNode (const offset & position, fstream * fio) {
/***************************************************************************
% Routine Name : TNode<Whatever> :: TNode
% File :         Tree.c
% 
% Description :  TNode constructor to read from disk.
% Parameters descriptions :
%  
% name               description
% ------------------ ---------------------------------------------------------
% position            The offset in the datafile corresponding to the position
%                     of the TNode we wish to read into memory.
% fio                 The filestream corresponding to the datafile where the
%                     tree is stored on disk.
***************************************************************************/
    
    // call to read 
    Read( position, fio );
}

template <class Whatever>
TNode<Whatever> :: TNode (Whatever & element, fstream * fio, long & occupancy): 
			data (element), height (0), balance (0), left (0), 
			right (0) {
/***************************************************************************
% Routine Name : TNode<Whatever> :: TNode
% File :         Tree.c
% 
% Description :  TNode constructor to write to disk. Allocates all the data
%                fields of what is being written
% Parameters descriptions :
%  
% name               description
% ------------------ ---------------------------------------------------------
% element             Element to be written to the disk.
% occupancy           Occupancy of the tree.
% fio                 The filestream corresponding to the datafile where the
%                     tree is stored on disk.
***************************************************************************/

    // seek to end
    fio->seekp( 0, ios :: end );
    // update current position
    this_position = fio->tellp();
    
    // call to write to disk
    Write( fio );
    // increase occupancy
    occupancy++;
}

template <class Whatever>
void TNode<Whatever> :: Write (fstream * fio) const {
/***************************************************************************
% Routine Name : TNode<Whatever> :: Write
% File :         Tree.c
% 
% Description :  writes node to disk.
% Parameters descriptions :
%  
% name               description
% ------------------ ---------------------------------------------------------
% fio                 The filestream corresponding to the datafile where the
%                     tree is stored on disk. 
***************************************************************************/
    
    // call to increment cost
    Tree <Whatever> :: IncrementCost();
    // seek to current position
    fio->seekp( this_position );
    // write to disk
    fio->write( (const char *) this, sizeof(TNode<Whatever>) );
    // debug message
    if(Tree<Whatever> :: debug_on)
        cerr << COST_WRITE << (const char *) data << "]" << endl;
}

template<class Whatever>
void Tree<Whatever> :: Set_Debug_On(){
/******************************************************************************
% Routine Name : Tree<Whatever> :: Set_Debug_On (public)
% File :         Tree.c
% 
% Description :  This function sets debug mode on
******************************************************************************/
    debug_on = 1; // turns debug messages on 
}

template<class Whatever>
void Tree<Whatever> :: Set_Debug_Off(){
/******************************************************************************
% Routine Name : Tree<Whatever> :: Set_Debug_Off (public)
% File :         Tree.c
% 
% Description :  This function sets debug mode off
******************************************************************************/
    debug_on = 0; // turns off debug messages 
}

template <class Whatever>
Tree<Whatever> :: Tree (const char * datafile) :
	fio (new fstream (datafile, ios :: out | ios :: in)) {
/***************************************************************************
% Routine Name : Tree :: Tree  (public)
% File :         Tree.c
% 
% Description :  allocates memory associated with the Tree.  It
%                will also allocate all the memory of the elements within
%                the tree.
% Parameters descriptions :
%  
% name               description
% ------------------ ---------------------------------------------------------
% fio                 The filestream corresponding to the datafile where the
%                     tree is stored on disk.    
***************************************************************************/
    offset begin, ending; // declare offsets
    tree_count = 1; // set tree count

    // debug messages for user 
    if( debug_on)
        cerr << TREE << tree_count << ALLOCATE;
    fio->seekp( 0, ios :: beg ); // seek to beg
    begin = fio->tellp(); // set begin offset
    fio->seekp( 0, ios :: end );  // seek to end
    ending = fio->tellp(); // set end offset
    // check to see if tree has not already been allocated 
    if( begin == ending ){
        root = occupancy = 0; // set root and occupancy
        fio->seekp( 0, ios :: beg );  //seek to beg
        // write root and occupancy
        fio->write( (const char *) &root, sizeof(root) );
        fio->write( (const char *) &occupancy, sizeof(occupancy) );
        root = fio->tellp(); // initialize root to p pointer location
    }
    // if it has
    else{
        fio->seekg( 0, ios :: beg ); // seek to beg
        // write root and occupancy
        fio->read( (char *) &root, sizeof(root) );
        fio->read( (char *) &occupancy, sizeof(occupancy) );
    }

}

template <class Whatever>
Tree<Whatever> :: ~Tree (void)
/***************************************************************************
% Routine Name : Tree :: ~Tree  (public)
% File :         Tree.c
% 
% Description :  deallocates memory associated with the Tree.  It
%                will also delete all the memory of the elements within
%                the table.
***************************************************************************/

{
	// debug message 
    if( debug_on)
        cerr << TREE << tree_count << DEALLOCATE;
    fio->seekp( 0, ios :: beg ); // seek to beginning
    // write root and occupancy 
    fio->write( (const char *) &root, sizeof(root) );
    fio->write( (const char *) &occupancy, sizeof(occupancy) );
    delete( fio ); // deletes reference to disk 
}	// end: ~Tree 

template <class Whatever>
ostream & operator << (ostream & stream, const TNode<Whatever> & nnn) {
	stream << "at height:  :" << nnn.height << " with balance:  "
		<< nnn.balance << "  ";
	return stream << nnn.data << "\n";
}

template <class Whatever>
ostream & Tree<Whatever> :: Write (ostream & stream) const
/***************************************************************************
% Routine Name : Tree :: Write (public)
% File :         Tree.c
% 
% Description : This funtion will output the contents of the Tree table
%               to the stream specificed by the caller.  The stream could be
%               cerr, cout, or any other valid stream.
%
% Parameters descriptions :
% 
% name               description
% ------------------ ------------------------------------------------------
% stream             A reference to the output stream.
% <return>           A reference to the output stream.
***************************************************************************/

{
        long old_cost = cost;

	stream << "Tree " << tree_count << ":\n"
		<< "occupancy is " << occupancy << " elements.\n";

	fio->seekg (0, ios :: end);
	offset end = fio->tellg ();

	// check for new file
	if (root != end) {
		TNode<Whatever> readRootNode (root, fio);
		readRootNode.Write_AllTNodes (stream, fio);
	}

        // ignore cost when displaying nodes to users
        cost = old_cost;

	return stream;
}

template <class Whatever>
ostream & TNode<Whatever> ::
Write_AllTNodes (ostream & stream, fstream * fio) const {
	if (left) {
		TNode<Whatever> readLeftNode (left, fio);
		readLeftNode.Write_AllTNodes (stream, fio);
	}
	stream << *this;
	if (right) {
		TNode<Whatever> readRightNode (right, fio);
		readRightNode.Write_AllTNodes (stream, fio);
	}

	return stream;
}

