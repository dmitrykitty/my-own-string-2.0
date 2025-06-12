#include "mystring.h"

#include <algorithm>
#include <cstring>
#include <random>

MyString::MyString(const char* text) {
    size_ = std::strlen(text);
    const std::size_t smallCount = std::min(size_, static_cast<std::size_t>(initialBufferSize_));
    std::copy_n(text, smallCount, smallText_.begin());

    if (smallCount < initialBufferSize_) {
        smallText_[smallCount] = '\0';
    }

    if (size_ > initialBufferSize_) {
        bigText_.assign(text + smallCount, size_ - smallCount);
    } else {
        bigText_.clear();
    }
    capacity_ = initialBufferSize_ + bigText_.capacity();
}

MyString::MyString(std::size_t length, char c) {
    if (length <= initialBufferSize_) {
        std::fill_n(smallText_.begin(), length, c);
        if (length < initialBufferSize_)
            smallText_[length] = '\0';
    } else {
        std::ranges::fill(smallText_, c);
        bigText_.assign(length - initialBufferSize_, c);
    }
    size_ = length;
    capacity_ = initialBufferSize_ + bigText_.capacity();
}


void MyString::clear() {
    size_ = 0;
    if (capacity_ <= initialBufferSize_) {
        smallText_[0] = '\0';
    } else {
        bigText_.clear();
    }
}

std::ostream& operator<<(std::ostream& out, const MyString& str) {
    std::size_t smallCount = std::min(str.size_, static_cast<std::size_t>(MyString::initialBufferSize_));
    out << std::string(str.smallText_.data(), smallCount);
    if (str.size_ > MyString::initialBufferSize_) {
        out << str.bigText_;
    }
    return out;
}

std::istream& operator>>(std::istream& in, MyString& str) {
    std::string tmp;
    if (!std::getline(in, tmp))
        return in;
    str = MyString(tmp.c_str());
    return in;
}

char& MyString::operator[](std::size_t index) {
    if (index >= size_)
        throw std::out_of_range("MyString::operator[]: invalid index");
    return index < initialBufferSize_ ? smallText_[index] : bigText_[index - initialBufferSize_];
}

const char& MyString::operator[](std::size_t index) const {
    if (index >= size_)
        throw std::out_of_range("MyString::operator[]: invalid index");
    return index < initialBufferSize_ ? smallText_[index] : bigText_[index - initialBufferSize_];
}

MyString& MyString::operator+=(char ch) {
    if (size_ < initialBufferSize_)
        smallText_[size_] = ch;
    else
        bigText_ += ch;

    size_++;
    capacity_ = initialBufferSize_ + bigText_.capacity();
    if (size_ < initialBufferSize_)
        smallText_[size_] = '\0';

    return *this;
}

bool MyString::operator<(const MyString& other) const {
    return this->toString() < other.toString();
}


void MyString::trim() {
    if (size_ == 0)
        return;

    std::size_t begin = 0;
    std::size_t end = size_;
    //isspace przyjmuje tylko unsigned char -> CAST
    while (begin < size_ && std::isspace(static_cast<unsigned char>((*this)[begin]))) {
        begin++;
    }

    if (begin == size_) {
        clear();
        return;
    }

    while (end > begin && std::isspace(static_cast<unsigned char>((*this)[end - 1]))) {
        end--;
    }

    std::size_t newSize = end - begin;

    if (begin > 0) {
        //przesuniecie na początku
        for (std::size_t i = 0; i < newSize; ++i) {
            (*this)[i] = (*this)[i + begin];
        }
    }
    size_ = newSize;

    if (size_ <= initialBufferSize_) {
        bigText_.clear();
        if (size_ < initialBufferSize_) {
            smallText_[size_] = '\0';
        }
    } else {
        bigText_.erase(size_ - initialBufferSize_);
    }
}

MyString& MyString::toLower() {
    for (auto& ch: *this) {
        ch = std::tolower(static_cast<unsigned char>(ch));
    }
    return *this;
}


std::set<MyString> MyString::getUniqueWords() const {
    std::set<MyString> uniqueWords;
    MyString tmp = *this;
    tmp.trim();
    tmp = tmp.toLower();

    MyString currentWord;
    for (const auto ch: tmp) {
        if (!std::isspace(static_cast<unsigned char>(ch))
            && std::isalpha(static_cast<unsigned>(ch))) {
            currentWord += ch;
        } else {
            if (!currentWord.empty()) {
                uniqueWords.insert(currentWord);
                currentWord.clear();
            }
        }
    }
    if (!currentWord.empty()) {
        uniqueWords.insert(currentWord);
    }

    return uniqueWords;
}

std::map<MyString, size_t> MyString::countWordsUsageIgnoringCases() const {
    MyString tmp = *this;
    tmp.trim();
    tmp = tmp.toLower();

    std::map<MyString, size_t> wordCounter;
    MyString currentWord;

    auto addWordToCounter = [&] {
        if (!currentWord.empty()) {
            ++wordCounter[currentWord];
            currentWord.clear();
        }
    };

    for (const auto ch: tmp) {
        if (std::isalpha(static_cast<unsigned>(ch)))
            currentWord += ch;
        else
            addWordToCounter();
    }
    addWordToCounter();

    return wordCounter;
}


std::string MyString::toString() const {
    std::string res;
    for (const auto ch: *this)
        res += ch;
    return res;
}

auto makeRandomCharGenerator() {
    return [] {
        constexpr std::string_view alphabet = "abcdefghijklmnopqrstuvwxyz";
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<size_t> dist(0, alphabet.length() - 1);

        return alphabet[dist(gen)];
    };
}

MyString MyString::generateRandomWord(size_t length) {
    if (length == 0)
        return MyString();

    MyString randomWord(length, '\0');
    auto generator = makeRandomCharGenerator();
    std::generate_n(randomWord.begin(), length, generator);
    return randomWord;
}
