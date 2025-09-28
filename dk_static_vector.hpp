/**
 * \file dk_static_vector.hpp - v0.1
 * \author KOH Swee Teck Dedrick
 * \brief
 *      An std::vector like container with a fixed capcaity and
 *      stack-based allocation.
 * 
 *      The container provides an interface similar to std::vector
 *      but does not perform any dynamic memory allocation. Its
 *      capacity is determined at compile-time.
 * 
 *  LICENSE
 *      License information at the end of the header.
 */

#ifndef DK_INCLUDE_DK_STATIC_VECTOR_HPP
#define DK_INCLUDE_DK_STATIC_VECTOR_HPP

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <utility>
#include <type_traits>

#if !defined(DK_ASSERT)
#   if defined(_MSC_VER)
#       if !defined(NDEBUG)
#           include <intrin.h>
#           define DK_ASSERT(x) do { if (!(x)) { __debugbreak(); } } while(false) /* NOLINT */
#       else
#           define DK_ASSERT(x) do { if (!(x)) { (void)(sizeof(x)); } } while(false) /* NOLINT */
#       endif
#   else
#       include <cassert>
#       define DK_ASSERT(x) assert(x) /* NOLINT */
#   endif
#endif

namespace dk {
    template <
        typename T,
        std::size_t N,
        typename SizeType = std::uint32_t>
    class static_vector {
    public:
        using value_type = T;
        using size_type = SizeType;
        using reference = value_type&;
        using const_reference = value_type const&;
        using pointer = T*;
        using const_pointer = T const*;
        using iterator = T*;
        using const_iterator = T const*;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        static_assert(std::is_unsigned_v<size_type>); // Must be unsigned integer.

    private:
        alignas(value_type) u8 m_buffer[sizeof(value_type) * N];
        size_type m_size;

    public:
        static_vector() noexcept :
            m_buffer{ },
            m_size{ 0 } { }

        static_vector(size_type count) noexcept :
            m_buffer{ },
            m_size{ count } {
            pointer const base = data();
            for (size_type i = 0; i < count; ++i) {
                new (base + i) value_type();
            }
        }

        static_vector(size_type count, value_type const &v) noexcept :
            m_buffer{ },
            m_size{ count } {
            pointer const base = data();
            for (size_type i = 0; i < count; ++i) {
                new (base + i) value_type(v);
            }
        }

        static_vector(std::initializer_list<value_type> list) noexcept :
            m_buffer{ },
            m_size{ std::end(list) - std::begin(list) } {
            pointer const base = data();
            size_type i = 0;
            for (auto it = std::begin(list); it != std::end(list); ++it, ++i) {
                new (base + i) value_type(*it);
            }
        }

        ~static_vector() {
            if constexpr (!std::is_trivially_destructible_v<value_type>) {
                for (auto it = begin(); it != end(); ++it) {
                    it->~value_type();
                }
            }
        }

        static_vector(static_vector const &rhs) noexcept :
            m_buffer{ },
            m_size{ 0 } {
            size_type const count = rhs.size();
            pointer const base = data();
            for (size_type i = 0; i < count; ++i) {
                new (base + i) value_type(rhs[i]);
            }
        }

        static_vector(static_vector &&rhs) noexcept :
            m_buffer{ },
            m_size{ 0 } {
            size_type const count = rhs.size();
            pointer const base = data();
            for (size_type i = 0; i < count; ++i) {
                new (base + i) value_type(std::move(rhs[i]));
            }
            rhs.clear();
        }

        auto operator=(static_vector tmp) noexcept -> static_vector& {
            this->swap(tmp);
            return *this;
        }

