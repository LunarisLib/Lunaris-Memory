#pragma once

#include <mutex>

namespace Lunaris {

	template<typename T>
	class Memory {
		struct block_data {
			T* shareable = nullptr;
			size_t count = 1;
			mutable std::mutex m_safe;
		};
		block_data* m_data = nullptr;

		void __make_valid(T*&&);
		void __make_valid();
		void __make_valid(T&&);
		void __make_valid(const T&);

		void __unref_auto();
	public:
		Memory() = default;
		Memory(const T&);		// copy
		Memory(T&&);			// move
		Memory(T*&&);			// absorb
		Memory(Memory&&);		// move
		Memory(const Memory&);	// ref+
		~Memory();				// ref-

		Memory<T>& operator=(const T&);			// copy
		Memory<T>& operator=(T&&);				// move
		Memory<T>& operator=(T*&&);				// absorb
		Memory<T>& operator=(Memory&&);			// move
		Memory<T>& operator=(const Memory&);	// ref+

		T* operator->();
		const T* operator->() const;

		T& operator*();
		const T& operator*() const;

		T* get();
		const T* get() const;

		size_t use_count() const;

		T* release(); // take it from all, but keep all referenced to the same thing (wow)
		void reset(); // ref-
	};
}

#include "memory.ipp"