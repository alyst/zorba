/*
 * Copyright 2006-2008 The FLWOR Foundation.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stack>

#include <atlconv.h>
#include <atlbase.h>

#include "zorbaerrors/error_manager.h"
#include "zorbaerrors/Assert.h"
#include "zorbatypes/URI.h"

#include "store/api/copymode.h"
#include "store/naive/atomic_items.h"
#include "store/naive/msdom_addon/msdom_node_items.h"
#include "store/naive/node_iterators.h"
#include "store/naive/simple_store.h"
#include "store/naive/basic_item_factory.h"
#include "store/naive/qname_pool.h"
#include "store/naive/store_defs.h"
#include "store/naive/nsbindings.h"
#include "store/naive/item_iterator.h"
#include "store/naive/dataguide.h"
#include "store/naive/store_defs.h"

#include "store/naive/msdom_addon/import_msdom.h"


#ifndef NDEBUG

#define NODE_TRACE(level, msg)                \
{                                             \
  if (level <= GET_STORE().getTraceLevel())   \
    std::cout << msg << std::endl;            \
}

#define NODE_TRACE1(msg) NODE_TRACE(1, msg);
#define NODE_TRACE2(msg) NODE_TRACE(2, msg);
#define NODE_TRACE3(msg) NODE_TRACE(3, msg);

#else

#define NODE_TRACE(msg)
#define NODE_TRACE1(msg)
#define NODE_TRACE2(msg)
#define NODE_TRACE3(msg)

#endif


namespace zorba { namespace simplestore {


/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
//  XmlTree                                                                    //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////


XmlTree::XmlTree(XmlNode* root, ulong id)
  :
  theRefCount(0),
  theId(id),
  theDataGuideRootNode(NULL)
{
  theDOMcreator = NULL;
  theRootNode = NULL;
  setRoot(root);
  during_import_DOM = false;
}


void XmlTree::free() throw()
{
  if (theRootNode != 0)
  {
    theRootNode->deleteTree();
    theRootNode = NULL;
  }

  if (theDataGuideRootNode != NULL)
  {
    theDataGuideRootNode->deleteTree();
    theDataGuideRootNode = NULL;
  }

  if(theDOMcreator)
    theDOMcreator->Release();

  delete this;
}

void XmlTree::setRoot(XmlNode* root)
{
  if(!root)
  {
    theRootNode = root; 
    return;
  }

  if(!theDOMcreator)
  {
    HRESULT   hr;
    hr = CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER, 
        IID_IXMLDOMDocument, (void**)&theDOMcreator);
    theDOMcreator->AddRef();
  }

  if(!during_import_DOM && !theRootNode)
  {
    IXMLDOMNode   *dom_root = root->GetDOMNode();
    IXMLDOMNode   *dom_new_child;
    theDOMcreator->appendChild(dom_root, &dom_new_child);
  }
  theRootNode = root; 
}

IXMLDOMDocument   *XmlTree::getDOMcreator()
{
  if(!theDOMcreator)
  {
    CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER, 
         IID_IXMLDOMDocument, (void**)&theDOMcreator);
    theDOMcreator->AddRef();
  }
  return theDOMcreator;
}

void XmlTree::setDOMcreator(IXMLDOMDocument *theDOMdoc)
{
  theDOMdoc->AddRef();
  if(theDOMcreator)
    theDOMcreator->Release();
  theDOMcreator = theDOMdoc;
}


/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
//  class XmlNode                                                              //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////


/*******************************************************************************
  Create a new node C within a given tree T and compute its ordpath based on its
  given position among the attributes or children of a given node P, who will
  become the parent of C.

  If P is NULL, C becomes the root (and single node) of T. In this case, C's
  ordpath is the root one (= 1). 

  If P != NULL and pos >= 0, C will become the "pos"-th child/attribute of P.
  In this case, the ordpath of C is computed based on the ordpaths of its left
  and right siblings (if any). Note: If "pos" >= P->numChildren, then C will
  be appended to P's children/attributes.

  If P != NULL and "pos" < 0, C will be appended to P's children/attributes,
  and C's ordpath is computed based on the current number of children/attributes
  in P. This ordpath evaluation assumes that P never had a child that was later
  deleted. The assumption holds because this case applies only during the
  evaluation of a node-constructor expression, or during the copying of a
  subtree S, where the node to be created is not the copy of the root of S.
********************************************************************************/
/*-XmlNode::XmlNode(
    XmlTree*              tree,
    XmlNode*              parent,
    long                  pos,
    store::StoreConsts::NodeKind nodeKind)
  :
  store::Item(),
  theParent(parent)
{
  assert(tree || parent);
  assert(parent == NULL || parent->getTree() != NULL);
  assert(tree == NULL || parent == NULL || parent->getTree() == tree);

  if (parent == NULL)
  {
    setTree(tree);
    tree->setRoot(this);
    theOrdPath.setAsRoot();
  }
  else
  {
    setTree(parent->getTree());
    setOrdPath(parent, pos, nodeKind);
  }
}
*/

XmlNode::XmlNode(
    XmlTree*              tree,
    XmlNode*              parent)
{
  assert(tree || parent);
  assert(parent == NULL || parent->getTree() != NULL);
  assert(tree == NULL || parent == NULL || parent->getTree() == tree);

  setTree(tree, parent);
}


/*******************************************************************************

********************************************************************************/
XmlNode::~XmlNode()
{
  ZORBA_ASSERT(theRefCount == 0);
}


void XmlNode::setParent(
                        XmlTree*              tree,
                        XmlNode*              parent,
                        long                  pos,
                        store::StoreConsts::NodeKind nodeKind)
{
  theParent = parent;

  assert(tree || parent);
  assert(parent == NULL || parent->getTree() != NULL);
  assert(tree == NULL || parent == NULL || parent->getTree() == tree);

  if (parent == NULL)
  {
    //setTree(tree);
    tree->setRoot(this);
    theOrdPath.setAsRoot();
  }
  else
  {
    //setTree(parent->getTree());
    setOrdPath(parent, pos, nodeKind);
  }
}

/*******************************************************************************

********************************************************************************/
void XmlNode::setTree(const XmlTree*        t,
                      XmlNode*              parent
                      )
{
  if (parent == NULL)
  {
    theTreeRCPtr = (long*)t;
  }
  else
  {
    theTreeRCPtr = (long*)parent->getTree();
  }
  SYNC_CODE(theRCLockPtr = &(t->getRCLock());)
}


/*******************************************************************************

********************************************************************************/
void XmlNode::setOrdPath(
    XmlNode*              parent,
    long                  pos,
    store::StoreConsts::NodeKind nodeKind)
{
  if (!parent->theOrdPath.isValid())
    return;

  ulong numChildren = parent->numChildren();
  ulong numAttrs = parent->numAttributes();

  if (nodeKind == store::StoreConsts::attributeNode)
  {
    if (pos < 0)
    {
      theOrdPath = parent->theOrdPath;
      theOrdPath.appendComp(2 * numAttrs + 1);
    }
    else if (numAttrs > 0)
    {
      ulong upos = (ulong)pos;

      if (upos == 0)
      {
        OrdPath::insertBefore(parent->theOrdPath,
                              parent->getAttr(0)->theOrdPath,
                              theOrdPath);
      }
      else if (upos == numAttrs && numChildren > 0)
      {
        OrdPath::insertInto(parent->theOrdPath,
                            parent->getAttr(upos-1)->theOrdPath,
                            parent->getChild(0)->theOrdPath,
                            theOrdPath);
      }
      else if (upos >= numAttrs)
      {
        OrdPath::insertAfter(parent->theOrdPath,
                             parent->getAttr(upos-1)->theOrdPath,
                             theOrdPath);
      }
      else
      {
        OrdPath::insertInto(parent->theOrdPath,
                            parent->getAttr(upos-1)->theOrdPath,
                            parent->getAttr(upos)->theOrdPath,
                            theOrdPath);
      }
    }
    else if (numChildren > 0)
    {
      OrdPath::insertBefore(parent->theOrdPath,
                            parent->getChild(0)->getOrdPath(),
                            theOrdPath);
    }
    else
    {
      theOrdPath = parent->theOrdPath;
      theOrdPath.appendComp(1);
    }
  }
  else // not attribute node
  {
    if (pos < 0)
    {
      theOrdPath = parent->theOrdPath;
      theOrdPath.appendComp(2 * (numAttrs + numChildren) + 1);
    }
    else if (numChildren > 0)
    {
      ulong upos = (ulong)pos;

      if (upos == 0 && numAttrs > 0)
      {
        OrdPath::insertInto(parent->theOrdPath,
                            parent->getAttr(numAttrs-1)->theOrdPath,
                            parent->getChild(0)->theOrdPath,
                            theOrdPath);
      }
      else if (upos == 0)
      {
        OrdPath::insertBefore(parent->theOrdPath,
                              parent->getChild(0)->theOrdPath,
                              theOrdPath);
      }
      else if (upos >= numChildren)
      {
        OrdPath::insertAfter(parent->theOrdPath,
                             parent->getChild(numChildren-1)->theOrdPath,
                             theOrdPath);
      }
      else
      {
        OrdPath::insertInto(parent->theOrdPath,
                            parent->getChild(upos-1)->theOrdPath,
                            parent->getChild(upos)->theOrdPath,
                            theOrdPath);
      }
    }
    else if (numAttrs > 0)
    {
      OrdPath::insertAfter(parent->theOrdPath,
                           parent->getAttr(numAttrs-1)->theOrdPath,
                           theOrdPath);
    }
    else
    {
      theOrdPath = parent->theOrdPath;
      theOrdPath.appendComp(1);
    }
  }
}


/*******************************************************************************

********************************************************************************/
/*-void XmlNode::setId(XmlTree* tree, const OrdPathStack* op)
{
  ZORBA_ASSERT(getTree() == NULL);

  setTree(tree);

  if (op != NULL)
    theOrdPath = *op;
  else
    theOrdPath.setAsRoot();
}
*/

/*******************************************************************************
  Return a hash value based on the id of the node.
********************************************************************************/
uint32_t XmlNode::hash(long timezone, XQPCollator* aCollation) const
{
  ulong tid = getTree()->getId();

  return hashfun::h32((void*)(&tid), sizeof(ulong), theOrdPath.hash());
}


/*******************************************************************************

********************************************************************************/
store::Item_t XmlNode::getEBV() const
{
  store::Item_t bVal;
  GET_FACTORY().createBoolean(bVal, true);
  return bVal;
}


/*******************************************************************************

********************************************************************************/
xqpStringStore_t XmlNode::getBaseURIInternal(bool& local) const
{
  local = false;
  return theParent ? theParent->getBaseURI() : 0;
}


/*******************************************************************************
  Remove the i-th child of "this" (it is assumed that i < numChildren).
********************************************************************************/
void XmlNode::removeChild(ulong i)
{
  XmlNode* child = getChild(i);

  children().remove(i);

  IXMLDOMNode   *dom_node = child->GetDOMNode();
  IXMLDOMNode   *dom_parent = GetDOMNode();
  IXMLDOMNode   *dom_removed_node;
  HRESULT       hr;

  if(!getTree()->during_import_DOM && dom_node && dom_parent)
  {
    hr = dom_parent->removeChild(dom_node, &dom_removed_node);
  }
  //if(SUCCEEDED(hr))
  //  dom_removed_node->Release();

  if (child->theParent == this)
    child->theParent = NULL;
}


/*******************************************************************************
  If the given node N is a child of "this", disconnect N from "this". Return
  true if N was a child of "this"; false otherwise.
********************************************************************************/
bool XmlNode::removeChild(XmlNode* child)
{
  bool found = children().remove(child);

  if (found)
  {
    if (child->theParent == this)
      child->theParent = NULL;

    IXMLDOMNode   *dom_node = child->GetDOMNode();
    IXMLDOMNode   *dom_parent = GetDOMNode();
    IXMLDOMNode   *dom_removed_node;
    HRESULT       hr;

    if(!getTree()->during_import_DOM && dom_node && dom_parent)
    {
     hr = dom_parent->removeChild(dom_node, &dom_removed_node);
    }
    //if(SUCCEEDED(hr))
    //  dom_removed_node->Release();
  }

  return found;
}