        auto swap(static_vector &rhs) noexcept -> void {
            pointer const lhs_base = data();
            pointer const rhs_base = rhs.data();

            size_type const lhs_size = size();
            size_type const rhs_size = rhs.size();
            size_type const min_size = lhs_size < rhs_size ? lhs_size : rhs_size;

            // NOTE(Dedrick): Swap the common elements.
            for (size_type i = 0; i < min_size; ++i) {
                std::swap(lhs_base[i], rhs_base[i]);
            }

            // NOTE(Dedrick): Move construct the tail elements.
            if (lhs_size < rhs_size) {
                for (size_type i = lhs_size; i < rhs_size; ++i) {
                    new (lhs_base + i) value_type(std::move(rhs_base[i]));

                    if constexpr (!std::is_trivially_destructible_v<value_type>) {
                        rhs_base[i].~value_type();
                    }
                }
            } else if (lhs_size > rhs_size) {
                for (size_type i = rhs_size; i < lhs_size; ++i) {
                    new (rhs_base + i) value_type(std::move(lhs_base[i]));

                    if constexpr (!std::is_trivially_destructible_v<value_type>) {
                        lhs_base[i].~value_type();
                    }
                }
            }

            // NOTE(Dedrick): Swap the size.
            std::swap(m_size, rhs.m_size);
        }

        [[nodiscard]] auto begin() noexcept -> iterator {
            return iterator{ data() };
        }

        [[nodiscard]] auto end() noexcept -> iterator {
            return iterator{ data() + size()};
        }

        [[nodiscard]] auto begin() const noexcept -> const_iterator {
            return const_iterator{ data() };
        }

        [[nodiscard]] auto end() const noexcept -> const_iterator {
            return const_iterator{ data() + size() };
        }

        [[nodiscard]] auto cbegin() const noexcept -> const_iterator {
            return const_iterator{ begin() };
        }

        [[nodiscard]] auto cend() const noexcept -> const_iterator {
            return const_iterator{ end() };
        }

        [[nodiscard]] auto rbegin() noexcept -> reverse_iterator {
            return std::reverse_iterator<iterator>{ end() };
        }

        [[nodiscard]] auto rend() noexcept -> reverse_iterator {
            return std::reverse_iterator<iterator>{ begin() };
        }

        [[nodiscard]] auto rbegin() const noexcept -> const_reverse_iterator {
            return std::reverse_iterator<const_reverse_iterator>{ end() };
        }

        [[nodiscard]] auto rend() const noexcept -> const_reverse_iterator {
            return std::reverse_iterator<const_reverse_iterator>{ begin() };
        }

        [[nodiscard]] auto crbegin() const noexcept -> const_reverse_iterator {
            return std::reverse_iterator<const_iterator>{ end() };
        }

        [[nodiscard]] auto crend() const noexcept -> const_reverse_iterator {
            return std::reverse_iterator<const_iterator>{ begin() };
        }

        [[nodiscard]] auto size() const noexcept -> size_type {
            return m_size;
        }

        [[nodiscard]] auto capacity() const noexcept -> size_type {
            return static_cast<size_type>(N);
        }

        [[nodiscard]] auto empty() const noexcept -> bool {
            return m_size == 0;
        }

        [[nodiscard]] auto front() noexcept -> reference {
            DK_ASSERT(!empty()); // front() called for empty array.

            return *begin();
        }

        [[nodiscard]] auto front() const noexcept -> const_reference {
            DK_ASSERT(!empty()); // front() called for empty array.

            return *begin();
        }

        [[nodiscard]] auto back() noexcept -> reference {
            DK_ASSERT(!empty()); // back() called for empty array.

            return *(end() - 1);
        }

        [[nodiscard]] auto back() const noexcept -> const_reference {
            DK_ASSERT(!empty()); // back() called for empty array.

            return *(end() - 1);
        }

        [[nodiscard]] auto data() noexcept -> pointer {
            return reinterpret_cast<pointer>(m_buffer);
        }

        [[nodiscard]] auto data() const noexcept -> const_pointer {
            return reinterpret_cast<const_pointer>(m_buffer);
        }

