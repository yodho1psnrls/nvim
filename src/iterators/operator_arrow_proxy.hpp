#pragma once

// NOTE: Taken from boost
// https://www.boost.org/doc/libs/1_88_0/boost/intrusive/detail/transform_iterator.hpp

template <class T>
struct operator_arrow_proxy {

  inline operator_arrow_proxy(const T &px) : m_value(px) {}

  inline T* operator->() const { return &m_value; }
  
  // This function is needed for MWCW and BCC, which won't call operator->
  // again automatically per 13.3.1.2 para 8
  // operator T*() const { return &m_value; }

  mutable T m_value;

};


template <class T>
struct operator_arrow_proxy<T&> {

  inline operator_arrow_proxy(T &px) : m_value(px) {}

  inline T* operator->() const { return &m_value; }

  // This function is needed for MWCW and BCC, which won't call operator->
  // again automatically per 13.3.1.2 para 8
  // operator T*() const { return &m_value; }

  T &m_value;

};

