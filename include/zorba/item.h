#ifndef ZORBA_ITEM_API_H
#define ZORBA_ITEM_API_H

#include <zorba/api_shared_types.h>

namespace zorba {

namespace store { class Item; }

class Item
{
public:
  Item();
  Item(const Item& other);
  Item(store::Item *item);
  ~Item();

  Item
  getType() const;

  Item
  getAtomizationValue() const;

  String
  getStringValue() const;

  bool
  isNode() const;

  bool
  isAtomic() const;

  bool
  isNull() const;

  void
  close();

  const Item& operator =(const Item& rhs);
  const Item& operator =(store::Item *rhs);

private:
  friend class Unmarshaller;
  friend class SAX2_XMLReaderNative;
  store::Item *m_item;

  store::Item * getInternalItem() const { return m_item; }
};

} // namespace zorba

#endif