/*******************************************************************************
  Remove the i-th attribute of "this" (it is assumed that i < numAttributes).
********************************************************************************/
void XmlNode::removeAttr(ulong i)
{
  XmlNode* attr = getAttr(i);

  attributes().remove(i);

  IXMLDOMElement    *dom_element = GetDOMElement();
  IXMLDOMAttribute   *dom_removed_attr;
  IXMLDOMAttribute    *dom_attr = attr->GetDOMAttribute();
  HRESULT   hr;

  if(!getTree()->during_import_DOM && dom_element && dom_attr)
  {
    hr = dom_element->removeAttributeNode(dom_attr, &dom_removed_attr);
    //if(SUCCEEDED(hr))
    //  dom_removed_attr->Release();
  }

  if (attr->theParent == this)
    attr->theParent = NULL;
}


/*******************************************************************************
  If the given node N is an attribute of "this", remove it as an attribute of
  "this". Return true if N was an attribute of "this"; false otherwise.
********************************************************************************/
bool XmlNode::removeAttr(XmlNode* attr)
{
  bool found = attributes().remove(attr);

  if (found)
  {
    IXMLDOMElement    *dom_element = GetDOMElement();
    IXMLDOMAttribute   *dom_removed_attr;
    IXMLDOMAttribute    *dom_attr = attr->GetDOMAttribute();
    HRESULT   hr;

    if(!getTree()->during_import_DOM && dom_element && dom_attr)
    {
      hr = dom_element->removeAttributeNode(dom_attr, &dom_removed_attr);
    }
    //if(SUCCEEDED(hr))
    //  dom_removed_attr->Release();
    if (attr->theParent == this)
      attr->theParent = NULL;
  }

  return found;
}


/*******************************************************************************
  Disconnect this node from its parent, if any.
********************************************************************************/
ulong XmlNode::disconnect() throw()
{
  if (theParent == NULL)
    return 0;

  ulong pos;

  if (getNodeKind() == store::StoreConsts::attributeNode)
  {
    pos = theParent->attributes().find(this);
    if (pos < theParent->numAttributes())
    {
      //-theParent->attributes().remove(pos);
      theParent->removeAttr(pos);
    }
  }
  else
  {
    pos = theParent->children().find(this);
    if (pos < theParent->numChildren())
    {
      //-theParent->children().remove(pos);
      theParent->removeChild(pos);
    }
  }

  theParent = NULL;

  return pos;
}

store::Item_t XmlNode::QNameFromDOM() const 
{
  IXMLDOMNode *dom_node = ((XmlNode*)this)->GetDOMNode();
  BSTR    bstr_baseName;
  BSTR    bstr_prefix;
  HRESULT hr_prefix;
  BSTR    bstr_namespace;
  HRESULT hr_ns;

  dom_node->get_baseName(&bstr_baseName);
  hr_prefix = dom_node->get_prefix(&bstr_prefix);
  hr_ns = dom_node->get_namespaceURI(&bstr_namespace);


  char  *str_baseName = NULL;
  char  *str_prefix = "";
  char  *str_namespace = "";

  str_baseName = ImportMSDOM::fromBSTR(bstr_baseName);

  if(SUCCEEDED(hr_prefix))
  {
    str_prefix = ImportMSDOM::fromBSTR(bstr_prefix);
  }

  if(SUCCEEDED(hr_ns))
  {
    str_namespace = ImportMSDOM::fromBSTR(bstr_namespace);
  }

  store::Item_t  retitem;
  GET_FACTORY().createQName(retitem, str_namespace, str_prefix, str_baseName);

  ::free(str_baseName);
  if(SUCCEEDED(hr_prefix))
    ::free(str_prefix);
  if(SUCCEEDED(hr_ns))
    ::free(str_namespace);

  return retitem;
}

void XmlNode::add_attribute(AttributeNode *attr, ulong pos, bool shared)
{
  if(!getTree()->during_import_DOM)
  {
    IXMLDOMElement    *dom_element = GetDOMElement();
    IXMLDOMAttribute  *dom_removed_attr;
    IXMLDOMAttribute  *dom_attr = attr->GetDOMAttribute();
    HRESULT   hr;

    if(dom_element && dom_attr)
    {
      hr = dom_element->setAttributeNode(dom_attr, &dom_removed_attr);
      if(FAILED(hr))
        ZORBA_ERROR(XQP0027_MS_DOM_ERROR);
    }
  }

  if (pos < 0)
    attributes().push_back(attr, shared);
  else
    attributes().insert(attr, pos, shared);
}

void XmlNode::add_child(XmlNode *child, ulong pos, bool shared)
{
  if((pos >= 0) && (pos < numChildren()))
  {
    if(!getTree()->during_import_DOM)
    {
      IXMLDOMNode       *dom_parent_node = GetDOMNode();
      IXMLDOMNode       *dom_new_node;
      IXMLDOMNode       *dom_node = child->GetDOMNode();
      VARIANT           beforeChild;
      HRESULT   hr;

      if(dom_parent_node && dom_node)
      {
        beforeChild.punkVal = children().get(pos)->GetDOMNode();
        beforeChild.vt = VT_UNKNOWN;
      
        hr = dom_parent_node->insertBefore(dom_node, beforeChild, &dom_new_node);

        if(FAILED(hr))
          ZORBA_ERROR(XQP0027_MS_DOM_ERROR);
      }
    }
    children().insert(child, pos, shared);
  }
  else
  {
    if(!getTree()->during_import_DOM)
    {
      IXMLDOMNode       *dom_parent_node = GetDOMNode();
      IXMLDOMNode       *dom_node = child->GetDOMNode();
      IXMLDOMNode       *dom_new_child;
      HRESULT   hr = S_OK;

      if(dom_parent_node && dom_node)
      {
        hr = dom_parent_node->appendChild(dom_node, &dom_new_child);

        if(FAILED(hr))
          ZORBA_ERROR(XQP0027_MS_DOM_ERROR);
      }
    }
    children().push_back(child, shared);
  }
}


/*******************************************************************************
  Connect this to the given parent at the given position.
********************************************************************************/
void XmlNode::connect(XmlNode* parent, ulong pos) throw()
{
  ZORBA_FATAL(theParent == NULL, "");

  theParent = parent;

  if (getNodeKind() == store::StoreConsts::attributeNode)
  {
    parent->add_attribute(reinterpret_cast<AttributeNode*>(this), pos, false);
  }
  else
  {
    parent->add_child(this, pos, false);
  }
}


/*******************************************************************************
  Disconnect "this" node and its subtree from its current xml tree and make it
  a member of a new given tree, placing it as a child or attribute of a given
  parent node P. The position among P's children/attributes where "this" is to
  be placed is also given.     
********************************************************************************/
void XmlNode::switchTree(
    XmlNode*        parent,
    ulong           pos,
    const store::CopyMode& copymode) throw()
{
  try
  {
    ulong refcount = 0;
    std::stack<XmlNode*> nodes;

    XmlTree* newTree = (parent == NULL ? 
                        new XmlTree(this, GET_STORE().getTreeId()) :
                        parent->getTree());

    XmlTree* oldTree = getTree();

    if (theParent != NULL)
      theParent->removeChild(this);

    if (oldTree->getRoot() == this)
      oldTree->setRoot(NULL);

    setTree(newTree, NULL);
    //-theParent = parent;

    if (parent)
    {
        setOrdPath(parent, pos, getNodeKind());
        connect(parent, pos);
    /*  if (getNodeKind() == store::StoreConsts::attributeNode)
      {
        setOrdPath(parent, pos, store::StoreConsts::attributeNode);

        parent->attributes().insert(this, pos, false);
      }
      else
      {
        setOrdPath(parent, pos, getNodeKind());

        parent->children().insert(this, pos, false);
      }
    */
    }
    else
    {
      theOrdPath.setAsRoot();
    }

    nodes.push(this);

    SYNC_CODE(oldTree->getRCLock().acquire());

    refcount += theRefCount;

    while (!nodes.empty())
    {
      XmlNode* n = nodes.top();
      nodes.pop();

      AttributeNode* baseUriAttr = NULL;
      AttributeNode* hiddenBaseUriAttr = NULL;
      ulong numAttrs = n->numAttributes();
      for (ulong i = 0; i < numAttrs; i++)
      {
        AttributeNode* attr = n->getAttr(i);
        refcount += attr->theRefCount;
        attr->setTree(newTree, NULL);
        attr->theOrdPath = n->theOrdPath;
        attr->theOrdPath.appendComp(2 * i + 1);

        if (attr->isBaseUri())
        {
          if (attr->isHidden())
            hiddenBaseUriAttr = attr;
          else
            baseUriAttr = attr;
        }
      }

      if (hiddenBaseUriAttr && n->theParent != 0)
      {
        ElementNode* elem = reinterpret_cast<ElementNode*>(n);

        // The current node N has a local base uri property. If this is due to
        // an explicit base uri attribute, then it must be re-resolved based
        // on the (potentially new) base uri of the parent node.
        if (baseUriAttr)
        {
          xqpStringStore_t absuri = parent->getBaseURI();
          xqpStringStore_t reluri = baseUriAttr->getBaseURI();
          elem->adjustBaseUriProperty(hiddenBaseUriAttr, absuri, reluri);
        }
        // The current node N has a local base uri property. If, however, N does 
        // not have an explicit base uri attribute, then N was the root of the
        // old tree, and since N is no longer a root in the new tree, its local
        // base uri must be removed.
        else
        {
          hiddenBaseUriAttr->disconnect();
          hiddenBaseUriAttr->deleteTree();
          elem->resetHaveBaseUri();
        }
      }

      ulong numChildren = n->numChildren();
      for (ulong i = 0; i < numChildren; i++)
      {
        XmlNode* child = n->getChild(i);
        refcount += child->theRefCount;
        child->setTree(newTree, NULL);
        child->theOrdPath = n->theOrdPath;
        child->theOrdPath.appendComp(2 * (numAttrs + i) + 1);

        nodes.push(child);
      }
    } // done traversing tree

    oldTree->getRefCount() -= refcount;
    if (oldTree->getRefCount() == 0)
    {
      SYNC_CODE(oldTree->getRCLock().release());
      oldTree->free();
    }
    else
    {
      SYNC_CODE(oldTree->getRCLock().release());
    }

    SYNC_CODE(newTree->getRCLock().acquire());

    newTree->getRefCount() += refcount;
    if (newTree->getRefCount() == 0)
    {
      SYNC_CODE(newTree->getRCLock().release());
      newTree->free();
    }
    else
    {
      SYNC_CODE(newTree->getRCLock().release());
    }
  }
  catch(...)
  {
    ZORBA_FATAL(0, "Unexpected exception");
  }
}


/*******************************************************************************

********************************************************************************/
void XmlNode::deleteTree() throw()
{
  ZORBA_FATAL(theRefCount == 0, "");

  ulong numChildren = this->numChildren();
  ulong numAttrs = this->numAttributes();

  for (ulong i = 0; i < numChildren; i++)
  {
    XmlNode* child = getChild(i);
    if (child->theParent == this)
    {
      child->deleteTree();
    }
  }

  for (ulong i = 0; i < numAttrs; i++)
  {
    XmlNode* attr = getAttr(i);
    if (attr->theParent == this)
    {
      attr->deleteTree();
    }
  } 

  delete this;
}


/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
//  class DocumentNode                                                         //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////



