// $Id: listmap.tcc,v 1.7 2015-04-28 19:22:02-07 - - $
// Partner: Darius Sakhapour(dsakhapo@ucsc.edu)
// Partner: Ryan Wong (rystwong@ucsc.edu)

#include "listmap.h"
#include "trace.h"

//
/////////////////////////////////////////////////////////////////
// Operations on listmap::node.
/////////////////////////////////////////////////////////////////
//

//
// listmap::node::node (link*, link*, const value_type&)
//
template <typename Key, typename Value, class Less>
listmap<Key,Value,Less>::node::node (node* next, node* prev,
                                     const value_type& value):
            link (next, prev), value (value) {
}

//
/////////////////////////////////////////////////////////////////
// Operations on listmap.
/////////////////////////////////////////////////////////////////
//

//
// listmap::~listmap()
//
template <typename Key, typename Value, class Less>
listmap<Key,Value,Less>::~listmap() {
   TRACE ('l', (void*) this);
}

//
// iterator listmap::insert (const value_type&)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::insert (const value_type& pair) {
   listmap<Key,Value,Less>::iterator curr;
   //Check if the listmap is empty first
   if(empty()){
      anchor_.next = new node(anchor(), anchor(), pair);
      anchor_.prev = anchor_.next;
      curr = begin();
   }
   else{
      curr = find(pair.first);
      //If the same key already exists, change it's value
      if(curr != end()){
         curr->second = pair.second;
      }
      //If the key doesn't exist yet
      else{
         curr = begin();
         while(curr != end() and less(curr->first, pair.first)){
            ++curr;
         }
         node* temp = new node(nullptr, nullptr, pair);
         //Have temp point to the next and prev nodes,
         //then have the prev node have its 'next' point to temp,
         //then have the next node have its 'prev' point to temp.
         temp->next = curr.where; --curr;
         temp->prev = curr.where;
         curr.where->next = temp;
         curr = temp->next;
         curr.where->prev = temp;
         --curr;  //curr now points to temp
      }
   }
   TRACE ('l', &pair << "->" << pair);
   return curr;
}

//
// listmap::find(const key_type&)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::find (const key_type& that) {
   listmap<Key,Value,Less>::iterator find = begin();
   while(find != end()){
      if(find->first == that) return find;
      ++find;
   }
   TRACE ('l', that);
   return find;
}

//
// iterator listmap::erase (iterator position)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::erase (iterator position) {
   listmap<Key,Value,Less>::iterator temp = position.where->next;
   position.where->next->prev = position.where->prev;
   position.where->prev->next = position.where->next;
   position.erase();
   TRACE ('l', &*position);
   return temp;
}

//
/////////////////////////////////////////////////////////////////
// Operations on listmap::iterator.
/////////////////////////////////////////////////////////////////
//

//
// listmap::value_type& listmap::iterator::operator*()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::value_type&
listmap<Key,Value,Less>::iterator::operator*() {
   TRACE ('l', where);
   return where->value;
}

//
// listmap::value_type* listmap::iterator::operator->()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::value_type* 
listmap<Key,Value,Less>::iterator::operator->() {
   TRACE ('l', where);
   return &(where->value);
}

//
// listmap::iterator& listmap::iterator::operator++()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator&
listmap<Key,Value,Less>::iterator::operator++() {
   TRACE ('l', where);
   where = where->next;
   return *this;
}

//
// listmap::iterator& listmap::iterator::operator--()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator&
listmap<Key,Value,Less>::iterator::operator--() {
   TRACE ('l', where);
   where = where->prev;
   return *this;
}

//
// bool listmap::iterator::operator== (const iterator&)
//
template <typename Key, typename Value, class Less>
inline bool listmap<Key,Value,Less>::iterator::operator==
            (const iterator& that) const {
   return this->where == that.where;
}

//
// bool listmap::iterator::operator!= (const iterator&)
//
template <typename Key, typename Value, class Less>
inline bool listmap<Key,Value,Less>::iterator::operator!=
            (const iterator& that) const {
   return this->where != that.where;
}

