#ifndef MYSTRING_H
#define MYSTRING_H

#include <array>
#include <iostream>
#include <map>
#include <set>


class MyString {
public:
    static constexpr int initialBufferSize_ = 20;

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

        bool operator==(const base_iterator& other) const { return index_ == other.index_; }
        bool operator!=(const base_iterator& other) const { return index_ != other.index_; }
    };

    using iterator = base_iterator<false>;
    using const_iterator = base_iterator<true>;

    iterator begin() { return {this, 0}; }
    iterator end() { return {this, size_}; }
    const_iterator begin() const { return {this, 0}; }
    const_iterator end() const { return {this, size_}; }
    const_iterator cbegin() const { return {this, 0};}
    const_iterator cend() const { return {this, size_};}

    [[nodiscard]] std::size_t size() const { return size_; }
    [[nodiscard]] std::size_t capacity() const { return capacity_; }
    [[nodiscard]] bool empty() const { return size_ == 0; }

    void clear();

    std::string toString() const;

    friend std::ostream& operator<<(std::ostream& out, const MyString& str);

    friend std::istream& operator>>(std::istream& in, MyString& str);

    char& operator[](std::size_t index);

    const char& operator[](std::size_t index) const;

    MyString& operator+=(char ch);

    bool operator==(const MyString& other) const;

    bool operator!=(const MyString&) const;

    bool operator<(const MyString&) const;

    bool operator>(const MyString&) const;

    std::set<MyString> getUniqueWords() const;
    std::map<MyString, size_t>  countWordsUsageIgnoringCases()const;
    static MyString generateRandomWord(size_t length);

    void trim();

    MyString& toLower();
};


#endif //MYSTRING_H