/*******************************************************************************

********************************************************************************/
DocumentNode::DocumentNode(
    XmlTree*          tree,
    xqpStringStore_t& baseUri,
    xqpStringStore_t& docUri,
    IXMLDOMDocument *dom_doc,
    IXMLDOMDocumentFragment *doc_fragment)
  :
  XmlNode(tree, NULL)
{
  theDOMdoc = dom_doc;
  theDOMfragment = doc_fragment;
  
  HRESULT   hr;
/*  if(!tree || !tree->getDOMcreator())
  {
    hr = CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER, 
         IID_IXMLDOMDocument, (void**)&theDOMdoc);

    if(FAILED(hr))
      ZORBA_ERROR(XQP0027_MS_DOM_ERROR);
  }
  else
    theDOMdoc = tree->getDOMcreator();

  theDOMdoc->AddRef();
*/

  if(!dom_doc && !doc_fragment)
  {
    hr = tree->getDOMcreator()->createDocumentFragment(&theDOMfragment);
    if(FAILED(hr))
      ZORBA_ERROR(XQP0027_MS_DOM_ERROR);
  }
  else
  {
    if(theDOMfragment)
      theDOMfragment->AddRef();
    if(theDOMdoc)
    {
      theDOMdoc->AddRef();
      //tree->setDOMcreator(theDOMdoc);
    }
  }

  if (baseUri != NULL && !baseUri->empty())
    theBaseUri.transfer(baseUri);

  theDocUri.transfer(docUri);

  setParent(tree, NULL, 0, store::StoreConsts::documentNode);
}


/*******************************************************************************

********************************************************************************/
DocumentNode::~DocumentNode()
{
  NODE_TRACE1("Deleted doc node " << this);
//  if(theDOMdoc)
//    theDOMdoc->Release();
  if(theDOMfragment)
    theDOMfragment->Release();
  if(theDOMdoc)
    theDOMdoc->Release();
}


/*******************************************************************************

********************************************************************************/
XmlNode* DocumentNode::copy2(
    XmlNode*               rootParent,
    XmlNode*               parent,
    long                   pos,
    const store::CopyMode& copymode) const
{
  ZORBA_ASSERT(rootParent == NULL && parent == NULL);

  XmlTree* tree = NULL;
  XmlNode* copyNode = NULL;

  xqpStringStore_t baseuri = theBaseUri;
  xqpStringStore_t docuri = theDocUri;

  try
  {
    tree = new XmlTree(NULL, GET_STORE().getTreeId());

    copyNode = new DocumentTreeNode(tree, baseuri, docuri);

    ulong numChildren = this->numChildren();
    for (ulong i = 0; i < numChildren; i++)
    {
      getChild(i)->copy2(rootParent, copyNode, -1, copymode);
    }
  }
  catch (...)
  {
    if (tree)
      tree->free();
    throw;
  }

  NODE_TRACE1("}");
  NODE_TRACE1("Copied doc node " << this << " to node " << copyNode);

  return copyNode;
}


store::Item* DocumentNode::getType() const
{
  return GET_STORE().theSchemaTypeNames[XS_UNTYPED];
}


xqpStringStore_t DocumentNode::getBaseURIInternal(bool& local) const
{
  local = true;
  return theBaseUri;
}


store::Iterator_t DocumentNode::getChildren() const
{
  ChildrenIteratorImpl* res = new ChildrenIteratorImpl();
  res->init((XmlNode*)this);
  return res;
}


void DocumentNode::getTypedValue(store::Item_t& val, store::Iterator_t& iter) const
{
  xqpStringStore_t rch = getStringValue();
  val = new UntypedAtomicItemImpl(rch);
  iter = NULL;
}


store::Item_t DocumentNode::getAtomizationValue() const
{
  xqpStringStore_t rch = getStringValue();
  return new UntypedAtomicItemImpl(rch);
}


xqpStringStore_t DocumentNode::getStringValue() const
{
  std::string buf;

  ulong numChildren = this->numChildren();
  for (ulong i = 0; i < numChildren; i++)
  {
    store::StoreConsts::NodeKind kind = getChild(i)->getNodeKind();

    if (kind != store::StoreConsts::commentNode && kind != store::StoreConsts::piNode)
      buf += getChild(i)->getStringValue()->str();
  }

  return new xqpStringStore(buf);
}


/*******************************************************************************

********************************************************************************/
xqp_string DocumentNode::show() const
{
  std::stringstream strStream;

  strStream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl
            << "<document";
  if (theBaseUri != NULL)
    strStream << " baseUri = \"" << *theBaseUri;
  if (theDocUri != NULL)
    strStream << " docUri = \"" << *theDocUri;
  strStream << "\">" << std::endl;

  store::Iterator_t iter = getChildren();
  store::Item_t item;
  while (iter->next(item))
  {
    strStream << item->show();
  }
  
  strStream << std::endl << "</document>";

  return strStream.str().c_str();
}


/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
//  class DocumentTreeNode                                                     //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////



/*******************************************************************************

********************************************************************************/
DocumentTreeNode::DocumentTreeNode(
    XmlTree*          tree,
    xqpStringStore_t& baseUri,
    xqpStringStore_t& docUri,
    IXMLDOMDocument *dom_doc,
    IXMLDOMDocumentFragment *doc_fragment)
  :
  DocumentNode(tree, baseUri, docUri, dom_doc, doc_fragment)
{
  NODE_TRACE1("{\nConstructing doc node " << this << " tree = "
              << getTree()->getId() << ":" << getTree()
              << " doc uri = " << (theDocUri != 0 ? theDocUri->c_str() : "NULL"));
}


/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
//  class DocumentDagNode                                                      //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////


/*******************************************************************************

********************************************************************************/
DocumentDagNode::DocumentDagNode(
    XmlTree*          tree,
    xqpStringStore_t& baseUri,
    xqpStringStore_t& docUri)
  :
  DocumentNode(tree, baseUri, docUri)
{
  NODE_TRACE1("{\nConstructing doc node " << this << " tree = "
              << getTree()->getId() << ":" << getTree()
              << " doc uri = " << (theDocUri != 0 ? theDocUri->c_str() : "NULL"));
}


/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
//  class ElementNode                                                          //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////


/*******************************************************************************
  localBindings will be NULL if this contructor is called from the copy() method
  (because in that case, the in-scope bindings must be computed by the copy()
  method based on the copy mode). Otherwise, localBindings should not be NULL
  (but may be empty).
********************************************************************************/
ElementNode::ElementNode(
    XmlTree*                 tree,
    XmlNode*                 parent,
    long                     pos,
    store::Item_t&           nodeName,
    store::Item_t&           typeName,
    bool                     haveValue,
    bool                     haveEmptyValue,
    bool                     isId,
    bool                     isIdRefs,
    const store::NsBindings* localBindings,
    IXMLDOMElement           *dom_element)
  :
  XmlNode(tree, parent),
  theFlags(0)
{
  theDOMElement = NULL;
  
  bool  ante_during = getTree()->during_import_DOM;
  getTree()->during_import_DOM = true;

  
  setParent(tree, parent, pos, store::StoreConsts::elementNode);

#ifdef ZORBA_MSDOM_IN_NATIVE_STORE_CACHING
  theName = nodeName;
#endif
  theTypeName.transfer(typeName);

  if (haveValue)
  {
    setHaveValue();

    if (haveEmptyValue)
      setHaveEmptyValue();
    else if (isId)
      setIsId();
    else if (isIdRefs)
      setIsIdRefs();
  }

  if (localBindings && !localBindings->empty())
  {
//#ifdef ZORBA_MSDOM_IN_NATIVE_STORE_CACHING
    theNsContext = new NsBindingsContext(*localBindings);
//#endif

    theFlags |= HaveLocalBindings;
  }

  if (localBindings)
    addBindingForQName(theName, false, true);

  getTree()->during_import_DOM = ante_during;
  if(!dom_element)
  {//create the MS DOM Element
    HRESULT   hr;
    xqpStringStore_t    name_str;

    name_str = theName->getStringValue();
    CMyBSTR      bstr_name(name_str->c_str());
    CComVariant   node_type(NODE_ELEMENT);
    xqpStringStore_t    ns_str;
    ns_str = nodeName->getNamespace();
    CMyBSTR      bstr_namespace(ns_str->c_str());
    IXMLDOMNode   *dom_node;
    hr = getTree()->getDOMcreator()->createNode(node_type, bstr_name, bstr_namespace, &dom_node);
    if(FAILED(hr))
      ZORBA_ERROR(XQP0027_MS_DOM_ERROR);
    dom_node->QueryInterface(IID_IXMLDOMElement, (void**)&theDOMElement);
    dom_node->Release();
  }
  else
  {
    theDOMElement = dom_element;
    theDOMElement->AddRef();
  }
  if(localBindings)
    add_ns_bindings_to_DOM(localBindings);
  if (parent)
  {
  /*  if (pos < 0)
      parent->children().push_back(this, false);
    else
      parent->children().insert(this, pos, false);
  */
    //connect(parent, pos);
    parent->add_child(this, pos, false);
    if(localBindings)
    {
#ifdef ZORBA_MSDOM_IN_NATIVE_STORE_CACHING
        setNsContext(parent->getNsContext());
#endif
    }
  }
}


/*******************************************************************************

********************************************************************************/
ElementNode::~ElementNode()
{
  NODE_TRACE1("Deleted elem node " << this);
  //NODE_TRACE3("nscontext " << theNsContext.getp() << ", "
  //            << (theNsContext != NULL ? theNsContext->getRefCount() : -1));
  if(theDOMElement)
    theDOMElement->Release();
}


