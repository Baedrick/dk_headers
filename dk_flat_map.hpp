#include <algorithm>    // lower_bound, upper_bound, sort, unique
#include <functional>   // less
#include <iterator>     // distance
#include <utility>      // pair, move, forward
#include <vector>       // vector

namespace dk {
    template <
        typename Key, typename T,
        typename Compare = std::less<Key>,
        typename Container = std::vector<std::pair<Key, T>>>
    class flat_map {
    public:
        using key_type = Key;
        using mapped_type = T;
        using value_type = std::pair<key_type, mapped_type>;
        using key_compare = Compare;
        using container = Container;
        using size_type = typename container::size_type;
        using iterator = typename container::iterator;
        using const_iterator = typename container::const_iterator;
        using reverse_iterator = typename container::reverse_iterator;
        using const_reverse_iterator = typename container::const_reverse_iterator;

    private:
        container m_container;

    public:
        flat_map() = default;

        explicit flat_map(std::initializer_list<value_type> list) : 
            flat_map() {
                m_container.reserve(list.size() * 2);
                for (auto it = std::begin(list); it != std::end(list); ++it) {
                    m_container.emplace_back(*it);
                }
                this->sort();
                this->remove_duplicates();
        }

        template <typename Iter>
        flat_map(Iter begin, Iter end) :
            flat_map() {
                m_container.reserve(std::distance(begin, end) * 2);
                for (; begin != end; ++begin) {
                    m_container.emplace_back(*begin);
                }
                this->sort();
                this->remove_duplicates();
        }

        flat_map(flat_map const &) = default;

        flat_map(flat_map &&) noexcept = default;

        flat_map& operator=(flat_map const &) = default;

        flat_map& operator=(flat_map &&) noexcept = default;

        ~flat_map() = default;

        [[nodiscard]] auto begin() noexcept -> iterator {
            return m_container.begin();
        }

        [[nodiscard]] auto end() noexcept -> iterator {
            return m_container.end();
        }

        [[nodiscard]] auto begin() const noexcept -> const_iterator {
            return m_container.begin();
        }

        [[nodiscard]] auto end() const noexcept -> const_iterator {
            return m_container.end();
        }

        [[nodiscard]] auto cbegin() const noexcept -> const_iterator {
            return m_container.cbegin();
        }

        [[nodiscard]] auto cend() const noexcept -> const_iterator {
            return m_container.cend();
        }

        [[nodiscard]] auto rbegin() noexcept -> reverse_iterator {
            return m_container.rbegin();
        }

        [[nodiscard]] auto rbegin() const noexcept -> const_reverse_iterator {
            return m_container.rbegin();
        }
        
        [[nodiscard]] auto crbegin() noexcept -> reverse_iterator {
            return m_container.crbegin();
        }

        [[nodiscard]] auto rend() noexcept -> reverse_iterator {
            return m_container.rend();
        }

        [[nodiscard]] auto rend() const noexcept -> const_reverse_iterator {
            return m_container.rend();
        }
        
        [[nodiscard]] auto crend() noexcept -> reverse_iterator {
            return m_container.crend();
        }

        [[nodiscard]] auto empty() const noexcept -> bool {
            return m_container.empty();
        }

        [[nodiscard]] auto size() const noexcept -> size_type {
            return m_container.size();
        }

        [[nodiscard]] auto max_size() const noexcept -> size_type {
            return m_container.max_size();
        }

        auto operator[](key_type const &key) -> mapped_type& {
            return this->emplace(key).first->second;
        }

        auto operator[](key_type &&key) -> mapped_type& {
            return this->emplace(std::move(key)).first->second;
        }

        template <typename... Args>
        auto emplace(Args &&...args) -> std::pair<iterator, bool> {
            value_type value(std::forward<Args>(args)...);
            auto it = this->lower_bound(value.first);
            if (it == std::end(m_container) || it->first != value.first) {
                it = m_container.emplace(this->upper_bound(value.first), std::move(value));
                return std::make_pair(it, true);
            }
            return std::make_pair(it, false);
        }

