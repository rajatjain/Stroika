#pragma once

#include <algorithm>
#include <stack>

#if qDebug
	#include <iostream>
    #include "../Shared/Headers/ContainerValidation.h"
#endif


namespace   ADT {
    namespace   BinaryTree {


template <typename KEY, typename VALUE, typename TRAITS>
SplayTree<KEY,VALUE,TRAITS>::SplayTree () :
	fHead (nullptr),
	fLength (0)
  #if qKeepADTStatistics
      ,fCompares (0)
      ,fRotations (0)
  #endif
{
}

template <typename KEY, typename VALUE, typename TRAITS>
SplayTree<KEY,VALUE,TRAITS>::SplayTree (const SplayTree& t) :
	fHead (nullptr),
	fLength (t.fLength)
  #if qKeepADTStatistics
      ,fCompares (t.fCompares)
      ,fRotations (t.fRotations)
  #endif
{
	fHead = DuplicateBranch (t.fHead);
}

template <typename KEY, typename VALUE, typename TRAITS>
SplayTree<KEY,VALUE,TRAITS>& SplayTree<KEY,VALUE,TRAITS>::operator= (const SplayTree& t)
{
	RemoveAll ();
	fLength = t.fLength;
	fHead = DuplicateBranch (t.fHead);

	return *this;
}


template <typename KEY, typename VALUE, typename TRAITS>
SplayTree<KEY,VALUE,TRAITS>::~SplayTree ()
{
	RemoveAll ();
}

template <typename KEY, typename VALUE, typename TRAITS>
bool	SplayTree<KEY,VALUE,TRAITS>::Find (const KeyType& key, ValueType* val)
{
	int	comparisonResult;
	size_t	height;
	Node* n = FindNode (key, &comparisonResult, &height);
	if (n != nullptr) {
		if (comparisonResult == 0) {
			if (val != nullptr) {
				*val = n->fEntry.GetValue ();
			}
			Splay (n, height, bool ((height > 8) and height > GetLength ()/10));

			return true;
		}
		else {
			// maybe splay the one we found? Seems to work poorly in practice
			Splay (n, height, bool ((height > 8) and height > GetLength ()/10));
		}
	}
	return false;
}


template <typename KEY, typename VALUE, typename TRAITS>
typename SplayTree<KEY,VALUE,TRAITS>::Node* SplayTree<KEY,VALUE,TRAITS>::Rotate (Node* n, Direction rotateDir)
{
	RequireNotNull (n);

	#if qKeepADTStatistics
		++fRotations;
	#endif

    Direction otherDir = Node::OtherDir (rotateDir);
	Node* newTop = n->GetChild (otherDir);
	RequireNotNull (newTop);

    if (n->GetParent () == nullptr) {
		Assert (n == fHead);
		fHead = newTop;
		fHead->SetParent (nullptr);
	}
	else {
	    n->GetParent ()->SetChild (Node::GetChildDir (n), newTop);
    }

    n->SetChild (otherDir, newTop->GetChild (rotateDir));
    newTop->SetChild (rotateDir, n);
	return newTop;
}


template <typename KEY, typename VALUE, typename TRAITS>
const std::vector<size_t>&	SplayTree<KEY,VALUE,TRAITS>::GetHeightWeights (SplayType st)
{
	switch (st) {
		case eAlwaysSplay:				return sAlwaysSplayDistribution;
		case eUniformDistribution:		return sUniformDistribution;
		case eNormalDistribution:		return sNormalDistribution;
		case eZipfDistribution:			return sZipfDistribution;
		case eCustomSplayType:			return sCustomSplayTypeDistribution;
		default:	AssertNotReached ();
	}
	AssertNotReached (); return sCustomSplayTypeDistribution;
}

template <typename KEY, typename VALUE, typename TRAITS>
void	SplayTree<KEY,VALUE,TRAITS>::SetCustomHeightWeights (const std::vector<size_t>& newHeightWeights)
{
	sCustomSplayTypeDistribution = newHeightWeights;
}

template <typename KEY, typename VALUE, typename TRAITS>
void    SplayTree<KEY,VALUE,TRAITS>::Prioritize (Node* n)
{
    RequireNotNull (n);
    Splay (n, 0, true);
}

template <typename KEY, typename VALUE, typename TRAITS>
void SplayTree<KEY,VALUE,TRAITS>::Splay (Node* n, size_t nodeHeight, bool forced)
{
	RequireNotNull (n);

	if (TRAITS::kSplayType == eNeverSplay) {
		return;
	}
	else if (TRAITS::kSplayType == eAlwaysSplay) {
		forced = true;
	}

//	static	std::tr1::uniform_int<size_t> sDist (1, 10000);

	const std::vector<size_t>&	kHeightBonus = GetHeightWeights (TRAITS::kSplayType);
//	size_t	dieRoll = (forced) ? 1 : std::tr1::uniform_int<size_t> (1, 10000) (GetEngine ());
//	size_t	dieRoll = (forced) ? 1 : rand () % 10000;

	/*
		Move upwards in the tree. In classic splay tree, move all the way to the top.
		Splay trees do a slightly more complicated action than simple rotations. Whenever possible it does two rotations
		at once: rotating the parent node and the grandparent node, making the node the new grandparent.
		In cases where the node being played is on the same side of its parent as it's parent is to its grandparent,
		if first rotates the grandparent, then the parent. If on opposite sides, it does the normal rotation sequence (parent, then grandparent);
		It only does a single rotation if the node has no grandparent (its parent is the head)
	 */
	while (n->GetParent () != nullptr) {
		Assert (n->GetParent ()->GetChild (kLeft) == n or n->GetParent ()->GetChild (kRight) == n);

		Node*	ancestor = n->GetParent ()->GetParent ();
		if (ancestor == nullptr) {
			if (not forced) {
				Assert (nodeHeight > 0);
				--nodeHeight;
				if (nodeHeight < kHeightBonus.size ()) {
 				    size_t	cutoff = kHeightBonus[nodeHeight];
 				    if ((cutoff == 0) or (size_t (rand () % 10000) > cutoff)) {
                         return;
                    }
				}
			}
			Rotate (n->GetParent (), Node::OtherDir (Node::GetChildDir (n)));
		}
		else {
			if (not forced) {
				Assert (nodeHeight > 1);
				nodeHeight -= 2;
				if (nodeHeight < kHeightBonus.size ()) {
 				    size_t	cutoff = kHeightBonus[nodeHeight];
 				    if ((cutoff == 0) or (size_t (rand () % 10000) > cutoff)) {
                         return;
                    }
				}
			}
			Node*	parent = n->GetParent ();
			if ((parent->GetChild (kLeft) == n and ancestor->GetChild (kLeft) == parent) or (parent->GetChild (kRight) == n and ancestor->GetChild (kRight) == parent)) {
				// zig-zig
				Rotate (ancestor, Node::OtherDir (Node::GetChildDir (n)));
				Rotate (parent, Node::OtherDir (Node::GetChildDir (n)));
			}
			else {
				// zig-zag
				Rotate (parent, Node::OtherDir (Node::GetChildDir (n)));
				Assert (ancestor->GetChild (kLeft) == n or ancestor->GetChild (kRight) == n);
				Rotate (ancestor, Node::OtherDir (Node::GetChildDir (n)));
			}
		}

	}
	Ensure ((n->GetParent () == nullptr) == (fHead == n));
	Ensure ((n->GetParent () == nullptr) or (n->GetParent ()->GetChild (kLeft) == n) or (n->GetParent ()->GetChild (kRight) == n));
}

template <typename KEY, typename VALUE, typename TRAITS>
size_t SplayTree<KEY,VALUE,TRAITS>::ForceToBottom (Node* n)
{
	RequireNotNull (n);
	size_t rotations = 0;
	while (n->GetChild (kLeft) != nullptr or n->GetChild (kRight) != nullptr) {
	   Direction rotDir = (n->GetChild (kLeft) == nullptr) or (n->GetChild (kRight) != nullptr and FlipCoin ()) ? kLeft : kRight;
		Rotate (n, rotDir);
		++rotations;
    }


	Ensure (n->GetChild (kLeft) == nullptr and n->GetChild (kRight) == nullptr);
	Ensure (fHead->GetParent () == nullptr);

	return rotations;
}

template <typename KEY, typename VALUE, typename TRAITS>
void	SplayTree<KEY,VALUE,TRAITS>::Add (const KeyType& key, const ValueType& val)
{
 	Node* n = new Node (key, val);
 	try {
        AddNode (n);
	}
	catch (const DuplicateAddException& exp) {
	    delete n;
	    throw;
    }
}

template <typename KEY, typename VALUE, typename TRAITS>
void	SplayTree<KEY,VALUE,TRAITS>::Add (const KeyType& keyAndValue)
{
    Add (keyAndValue, keyAndValue);
}


template <typename KEY, typename VALUE, typename TRAITS>
void	SplayTree<KEY,VALUE,TRAITS>::AddNode (Node* n)
{
	RequireNotNull (n);

	int	comp;
	size_t height;
	Node* nearest =  FindNode (n->fEntry.GetKey (), &comp, &height);
	if (nearest == nullptr) {
		Assert (fHead == nullptr);
		fHead = n;
	}
	else {
		if (comp == 0) {
            if (TRAITS::kPolicy & ADT::eDuplicateAddThrowException) {
                throw DuplicateAddException ();
            }
			if (FlipCoin ()) {
			    n->SetChild (kLeft, nearest->GetChild (kLeft));
				nearest->SetChild (kLeft, n);
			}
			else {
			    n->SetChild (kRight, nearest->GetChild (kRight));
				nearest->SetChild (kRight, n);
			}
			height++;
		}
		else if (comp < 0) {
			Assert (nearest->GetChild (kLeft) == nullptr);
			nearest->SetChild (kLeft, n);
		}
		else {
			Assert (nearest->GetChild (kRight) == nullptr);
			nearest->SetChild (kRight, n);
		}

		Splay (n, height, bool ((height > 8) and height > GetLength ()/10));
	}

	fLength++;
}



template <typename KEY, typename VALUE, typename TRAITS>
void	SplayTree<KEY,VALUE,TRAITS>::Remove (const KeyType& key)
{
	int	comp;
	Node* n =  FindNode (key, &comp, nullptr);

	if ((n == nullptr) or (comp != 0)) {
		if (not (TRAITS::kPolicy & ADT::eInvalidRemoveIgnored)) {
		    throw InvalidRemovalException ();
		}
	}
	else {
		Assert (n->fEntry.GetKey () == key);
		RemoveNode (n);
	}
}

template <typename KEY, typename VALUE, typename TRAITS>
void	SplayTree<KEY,VALUE,TRAITS>::SwapNodes (Node* parent, Node* child)
{
	RequireNotNull (parent);

	Node* ancestor = parent->GetParent ();
	if (ancestor == nullptr) {
		Assert (fHead == parent);
		fHead = child;
		if (fHead != nullptr) {
            fHead->SetParent (nullptr);
		}
	}
	else {
	    ancestor->SetChild (Node::GetChildDir (parent), child);
	}
}

template <typename KEY, typename VALUE, typename TRAITS>
void	SplayTree<KEY,VALUE,TRAITS>::RemoveNode (Node* n)
{
	RequireNotNull (n);

	Splay (n, 0, true);
	Assert (n == fHead);

	if (n->GetChild (kLeft) == nullptr and n->GetChild (kRight) == nullptr) {
	    SwapNodes (n, nullptr);
	}
	else if (n->GetChild (kLeft) == nullptr) {
		SwapNodes (n, n->GetChild (kRight));
	}
	else if (n->GetChild (kRight) == nullptr) {
		SwapNodes (n, n->GetChild (kLeft));
	}
	else {
	    Node* minNode = Node::GetFirst (n->GetChild (kRight));

		AssertNotNull (minNode);
		if (minNode->GetParent () != n) {
			SwapNodes (minNode, minNode->GetChild (kRight));
			minNode->SetChild (kRight, n->GetChild (kRight));
		}
		SwapNodes (n, minNode);
		minNode->SetChild (kLeft, n->GetChild (kLeft));
	}

	delete n;
	--fLength;
}

template <typename KEY, typename VALUE, typename TRAITS>
void	SplayTree<KEY,VALUE,TRAITS>::RemoveAll ()
{
	// iterate rather than natural tail recursive version because splay trees get deep
	std::stack<Node*> nodes;
	if (fHead != nullptr) {
		nodes.push(fHead);
		while (not nodes.empty ()) {
			Node* curNode = nodes.top (); nodes.pop ();

			AssertNotNull (curNode);
			if (curNode->GetChild (kLeft) != nullptr) {
				nodes.push (curNode->GetChild (kLeft));
			}
			if (curNode->GetChild (kRight) != nullptr) {
				nodes.push (curNode->GetChild (kRight));
			}
			delete curNode;
		}

		fHead = nullptr;
	}
	fLength = 0;
}

template <typename KEY, typename VALUE, typename TRAITS>
size_t	SplayTree<KEY,VALUE,TRAITS>::GetLength () const
{
	Assert ((fLength == 0) == (fHead == nullptr));
	return fLength;
}


template <typename KEY, typename VALUE, typename TRAITS>
typename SplayTree<KEY,VALUE,TRAITS>::Node*	SplayTree<KEY,VALUE,TRAITS>::FindNode (const KeyType& key, int* comparisonResult, size_t* height)  const
{
	RequireNotNull (comparisonResult);

	Node*	n = fHead;
	Node*	nearest = n;
	if (height != nullptr) {
		*height = 0;
	}
	*comparisonResult = 0;
	while (n != nullptr) {
		#if qKeepADTStatistics
			const_cast<SplayTree<KEY,VALUE,TRAITS> *> (this)->fCompares++;
		#endif
		if (height != nullptr) {
			*height += 1;
		}
		nearest = n;
		*comparisonResult = TRAITS::Comparer::Compare (key, n->fEntry.GetKey ());
		if (*comparisonResult == 0) {
			return n;
		}
		n = (*comparisonResult < 0) ? n->GetChild (kLeft) : n->GetChild (kRight);
	}
	return nearest;
}

template <typename KEY, typename VALUE, typename TRAITS>
typename	SplayTree<KEY,VALUE,TRAITS>::Node*	SplayTree<KEY,VALUE,TRAITS>::GetFirst (size_t* height) const
{
	Node* n = fHead;
	if (height != nullptr) {
		*height = 0;
	}
	while (n != nullptr and n->GetChild (kLeft) != nullptr) {
		if (height != nullptr) {
			*height += 1;
		}
		n = n->GetChild (kLeft);
	}
	return n;
}

template <typename KEY, typename VALUE, typename TRAITS>
typename	SplayTree<KEY,VALUE,TRAITS>::Node*	SplayTree<KEY,VALUE,TRAITS>::GetLast (size_t* height) const
{
	Node* n = fHead;
	if (height != nullptr) {
		*height = 0;
	}
	while (n != nullptr and n->GetChild (kRight) != nullptr) {
		if (height != nullptr) {
			*height += 1;
		}
		n = n->GetChild (kRight);
	}
	return n;
}

template <typename KEY, typename VALUE, typename TRAITS>
SplayType	SplayTree<KEY,VALUE,TRAITS>::GetSplayType ()
{
	return TRAITS::kSplayType;
}

template <typename KEY, typename VALUE, typename TRAITS>
typename SplayTree<KEY,VALUE,TRAITS>::Node*	SplayTree<KEY,VALUE,TRAITS>::DuplicateBranch (Node* n)
{
	// sadly, SplayTrees get very deep, so they hate recursion

	std::stack<Node*> nodes;
	std::stack<Node*> parents;
	std::stack<bool> childIsLeft;

	Node*	newTop = nullptr;

	nodes.push (n);
	parents.push (nullptr);
	childIsLeft.push (true);
	while (not nodes.empty ()) {
		Node* branchTop = nodes.top  ();	nodes.pop ();
		Node* newParent = parents.top  ();	parents.pop ();
		bool isLeft = childIsLeft.top  ();	childIsLeft.pop ();
		if (branchTop != nullptr) {
			Node*	newNode = new Node (*branchTop);

			if (newParent == nullptr) {
				Assert (newTop == nullptr);
				newTop = newNode;
			}
			else {
				if (isLeft) {
					newParent->SetChild (kLeft, newNode);
				}
				else {
					newParent->SetChild (kRight, newNode);
				}
			}
			if (branchTop->GetChild (kLeft) != nullptr) {
				nodes.push (branchTop->GetChild (kLeft));
				parents.push (newNode);
				childIsLeft.push (true);
			}
			if (branchTop->GetChild (kRight) != nullptr) {
				nodes.push (branchTop->GetChild (kRight));
				parents.push (newNode);
				childIsLeft.push (false);
			}
		}
	}
	return newTop;
}

template <typename KEY, typename VALUE, typename TRAITS>
typename SplayTree<KEY,VALUE,TRAITS>::Node*   SplayTree<KEY,VALUE,TRAITS>::Node::RebalanceBranch (Node* oldTop, size_t length)
{
    if (oldTop == nullptr) {
        Assert (length == 0);
        return nullptr;
    }

    // technique discussed in Galperin and Rivest, see http://www.akira.ruc.dk/~keld/teaching/algoritmedesign_f07/Artikler/03/Galperin93.pdf


    // turns the tree into a linked list (by fRight pointers) in sort order
    // currently slightly slower than building an array, but no mallocs
    // conceptually, flatten is just plain old iteration, but tree iteration is naturally recursive, unfortunately
	std::function<Node*(Node*,Node*)>	Flatten = [&Flatten] (Node* x, Node* y)
	{
        x->fParent = nullptr;

        Node* cRight = x->fChildren[kRight];
        Node* cLeft = x->fChildren[kLeft];

        x->SetChild (kLeft, nullptr);

        x->fChildren[kRight] = (cRight == nullptr) ? y : Flatten (cRight, y);
        return (cLeft == nullptr) ? x :  Flatten (cLeft, x);
	};

    // takes the results of flatten (ordered linked list, by right child pointers) and constructs the proper
    // balanced tree out of it
	std::function<Node*(size_t,Node*)>	RebuildBranch = [&RebuildBranch] (size_t n, Node* x)
	{
	    RequireNotNull (x);
	    Require (n > 0);

        size_t  tmp = (n-1)/2;
        Node* r = (tmp == 0) ? x : RebuildBranch (tmp, x);
        AssertNotNull (r);

        tmp = n/2;
        Node* s = (tmp == 0) ? r->GetChild (kRight) : RebuildBranch (tmp, r->GetChild (kRight));
        AssertNotNull (s);

        r->SetChild (kRight, s->GetChild (kLeft));
        s->SetChild (kLeft, r);
        return s;
	};

    Node*   oldParent = oldTop->GetParent ();
    Direction   oldDir = GetChildDir (oldTop);

	Node    dummy = *oldTop;
    Node* result = RebuildBranch (length, Flatten (oldTop, &dummy));
    result = result->GetChild (kLeft);  // original result was our dummy last
    AssertNotNull (result);

    SetChild_Safe (oldParent, result, oldDir);
    return result;
}

template <typename KEY, typename VALUE, typename TRAITS>
void	SplayTree<KEY,VALUE,TRAITS>::ReBalance ()
{
    fHead = Node::RebalanceBranch (fHead, fLength);
}


template <typename KEY, typename VALUE, typename TRAITS>
SplayTree<KEY,VALUE,TRAITS>::Node::Node (const KeyType& key, const ValueType& val)	:
	fEntry (key, val),
	fParent (nullptr)
{
    fChildren[kLeft] = nullptr;
    fChildren[kRight] = nullptr;
}

template <typename KEY, typename VALUE, typename TRAITS>
SplayTree<KEY,VALUE,TRAITS>::Node::Node (const Node& n)	:
	fEntry (n.fEntry),
	fParent (nullptr)
{
    fChildren[kLeft] = nullptr;
    fChildren[kRight] = nullptr;
}


template <typename KEY, typename VALUE, typename TRAITS>
typename SplayTree<KEY,VALUE,TRAITS>::Node*   SplayTree<KEY,VALUE,TRAITS>::Node::GetParent () const
{
    return fParent;
}

template <typename KEY, typename VALUE, typename TRAITS>
void    SplayTree<KEY,VALUE,TRAITS>::Node::SetParent (Node* p)
{
    fParent = p;
}