/*******************************************************************************

********************************************************************************/
XmlNode* ElementNode::copy2(
    XmlNode*               rootParent,
    XmlNode*               parent,
    long                   pos,
    const store::CopyMode& copymode) const
{
  assert(parent != NULL || rootParent == NULL);

  if (parent != NULL && !copymode.theDoCopy)
  {
    //parent->children().push_back(const_cast<ElementNode*>(this), true);
    parent->add_child((ElementNode*)this, -1, true);

    NODE_TRACE1("Copied elem node " << this << " to node " << this
              << " name = " << *theName->getStringValue() << " parent = "
              << parent << " pos = " << pos);
    return const_cast<ElementNode*>(this);
  }

  XmlTree* tree = NULL;
  ElementTreeNode* copyNode = NULL;

  store::Item_t theName = getNodeName();
  store::Item_t nodeName = theName;
  store::Item_t typeName;
  bool haveValue, haveEmptyValue, haveTypedValue, haveListValue, isId, isIdRefs; 

  if (copymode.theTypePreserve)
  { 
    typeName = theTypeName;
    haveValue = this->haveValue();
    haveEmptyValue = this->haveEmptyValue();
    haveTypedValue = this->haveTypedValue();
    haveListValue = this->haveListValue();
    isId = this->isId();
    isIdRefs = this->isIdRefs();
  }
  else
  {
    typeName = GET_STORE().theSchemaTypeNames[XS_UNTYPED];
    haveValue = true;
    isId = isIdRefs = haveEmptyValue = haveTypedValue = haveListValue = false;
  }

  xqpStringStore_t baseUri;

  try
  {
    if (parent == NULL)
      tree = new XmlTree(NULL, GET_STORE().getTreeId());

    pos = (parent == rootParent ? pos : -1);

    copyNode = new ElementTreeNode(tree, parent, pos, nodeName, typeName,
                                   haveValue, haveEmptyValue, isId, isIdRefs,
                                   NULL, baseUri);
    if (copymode.theNsPreserve)
    {
      // If we are copying the root of an xml subtree, or a node that does
      // not inherit ns bindings directly from its parent (but may inherit
      // from another ancestor).
      if (parent == rootParent ||
          theNsContext == NULL ||
          (haveLocalBindings() &&
           theNsContext->getParent() != theParent->getNsContext()) ||
          theNsContext != theParent->getNsContext())
      {
        if (theNsContext != NULL)
        {
          std::auto_ptr<NsBindingsContext> ctx(new NsBindingsContext());
          getNamespaceBindings(ctx->getBindings());

          if (!ctx->empty())
          {
            copyNode->theNsContext = ctx.release();
            copyNode->add_ns_bindings_to_DOM(&copyNode->theNsContext->getBindings());

            copyNode->theFlags |= HaveLocalBindings;
          }
        }

        if (rootParent && copymode.theNsInherit)
        {
          // If "this" does not belong to any namespace and the root parent
          // has a default ns binding, then undeclare this default binding.
          xqpStringStore* prefix = theName->getPrefix();
          if (prefix->empty() &&
              theName->getNamespace()->empty() &&
              rootParent->getNodeKind() == store::StoreConsts::elementNode)
          {
            xqpStringStore* ns = reinterpret_cast<ElementNode*>(rootParent)->
                                 findBinding(prefix);
            if (ns != NULL)
              copyNode->addLocalBinding(prefix, theName->getNamespace());//ns);
          }

          copyNode->setNsContext(rootParent->getNsContext());
        }
      }
      // Else the node we are copying is not the root of the xml subtree and
      // it inherits ns bindings directly from its parent.
      else
      {
        if (haveLocalBindings())
        {
          copyNode->theNsContext = new NsBindingsContext(getLocalBindings());
          copyNode->add_ns_bindings_to_DOM(&copyNode->theNsContext->getBindings());
          copyNode->theFlags |= HaveLocalBindings;
        }

        copyNode->setNsContext(parent->getNsContext());
      }
    }
    else // ! nsPreserve
    {
      if (copymode.theTypePreserve &&
          (theTypeName == GET_STORE().theSchemaTypeNames[XS_QNAME] ||
           theTypeName == GET_STORE().theSchemaTypeNames[XS_NOTATION]))
      {
        ZORBA_ERROR(XQTY0086);
      }

      xqpStringStore* prefix;
      xqpStringStore* ns;
      std::auto_ptr<NsBindingsContext> ctx(new NsBindingsContext);

      prefix = theName->getPrefix();
      ns = getNsContext()->findBinding(prefix);

      // ns may be null only if the prefix was empty and there was no default
      // namespace declaration in scope.
      ZORBA_ASSERT(prefix->str() == "" || ns != NULL);

      if (ns != NULL)
      {
        xqpStringStore* ns2 = NULL;

        if (rootParent &&
            rootParent->getNodeKind() == store::StoreConsts::elementNode &&
            copymode.theNsInherit)
        {
          ns2 = rootParent->getNsContext()->findBinding(prefix);
        }

        if (ns2 == NULL || !ns->byteEqual(*ns2))
          ctx->addBinding(prefix, ns);
      }

      ulong numAttrs = numAttributes();

      for (ulong i = 0; i < numAttrs; i++)
      {
        prefix = getAttr(i)->getNodeName()->getPrefix();
        ns = getNsContext()->findBinding(prefix);

        ZORBA_ASSERT(prefix->empty() || prefix->str() == "xml" || ns != NULL);

        if (ns != NULL)
        {
          xqpStringStore* ns2 = NULL;

          if (rootParent &&
              rootParent->getNodeKind() == store::StoreConsts::elementNode &&
              copymode.theNsInherit)
          {
            ns2 = rootParent->getNsContext()->findBinding(prefix);
          }

          if (ns2 == NULL || !ns->byteEqual(*ns2))
            ctx->addBinding(prefix, ns);
        }
      }

      if (!ctx->empty())
      {
        copyNode->theNsContext = ctx.release();
        copyNode->add_ns_bindings_to_DOM(&copyNode->theNsContext->getBindings());
        copyNode->theFlags |= HaveLocalBindings;
      }

      if (copymode.theNsInherit)
      {
        copyNode->setNsContext(rootParent->getNsContext());
      }
    }

    // Copy the attributes of this node
    AttributeNode* baseUriAttr = NULL;
    AttributeNode* hiddenBaseUriAttr = NULL;
    ulong numAttrs = this->numAttributes();
    for (ulong i = 0; i < numAttrs; i++)
    {
      AttributeNode* attr = getAttr(i);

      if (attr->isBaseUri())
      {
        if (attr->isHidden())
        {
          hiddenBaseUriAttr = attr;
          continue;
        }
        else
        {
          baseUriAttr = attr;
        }
      }

      attr->copy2(rootParent, copyNode, -1, copymode);
    }

    if (hiddenBaseUriAttr)
    {
      if (baseUriAttr)
      {
        xqpStringStore_t absuri = (parent ? parent->getBaseURI() : NULL);
        xqpStringStore_t reluri = baseUriAttr->getBaseURI();
        copyNode->addBaseUriProperty(absuri, reluri);
      }
      else if (parent == 0)
      {
        xqpStringStore_t absuri = hiddenBaseUriAttr->getStringValueP();
        xqpStringStore_t reluri;
        copyNode->addBaseUriProperty(absuri, reluri);
      }
      else
      {
        // do not add explicit base uri property in the copy ==> copy inherits
        // the base-uri property of its parent.
      }
    }

    // Copy the children of this node
    ulong numChildren = this->numChildren();
    for (ulong i = 0; i < numChildren; i++)
    {
      getChild(i)->copy2(rootParent, copyNode, -1, copymode);
    }
  }
  catch (...)
  {
    if (tree)
    {
      tree->free();
    }
    else if (copyNode && (parent == rootParent))
    {
      copyNode->disconnect();
      copyNode->deleteTree();
    }

    throw;
  }

  NODE_TRACE1("Copied elem node " << this << " to node " << copyNode
              << " name = " << *theName->getStringValue() << " parent = "
              << (parent ? parent : 0x0)
              << " pos = " << pos << " copy mode = " << copymode.toString());

  return copyNode;
}


/*******************************************************************************

********************************************************************************/
void ElementNode::getTypedValue(store::Item_t& val, store::Iterator_t& iter) const
{
  if (haveValue())
  {
    if (haveTypedValue())
    {
      assert(numChildren() == 1 &&
             getChild(0)->getNodeKind() == store::StoreConsts::textNode);

      const TextNode* child = reinterpret_cast<const TextNode*>(getChild(0));
      if (haveListValue())
      {
        ItemVector* vec = reinterpret_cast<ItemVector*>(child->getValue());
        iter = new ItemIterator(vec->getItems());
        val = NULL;
      }
      else
      {
        val = child->getValue();
        iter = NULL;
      }
    }
    else if (haveEmptyValue())
    {
      val = NULL;
      iter = NULL;
    }
    else
    {
      xqpStringStore_t rch = getStringValue();
      val = new UntypedAtomicItemImpl(rch);
    }
  }
  else
  {
    ZORBA_ERROR_DESC_OSS(FOTY0012,
                        "The element node " << *theName->getStringValue()
                        << " with type " << *theTypeName->getStringValue()
                        << " does not have a typed value");
  }
}


/*******************************************************************************

********************************************************************************/
store::Item_t ElementNode::getAtomizationValue() const
{
  xqpStringStore_t rch = getStringValue();
  return new UntypedAtomicItemImpl(rch);
}


/*******************************************************************************

********************************************************************************/
xqpStringStore_t ElementNode::getStringValue() const
{
  std::string buf;

  ulong numChildren = this->numChildren();
  for (ulong i = 0; i < numChildren; i++)
  {
    store::StoreConsts::NodeKind kind = getChild(i)->getNodeKind();

    if (kind != store::StoreConsts::commentNode &&
        kind != store::StoreConsts::piNode)
      buf += getChild(i)->getStringValue()->str();
  }

  return new xqpStringStore(buf);
}


/*******************************************************************************

********************************************************************************/
store::Item_t ElementNode::getNilled() const
{
  if (theTypeName == GET_STORE().theSchemaTypeNames[XS_UNTYPED])
    return new BooleanItemNaive(false);

  bool nilled = true;
  ulong numChildren = this->numChildren();
  for (ulong i = 0; i < numChildren; i++)
  {
    if (getChild(i)->getNodeKind() == store::StoreConsts::elementNode ||
        getChild(i)->getNodeKind() == store::StoreConsts::textNode)
    {
      nilled = false;
      break;
    }
  }

  if (!nilled)
    return new BooleanItemNaive(false);

  nilled = false;

  //const char* xsi = "http://www.w3.org/2001/XMLSchema-instance";
  //ulong xsilen = strlen(xsi);

  ulong numAttrs = this->numAttributes();
  for (ulong i = 0; i < numAttrs; i++)
  {
    AttributeNode* attr = getAttr(i);
    if (attr->getNodeName()->getNamespace()->byteEqual("xsi", 3) &&
        attr->getNodeName()->getLocalName()->byteEqual("nil", 3))
    {
      nilled = true;
      break;
    }
  }

  return new BooleanItemNaive(nilled);
}


/*******************************************************************************

********************************************************************************/
store::Iterator_t ElementNode::getAttributes() const
{
  AttributesIteratorImpl* res = new AttributesIteratorImpl();
  res->init((XmlNode*)this);
  return res;
}


/*******************************************************************************

********************************************************************************/
store::Iterator_t ElementNode::getChildren() const
{
  ChildrenIteratorImpl* res = new ChildrenIteratorImpl();
  res->init((XmlNode*)this);
  return res;
}


/*******************************************************************************
  Carefull with this function: it generates the namespaces in the reverse order.
  The higher parent gives the latest namespaces, instead of first.
********************************************************************************/
void ElementNode::getNamespaceBindings(
    store::NsBindings&            bindings,
    store::StoreConsts::NsScoping ns_scoping) const
{
  assert(bindings.empty());

  if (theNsContext != NULL)
  {
    if(ns_scoping != store::StoreConsts::ONLY_PARENT_NAMESPACES)
    {
      bindings = theNsContext->getBindings();
    }

    if(ns_scoping == store::StoreConsts::ONLY_LOCAL_NAMESPACES)
      return;

    const NsBindingsContext* parentContext = theNsContext->getParent();

    while (parentContext != NULL)
    {
      const store::NsBindings& parentBindings = parentContext->getBindings();
      ulong parentSize = parentBindings.size();
      ulong currSize = bindings.size();

      // for each parent binding, add it to the result, if it doesn't have the
      // same prefix as another binding that is already in the result.
      for (ulong i = 0; i < parentSize; i++)
      {
        ulong j;
        for (j = 0; j < currSize; j++)
        {
          if (bindings[j].first.byteEqual(parentBindings[i].first))
            break;
        }

        if (j == currSize)
        {
          bindings.push_back(parentBindings[i]);
        }
      }

      parentContext = parentContext->getParent();
    }
  }
}


/*******************************************************************************
  Make "this" inherit the ns bindings of its parent.
********************************************************************************/
void ElementNode::setNsContext(NsBindingsContext* parentCtx)
{
  if (theNsContext == NULL)
    theNsContext = parentCtx;
  else if (theNsContext.getp() != parentCtx)
    theNsContext->setParent(parentCtx);
}


/*******************************************************************************
  Search all the in-scope ns bindings of "this" to find the ns uri associated
  with the given prefix. Return NULL if no binding exists for the given prefix.
********************************************************************************/
xqpStringStore* ElementNode::findBinding(const xqpStringStore* prefix) const
{
  if (theNsContext == NULL)
    return NULL;

  return theNsContext->findBinding(prefix);
}


/*******************************************************************************

********************************************************************************/
const store::NsBindings& ElementNode::getLocalBindings() const
{
  ZORBA_ASSERT(haveLocalBindings());
  return theNsContext->getBindings();
}


