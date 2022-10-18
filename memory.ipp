#include "memory.h"
#pragma once

namespace Lunaris {

	template<typename T>
	inline void Memory<T>::__make_valid(T*&& ptr)
	{
		if (m_data) {
			std::lock_guard<std::mutex> l(m_data->m_safe);

			if (m_data->shareable) {
				m_data->destr(m_data->shareable);
			}

			m_data->shareable = ptr;
			ptr = nullptr;
		}
		else {
			m_data = new block_data();
			m_data->shareable = ptr;
			m_data->count = 1;
			ptr = nullptr;
		}
	}

	template<typename T>
	inline void Memory<T>::__make_valid()
	{
		if (!m_data) {
			m_data = new block_data();
			m_data->shareable = new T();
			m_data->count = 1;
		}
		else if (!m_data->shareable) {
			std::lock_guard<std::mutex> l(m_data->m_safe);
			m_data->shareable = new T();
		}
	}

	template<typename T>
	inline void Memory<T>::__make_valid(T&& oth)
	{
		if (!m_data) {
			m_data = new block_data();
			m_data->shareable = new T{ (T&&)oth };
			m_data->count = 1;
		}
		else if (!m_data->shareable) {
			std::lock_guard<std::mutex> l(m_data->m_safe);
			m_data->shareable = new T{ (T&&)oth };
		}
		else {
			std::lock_guard<std::mutex> l(m_data->m_safe);
			(*m_data->shareable) = (T&&)oth;
		}
	}

	template<typename T>
	inline void Memory<T>::__make_valid(const T& oth)
	{
		if (!m_data) {
			m_data = new block_data();
			m_data->shareable = new T{ oth };
			m_data->count = 1;
		}
		else if (!m_data->shareable) {
			std::lock_guard<std::mutex> l(m_data->m_safe);
			m_data->shareable = new T{ oth };
		}
		else {
			std::lock_guard<std::mutex> l(m_data->m_safe);
			(*m_data->shareable) = oth;
		}
	}

	template<typename T>
	inline void Memory<T>::__unref_auto()
	{
		if (!m_data) return;

		std::unique_lock<std::mutex> l(m_data->m_safe);
		if (--m_data->count == 0) {
			if (m_data->shareable) {
				m_data->destr(m_data->shareable);
			}
			m_data->shareable = nullptr;
			l.unlock();
			delete m_data;
		}

		m_data = nullptr;
	}

	template<typename T>
	inline Memory<T>::Memory(const T& oth, std::function<void(T*)> destr)
	{
		__make_valid(oth);
		if (!destr) throw - 3; // you can set an "empty-like" one, but if it's empty, consider this a bug
		m_data->destr = destr;
	}

	template<typename T>
	inline Memory<T>::Memory(T&& oth, std::function<void(T*)> destr)
	{
		__make_valid((T&&)(oth));
		if (!destr) throw - 3; // you can set an "empty-like" one, but if it's empty, consider this a bug
		m_data->destr = destr;
	}

	template<typename T>
	inline Memory<T>::Memory(T*&& oth, std::function<void(T*)> destr)
	{
		__make_valid((T*&&)oth);
		if (!destr) throw - 3; // you can set an "empty-like" one, but if it's empty, consider this a bug
		m_data->destr = destr;
	}

	template<typename T>
	inline Memory<T>::Memory(Memory&& oth)
		: m_data(oth.m_data)
	{
		if (m_data) {
			std::lock_guard<std::mutex> l(m_data->m_safe);
			oth.m_data = nullptr;
		}
	}

	template<typename T>
	inline Memory<T>::Memory(const Memory& oth)
		: m_data(oth.m_data)
	{
		if (m_data) {
			std::lock_guard<std::mutex> l(m_data->m_safe);
			++m_data->count;
		}
	}

	template<typename T>
	inline Memory<T>::~Memory()
	{
		__unref_auto();
	}

	template<typename T>
	inline Memory<T>& Memory<T>::operator=(const T& oth)
	{
		__make_valid(oth);
		return *this;
	}

	template<typename T>
	inline Memory<T>& Memory<T>::operator=(T&& oth)
	{
		__make_valid((T&&)oth);
		return *this;
	}

	template<typename T>
	inline Memory<T>& Memory<T>::operator=(T*&& oth)
	{
		__make_valid((T*&&)oth);
		return *this;
	}

	template<typename T>
	inline Memory<T>& Memory<T>::operator=(Memory&& oth)
	{
		__unref_auto();
		m_data = oth.m_data;
		if (m_data) {
			std::lock_guard<std::mutex> l(m_data->m_safe);
			oth.m_data = nullptr;
		}
		return *this;
	}

	template<typename T>
	inline Memory<T>& Memory<T>::operator=(const Memory& oth)
	{
		__unref_auto();
		m_data = oth.m_data;
		if (m_data) {
			std::lock_guard<std::mutex> l(m_data->m_safe);
			++m_data->count;
		}
		return *this;
	}

	template<typename T>
	inline T* Memory<T>::operator->()
	{
		return m_data ? m_data->shareable : nullptr;
	}

	template<typename T>
	inline const T* Memory<T>::operator->() const
	{
		return m_data ? m_data->shareable : nullptr;
	}

	template<typename T>
	inline T& Memory<T>::operator*()
	{
		if (m_data) return *m_data->shareable;
		throw - 2;
	}

	template<typename T>
	inline const T& Memory<T>::operator*() const
	{
		if (m_data) return *m_data->shareable;
		throw - 2;
	}

	template<typename T>
	inline T* Memory<T>::get()
	{
		return m_data ? m_data->shareable : nullptr;
	}

	template<typename T>
	inline const T* Memory<T>::get() const
	{
		return m_data ? m_data->shareable : nullptr;
	}

	template<typename T>
	inline size_t Memory<T>::use_count() const
	{
		return m_data ? m_data->count : 0;
	}

	template<typename T>
	void Memory<T>::set_destructor(std::function<void(T*)> destr)
	{
		if (!m_data) return;
		if (!destr) throw -3; // you can set an "empty-like" one, but if it's empty, consider this a bug
		std::lock_guard<std::mutex> l(m_data->m_safe);
		m_data->destr = destr;
	}

	template<typename T>
	inline T* Memory<T>::release()
	{
		if (!m_data) return nullptr;
		std::lock_guard<std::mutex> l(m_data->m_safe);
		T* ptr = m_data->shareable;
		m_data->shareable = nullptr;
		return ptr;
	}

	template<typename T>
	inline void Memory<T>::reset()
	{
		__unref_auto();
	}

	template<typename T>
	inline bool Memory<T>::has_value() const
	{
		return m_data && m_data->shareable;
	}

	template<typename T>
	inline Memory<T>::operator bool() const
	{
		return m_data && m_data->shareable;
	}
}