        auto operator[](size_type idx) noexcept -> reference {
            DK_ASSERT(idx < size()); // Out of bounds.

            return data[idx];
        }

        auto operator[](size_type idx) const noexcept -> reference {
            DK_ASSERT(idx < size()); // Out of bounds.

            return data[idx];
        }

        auto push_back(value_type const &v) noexcept -> void {
            DK_ASSERT(size() < N); // Vector is full.

            emplace_back(v);
        }

        auto push_back(value_type &&v) noexcept -> void {
            DK_ASSERT(size() < N); // Vector is full.

            emplace_back(std::move(v));
        }

        template <typename... Args>
        auto emplace_back(Args &&...args) noexcept -> reference {
            DK_ASSERT(size() < N); // Vector is full.

            pointer const base = data();
            pointer const ptr = new (base + m_size++) value_type(std::forward<Args>(args)...);
            return *ptr;
        }

        auto pop_back() noexcept -> void {
            DK_ASSERT(!empty()); // pop_back() called for empty array.

            size_type const new_size = size() - 1;
            if constexpr (!std::is_trivially_destructible_v<value_type>) {
                data()[new_size].~value_type();
            }
            m_size = new_size;
        }

        template <typename... Args>
        auto emplace(const_iterator pos, Args &&...args) noexcept -> iterator {
            DK_ASSERT(size() < N); // Vector is full.
            DK_ASSERT(pos >= cbegin() && pos <= cend()); // Iterator out of bounds.

            const size_type index = pos - cbegin();
            pointer p_insert = data() + index;

            if (index < m_size) {
                // NOTE(Dedrick): Not inserting at the end, so we need to make space.
                // Move construct the last element into the uninitialized space at the new end.
                new (end()) value_type(std::move(back()));

                // NOTE(Dedrick): Shift existing elements one position to the right.
                for (iterator it = end() - 1; it > p_insert; --it) {
                    *it = std::move(*(it - 1));
                }
                
                // NOTE(Dedrick): The object at the insertion point is now moved-from.
                // Destroy it and construct the new element in its place.
                if constexpr (!std::is_trivially_destructible_v<value_type>) {
                    p_insert->~value_type();
                }
                new (p_insert) value_type(std::forward<Args>(args)...);

            } else {
                // NOTE(Dedrick): Inserting at the end is just emplace_back.
                new (p_insert) value_type(std::forward<Args>(args)...);
            }

            ++m_size;
            return p_insert;
        }

        auto insert(const_iterator pos, value_type const &value) noexcept -> iterator {
            return emplace(pos, value);
        }

        auto insert(const_iterator pos, value_type &&value) noexcept -> iterator {
            return emplace(pos, std::move(value));
        }

        auto erase(const_iterator pos) noexcept -> iterator {
            DK_ASSERT(!empty());
            DK_ASSERT(pos >= cbegin() && pos < cend());

            // NOTE(Dedrick): Convert const_iterator to a mutable iterator.
            iterator const it = begin() + (pos - cbegin());

            // NOTE(Dedrick): Shift all elements after this one to the left.
            std::move(it + 1, end(), it);

            // NOTE(Dedrick): Destroy the now-duplicate last element and shrink the vector.
            pop_back();

            return it;
        }

        auto erase(const_iterator first, const_iterator last) noexcept -> iterator {
            DK_ASSERT(first >= cbegin() && first <= cend());
            DK_ASSERT(last >= first && last <= cend());

            iterator const it_first = begin() + (first - cbegin());
            iterator const it_last = begin() + (last - cbegin());

            if (it_first == it_last) {
                return it_first;
            }

            // NOTE(Dedrick): Move the elements from after the erased range to fill the gap.
            iterator const new_end = std::move(it_last, end(), it_first);

            // NOTE(Dedrick): The new size is the distance from the beginning to the new end.
            size_type const new_size = std::distance(begin(), new_end);

            // NOTE(Dedrick): Destroy the leftover elements at the end and update the size.
            destruct_and_downsize(new_size);

            return it_first;
        }