/*******************************************************************************
  Add the ns binding that is implied by the given qname, if such a binding does
  not exist already among the bindings of "this" node. The method returns true
  if a binding was added, or false otherwise.

  The method also checks if the given binding conflicts with the current bindings
  of "this" node. If a conflict exists and replacePrefix is false, an error is
  thrown. Else, if a conflict exists and replacePrefix is true, then the method
  creates and returns a qname with the same local name and ns uri as the given
  qname, but with a prefix that is artificially generated so that the conflict
  is resolved.

  This method is used by the ElementNode and AttributeNode constructors with
  replacePrefix set to true. It is also used by updating methods with 
  replacePrefix set to false.
********************************************************************************/
bool ElementNode::addBindingForQName(
    store::Item_t& qname,
    bool           isAttr,
    bool           replacePrefix)
{
  xqpStringStore* prefix = qname->getPrefix();
  xqpStringStore* ns = qname->getNamespace();

  // If ns is empty, then prefix must be empty
  ZORBA_FATAL(!ns->empty() || prefix->empty(),
              "prefix = " << prefix->str() << "ns = " << ns->str());

  // No ns binding is implied by an attribute qname whose ns uri is empty
  if (ns->empty() && isAttr)
    return false;

  if (prefix->str() != "xml")
  {
    xqpStringStore* ns2 = findBinding(prefix);

    if (ns2 == NULL)
    {
      if (!ns->empty())
      {
        addLocalBinding(prefix, ns);
        return true;
      }
    }
    else if (!ns2->byteEqual(*ns))
    {
      if (replacePrefix)
      {
        //std::cout << "Prefix: " << prefix->str() << " ns: " << ns->c_str() << " ns2: " << ns2->c_str() << " local: " << qname->getLocalName()->str() << "\n";
        ZORBA_FATAL(!ns->empty(), "");

        xqpStringStore_t prefix(new xqpStringStore("XXX"));

        while (findBinding(prefix) != NULL)
          prefix = prefix->append("X");

        GET_FACTORY().createQName(qname, ns, prefix, qname->getLocalName());
        addLocalBinding(prefix, ns);
      }
      else
      {
        ZORBA_ERROR_DESC_OSS(XUDY0024,
                             "The implied namespace binding of "
                             << qname->show()
                             << " conflicts with namespace binding ["
                             << prefix->str() << ", " << ns2->str() << "]");
      }
    }
  }
  return false;
}


/*******************************************************************************
  Same as above, but used when we know that no ns binding conflict exists
********************************************************************************/
void ElementNode::addBindingForQName2(const store::Item* qname)
{
  xqpStringStore* prefix = qname->getPrefix();
  xqpStringStore* ns = qname->getNamespace();

  // If ns is empty, then prefix must be empty
  ZORBA_FATAL(!ns->empty() || prefix->empty(),
              "prefix = " << prefix->str() << "ns = " << ns->str());

  if (prefix->str() != "xml")
  {
    xqpStringStore* ns2 = findBinding(prefix);

    if (ns2 == NULL)
    {
      if (!ns->empty())
        addLocalBinding(prefix, ns);
    }
    else if (!ns2->byteEqual(*ns))
    {
      ZORBA_FATAL(0, "");
    }
  }
}


/*******************************************************************************
  Add a given ns binding to the local ns bindings of "this", if it's not already
  there. It is assumed that the given binding does not conflit with the other
  local bindings of "this" (ZORBA_FATAL is called if this condition is not true).

  Note: it is possible to add the binding (empty --> empty); this happens when we
  need to delete the default binding (empty --> ns) from the bindings of "this".
********************************************************************************/
void ElementNode::addLocalBinding(xqpStringStore* prefix, xqpStringStore* ns)
{
  if (!haveLocalBindings())
  {
    NsBindingsContext* parent = theNsContext;
    theNsContext = new NsBindingsContext(0);
    theNsContext->setParent(parent);
  }

  theNsContext->addBinding(prefix, ns);

  addLocalBinding_DOM(prefix, ns);

  theFlags |= HaveLocalBindings;
}

/*******************************************************************************
  Remove the given ns binding from the local ns bindings of "this", if it is
  there.
********************************************************************************/
void ElementNode::removeLocalBinding(xqpStringStore* prefix, xqpStringStore* ns)
{
  if (haveLocalBindings())
  {
    theNsContext->removeBinding(prefix, ns);
    //remove from DOM also
    if(theDOMElement)
    {
      std::string qname;

      if(prefix->empty())
      {
        qname = "xmlns";
      }
      else
      {
        qname = "xmlns:";
        qname += prefix->c_str();
      }

      CMyBSTR  bstr_qname(qname.c_str());
      theDOMElement->removeAttribute(bstr_qname);
    }
  }
}

void ElementNode::add_ns_bindings_to_DOM(const store::NsBindings* localBindings)
{//keep the namespaces as attributes in MS DOM
  if(!getTree()->during_import_DOM)
  {
    store::NsBindings::const_iterator   ns_it;
    for(ns_it = localBindings->begin(); ns_it != localBindings->end(); ns_it++)
    {
      addLocalBinding_DOM((*ns_it).first.getStore(), (*ns_it).second.getStore());
    }
  }
}

void ElementNode::addLocalBinding_DOM(xqpStringStore* prefix, xqpStringStore* ns)
{
  if((!getTree()->during_import_DOM) && (theDOMElement))
  {
    xqpString   qn;
    if(prefix->empty())
      qn = "xmlns";
    else
      qn = xqpString::concat("xmlns:", prefix->c_str());
    CMyBSTR      bstr_name(qn.c_str());
    CMyBSTR      bstr_namespace(ns->c_str());
    CComVariant   var_namespace(bstr_namespace);

    HRESULT   hr;
    hr = theDOMElement->setAttribute(bstr_name, var_namespace);
  }
}



/*******************************************************************************
  Check if the ns binding implied by the given qname conflicts with the current
  ns bindings of "this" node.
********************************************************************************/
void ElementNode::checkNamespaceConflict(
    const store::Item*  qname,
    XQUERY_ERROR        ecode) const
{
  const QNameItemImpl* qn = reinterpret_cast<const QNameItemImpl*>(qname);

  xqpStringStore* prefix = qn->getPrefix();
  xqpStringStore* ns = qn->getNamespace();

  // Nothing to do if the qname does not imply any ns binding
  if (prefix->empty() && ns->empty())
    return;

  xqpStringStore* ns2 = findBinding(prefix);

  if (ns2 != NULL && !ns2->byteEqual(*ns))
  {
    ZORBA_ERROR_DESC_OSS(ecode,
                         "The implied namespace binding of " << qname->show()
                         << " conflicts with namespace binding ["
                         << prefix->str() << ", " << ns2->str() << "]");
  }
}


/*******************************************************************************
  Check that "this" does not have an attr with the same name as the given name.
********************************************************************************/
void ElementNode::checkUniqueAttr(const store::Item* attrName) const
{
  ulong numAttrs = numAttributes();
  for (ulong i = 0; i < numAttrs; i++)
  {
    XmlNode* attr = getAttr(i);
    if (attr->getNodeName()->equals(attrName))
    {
      ZORBA_ERROR_PARAM_OSS(XQDY0025,
                            "Attribute name " << *attrName->getStringValue() 
                            << " is not unique", "");
    }
  }
}


/*******************************************************************************
  Compute the base-uri property of this element node and store it as a a special
  ("hidden") attribute of the node. In general, the base-uri property is 
  computed by resolving the "relUri" based on the absolute "absUri". However,
  "relUri" may be an absolute uri already, in which case the base-uri property
  of the node is set to "relUri". It is also possible that "relUri" is NULL,
  in which case, the base-uri property of the node is set to "absUri". It is
  assumed that both "absUri" and "relUri" (if not NULL) are well-formed uri
  strings.
********************************************************************************/
void ElementNode::addBaseUriProperty(
    xqpStringStore_t& absUri,
    xqpStringStore_t& relUri)
{
  ZORBA_FATAL(absUri != NULL && !absUri->empty(), "");

  const SimpleStore& store = GET_STORE();

  store::Item_t qname = store.getQNamePool().insert(store.XML_URI, "xml", "base");
  store::Item_t tname = store.theSchemaTypeNames[XS_ANY_URI];

  store::Item_t typedValue;
  if (relUri == NULL)
  {
    typedValue = new AnyUriItemImpl(absUri);
  }
  else
  { 
    xqpStringStore_t resolvedUriString;
    try {
      URI absoluteURI(&*absUri);
      URI resolvedURI(absoluteURI, &*relUri);
      resolvedUriString = resolvedURI.toString().getStore();
    } catch (error::ZorbaError&) {
      resolvedUriString.transfer(relUri);
    }

    typedValue = new AnyUriItemImpl(resolvedUriString);
  }

  new AttributeNode(NULL, this, 0, qname, tname, typedValue, false,
                    false, false, true);
  setHaveBaseUri();
}


/*******************************************************************************
  Change the base-uri property of this node. The new base-uri value is computed
  by resolving the "relUri" based on the "absUri". If "relUri" is NULL, then the
  new base-uri value is set to "absUri". If "relUri" is already an absolute uri,
  then the new base-uri value is set to "relUri". "attr" is the hidden attribute
  that stores the base-uri value.
********************************************************************************/
void ElementNode::adjustBaseUriProperty(
    AttributeNode*    attr,
    xqpStringStore_t& absUri,
    xqpStringStore_t& relUri)
{
  ZORBA_FATAL(absUri != NULL && !absUri->empty(), "");

  store::Item_t typedValue;
  if (relUri == NULL)
  {
    typedValue = new AnyUriItemImpl(absUri);
  }
  else
  { 
    xqpStringStore_t resolvedUriString;
    try {
      URI lAbsoluteUri(&*absUri);
      URI lResolvedUri(lAbsoluteUri, &*relUri);
      resolvedUriString = lResolvedUri.toString().getStore();
    } catch (error::ZorbaError& e) {
      ZORBA_FATAL(e.theErrorCode, e.theDescription);
    }
    typedValue = new AnyUriItemImpl(resolvedUriString);
  }

  attr->setTypedValue(typedValue);
}


/*******************************************************************************

********************************************************************************/
xqp_string ElementNode::show() const
{
  std::stringstream str;

  str <<  "<" << *getNodeName()->getStringValue() << "-elem";

  str << " nid=\"" << theOrdPath.show() << "\"";

  store::NsBindings nsBindings;
  getNamespaceBindings(nsBindings);

  for (ulong i = 0; i < nsBindings.size(); i++)
  {
    str << " xmlns:" <<  nsBindings[i].first << "=\""
        << nsBindings[i].second << "\"";
  }

  store::Iterator_t iter = getAttributes();
  store::Item_t item;
  while (iter->next(item))
  {
    str << " " << item->show();
  }

  str << ">";

  iter = getChildren();
  iter->open();
  while (iter->next(item))
  {
    str << item->show();
  }

  str << "</" << *theName->getStringValue() << "-elem>";
  return str.str().c_str();
}

store::Item* ElementNode::getNodeName() const 
{
#ifdef ZORBA_MSDOM_IN_NATIVE_STORE_CACHING
  return theName.getp(); 
#endif

  return QNameFromDOM();
}



/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
//  class ElementTreeNode                                                      //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////


/*******************************************************************************

********************************************************************************/
ElementTreeNode::ElementTreeNode(
    XmlTree*                    tree,
    XmlNode*                    parent,
    long                        pos,
    store::Item_t&              nodeName,
    store::Item_t&              typeName,
    bool                        haveTypedValue,
    bool                        haveEmptyValue,
    bool                        isId,
    bool                        isIdRefs,
    const store::NsBindings*    localBindings,
    xqpStringStore_t&           baseUri,
    IXMLDOMElement             *dom_element)
  :
  ElementNode(tree, parent, pos, nodeName,
              typeName, haveTypedValue, haveEmptyValue, isId, isIdRefs,
              localBindings, dom_element)
{
  try
  {
    // Setting the base uri property of "this" cannot be done in the ElementNode
    // constructor, because it involves the creation of an attribute node having
    // "this" as the parent, and within the attribute constructor the class type
    // of "this" cannot be ElementNode.
    if (baseUri != NULL)
    {
      xqpStringStore_t dummy;
      addBaseUriProperty(baseUri, dummy);
    }
  }
  catch(...)
  {
    if (parent)
      parent->removeChild(this);

    if (numAttributes() != 0)
    {
      ulong pos = 0;
      XmlNode* attr = getAttr(pos);
      removeAttr(pos);
      delete attr;
    }

    throw;
  }

  NODE_TRACE1("Constructed element node " << this << " parent = "
              << std::hex << (parent ? (ulong)parent : 0) << " pos = " << pos 
              << " tree = " << getTree()->getId() << ":" << getTree()
              << " ordpath = " << theOrdPath.show()
              << " name = " << *theName->getStringValue());
}


