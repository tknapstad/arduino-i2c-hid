#ifndef __SERIALIZABLE_H
#define __SERIALIZABLE_H

template< typename T >
union Serializable {
   T data;
   unsigned char bytes[sizeof(T)];

   Serializable() { memset(bytes, 0, size()); }
   inline const size_t size() { return sizeof(T); }
   inline const unsigned char *begin() { return bytes; }
   inline const unsigned char *end() { return bytes + size; }
};

#endif
