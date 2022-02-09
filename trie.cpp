#include "trie.hpp"
#include <iostream>
#include <set>

#include <utility>
#include <algorithm>

std::string get_alphabet() {
    return "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUWXYZ0123456789,.:;/\\\"'";
}

void delete_trie_node(trie_node *pNode) {
    for (auto child : pNode->children) {
        if (child != nullptr) {
            delete_trie_node(child);
        }
    }
    delete pNode;
}

int get_letter_index(char letter) {
    std::string alphabet = get_alphabet();
    int index = letter - 'a';
    if (index < 0) {
        return alphabet.find(letter);
    }
    return index;

}

bool already_iterated_payload(char payload, char previous_payload) {
    std::string alphabet = get_alphabet();
    return alphabet.find(payload) <= alphabet.find(previous_payload);
}

trie::trie() {
    auto *trieNode = new trie_node();
    m_root = trieNode;
};

trie::~trie() {
    auto node = this->m_root;
    delete_trie_node(node);
}

bool trie::insert(const std::string &str) {
    trie_node *current_node = m_root;
    if (str.empty()) {
        if (current_node->is_terminal) {
            return false;
        }
        current_node->is_terminal = true;
        m_size += 1;
        return true;
    }
    for (int i = 0; i < str.size(); i++) {
        int strindex = get_letter_index(str[i]);
        if (current_node->children[strindex] == nullptr) {
            auto *new_node = new trie_node();
            new_node->payload = str[i];
            current_node->children[strindex] = new_node;
            new_node->parent = current_node;
            current_node = new_node;
        } else {
            current_node = current_node->children[strindex];
        }
        if (i == str.size() - 1) {
            if (current_node->is_terminal) {
                return false;
            }
            current_node->is_terminal = true;
        }
    }
    m_size += 1;
    return true;
}

bool trie::contains(const std::string &str) const {
    trie_node *current_node = m_root;
    if (str.empty()) {
        return current_node->is_terminal;
    }
    for (int i = 0; i < str.size() + 1; i++) {
        if (i == str.size()) {
            if (current_node->is_terminal) {
                return true;
            }
            return false;
        }
        int strindex = get_letter_index(str[i]);
        if (current_node->children[strindex] == nullptr) {
            return false;
        } else {
            current_node = current_node->children[strindex];
        }
    }
    return false;
}

bool trie::erase(const std::string &str) {
    trie_node *current_node = m_root;
    if (str.empty()) {
        if (current_node->is_terminal) {
            current_node->is_terminal = false;
            return true;
        }
        return false;
    }
    int strindex;
    for (int i = 0; i < str.size() + 1; i++) {
        if (i == str.size()) {
            if (current_node->is_terminal) {
                break;
            }
            return false;
        }
        strindex = get_letter_index(str[i]);
        if (current_node->children[strindex] == nullptr) {
            return false;
        } else {
            current_node = current_node->children[strindex];
        }
    }
    current_node->is_terminal = false;
    while (current_node->parent != nullptr) {
        if (current_node->is_terminal) {
            break;
        }
        for (auto child : current_node->children) {
            if (child != nullptr) {
                m_size -= 1;
                return true;
            }
        }
        trie_node *temp = current_node->parent;
        strindex = get_letter_index(current_node->payload);
        delete current_node;
        current_node = temp;
        current_node->children[strindex] = nullptr;
    }
    m_size -= 1;
    return true;
}

bool trie::empty() const {
    for (auto rootChild: this->m_root->children) {
        if (rootChild != nullptr) {
            return false;
        }
    }
    return true;
}

size_t trie::size() const {
    return m_size;
}

trie::const_iterator trie::begin() const {
    auto current_node = this->m_root;
    bool found_child = false;
    while (current_node != nullptr) {
        if (current_node->is_terminal) {
            return {current_node};
        }
        for (auto child: current_node->children) {
            if (child != nullptr) {
                current_node = child;
                found_child = true;
                break;
            }
        }
        if (!found_child) {
            return {nullptr};
        }
    }
    return {nullptr};
}

trie::const_iterator trie::end() const {
    return {nullptr};
};

size_t dfs_size(trie_node *trieNode) {
    size_t size = 0;
    if (trieNode->is_terminal) {
        size += 1;
    }
    for (auto child: trieNode->children) {
        if (child != nullptr) {
            size += dfs_size(child);
        }
    }
    return size;
}

trie::trie(const std::vector<std::string> &strings) {
    auto *trieNode = new trie_node();
    m_root = trieNode;
    for (const auto &str : strings) {
        this->insert(str);
    }
}