/*******************************************************************************

********************************************************************************/
xqpStringStore_t ElementTreeNode::getBaseURIInternal(bool& local) const
{
  ulong numAttrs = numAttributes();
  for (ulong i = 0; i < numAttrs; i++)
  {
    AttributeNode* attr = getAttr(i);
    if (attr->isBaseUri() && attr->isHidden())
    {
      local = true;
      return attr->getStringValue();
    }
  }

  local = false;
  return theParent ? theParent->getBaseURI().getp() : 0;
}


/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
//  class ElementDagNode                                                       //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////


/*******************************************************************************

********************************************************************************/
ElementDagNode::ElementDagNode(
    XmlTree*                    tree,
    XmlNode*                    parent,
    long                        pos,
    store::Item_t&              nodeName,
    store::Item_t&              typeName,
    bool                        haveTypedValue,
    bool                        haveEmptyValue,
    bool                        isId,
    bool                        isIdRefs,
    const store::NsBindings*    localBindings,
    xqpStringStore_t&           baseUri)
  :
  ElementNode(tree, parent, pos, nodeName,
              typeName, haveTypedValue, haveEmptyValue, isId, isIdRefs,
              localBindings)
{
  try
  {
    if (baseUri != NULL)
    {
      xqpStringStore_t dummy;
      addBaseUriProperty(baseUri, dummy);
    }
  }
  catch(...)
  {
    if (parent)
      parent->removeChild(this);

    throw;
  }

  NODE_TRACE1("Constructed element node " << this << " parent = " 
              << std::hex << (parent ? (ulong)parent : 0) << " pos = " << pos 
              << " tree = " << getTree()->getId() << ":" << getTree()
              << " ordpath = " << theOrdPath.show()
              << " name = " << *theName->getStringValue());
}


/*******************************************************************************

********************************************************************************/
xqpStringStore_t ElementDagNode::getBaseURIInternal(bool& local) const
{
  ulong numAttrs = numAttributes();
  for (ulong i = 0; i < numAttrs; i++)
  {
    AttributeNode* attr = getAttr(i);
    if (attr->isBaseUri() && attr->isHidden())
    {
      local = true;
      return attr->getStringValue();
    }
  }

  local = false;
  return theParent ? theParent->getBaseURI().getp() : new xqpStringStore("");
}


/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
//  class AttributeNode                                                        //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////



/*******************************************************************************

********************************************************************************/
AttributeNode::AttributeNode(
    XmlTree*                    tree,
    XmlNode*                    parent,
    long                        pos,
    store::Item_t&              attrName,
    store::Item_t&              typeName,
    store::Item_t&              typedValue,
    bool                        isListValue,
    bool                        isId,
    bool                        isIdRef,
    bool                        hidden,
    IXMLDOMAttribute           *dom_attribute)
  :
  XmlNode(tree, parent),
  theFlags(0)
{
  ElementNode* p = reinterpret_cast<ElementNode*>(parent);

  if (p)
    p->checkUniqueAttr(attrName);

  theDOMAttribute = NULL;
  
  bool  ante_during = getTree()->during_import_DOM;
  getTree()->during_import_DOM = true;

  setParent(tree, parent, pos, store::StoreConsts::attributeNode);
  
  // Normally, no exceptions are expected by the rest of the code here, but
  // just to be safe, we use a try-catch.
  try
  {
    const xqpStringStore    *attr_prefix = attrName->getPrefix();
    const xqpStringStore    *attr_namespace = attrName->getNamespace();
    if (attr_prefix->empty() && !attr_namespace->empty())
    {
      xqpStringStore_t prefix(new xqpStringStore("XXX"));
      GET_FACTORY().createQName(attrName,
                                attrName->getNamespace(),
                                prefix,
                                attrName->getLocalName());
    }
/*  else if(!attr_prefix->empty() && !attr_namespace->empty())
    {
      //check if the parent has a different prefix assigned for the namespace
      //or if he has the namespace at all
      xqpStringStore  *orig_prefix;
      orig_prefix = parent->getPrefixForNamespace(attr_namespace);
      if(orig_prefix && !orig_prefix->empty())
      {
        if(!orig_prefix->byteEqual(attr_prefix))
        {//change the prefix to the one from parent element
          GET_FACTORY().createQName(attrName,
                                    attrName->getNamespace(),
                                    orig_prefix,
                                    attrName->getLocalName());
        }
      }
      else
      {
        //we must invent some prefix-namespace binding
        //find a prefix not used
        int   i = 1;
        xqpString    temp_prefix;
        while(1)
        {
          char  stri[20];
          const xqpStringStore    *temp_ns;

          sprintf(stri, "XX%d", i);
          temp_prefix = stri;
          temp_ns = p->findBinding(temp_prefix.getStore());
          if(!temp_ns || temp_ns->empty())
            break;
          i++;
        }
        p->addLocalBinding(temp_prefix, attr_namespace);
        GET_FACTORY().createQName(attrName,
                                  attrName->getNamespace(),
                                  temp_prefix,
                                  attrName->getLocalName());
      }
    }
*/

#ifdef ZORBA_MSDOM_IN_NATIVE_STORE_CACHING
    theName = attrName;
#endif
    theTypeName.transfer(typeName);
    //-theTypedValue.transfer(typedValue);
    setTypedValue(typedValue);

    if (isListValue)
      setHaveListValue();

    QNameItemImpl* qn = reinterpret_cast<QNameItemImpl*>(attrName.getp());

    if (qn->isBaseUri())
      theFlags |= IsBaseUri;
    else if (qn->isId() || isId)
      theFlags |= IsId;
    else if (isIdRef)
      theFlags |= IsIdRefs;

    if (hidden)
      setHidden();

    if (p)
    {
      // If this is an explicit base uri attribute, set or update the base-uri
      // property of the parent. Else, add the ns binding implied by the attr
      // name into the in-scope ns bindings of the parent (if this ns binding
      // is not there already) 
      if (isBaseUri() && !isHidden())
      {
        xqpStringStore_t parentBaseUri = p->getBaseURI();
        xqpStringStore_t baseUri = this->getStringValue();

        if (p->haveBaseUri())
        {
          ulong numAttrs = p->numAttributes();
          for (ulong i = 0; i < numAttrs; i++)
          {
            AttributeNode* attr = p->getAttr(i);
            if (attr->isBaseUri() && attr->isHidden())
            {
              attr->disconnect();
              delete attr;
              break;
            }
          }
        }
        
        if (parentBaseUri == NULL)
          p->addBaseUriProperty(baseUri, parentBaseUri);
        else
          p->addBaseUriProperty(parentBaseUri, baseUri);
      }
      else if (!isHidden())
      {
        p->addBindingForQName(theName, true, true);
      }
    }
    if(!dom_attribute)
    {//create the MS DOM Attribute
      HRESULT   hr;
      xqpStringStore_t    name_str;

      name_str = theName->getStringValue();
      CMyBSTR      bstr_name(name_str->c_str());
      CComVariant   node_type(NODE_ATTRIBUTE);
      xqpStringStore_t    ns_str;
      ns_str = attrName->getNamespace();
      CMyBSTR      bstr_namespace(ns_str->c_str());
      IXMLDOMNode   *dom_node;
      hr = getTree()->getDOMcreator()->createNode(node_type, bstr_name, bstr_namespace, &dom_node);
      if(FAILED(hr))
        ZORBA_ERROR(XQP0027_MS_DOM_ERROR);
      dom_node->QueryInterface(IID_IXMLDOMAttribute, (void**)&theDOMAttribute);
      dom_node->Release();
    }
    else
    {
      theDOMAttribute = dom_attribute;
      theDOMAttribute->AddRef();
    }
    getTree()->during_import_DOM = ante_during;
    if((!getTree()->during_import_DOM) && theDOMAttribute)
    {//set the DOM attribute value
      xqpStringStore_t    str_value = theTypedValue->getStringValue();
      CMyBSTR    bstr_value(str_value->c_str());
      CComVariant   var_value(bstr_value);
      theDOMAttribute->put_value(var_value);
    }
    if(p)
    {
      // Connect "this" to its parent. We do this at the end of this method
      // so that we don't have to undo it inside the catch clause below.
      //-if (pos < 0)
      //-  theParent->attributes().push_back(this, false);
      //-else
      //-  theParent->attributes().insert(this, pos, false);
      //connect(parent, pos);
      parent->add_attribute(this, pos, false);

    }
  }
  catch (...)
  {
    theName = NULL;
    theTypeName = NULL;
    theTypedValue = NULL;

    throw;
  }

  NODE_TRACE1("Constructed attribute node " << this << " parent = "
              << std::hex << (parent ? (ulong)parent : 0) << " pos = " << pos 
              << " tree = " << getTree()->getId() << ":" << getTree()
              << " ordpath = " << theOrdPath.show()
              << " name = " << *theName->getStringValue()
              << " value = " << *getStringValue());
}


/*******************************************************************************

********************************************************************************/
AttributeNode::~AttributeNode()
{
  NODE_TRACE1("Deleted attr node " << this);
  if(theDOMAttribute)
    theDOMAttribute->Release();
}


/*******************************************************************************

********************************************************************************/
void AttributeNode::setTypedValue(store::Item_t& value)
{
  resetHaveListValue();
  theTypedValue.transfer(value);

  if((!getTree()->during_import_DOM) && theDOMAttribute)
  {//set the DOM attribute value
    xqpStringStore_t    str_value = theTypedValue->getStringValue();
    CMyBSTR    bstr_value(str_value->c_str());
    CComVariant   var_value(bstr_value);
    theDOMAttribute->put_value(var_value);
  }
}

store::Item* AttributeNode::getNodeName() const 
{
#ifdef ZORBA_MSDOM_IN_NATIVE_STORE_CACHING
  return theName.getp();
#endif

  return QNameFromDOM();
}


/*******************************************************************************

********************************************************************************/
void AttributeNode::getTypedValue(store::Item_t& val, store::Iterator_t& iter) const
{
  if (haveListValue())
  {
    iter = new ItemIterator(getValueVector().getItems());
    val = NULL;
  }
  else
  {
    val = theTypedValue;
    iter = NULL;
  }
}


/*******************************************************************************

********************************************************************************/
xqpStringStore_t AttributeNode::getStringValue() const
{
  if (haveListValue())
  {
    const std::vector<store::Item_t>& items = getValueVector().getItems();

    std::string str = items[0]->getStringValue()->c_str();

    ulong size = items.size();
    for (ulong i = 1; i < size; i++)
    {
      str += " ";
      str += items[i]->getStringValue()->str();
    }

    return new xqpStringStore(str);
  }
  else
  {
    return theTypedValue->getStringValue();
  }
}


/*******************************************************************************

********************************************************************************/
store::Item_t AttributeNode::getAtomizationValue() const
{
  if (haveListValue())
  {
    ZORBA_ASSERT(0);
  }
  else
  {
    return theTypedValue;
  }
}