        auto insert(value_type const &value) -> std::pair<iterator, bool> {
            return this->emplace(value);
        }

        auto insert(value_type &&value) -> std::pair<iterator, bool> {
            return this->emplace(std::move(value));
        }

        auto erase(iterator pos) -> iterator {
            return m_container.erase(pos);
        }

        auto erase(const_iterator pos) -> iterator {
            return m_container.erase(pos);
        }

        auto erase(const_iterator begin, const_iterator end) -> iterator {
            return m_container.erase(begin, end);
        }

        auto erase(key_type const &key) -> size_type {
            auto const it = this->lower_bound(key);
            if (it != std::end(m_container) && it->first == key) {
                m_container.erase(it);
                return 1;
            }
            return 0;
        }

        auto swap(flat_map &other) noexcept -> void {
            m_container.swap(other.m_container);
        }

        auto clear() noexcept -> void {
            m_container.clear();
        }

        auto find(key_type const &key) noexcept -> iterator {
            auto const it = this->lower_bound(key);
            if (it != std::end(m_container) && it->first == key) {
                return it;
            }
            return std::end(m_container);
        }

        auto find(key_type const &key) const noexcept -> const_iterator {
            auto const it = this->lower_bound(key);
            if (it != std::cend(m_container) && it->first == key) {
                return it;
            }
            return std::cend(m_container);
        }

        [[nodiscard]] auto contains(key_type const &key) noexcept -> bool {
            return this->find(key) != std::end(m_container);
        }

        [[nodiscard]] auto contains(key_type const &key) const noexcept -> bool {
            return this->find(key) != std::cend(m_container);
        }

        [[nodiscard]] auto lower_bound(key_type const &key) -> iterator {
            return std::lower_bound(std::begin(m_container), std::end(m_container), key, compare_op());
        }

        [[nodiscard]] auto lower_bound(key_type const &key) const -> const_iterator {
            return std::lower_bound(std::cbegin(m_container), std::cend(m_container), key, compare_op());
        }

        [[nodiscard]] auto upper_bound(key_type const &key) -> iterator {
            return std::upper_bound(std::begin(m_container), std::end(m_container), key, compare_op());
        }

        [[nodiscard]] auto upper_bound(key_type const &key) const -> const_iterator {
            return std::upper_bound(std::cbegin(m_container), std::cend(m_container), key, compare_op());
        }

        [[nodiscard]] auto equal_range(key_type const &key) -> std::pair<iterator, iterator> {
            auto const lower_it = std::lower_bound(std::begin(m_container), std::end(m_container), key, compare_op());
            auto const upper_it = std::upper_bound(lower_it, std::end(m_container), key, compare_op());
            return std::make_pair(lower_it, upper_it);
        }

        [[nodiscard]] auto equal_range(key_type const &key) const -> std::pair<const_iterator, const_iterator> {
            auto const lower_it = std::lower_bound(std::cbegin(m_container), std::cend(m_container), key, compare_op());
            auto const upper_it = std::upper_bound(lower_it, std::cend(m_container), key, compare_op());
            return std::make_pair(lower_it, upper_it);
        }

    private:
        struct compare_op {
            auto operator()(value_type const &a, value_type const &b) const noexcept -> bool {
                return key_compare()(a.first, b.first);
            }

            auto operator()(value_type const &value, key_type const &key) const noexcept -> bool {
                return key_compare()(value.first, key);
            }

            auto operator()(key_type const &key, value_type const &value) const noexcept -> bool {
                return key_compare()(key, value.first);
            }
        };

        auto sort() -> void {
            std::sort(std::begin(m_container), std::end(m_container), compare_op());
        }

        auto remove_duplicates() -> void {
            auto end = std::unique(std::begin(m_container), std::end(m_container), [](value_type const &a, value_type const &b) {
                return a.first == b.first;
            });
            m_container.erase(end, std::end(m_container));
        }
    };
}
