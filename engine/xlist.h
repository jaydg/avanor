/*
This file is part of "Avanor, the Land of Mystery" roguelike game
Home page: http://www.avanor.com/
Copyright (C) 2000-2003 Vadim Gaidukevich

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef __XLIST_H
#define __XLIST_H

#include "engine/xobject.h"

template <class T> class XList
{
        struct XNode {
            XNode* pNext;
            XNode* pPrev;
            XObject* o;
        };

        XNode* pBeginList;
    public:
        XList()
        {
            pBeginList = iterator::CreateNode();
            pBeginList->o = NULL;
            pBeginList->pNext = pBeginList;
            pBeginList->pPrev = pBeginList;
        }

        ~XList()
        {
            erase(begin(), end());
            iterator::DestroyNode(pBeginList);
        }

        class iterator
        {
            protected:
                friend class XList<T>;
                XNode* position;

                static XNode* CreateNode()
                {
                    return new XNode();
                }

                static void DestroyNode(XNode * pNode)
                {
                    delete pNode;
                }

            public:
                iterator() {}

                iterator(XNode * pNode) : position(pNode) {}

                void operator++()
                {
                    assert(position);
                    position = position->pNext;

                    while (true) {
                        //position->o == NULL means end of list
                        if (position->o == NULL || position->o->isValid()) {
                            return;
                        }

                        destroy();
                    }
                }

                void operator++(int)
                {
                    operator++();
                }

                void operator--()
                {
                    assert(position);
                    position = position->pPrev;
                }

                void operator--(int)
                {
                    operator--();
                }

                iterator &operator=(const iterator& x)
                {
                    position = x.position;
                    return *this;
                }

                bool operator==(const iterator& x)
                {
                    return position == x.position;
                }

                bool operator!=(const iterator& x)
                {
                    return !(position == x.position);
                }

                T operator*()
                {
                    return (T)position->o;
                }

                T operator->()
                {
                    return (T)position->o;
                }

                operator T()
                {
                    return (T)position->o;
                }

                iterator destroy()
                {
                    XNode * pNext = position->pNext;
                    XNode * pPrev = position->pPrev;
                    pPrev->pNext = pNext;
                    pNext->pPrev = pPrev;
                    position->o->Release();
                    DestroyNode(position);
                    position = pNext;
                    return iterator(pNext);
                }
        };

        iterator begin()
        {
            XNode * pNode = pBeginList->pNext;

            while (true) {
                if (pNode->o == NULL || pNode->o->isValid()) {
                    return iterator(pNode);
                }

                pNode = erase(iterator(pNode)).position;
            }
        }

        iterator end()
        {
            return iterator(pBeginList);
        }

        bool empty()
        {
            //we need to call begin for remove invalid objects
            return (begin().position->o == NULL);
        }

        iterator insert(iterator it, T object)
        {
            assert(object);
            XNode * tNode = iterator::CreateNode();
            tNode->o = object;
            object->AddRef();
            tNode->pNext = it.position;
            tNode->pPrev = it.position->pPrev;
            it.position->pPrev->pNext = tNode;
            it.position->pPrev = tNode;
            it.position = tNode;
            return it;
        }

        iterator erase(iterator it)
        {
            return it.destroy();
        }

        iterator erase(iterator begin, iterator end)
        {
            while (begin != end) {
                begin = erase(begin);
            }

            return begin;
        }

        void clear()
        {
            return erase(begin(), end());
        }

        void Add(T object)
        {
            push_back(object);
        }

        void push_front(const T& o)
        {
            insert(begin(), o);
        }

        void push_back(const T& o)
        {
            insert(end(), o);
        }

        void pop_front()
        {
            erase(begin());
        }

        void pop_back()
        {
            iterator tmp = end();
            erase(--tmp);
        }

        size_t size()
        {
            size_t count = 0;

            for (iterator it = begin(); it != end(); it++) {
                count++;
            }

            return count;
        }

        iterator Find(XGUID xguid)
        {
            for (iterator it = begin(); it != end(); it++)
                if ((*it)->guid() == xguid) {
                    return it;
                }

            return end();
        }

        T Remove(XGUID xguid)
        {
            return Remove(Find(xguid));
        }

        T Remove(iterator it)
        {
            if (it == end()) {
                return NULL;
            }

            T p = (*it);
            it = erase(it);
            return p;
        }

        T RemoveFirst()
        {
            return Remove(begin());
        }

        void KillAll()
        {
            T o;

            while ((o = RemoveFirst())) {
                o->Invalidate();
            }
        }

        void StoreList(XFile * f)
        {
            unsigned long count = size();
            f->Write(&count, sizeof(count));

            for (iterator it = begin(); it != end(); ++it) {
                XObject::StorePointer(f, it);
            }
        }

        void RestoreList(XFile * f)
        {
            assert(empty());
            unsigned long count = 0;
            f->Read(&count, sizeof(count));

            while (count-- > 0) {
                T p = (T)XObject::RestorePointer(f, NULL);
                push_back(p);
            }
        }
};

template <class T> class XSortedList : public XList < T >
{
        typedef typename XList<T>::iterator XList_iteraror;
    public:
        XList_iteraror insert(XList_iteraror it, T object)
        {
            XList_iteraror i;

            for (i = this->begin(); i != this->end(); i++) {
                if (object->im < i->im) {
                    break;
                }

                if (object->im == i->im && object->Compare(i) <= 0) {
                    break;
                }
            }

            if (object->GetRef() == 0 && i != this->end() && i->GetRef() == 1
                && object->im == i->im && object->Compare(i) == 0) {
                i->Concat(object);
                return i;
            } else {
                return XList<T>::insert(i, object);
            }
        }

        void Add(T object)
        {
            push_back(object);
        }

        void push_front(const T& o)
        {
            insert(this->begin(), o);
        }

        void push_back(const T& o)
        {
            insert(this->end(), o);
        }

};

template <class T> class XQList
{
        struct XNode {
            XNode* pNext;
            XNode* pPrev;
            T o;
        };

        XNode* pBeginList;
    public:
        XQList()
        {
            pBeginList = iterator::CreateNode();
            pBeginList->pNext = pBeginList;
            pBeginList->pPrev = pBeginList;
        }

        ~XQList()
        {
            erase(begin(), end());
            iterator::DestroyNode(pBeginList);
        }

        class iterator
        {
            protected:
                friend class XQList<T>;
                XNode* position;

                static XNode* CreateNode()
                {
                    return new XNode();
                }

                static void DestroyNode(XNode * pNode)
                {
                    delete pNode;
                }

            public:
                iterator() {}

                iterator(XNode * pNode) : position(pNode) {}

                void operator++()
                {
                    assert(position);
                    position = position->pNext;
                }

                void operator++(int)
                {
                    operator++();
                }

                void operator--()
                {
                    assert(position);
                    position = position->pPrev;
                }

                void operator--(int)
                {
                    operator--();
                }

                iterator &operator=(const iterator& x)
                {
                    position = x.position;
                    return *this;
                }

                bool operator==(const iterator& x)
                {
                    return position == x.position;
                }

                bool operator!=(const iterator& x)
                {
                    return !(position == x.position);
                }

                T operator*()
                {
                    return (T)position->o;
                }

                iterator destroy()
                {
                    XNode * pNext = position->pNext;
                    XNode * pPrev = position->pPrev;
                    pPrev->pNext = pNext;
                    pNext->pPrev = pPrev;
                    DestroyNode(position);
                    position = pNext;
                    return iterator(pNext);
                }
        };

        iterator begin()
        {
            return iterator(pBeginList->pNext);
        }

        iterator end()
        {
            return iterator(pBeginList);
        }

        bool empty()
        {
            return (begin() == end());
        }

        iterator insert(iterator it, T object)
        {
            XNode * tNode = iterator::CreateNode();
            tNode->o = object;
            tNode->pNext = it.position;
            tNode->pPrev = it.position->pPrev;
            it.position->pPrev->pNext = tNode;
            it.position->pPrev = tNode;
            it.position = tNode;
            return it;
        }

        iterator erase(iterator it)
        {
            return it.destroy();
        }

        iterator erase(iterator begin, iterator end)
        {
            while (begin != end) {
                begin = erase(begin);
            }

            return begin;
        }

        void clear()
        {
            erase(begin(), end());
        }

        void Add(T object)
        {
            push_back(object);
        }

        void push_front(const T& o)
        {
            insert(begin(), o);
        }

        void push_back(const T& o)
        {
            insert(end(), o);
        }

        void pop_front()
        {
            erase(begin());
        }

        void pop_back()
        {
            iterator tmp = end();
            erase(--tmp);
        }

        size_t size()
        {
            size_t count = 0;

            for (iterator it = begin(); it != end(); it++) {
                count++;
            }

            return count;
        }

        T Remove(iterator it)
        {
            if (it == end()) {
                return NULL;
            }

            T p = (*it);
            it = erase(it);
            return p;
        }

        T RemoveFirst()
        {
            return Remove(begin());
        }

        T &front()
        {
            return pBeginList->pNext->o;
        }
};

#endif