/*******************************************************************************

********************************************************************************/
XmlNode* AttributeNode::copy2(
    XmlNode*               rootParent,
    XmlNode*               parent,
    long                   pos,
    const store::CopyMode& copymode) const
{
  assert(parent != NULL || rootParent == NULL);
  ZORBA_FATAL(!isHidden(), "");

  XmlTree* tree = NULL;
  AttributeNode* copyNode = NULL;
  store::Item_t nodeName = getNodeName();//-theName;
  store::Item_t typeName;
  store::Item_t typedValue;

  ElementNode* p = reinterpret_cast<ElementNode*>(parent);

  // Skip copy if caller says so. For simplicity, base-uri attributes are
  // always copied.
  if (parent != NULL && !copymode.theDoCopy && !isBaseUri())
  {
    try
    {
      p->checkNamespaceConflict(theName, XQDY0025);
    }
    catch(error::ZorbaError&)
    {
      // Resolving the namespace conflict requires that the prefix of the
      // attribute name is changed. However, we cannot do that if the attr
      // is not owned by its parent. So, in this case we do the actual copy.
      goto docopy;
    }

    p->checkUniqueAttr(theName);
    p->addBindingForQName2(theName);

    ZORBA_FATAL(dynamic_cast<ElementDagNode*>(parent) != NULL, "");

    //-if (pos < 0)
    //-  p->attributes().push_back(const_cast<AttributeNode*>(this), true);
    //-else
    //-  p->attributes().insert(const_cast<AttributeNode*>(this), pos, true);
    p->add_attribute((AttributeNode*)this, pos, true);

    copyNode = const_cast<AttributeNode*>(this);
  }
  else
  {
docopy:
    bool isListValue;
    bool isId, isIdRefs;

    if (copymode.theTypePreserve)
    {
      typeName = theTypeName;
      typedValue = theTypedValue;

      isListValue = haveListValue();
      isId = this->isId();
      isIdRefs = this->isIdRefs();
    }
    else
    {
      isListValue = false;
      isId = false;
      isIdRefs = false;

      typeName = GET_STORE().theSchemaTypeNames[XS_UNTYPED_ATOMIC];

      if (!haveListValue() &&
          theTypedValue->getType() == GET_STORE().theSchemaTypeNames[XS_UNTYPED_ATOMIC])
      {
        typedValue = theTypedValue;
      }
      else
      {
        xqpStringStore_t rch = getStringValue();
        typedValue = new UntypedAtomicItemImpl(rch);
      }
    }

    try
    {
      if (parent == NULL)
        tree = new XmlTree(NULL, GET_STORE().getTreeId());

      copyNode = new AttributeNode(tree, parent, pos, nodeName,
                                   typeName, typedValue, isListValue, isId, isIdRefs);
    }
    catch (...)
    {
      if (tree) delete tree;
      throw;
    }
  }

  NODE_TRACE1("Copied attribute node " << this << " to node " << copyNode
              << " name = " << theName->show() << " parent = "
              << std::hex << (parent ? (ulong)parent : 0) << " pos = " << pos
              << " copy mode = " << copymode.toString());

  return copyNode;
}


/*******************************************************************************

********************************************************************************/
xqp_string AttributeNode::show() const
{
  return xqpString::concat(getNodeName()->getStringValue(), "=\"",
                           getStringValue()->str(), "\"");
}


/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
//  class TextNode                                                             //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////


/*******************************************************************************

********************************************************************************/
TextNode::TextNode(
    XmlTree*          tree,
    XmlNode*          parent,
    long              pos,
    xqpStringStore_t& content,
    bool              is_cdata,
    IXMLDOMText      *dom_text)
  :
  XmlNode(tree, parent)
{
  this->is_cdata = is_cdata;
  theContent.theDOMText = NULL;
  if(!dom_text)
  {//create the MS DOM Text Node
    HRESULT   hr;
    CMyBSTR  bstr_content("");

    if(!is_cdata)
    {
      hr = getTree()->getDOMcreator()->createTextNode(bstr_content, &theContent.theDOMText);
    }
    else
    {
      IXMLDOMCDATASection   *cdata_node;
      hr = getTree()->getDOMcreator()->createCDATASection(bstr_content, &cdata_node);
      theContent.theDOMText = cdata_node;//CDATA is derived from Text
    }
    if(FAILED(hr))
      ZORBA_ERROR(XQP0027_MS_DOM_ERROR);
  }
  else
  {
    theContent.theDOMText = dom_text;
    theContent.theDOMText->AddRef();
  }

  setParent(tree, parent, pos, store::StoreConsts::textNode);

  setText(content);

  if (parent)
  {
    ElementNode* p = dynamic_cast<ElementNode*>(parent);
    ZORBA_FATAL(p == NULL || !p->haveTypedValue(), "");

    //-if (pos < 0)
    //-  parent->children().push_back(this, false);
    //-else
    //-  parent->children().insert(this, pos, false);
    //connect(parent, pos);
    parent->add_child(this, pos, false);
  }

  NODE_TRACE1("Constructed text node " << this << " parent = "
              << std::hex << (parent ? (ulong)parent : 0) << " pos = " << pos
              << " tree = " << getTree()->getId() << ":" << getTree()
              << " ordpath = " << theOrdPath.show() << " content = "
              << *getText());
}


/*******************************************************************************

********************************************************************************/
TextNode::TextNode(
    XmlNode*          parent,
    store::Item_t&    content,
    bool              isListValue,
    IXMLDOMText      *dom_text)
  :
  XmlNode(NULL, parent)
{
  assert(parent != NULL);

  theContent.theDOMText = NULL;
  
  if(!dom_text)
  {//create the MS DOM Text Node
    HRESULT   hr;
    CMyBSTR  bstr_content("");

    hr = getTree()->getDOMcreator()->createTextNode(bstr_content, &theContent.theDOMText);
    if(FAILED(hr))
      ZORBA_ERROR(XQP0027_MS_DOM_ERROR);
  }
  else
  {
    theContent.theDOMText = dom_text;
    theContent.theDOMText->AddRef();
  }
  
  setParent(NULL, parent, -1, store::StoreConsts::textNode);

  setValue(content);

  ElementNode* p = reinterpret_cast<ElementNode*>(parent);

  ZORBA_ASSERT(p->numChildren() == 0);
  ZORBA_ASSERT(p->haveValue() && !p->haveEmptyValue());

  //-p->children().push_back(this, false);
  //connect(p, -1);
  p->add_child(this, -1, false);

  p->setHaveTypedValue();
  if (isListValue)
    p->setHaveListValue();

  NODE_TRACE1("Constructed text node " << this << " parent = "
              << std::hex << (parent ? (ulong)parent : 0) 
              << " ordpath = " << theOrdPath.show() << " content = "
              << getValue()->getStringValue()->c_str());
}


/*******************************************************************************

********************************************************************************/
TextNode::~TextNode()
{
  if (isTyped())
  {
    theContent.setValue(NULL);
  }
#ifdef ZORBA_MSDOM_IN_NATIVE_STORE_CACHING
  else
    theContent.setText(NULL);
#endif
  if(theContent.theDOMText)
    theContent.theDOMText->Release();
}

void TextNodeContent::setText(xqpStringStore_t& text, bool update_DOM)
{
#ifdef ZORBA_MSDOM_IN_NATIVE_STORE_CACHING
  if (theContent.text != NULL)
    theContent.text->removeReference(NULL
                                  SYNC_PARAM2(theContent.text->getRCLock()));

  theContent.text = text;
  if(theContent.text != NULL)
    theContent.text->addReference(NULL
                                    SYNC_PARAM2(theContent.text->getRCLock()));
#endif
  if(update_DOM && text && theDOMText)
  {
    CMyBSTR  bstr_text(text->c_str());
    CComVariant var_text(bstr_text);
    theDOMText->put_nodeValue(var_text);
  }
}

void TextNodeContent::setText(xqpStringStore* text, bool update_DOM)
{
#ifdef ZORBA_MSDOM_IN_NATIVE_STORE_CACHING
  if (theContent.text != NULL)
    theContent.text->removeReference(NULL
                                  SYNC_PARAM2(theContent.text->getRCLock()));

  theContent.text = text;
  if(theContent.text != NULL)
    theContent.text->addReference(NULL
                                    SYNC_PARAM2(theContent.text->getRCLock()));
#endif
  if(update_DOM && text && theDOMText)
  {
    CMyBSTR  bstr_text(text->c_str());
    CComVariant var_text(bstr_text);
    theDOMText->put_nodeValue(var_text);
  }
}

void TextNodeContent::setValue(store::Item_t& val, bool update_DOM)
{
  if (theContent.value != NULL)
    theContent.value->removeReference(NULL
                                    SYNC_PARAM2(theContent.value->getRCLock()));

  theContent.value = val;
  if(theContent.value != NULL)
    theContent.value->addReference(NULL
                                  SYNC_PARAM2(theContent.value->getRCLock()));

  if(update_DOM && val && theDOMText)
  {
    CMyBSTR  bstr_text(val->getStringValue()->c_str());
    CComVariant var_text(bstr_text);
    theDOMText->put_nodeValue(var_text);
  }
}

void TextNodeContent::setValue(store::Item* val, bool update_DOM)
{
  if (theContent.value != NULL)
    theContent.value->removeReference(NULL
                                    SYNC_PARAM2(theContent.value->getRCLock()));

  theContent.value = val;
  if(theContent.value != NULL)
    theContent.value->addReference(NULL
                                  SYNC_PARAM2(theContent.value->getRCLock()));

  if(update_DOM && val && theDOMText)
  {
    CMyBSTR  bstr_text(val->getStringValue()->c_str());
    CComVariant var_text(bstr_text);
    theDOMText->put_nodeValue(var_text);
  }
}

xqpStringStore* TextNodeContent::getText() const      
{
#ifdef ZORBA_MSDOM_IN_NATIVE_STORE_CACHING
  return theContent.text;
#endif
  VARIANT  var_text;
  theDOMText->get_nodeValue(&var_text);
  char  *str_text = NULL;
  int   len;

  len = WideCharToMultiByte(CP_UTF8, 0, var_text.bstrVal, -1, NULL, 0, NULL, NULL);
  str_text = (char*)::malloc(len+1);
  WideCharToMultiByte(CP_UTF8, 0, var_text.bstrVal, -1, str_text, len, NULL, NULL);
  str_text[len] = 0;

  xqpStringStore *xss = new xqpStringStore(str_text);
  ::free(str_text);
  return xss;
}

/*******************************************************************************

********************************************************************************/
bool TextNode::isTyped() const 
{
  ElementNode* p = dynamic_cast<ElementNode*>(theParent);

  if (p == NULL)
    return false;

  return p->haveTypedValue();
}


/*******************************************************************************

********************************************************************************/
XmlNode* TextNode::copy2(
    XmlNode*               rootParent,
    XmlNode*               parent,
    long                   pos,
    const store::CopyMode& copymode) const
{
  assert(parent != NULL || rootParent == NULL);

  XmlTree* tree = NULL;
  TextNode* copyNode = NULL;
  xqpStringStore_t textContent;
  store::Item_t typedContent;

  try
  {
    if (parent == NULL)
    {
      tree = new XmlTree(NULL, GET_STORE().getTreeId());

      textContent = getText();
      copyNode = new TextNode(tree, NULL, pos, textContent, is_cdata);
    }
    else
    {
      // Merge adjacent text nodes (if we don't merge, then a query which, say,
      // counts the number of text nodes of some element node will return the
      // wrong result).
      ulong pos2 = (pos >= 0 ? pos : parent->numChildren());
   
      XmlNode* lsib = (pos2 > 0 ? parent->getChild(pos2-1) : NULL);

      if (!is_cdata && 
        lsib != NULL && lsib->getNodeKind() == store::StoreConsts::textNode && !lsib->get_isCDATA())
      {
        TextNode* textSibling = reinterpret_cast<TextNode*>(lsib);

        ZORBA_ASSERT(!isTyped());
        ZORBA_ASSERT(!textSibling->isTyped());

        if (lsib->theParent == parent)
        {
          textContent = textSibling->getText()->append(getText());
          textSibling->setText(textContent);
          copyNode = textSibling;
        }
        else
        {
          textContent = textSibling->getText()->append(getText());

          parent->removeChild(pos2-1);

          copyNode = new TextNode(tree, parent, pos2-1, textContent, false);
        }
      }
      // Skip copy if caller says so.
      else if (!copymode.theDoCopy)
      {
        //-if (pos < 0)
        //-  parent->children().push_back(const_cast<TextNode*>(this), true);
        //-else
        //-  parent->children().insert(const_cast<TextNode*>(this), pos, true);
        parent->add_child((XmlNode*)this, pos, true);
        
        copyNode = const_cast<TextNode*>(this);
      }
      // Regular copy
      else if (isTyped())
      {
        if (copymode.theTypePreserve)
        {
          ElementNode* myParent = reinterpret_cast<ElementNode*>(theParent);
          typedContent = getValue();
          copyNode = new TextNode(parent, typedContent, myParent->haveListValue());
        }
        else
        {
          textContent = getValue()->getStringValue();
          copyNode = new TextNode(NULL, parent, pos, textContent, is_cdata);
        }
      }
      else
      {
        textContent = getText();
        copyNode = new TextNode(NULL, parent, pos, textContent, is_cdata);
      }
    }
  }
  catch (...)
  {
    if (tree) delete tree;
    throw;
  }

  NODE_TRACE1("Copied text node " << this << " to node " << copyNode
              << " parent = " << std::hex << (parent ? (ulong)parent : 0)
              << " pos = " << pos);

  return copyNode;
}