        auto clear() noexcept -> void {
            destruct_and_downsize(0);
        }

        auto resize(size_type count) noexcept -> void {
            // NOTE(Dedrick): Shrink the vector and deconstruct elements.
            if (count <= size()) {
                destruct_and_downsize(count);
                return;
            }

            // NOTE(Dedrick): Default construct new elements.
            pointer const base = data();
            for (size_type i = size(); i < count; ++i) {
                new (base + i) value_type();
            }
            m_size = count;
        }

        auto resize(size_type count, value_type const &v) noexcept -> void {
            // NOTE(Dedrick): Shrink the vector and deconstruct elements.
            if (count <= size()) {
                destruct_and_downsize(count);
                return;
            }

            // NOTE(Dedrick): Default construct new elements.
            pointer const base = data();
            for (size_type i = size(); i < count; ++i) {
                new (base + i) value_type(v);
            }
            m_size = count;
        }

    private:
        auto destruct_and_downsize(usize idx) noexcept -> void {
            DK_ASSERT(idx <= size());

            if constexpr (!std::is_trivially_destructible_v<value_type>) {
                for (auto it = begin() + idx; it != end(); ++it) {
                    it->~value_type();
                }
            }
            m_size = idx;
        }
    };

    template <typename T, std::size_t N, typename SizeType>
    [[nodiscard]] auto operator==(
        static_vector<T, N, SizeType> const &lhs,
        static_vector<T, N, SizeType> const &rhs
    ) -> bool {
        if (lhs.size() != rhs.size()) {
            return false;
        }
        return std::equal(std::begin(lhs), std::end(lhs), std::begin(rhs));
    }

#if __cplusplus >= 202002L // C++20
    template <typename T, std::size_t N, typename SizeType>
    [[nodiscard]] auto operator<=>(
        static_vector<T, N, SizeType> const &lhs,
        static_vector<T, N, SizeType> const &rhs
    ) {
        return std::lexicographical_compare_three_way(
            std::begin(lhs), std::end(lhs),
            std::begin(rhs), std::end(rhs));
    }
#else
    template <typename T, std::size_t N, typename SizeType>
    [[nodiscard]] auto operator!=(
        static_vector<T, N, SizeType> const &lhs,
        static_vector<T, N, SizeType> const &rhs
    ) -> bool {
        return !(lhs == rhs);
    }

    template <typename T, std::size_t N, typename SizeType>
    [[nodiscard]] bool operator<(const static_vector<T, N, SizeType>& lhs, const static_vector<T, N, SizeType>& rhs) {
        return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    template <typename T, std::size_t N, typename SizeType>
    [[nodiscard]] bool operator<=(const static_vector<T, N, SizeType>& lhs, const static_vector<T, N, SizeType>& rhs) {
        return !(rhs < lhs);
    }

    template <typename T, std::size_t N, typename SizeType>
    [[nodiscard]] bool operator>(const static_vector<T, N, SizeType>& lhs, const static_vector<T, N, SizeType>& rhs) {
        return rhs < lhs;
    }

    template <typename T, std::size_t N, typename SizeType>
    [[nodiscard]] bool operator>=(const static_vector<T, N, SizeType>& lhs, const static_vector<T, N, SizeType>& rhs) {
        return !(lhs < rhs);
    }
#endif // __cplusplus >= 202002L
}

/**
 * Revision History:
 *     0.1 (2025-09-27) first version;
 */

/**
 * This software is available under two licenses (A) or (B) - chose whichever
 * you prefer.
 * ---
 * (A) zlib License
 *
 * Copyright (C) 2025 KOH Swee Teck Dedrick
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 * 
 * ---
 * (B) MIT License
 * 
 * Copyright (C) 2025 KOH Swee Teck Dedrick
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#endif // DK_INCLUDE_DK_STATIC_VECTOR_HPP