 template <typename KEY, typename VALUE, typename TRAITS>
 typename SplayTree<KEY,VALUE,TRAITS>::Node::Node*   SplayTree<KEY,VALUE,TRAITS>::Node::GetChild (Direction direction)
 {
    Require (direction == kLeft or direction == kRight);
    return (fChildren[direction]);
}

template <typename KEY, typename VALUE, typename TRAITS>
 void   SplayTree<KEY,VALUE,TRAITS>::Node::SetChild (Direction direction, Node* n)
{
    Require (direction == kLeft or direction == kRight);
    fChildren[direction] = n;
    if (n != nullptr) {
        n->fParent = this;
    }
}
template <typename KEY, typename VALUE, typename TRAITS>
bool    SplayTree<KEY,VALUE,TRAITS>::Node::IsChild (Direction direction)
{
    Require (direction == kLeft or direction == kRight);
    return (fParent != nullptr and fParent->GetChild (direction) == this);
}

template <typename KEY, typename VALUE, typename TRAITS>
Direction     SplayTree<KEY,VALUE,TRAITS>::Node::GetChildDir (Node* n)
{
    if (n != nullptr and n->GetParent () != nullptr) {
        if (n == n->GetParent ()->GetChild (kLeft)) {
            return kLeft;
        }
        if (n == n->GetParent ()->GetChild (kRight)) {
            return kRight;
        }
    }
    return kBadDir;
}

template <typename KEY, typename VALUE, typename TRAITS>
Direction    SplayTree<KEY,VALUE,TRAITS>::Node::OtherDir (Direction dir)
{
    Require (dir == kLeft or dir == kRight);
    return ((dir == kLeft) ? kRight : kLeft);
}

template <typename KEY, typename VALUE, typename TRAITS>
void    SplayTree<KEY,VALUE,TRAITS>::Node::SetChild_Safe (Node* parent, Node* n, Direction d)
{
   Require (parent == nullptr or d == kLeft or d == kRight);
   if (parent == nullptr) {
       if (n != nullptr) {
           n->fParent = nullptr;
       }
   }
   else {
       parent->SetChild (d, n);
   }
}

template <typename KEY, typename VALUE, typename TRAITS>
typename SplayTree<KEY,VALUE,TRAITS>::Node*	SplayTree<KEY,VALUE,TRAITS>::Node::GetFirst (Node* n)
{
 	while (n != nullptr and n->GetChild (kLeft) != nullptr) {
		n = n->GetChild (kLeft);
	}
	return n;
}

template <typename KEY, typename VALUE, typename TRAITS>
typename SplayTree<KEY,VALUE,TRAITS>::Node*	SplayTree<KEY,VALUE,TRAITS>::Node::GetLast (Node* n)
{
  	while (n != nullptr and n->GetChild (kRight) != nullptr) {
		n = n->GetChild (kRight);
	}
	return n;
}

#if qDebug

template <typename KEY, typename VALUE, typename TRAITS>
void	SplayTree<KEY,VALUE,TRAITS>::ValidateBranch (Node* n, size_t& count)
{
	RequireNotNull (n);
	++count;
	if (n->GetChild (kLeft) != nullptr) {
		Assert (TRAITS::Comparer::Compare (n->fEntry.GetKey (), n->GetChild (kLeft)->fEntry.GetKey ()) >= 0);
		Assert (n->GetChild (kLeft)->GetParent () == n);
		ValidateBranch (n->GetChild (kLeft), count);
	}
	if (n->GetChild (kRight) != nullptr) {
		// we cannot do strict < 0, because rotations can put on either side
		Assert (TRAITS::Comparer::Compare (n->fEntry.GetKey (), n->GetChild (kRight)->fEntry.GetKey ()) <= 0);
		Assert (n->GetChild (kRight)->GetParent () == n);
		ValidateBranch (n->GetChild (kRight), count);
	}
}

template <typename KEY, typename VALUE, typename TRAITS>
void	SplayTree<KEY,VALUE,TRAITS>::ValidateAll () const
{
	size_t	count = 0;

	if (fHead != nullptr) {
		ValidateBranch (fHead, count);
	}
	Assert (count == fLength);
}

template <typename KEY, typename VALUE, typename TRAITS>
void	SplayTree<KEY,VALUE,TRAITS>::ListAll () const
{
	std::function<void(Node*)>	ListNode = [&ListNode] (Node* n)
	{
		if (n->GetChild (kLeft) != nullptr) {
			ListNode (n->GetChild (kLeft));
		}
        std::cout << "(" << n->fEntry.GetKey () << ")";
		if (n->GetChild (kRight) != nullptr) {
			ListNode (n->GetChild (kRight));
		}
	};

    std::cout << "[";
	if (fHead != nullptr) {
		ListNode (fHead);
	}
    std::cout << "]" << std::endl;
}


#endif

#if qKeepADTStatistics
template <typename KEY, typename VALUE, typename TRAITS>
size_t	SplayTree<KEY,VALUE,TRAITS>::CalcHeight (size_t* totalHeight) const
{
	size_t maxHeight = 0;

	std::stack<Node*> nodes;
	std::stack<size_t> heights;
	nodes.push (fHead);
	heights.push (0);
	while (not nodes.empty ()) {
		Node* curNode = nodes.top  ();
		nodes.pop ();
		size_t height = heights.top  ();
		heights.pop ();
		if (curNode == nullptr) {
			if (totalHeight != nullptr) {
				*totalHeight += height;
			}
			maxHeight = std::max (maxHeight, height);
		}
		else {
			heights.push (height+1);
			nodes.push (curNode->GetChild (kLeft));
			heights.push (height+1);
			nodes.push (curNode->GetChild (kRight));
		}
	}
	return maxHeight;
}
#endif

const size_t kAlwaysWeights[] = {10000};
template <typename KEY, typename VALUE, typename TRAITS>
std::vector<size_t>	SplayTree<KEY,VALUE,TRAITS>::sAlwaysSplayDistribution (kAlwaysWeights, kAlwaysWeights + sizeof(kAlwaysWeights) / sizeof(kAlwaysWeights[0]));


const size_t kUniformWeights[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  };
template <typename KEY, typename VALUE, typename TRAITS>
std::vector<size_t>	SplayTree<KEY,VALUE,TRAITS>::sUniformDistribution (kUniformWeights, kUniformWeights + sizeof(kUniformWeights) / sizeof(kUniformWeights[0]));

//const size_t kNormalWeights[] ={0, 0, 100, 100, 250, 250, 250, 250, 250, 250, 250};	//30.1401/30.0162/21.5254
const size_t kNormalWeights[] = {2, 2, 6, 7, 7, 7, 7, 7, 3, 3, 3, 3, 19, 42, 47, 1463, 2000 };  //27.50
template <typename KEY, typename VALUE, typename TRAITS>
std::vector<size_t>	SplayTree<KEY,VALUE,TRAITS>::sNormalDistribution (kNormalWeights, kNormalWeights + sizeof(kNormalWeights) / sizeof(kNormalWeights[0]));

//const size_t kZifpWeights[] = {0, 5, 15, 30, 30, 60, 60, 125, 125, 250, 250};
const size_t kZifpWeights[] = {0, 0, 0, 0, 0, 1, 1, 1, 1, 4, 7, 11, 24, 108, 202, 561, 1711  };
template <typename KEY, typename VALUE, typename TRAITS>
std::vector<size_t>	SplayTree<KEY,VALUE,TRAITS>::sZipfDistribution (kZifpWeights, kZifpWeights + sizeof(kZifpWeights) / sizeof(kZifpWeights[0]));

template <typename KEY, typename VALUE, typename TRAITS>
std::vector<size_t>	SplayTree<KEY,VALUE,TRAITS>::sCustomSplayTypeDistribution (SplayTree<KEY,VALUE,TRAITS>::sUniformDistribution);


template <typename KEY, typename VALUE, typename TRAITS>
typename SplayTree<KEY,VALUE,TRAITS>::Engine&	SplayTree<KEY,VALUE,TRAITS>::GetEngine ()
{
    static	std::mt19937	sEngine;
	static	bool	sFirstTime = true;
	if (sFirstTime) {
		sFirstTime = false;
        sEngine.seed (static_cast<unsigned int> (time (NULL)));
	}
	return sEngine;
}



#if qDebug


template <typename KEYTYPE>
void    SplayTreeValidationSuite (size_t testDataLength, bool verbose)
{
    TestTitle   tt ("SplayTree Validation", 0, verbose);

    RunSuite<SplayTree<KEYTYPE, size_t>, KEYTYPE> (testDataLength, verbose, 1);

    typedef SplayTree<int, int, SplayTraits<
        KeyValue<int,int>,
        ADT::DefaultComp<int>,
        ADT::eDuplicateAddThrowException,
        SplayType::eDefaultSplayType> >  NoDupAddTree;

    DuplicateAddBehaviorTest<SplayTree<int, int>, NoDupAddTree> (testDataLength, verbose, 1);

   typedef    SplayTree<int, int, SplayTraits<
        KeyValue<int,int>,
        ADT::DefaultComp<int>,
        ADT::eInvalidRemoveIgnored,
        SplayType::eDefaultSplayType> > InvalidRemoveIgnoredTree;
    InvalidRemoveBehaviorTest<SplayTree<int, int>, InvalidRemoveIgnoredTree> (verbose, 1);

    typedef    SplayTree<string, string, SplayTraits<
        SharedStringKeyValue,
        CaseInsensitiveCompare,
        ADT::eDefaultPolicy,
        SplayType::eDefaultSplayType> > SharedCaseInsensitiveString;
    StringTraitOverrideTest<SharedCaseInsensitiveString> (verbose, 1);

    typedef SplayTree<HashKey<string>, string>   HashedString;
    HashedStringTest<HashedString> (verbose, 1);
}

#endif

    }   // namespace BinaryTree
}   // namespace ADT