/*******************************************************************************

********************************************************************************/
store::Item* TextNode::getType() const
{
  return GET_STORE().theSchemaTypeNames[XS_UNTYPED_ATOMIC];
}


void TextNode::getTypedValue(store::Item_t& val, store::Iterator_t& iter) const
{
  xqpStringStore_t rch;

  if (isTyped())
  {
    rch = getValue()->getStringValue();
    val = new UntypedAtomicItemImpl(rch);
  }
  else
  {
    rch = getText(); 
    val = new UntypedAtomicItemImpl(rch);
  }
  iter = NULL;
}


store::Item_t TextNode::getAtomizationValue() const
{
  xqpStringStore_t rch;

  if (isTyped())
  {
    rch = getValue()->getStringValue();
    return new UntypedAtomicItemImpl(rch);
  }
  else
  {
    rch = getText(); 
    return new UntypedAtomicItemImpl(rch);
  }
}


xqpStringStore_t TextNode::getStringValue() const
{
  if (isTyped())
  {
    return getValue()->getStringValue();
  }
  else
  {
    return getText();
  }
}


/*******************************************************************************

********************************************************************************/
xqp_string TextNode::show() const
{
  return xqpString::concat("<text nid=\"", theOrdPath.show(), "\">", getStringValue(), "</text>");
}


/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
//  class PiNode                                                               //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////



/*******************************************************************************

********************************************************************************/
PiNode::PiNode(
    XmlTree*          tree,
    XmlNode*          parent,
    long              pos,
    xqpStringStore_t& target,
    xqpStringStore_t& content,
    IXMLDOMProcessingInstruction      *dom_pi)
  :
  XmlNode(tree, parent)
{
#ifdef ZORBA_MSDOM_IN_NATIVE_STORE_CACHING
  theTarget = target;
  theContent = content;
#endif

  theDOMpi = NULL;
  
  if(!dom_pi)
  {
    CMyBSTR  bstr_target(target->c_str());
    CMyBSTR  bstr_content(content->c_str());
    HRESULT   hr;

    hr = getTree()->getDOMcreator()->createProcessingInstruction(bstr_target, bstr_content, &theDOMpi);
    if(FAILED(hr))
      ZORBA_ERROR(XQP0027_MS_DOM_ERROR);
  }
  else
  {
    theDOMpi = dom_pi;
    theDOMpi->AddRef();
  }
  
  setParent(tree, parent, pos, store::StoreConsts::piNode);

  if (parent)
  {
    //-if (pos < 0)
    //-  parent->children().push_back(this, false);
    //-else
    //-  parent->children().insert(this, pos, false);
    parent->add_child(this, pos, false);
  }

  NODE_TRACE1("Constructed pi node " << this << " parent = "
              << std::hex << (parent ? (ulong)parent : 0) << " pos = " << pos
              << " tree = " << getTree()->getId() << ":" << getTree()
              << " ordpath = " << theOrdPath.show() << " target = " << *theTarget);
}


/*******************************************************************************

********************************************************************************/
PiNode::~PiNode()
{
  NODE_TRACE1("Deleted pi node " << this);
  if(theDOMpi)
    theDOMpi->Release();
}


/*******************************************************************************

********************************************************************************/
XmlNode* PiNode::copy2(
    XmlNode*               rootParent,
    XmlNode*               parent,
    long                   pos,
    const store::CopyMode& copymode) const
{
  assert(parent != NULL || rootParent == NULL);

  PiNode* copyNode = NULL;
  XmlTree* tree = NULL;
  xqpStringStore_t content;
  xqpStringStore_t target;

  try
  {
    if (parent == NULL)
    {
      tree = new XmlTree(NULL, GET_STORE().getTreeId());

      target = getTarget();
      content = getStringValue();
      copyNode = new PiNode(tree, NULL, pos, target, content);
    }
    // Skip copy if caller says so.
    else if (!copymode.theDoCopy)
    {
      //-if (pos < 0)
      //-  parent->children().push_back(const_cast<PiNode*>(this), true);
      //-else
      //-  parent->children().insert(const_cast<PiNode*>(this), pos, true);
      parent->add_child((XmlNode*)this, pos, true);

      copyNode = const_cast<PiNode*>(this);
    }
    else
    {
      target = getTarget();
      content = getStringValue();
      copyNode = new PiNode(tree, parent, pos, target, content);
    }
  }
  catch (...)
  {
    if (tree) delete tree;
    throw;
  }

  NODE_TRACE1("Copied pi node " << this << " to node " << copyNode
              << " parent = " << std::hex << (parent ? (ulong)parent : 0)
              << " pos = " << pos);

  return copyNode;
}


/*******************************************************************************

********************************************************************************/
store::Item* PiNode::getType() const
{
  return GET_STORE().theSchemaTypeNames[XS_UNTYPED_ATOMIC];
}


void PiNode::getTypedValue(store::Item_t& val, store::Iterator_t& iter) const
{
  xqpStringStore_t rch = getStringValue(); 
  val = new StringItemNaive(rch);
  iter = NULL;
}


store::Item_t PiNode::getAtomizationValue() const
{
  xqpStringStore_t rch = getStringValue(); 
  return new StringItemNaive(rch);
}

xqpStringStore_t PiNode::getStringValue() const   
{
#ifdef ZORBA_MSDOM_IN_NATIVE_STORE_CACHING
  return theContent;
#else
  VARIANT  var_text;
  theDOMpi->get_nodeValue(&var_text);
  char  *str_text = NULL;
  int   len;

  len = WideCharToMultiByte(CP_UTF8, 0, val_text.bstrVal, -1, NULL, 0, NULL, NULL);
  str_text = (char*)::malloc(len+1);
  WideCharToMultiByte(CP_UTF8, 0, var_text.bstrVal, -1, str_text, len, NULL, NULL);
  str_text[len] = 0;

  xqpStringStore *xss = new xqpStringStore(str_text);
  ::free(str_text);
  return xss;
#endif
}


/*******************************************************************************

********************************************************************************/
xqp_string PiNode::show() const
{
  return "<?" + getTarget()->str() + " " + getStringValue()->str() + "?>";
}


/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
//  class CommentNode                                                          //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////


/*******************************************************************************

********************************************************************************/
CommentNode::CommentNode(
    XmlTree*          tree,
    XmlNode*          parent,
    long              pos,
    xqpStringStore_t& content,
    IXMLDOMComment    *dom_comment)
  :
  XmlNode(tree, parent)
{
#ifdef ZORBA_MSDOM_IN_NATIVE_STORE_CACHING
  theContent = content;
#endif

  theDOMComment = NULL;
  
  if(!dom_comment)
  {
    CMyBSTR  bstr_content(content->c_str());
    HRESULT   hr;

    hr = getTree()->getDOMcreator()->createComment(bstr_content, &theDOMComment);
    if(FAILED(hr))
      ZORBA_ERROR(XQP0027_MS_DOM_ERROR);
  }
  else
  {
    theDOMComment = dom_comment;
    theDOMComment->AddRef();
  }
  
  setParent(tree, parent, pos, store::StoreConsts::commentNode);

  if (parent)
  {
    //-if (pos < 0)
    //-  parent->children().push_back(this, false);
    //-else
    //-  parent->children().insert(this, pos, false);
    parent->add_child(this, pos, false);
  }

  NODE_TRACE1("Constructed comment node " << this << " parent = "
              << std::hex << (parent ? (ulong)parent : 0) << " pos = " << pos
              << " tree = " << getTree()->getId() << ":" << getTree()
              << " ordpath = " << theOrdPath.show() << " content = "
              << getStringValue()->c_str());
}


/*******************************************************************************

********************************************************************************/
CommentNode::~CommentNode()
{
  NODE_TRACE1("Deleted comment node " << this);
  if(theDOMComment)
    theDOMComment->Release();
}


/*******************************************************************************

********************************************************************************/
XmlNode* CommentNode::copy2(
    XmlNode*               rootParent,
    XmlNode*               parent,
    long                   pos,
    const store::CopyMode& copymode) const
{
  assert(parent != NULL || rootParent == NULL);

  CommentNode* copyNode = NULL;
  XmlTree* tree = NULL;
  xqpStringStore_t content;

  try
  {
    if (parent == NULL)
    {
      tree = new XmlTree(NULL, GET_STORE().getTreeId());

      content = getStringValue();
      copyNode = new CommentNode(tree, NULL, pos, content);
    }
    // Skip copy if caller says so.
    else if (!copymode.theDoCopy)
    {
      //-if (pos < 0)
      //-  parent->children().push_back(const_cast<CommentNode*>(this), true);
      //-else
      //-  parent->children().insert(const_cast<CommentNode*>(this), pos, true);
      parent->add_child((XmlNode*)this, pos, true);
      
      copyNode = const_cast<CommentNode*>(this);
    }
    else
    {
      content = getStringValue();
      copyNode = new CommentNode(tree, parent, pos, content);
    }
  }
  catch (...)
  {
    if (tree) delete tree;
    throw;
  }

  NODE_TRACE1("Copied coment node " << this << " to node " << copyNode
              << " parent = " << std::hex << (parent ? (ulong)parent : 0)
              << " pos = " << pos);

  return copyNode;
}


/*******************************************************************************

********************************************************************************/
store::Item* CommentNode::getType() const
{
  return GET_STORE().theSchemaTypeNames[XS_UNTYPED_ATOMIC];
}


void CommentNode::getTypedValue(store::Item_t& val, store::Iterator_t& iter) const
{
  xqpStringStore_t rch = getStringValue(); 
  val = new StringItemNaive(rch);
  iter = NULL;
}


store::Item_t CommentNode::getAtomizationValue() const
{
  xqpStringStore_t rch = getStringValue(); 
  return new StringItemNaive(rch);
}

xqpStringStore_t CommentNode::getStringValue() const   
{
#ifdef ZORBA_MSDOM_IN_NATIVE_STORE_CACHING
  return theContent;
#else
  VARIANT var_text;
  theDOMComment->get_nodeValue(&var_text);
  char  *str_text = NULL;
  int   len;

  len = WideCharToMultiByte(CP_UTF8, 0, var_text.bstrVal, -1, NULL, 0, NULL, NULL);
  str_text = (char*)::malloc(len+1);
  WideCharToMultiByte(CP_UTF8, 0, var_text.bstrVal, -1, str_text, len, NULL, NULL);
  str_text[len] = 0;

  xqpStringStore *xss = new xqpStringStore(str_text);
  ::free(str_text);
  return xss;
#endif
}

xqpStringStore* CommentNode::getStringValueP() const   
{
#ifdef ZORBA_MSDOM_IN_NATIVE_STORE_CACHING
  return theContent.getp();
#else
  VARIANT  var_text;
  theDOMComment->get_text(&var_text);
  char  *str_text = NULL;
  int   len;

  len = WideCharToMultiByte(CP_UTF8, 0, var_text.bstrVal, -1, NULL, 0, NULL, NULL);
  str_text = (char*)::malloc(len+1);
  WideCharToMultiByte(CP_UTF8, 0, var_text.bstrVal, -1, str_text, len, NULL, NULL);
  str_text[len] = 0;

  temp_xqpString = new xqpStringStore(str_text);
  ::free(str_text);
  return temp_xqpString.getp();
#endif
}

/*******************************************************************************

********************************************************************************/
xqp_string CommentNode::show() const
{
  return "<!--" + getStringValue()->str() + "-->";
}

} // namespace store
} // namespace zorba