trie::trie(const trie &rhs) {
    auto *trieNode = new trie_node();
    m_root = trieNode;
    if (!rhs.empty()) {
        trie::const_iterator it;
        for (it = rhs.begin(); it != rhs.end(); it++) {
            std::string str = *it;
            this->insert(str);
        }
    }
}

trie::trie(trie &&rhs) {
    auto *trieNode = new trie_node();
    m_root = trieNode;
    if (!rhs.empty()) {
        trie::const_iterator it;
        std::vector<std::string> content;
        for (it = rhs.begin(); it != rhs.end(); it++) {
            content.push_back(*it);
        }
        for (const auto &c : content) {
            rhs.erase(c);
            this->insert(c);
        }
    }

}

trie &trie::operator=(trie &&rhs) {
    if (rhs.m_root == this->m_root) {
        return *this;
    }
    const_iterator it;
    std::vector<std::string> content;
    if (!this->empty()) {
        for (it = this->begin(); it != this->end(); it++) {
            content.push_back(*it);
        }
        for (const auto &c : content) {
            this->erase(c);
        }
    }
    if (!rhs.empty()) {
        content.clear();
        for (it = rhs.begin(); it != rhs.end(); it++) {
            content.push_back(*it);
        }
        for (const auto &c : content) {
            rhs.erase(c);
            this->insert(c);
        }
    }
    return *this;
}

trie &trie::operator=(const trie &rhs) {
    if (rhs.m_root == this->m_root) {
        return *this;
    }
    const_iterator it;
    std::vector<std::string> content;
    if (!this->empty()) {
        for (it = this->begin(); it != this->end(); it++) {
            content.push_back(*it);
        }
        for (const auto &c : content) {
            this->erase(c);
        }
    }
    if (!rhs.empty()) {
        trie::const_iterator it;
        for (it = rhs.begin(); it != rhs.end(); it++) {
            std::string str = *it;
            this->insert(str);
        }
    }
    return *this;
}

void trie::swap(trie &rhs) {
    trie_node *temp_node = rhs.m_root;
    int temp_size = rhs.m_size;
    rhs.m_root = this->m_root;
    rhs.m_size = this->m_size;
    this->m_size = temp_size;
    this->m_root = temp_node;
}

bool trie::operator==(const trie &rhs) const {
    std::set<std::string> set1;
    std::set<std::string> set2;
    if (!this->empty()) {
        const_iterator it;
        for (it = this->begin(); it != this->end(); it++) {
            set1.insert(*it);
        }
    }
    if (!rhs.empty()) {
        const_iterator it;
        for (it = rhs.begin(); it != rhs.end(); it++) {
            set2.insert(*it);
        }
    }
    return set1 == set2;
}

int alphabet_index(char letter) {
    std::string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    return alphabet.find(letter);
}

bool trie::operator<(const trie &rhs) const {
    if (rhs.empty() && this->empty()) {
        return false;
    }
    if (rhs.empty() && !this->empty()) {
        return false;
    }
    if (!rhs.empty() && this->empty()) {
        return true;
    }
    const_iterator it;
    std::vector<std::string> lhs_v = {this->begin(), this->end()};
    std::vector<std::string> rhs_v = {rhs.begin(), rhs.end()};
    for (int i = 0; i < std::min(lhs_v.size(), rhs_v.size()); i++) {
        if (lhs_v[i] != rhs_v[i]) {
            for (int j = 0; j < std::min(lhs_v[i].size(), rhs_v[i].size()); j++) {
                if (lhs_v[i][j] != rhs_v[i][j]) {
                    if (alphabet_index(lhs_v[i][j]) < alphabet_index(rhs_v[i][j])) {
                        return true;
                    }
                    return false;
                }
            }
            return lhs_v[i].size() < rhs_v[i].size();
        }
    }
    return lhs_v.size() < rhs_v.size();
}

std::vector<std::string> trie::get_prefixes(const std::string &str) const {
    std::vector<std::string> content = {this->begin(), this->end()};
    std::vector<std::string> new_content;
    for (auto element : content) {
        if ((str.rfind(element, 0) == 0)) {
            new_content.push_back(element);
        }
    }
    return new_content;
}

std::vector<std::string> trie::search_by_prefix(const std::string &prefix) const {
    std::vector<std::string> content = {this->begin(), this->end()};
    std::vector<std::string> new_content;
    for (auto element : content) {
        if ((element.rfind(prefix, 0) == 0)) {
            new_content.push_back(element);
        }
    }
    return new_content;
}

