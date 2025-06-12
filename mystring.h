#ifndef MYSTRING_H
#define MYSTRING_H

#include <array>
#include <iostream>
#include <map>
#include <set>
#include <compare>
#include <iterator>
#include <vector>


class MyString {
public:
    static constexpr int initialBufferSize_ = 20;
    using value_type = char;
    using reference = char&;
    using const_reference = const char&;
    using size_type = std::size_t;


private:
    std::array<char, 20> smallText_{};
    std::string bigText_;
    std::size_t capacity_{initialBufferSize_ + bigText_.capacity()};
    std::size_t size_{};

public:
    MyString() = default;

    MyString(const char* text);
    MyString(std::size_t length, char ch);

    MyString(const MyString& other) = default;

    template<bool IsConst>
    class base_iterator {
    public:
        using value_type = std::conditional<IsConst, const char, char>;
        using string_pointer_type = std::conditional_t<IsConst, const MyString *, MyString *>;
        using char_reference_type = std::conditional_t<IsConst, const char &, char &>;
        using char_pointer_type = std::conditional_t<IsConst, const char *, char *>;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::random_access_iterator_tag;

    private:
        string_pointer_type str_;
        std::size_t index_;

    public:
        base_iterator(string_pointer_type str, std::size_t index) : str_(str), index_(index) {}


        char_reference_type operator*() const { return (*str_)[index_]; }
        char_pointer_type operator->() const { return &(*str_)[index_]; }

        base_iterator& operator++() {
            ++index_;
            return *this;
        }

        base_iterator operator++(int) {
            base_iterator tmp = *this;
            ++index_;
            return tmp;
        }
        base_iterator& operator--() {
            --index_;
            return *this;
        }

        base_iterator operator--(int) {
            base_iterator tmp = *this;
            --index_;
            return tmp;
        }

        base_iterator& operator+=(difference_type n) { index_ += n; return *this; }
        base_iterator& operator-=(difference_type n) { index_ -= n; return *this; }

        friend base_iterator operator+(base_iterator it, difference_type n) { it += n; return it; }
        friend base_iterator operator+(difference_type n, base_iterator it) { it += n; return it; }
        friend base_iterator operator-(base_iterator it, difference_type n) { it -= n; return it; }

        difference_type operator-(const base_iterator& other) const {
            return static_cast<difference_type>(index_) - static_cast<difference_type>(other.index_);
        }

        char_reference_type operator[](difference_type n) const {
            return *(*this + n);
        }

        auto operator<=>(const base_iterator& other) const = default;



        bool operator==(const base_iterator& other) const { return index_ == other.index_; }
        bool operator!=(const base_iterator& other) const { return index_ != other.index_; }
    };

    using iterator = base_iterator<false>;
    using const_iterator = base_iterator<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin() { return {this, 0}; }
    iterator end() { return {this, size_}; }
    const_iterator begin() const { return {this, 0}; }
    const_iterator end() const { return {this, size_}; }
    const_iterator cbegin() const { return {this, 0};}
    const_iterator cend() const { return {this, size_};}

    reverse_iterator rbegin() { return std::reverse_iterator(end()); }
    reverse_iterator rend() { return std::reverse_iterator(begin()); }
    const_reverse_iterator rbegin() const { return std::reverse_iterator(end()); }
    const_reverse_iterator rend() const { return std::reverse_iterator(begin()); }
    const_reverse_iterator crbegin() const { return std::reverse_iterator(cend()); }
    const_reverse_iterator crend() const { return std::reverse_iterator(cbegin()); }


    [[nodiscard]] std::size_t size() const { return size_; }
    [[nodiscard]] std::size_t capacity() const { return capacity_; }
    [[nodiscard]] bool empty() const { return size_ == 0; }

    void clear();

    [[nodiscard]] std::string toString() const;

    friend std::ostream& operator<<(std::ostream& out, const MyString& str);

    friend std::istream& operator>>(std::istream& in, MyString& str);

    char& operator[](std::size_t index);

    const char& operator[](std::size_t index) const;

    MyString& operator+=(char ch);
    void push_back(char ch){ *this += ch; }
    MyString& operator+=(const MyString& other);


    bool operator==(const MyString& other) const = default;
    std::strong_ordering operator<=>(const MyString&) const;

    [[nodiscard]] std::set<MyString> getUniqueWords() const;
    [[nodiscard]] std::map<MyString, size_t>  countWordsUsageIgnoringCases()const;
    static MyString generateRandomWord(size_t length);
    [[nodiscard]] bool startsWith(const MyString&) const;
    [[nodiscard]] bool endsWith(const MyString&) const;
    MyString join(const std::vector<MyString> &texts) const;

    void trim();

    MyString& toLower();
};


#endif //MYSTRING_H