trie trie::operator|(const trie &rhs) const {

    std::vector<std::string> content_l = {this->begin(), this->end()};
    std::vector<std::string> content_r = {rhs.begin(), rhs.end()};
    std::set<std::string> joined;
    std::set_union(content_r.begin(), content_r.end(), content_l.begin(), content_l.end(),
                   std::inserter(joined, joined.begin()));
    trie t;
    for (const auto &s : joined) {
        t.insert(s);
    }
    return t;

}

trie trie::operator&(const trie &rhs) const {
    std::vector<std::string> content_l = {this->begin(), this->end()};
    std::vector<std::string> content_r = {rhs.begin(), rhs.end()};
    std::set<std::string> joined;
    std::set_intersection(content_r.begin(), content_r.end(), content_l.begin(), content_l.end(),
                          std::inserter(joined, joined.begin()));
    trie t;
    for (const auto &s : joined) {
        t.insert(s);
    }
    return t;
}

bool operator!=(const trie &lhs, const trie &rhs) {
    return !(lhs == rhs);
}

bool operator>(const trie &lhs, const trie &rhs) {
    if (rhs.empty() && lhs.empty()) {
        return false;
    }
    if (rhs.empty() && !lhs.empty()) {
        return true;
    }
    if (!rhs.empty() && lhs.empty()) {
        return false;
    }
    return !(lhs < rhs) & (lhs != rhs);
}

bool operator<=(const trie &lhs, const trie &rhs) {
    if (rhs.empty() && lhs.empty()) {
        return true;
    }
    if (rhs.empty() && !lhs.empty()) {
        return false;
    }
    if (!rhs.empty() && lhs.empty()) {
        return true;
    }
    return (lhs < rhs) | (lhs == rhs);
}

bool operator>=(const trie &lhs, const trie &rhs) {
    if (rhs.empty() && lhs.empty()) {
        return true;
    }
    if (rhs.empty() && !lhs.empty()) {
        return true;
    }
    if (!rhs.empty() && lhs.empty()) {
        return false;
    }
    return (lhs > rhs) | (lhs == rhs);
}


trie::const_iterator &trie::const_iterator::operator++() {
    auto temp_node = this->current_node;
    bool children_available = false;
    while (true) {
        for (auto child: temp_node->children) {
            if (child != nullptr) {
                if (child->is_terminal) {
                    this->current_node = child;
                    return *this;
                }
                children_available = true;
                temp_node = child;
            }
        }
        if (!children_available) {
            break;
        }
    }
    char previous_payload = temp_node->payload;
    temp_node = this->current_node->parent;
    while (temp_node != nullptr) {
        for (auto child : temp_node->children) {
            if (child != nullptr) {
                if (already_iterated_payload(child->payload, previous_payload)) {
                    continue;
                }
                temp_node = child;
                if (temp_node->is_terminal) {
                    this->current_node = temp_node;
                    return *this;
                }
                while (true) {
                    for (auto inner_child: temp_node->children) {
                        if (inner_child != nullptr) {
                            if (inner_child->is_terminal) {
                                this->current_node = inner_child;
                                return *this;
                            } else {
                                temp_node = inner_child;
                                break;
                            }
                        }
                    }
                }
            }
        }
        previous_payload = temp_node->payload;
        temp_node = temp_node->parent;
    }
    this->current_node = nullptr;
    return *this;
}

trie::const_iterator trie::const_iterator::operator++(int) {
    auto temp_node = this->current_node;
    bool children_available = false;
    while (true) {
        for (auto child: temp_node->children) {
            if (child != nullptr) {
                if (child->is_terminal) {
                    const_iterator temp(*this);
                    this->current_node = child;
                    return temp;
                }
                children_available = true;
                temp_node = child;
            }
        }
        if (!children_available) {
            break;
        }
    }
    char previous_payload = temp_node->payload;
    temp_node = this->current_node->parent;
    while (temp_node != nullptr) {
        for (auto child : temp_node->children) {
            if (child != nullptr) {
                if (already_iterated_payload(child->payload, previous_payload)) {
                    continue;
                }
                temp_node = child;
                if (temp_node->is_terminal) {
                    const_iterator temp(*this);
                    this->current_node = temp_node;
                    return temp;
                }
                while (true) {
                    for (auto inner_child: temp_node->children) {
                        if (inner_child != nullptr) {
                            if (inner_child->is_terminal) {
                                const_iterator temp(*this);
                                this->current_node = inner_child;
                                return temp;
                            } else {
                                temp_node = inner_child;
                                break;
                            }
                        }
                    }
                }
            }
        }
        previous_payload = temp_node->payload;
        temp_node = temp_node->parent;
    }
    const_iterator temp(*this);
    this->current_node = nullptr;
    return temp;
}








