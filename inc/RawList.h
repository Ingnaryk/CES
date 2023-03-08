#ifndef _RAWLIST_H_
#define _RAWLIST_H_

#include <tuple>

#include "any.h"

template <typename T>
class RawList
{
private:
    struct Node
    {
        T data{};
        Node *prev{nullptr};
        Node *next{nullptr};
        template <typename... Args>
        Node(Args &&...args)
        {
            ::new (&data) T(std::forward<Args>(args)...);
        }
        Node *operator+(ptrdiff_t offset)
        {
            Node *currentNode = this;
            while (offset > 0 && currentNode)
                offset-- && (currentNode = currentNode->next);
            return currentNode;
        }
        Node *operator-(ptrdiff_t offset)
        {
            Node *currentNode = this;
            while (offset > 0 && currentNode)
                offset-- && (currentNode = currentNode->prev);
            return currentNode;
        }
    };
    //////////////////////////////////Internal usage
    std::tuple<Node *, Node *> _IL2HeadTail(const std::initializer_list<T> &elements)
    {
        Node *nodes = (Node *)::operator new(elements.size() * sizeof(Node));
        Node *node = nodes;
        for (; node != nodes + elements.size(); node++)
        {
            ::new (node) Node{*(elements.begin() + (node - nodes))};
            if (node > nodes)
                node->prev = node - 1;
            if (node->prev)
                node->prev->next = node;
        }
        return {nodes, node - 1};
    }
    template <typename OP_Func>
    void _quickSort(ptrdiff_t left, ptrdiff_t right, const OP_Func &op)
    {
        if (left >= right)
            return;
        T pivot = head->operator+(left + (right - left) / 2)->data;
        ptrdiff_t i = left, j = right;
        Node *leftNode = head->operator+(i), *rightNode = tail->operator-(length - 1 - j);
        while (true)
        {
            while (op(leftNode->data, pivot))
            {
                i++;
                leftNode = leftNode->next;
            }
            while (!op(rightNode->data, pivot) && rightNode->data != pivot)
            {
                j--;
                rightNode = rightNode->prev;
            }
            if (i >= j)
                break;
            std::swap(leftNode->data, rightNode->data);
        }
        _quickSort(left, j, op);
        _quickSort(j + 1, right, op);
    }
    //////////////////////////////////Variadic templates functions' end
    ptrdiff_t _push(std::initializer_list<T> elements)
    {
        for (const T &ele : elements)
        {
            Node *newTail = new Node(ele);
            newTail->prev = tail;
            if (tail)
                tail->next = newTail;
            tail = newTail;
            const_cast<ptrdiff_t &>(length)++;
        }
        return length;
    }
    ptrdiff_t _unshift(std::initializer_list<T> elements)
    {
        for (auto itor = elements.end() - 1; itor >= elements.begin(); itor--)
        {
            Node *newHead = new Node(*itor);
            newHead->next = head;
            if (head)
                head->prev = newHead;
            head = newHead;
            const_cast<ptrdiff_t &>(length)++;
        }
        return length;
    }
    //////////////////////////////////Wrapped data
    Node *head{nullptr};
    Node *tail{nullptr};

public:
    //////////////////////////////////ES Property
    const ptrdiff_t length{0};
    //////////////////////////////////Constructor
    explicit RawList(ptrdiff_t size) : length{size}
    {
        T *data = (T *)::operator new(length * sizeof(T));
        std::initializer_list<T> packs;
        std::pair<const T *, size_t> packs_data(data, length);
        memcpy(&packs, &packs_data, sizeof(packs_data));
        auto [head, tail] = _IL2HeadTail(packs);
        this->head = head, this->tail = tail;
        ::operator delete(data);
    }
    RawList(const std::initializer_list<T> &elements) : length{(ptrdiff_t)elements.size()}
    {
        auto [head, tail] = _IL2HeadTail(elements);
        this->head = head, this->tail = tail;
    }
    RawList(const RawList<T> &otherList) : length{otherList.length}
    {
        Node *currentNode = otherList.head;
        T *data = (T *)::operator new(length * sizeof(T));
        for (int i = 0; i < length; i++)
        {
            ::new (data + i) T(currentNode->data);
            currentNode = currentNode->next;
        }
        std::initializer_list<T> packs;
        std::pair<const T *, size_t> packs_data(data, length);
        memcpy(&packs, &packs_data, sizeof(packs_data));
        auto [head, tail] = _IL2HeadTail(packs);
        this->head = head, this->tail = tail;
        ::operator delete(data);
    }
    RawList(RawList<T> &&rvList) : length{rvList.length}
    {
        head = rvList.head;
        tail = rvList.tail;
        rvList.head = rvList.tail = nullptr;
    }
    //////////////////////////////////ES Method
    template <typename... Args>
    ptrdiff_t push(const T &element, const Args &...elements)
    {
        _push(std::initializer_list<T>{element, elements...});
        return length;
    }
    template <typename... Args>
    ptrdiff_t unshift(const T &element, const Args &...elements)
    {
        _unshift(std::initializer_list<T>{element, elements...});
        return length;
    }
    any pop()
    {
        if (length <= 0)
            return undefined;
        T last = std::move(tail->data);
        Node *tailPrev = tail->prev;
        tailPrev && (tailPrev->next = nullptr);
        delete tail;
        tail = tailPrev;
        const_cast<ptrdiff_t &>(length)--;
        return last;
    }
    any shift()
    {
        if (length <= 0)
            return undefined;
        T first = std::move(head->data);
        Node *headNext = head->next;
        headNext && (headNext->prev = nullptr);
        delete head;
        head = headNext;
        const_cast<ptrdiff_t &>(length)--;
        return first;
    }
    any at(ptrdiff_t index) const
    {
        index < 0 && (index += length);
        if (index < 0 || index >= length)
            return undefined;
        return (head->operator+(index))->data;
    }
    ptrdiff_t indexOf(const T &value, ptrdiff_t fromIndex = 0) const
    {
        (fromIndex < 0 && fromIndex > -length) && (fromIndex += length) || fromIndex < 0 && (fromIndex = 0);
        for (Node *currentNode = head->operator+(fromIndex); currentNode != nullptr; currentNode = currentNode->next, fromIndex++)
        {
            if (currentNode->data == value)
                return fromIndex;
        }
        return -1;
    }
    ptrdiff_t lastIndexOf(const T &value, ptrdiff_t fromIndex = -1) const
    {
        (fromIndex < 0 && fromIndex > -length) && (fromIndex += length) || fromIndex >= length && (fromIndex = length - 1);
        for (Node *currentNode = tail->operator-(length - 1 - fromIndex); currentNode != nullptr; currentNode = currentNode->prev, fromIndex--)
        {
            if (currentNode->data == value)
                return fromIndex;
        }
        return -1;
    }
    bool includes(const T &value) const
    {
        for (Node *currentNode = head; currentNode != nullptr; currentNode = currentNode->next)
        {
            if (currentNode->data == value)
                return true;
        }
        return false;
    }
    std::string join(const char *separator = ",") const
    {
        std::stringstream ss;
        for (Node *currentNode = head; currentNode != nullptr; currentNode = currentNode->next)
        {
            ss << currentNode->data << (currentNode->next ? separator : "");
        }
        return ss.str();
    }
    void forEach(const std::function<void(T, ptrdiff_t, std::reference_wrapper<RawList<T>>)> &callbaclFn)
    {
        ptrdiff_t crtIndex = 0;
        for (Node *currentNode = head; currentNode != nullptr; currentNode = currentNode->next, crtIndex++)
            callbaclFn(currentNode->data, crtIndex, std::ref(*this));
    }
    bool some(const std::function<bool(T, ptrdiff_t, std::reference_wrapper<RawList<T>>)> &predicate)
    {
        ptrdiff_t crtIndex = 0;
        for (Node *currentNode = head; currentNode != nullptr; currentNode = currentNode->next, crtIndex++)
        {
            if (predicate(currentNode->data, crtIndex, std::ref(*this)))
                return true;
        }
        return false;
    }
    bool every(const std::function<bool(T, ptrdiff_t, std::reference_wrapper<RawList<T>>)> &predicate)
    {
        ptrdiff_t crtIndex = 0;
        for (Node *currentNode = head; currentNode != nullptr; currentNode = currentNode->next, crtIndex++)
        {
            if (!predicate(currentNode->data, crtIndex, std::ref(*this)))
                return false;
        }
        return true;
    }
    any find(const std::function<bool(T, ptrdiff_t, std::reference_wrapper<RawList<T>>)> &predicate)
    {
        ptrdiff_t crtIndex = 0;
        for (Node *currentNode = head; currentNode != nullptr; currentNode = currentNode->next, crtIndex++)
        {
            if (predicate(currentNode->data, crtIndex, std::ref(*this)))
                return currentNode->data;
        }
        return undefined;
    }
    any findLast(const std::function<bool(T, ptrdiff_t, std::reference_wrapper<RawList<T>>)> &predicate)
    {
        ptrdiff_t crtIndex = length - 1;
        for (Node *currentNode = tail; currentNode != nullptr; currentNode = currentNode->prev, crtIndex--)
        {
            if (predicate(currentNode->data, crtIndex, std::ref(*this)))
                return currentNode->data;
        }
        return undefined;
    }
    ptrdiff_t findIndex(const std::function<bool(T, ptrdiff_t, std::reference_wrapper<RawList<T>>)> &predicate)
    {
        ptrdiff_t crtIndex = 0;
        for (Node *currentNode = head; currentNode != nullptr; currentNode = currentNode->next, crtIndex++)
        {
            if (predicate(currentNode->data, crtIndex, std::ref(*this)))
                return crtIndex;
        }
        return -1;
    }
    ptrdiff_t findLastIndex(const std::function<bool(T, ptrdiff_t, std::reference_wrapper<RawList<T>>)> &predicate)
    {
        ptrdiff_t crtIndex = length - 1;
        for (Node *currentNode = tail; currentNode != nullptr; currentNode = currentNode->prev, crtIndex--)
        {
            if (predicate(currentNode->data, crtIndex, std::ref(*this)))
                return crtIndex;
        }
        return -1;
    }
    T reduce(const std::function<T(T, T, ptrdiff_t, std::reference_wrapper<RawList<T>>)> &callbackFn, const std::optional<T> &_initialValue = std::nullopt)
    {
        if (length <= 0)
            return T{};
        Node *currentNode = head;
        ptrdiff_t crtIndex = 0;
        T accumulator;
        if (_initialValue.has_value())
            accumulator = *_initialValue;
        else
        {
            accumulator = currentNode->data;
            currentNode = currentNode->next;
            crtIndex++;
        }
        for (; currentNode != nullptr; currentNode = currentNode->next, crtIndex++)
            accumulator = callbackFn(accumulator, currentNode->data, crtIndex, std::ref(*this));
        return accumulator;
    }
    T reduceRight(const std::function<T(T, T, ptrdiff_t, std::reference_wrapper<RawList<T>>)> &callbackFn, const std::optional<T> &_initialValue = std::nullopt)
    {
        if (length <= 0)
            return T{};
        Node *currentNode = tail;
        ptrdiff_t crtIndex = length - 1;
        T accumulator;
        if (_initialValue.has_value())
            accumulator = *_initialValue;
        else
        {
            accumulator = currentNode->data;
            currentNode = currentNode->prev;
            crtIndex--;
        }
        for (; currentNode != nullptr; currentNode = currentNode->prev, crtIndex--)
            accumulator = callbackFn(accumulator, currentNode->data, crtIndex, std::ref(*this));
        return accumulator;
    }
    RawList<T> filter(const std::function<bool(T, ptrdiff_t, std::reference_wrapper<RawList<T>>)> &predicate)
    {
        RawList<T> filterList(0);
        ptrdiff_t crtIndex = 0;
        for (Node *currentNode = head; currentNode != nullptr; currentNode = currentNode->next, crtIndex++)
        {
            if (predicate(currentNode->data, crtIndex, std::ref(*this)))
                filterList.push(currentNode->data);
        }
        return filterList;
    }
    template <typename U>
    RawList<U> map(const std::function<U(T, ptrdiff_t, std::reference_wrapper<RawList<T>>)> &mapFn)
    {
        RawList<U> mapList(0);
        ptrdiff_t crtIndex = 0;
        for (Node *currentNode = head; currentNode != nullptr; currentNode = currentNode->next, crtIndex++)
            mapList.push(mapFn(currentNode->data, crtIndex, std::ref(*this)));
        return mapList;
    }
    RawList<T> concat(const RawList<T> &otherList) const
    {
        RawList<T> concatList{*this};
        for (Node *currentNode = otherList.head; currentNode != nullptr; currentNode = currentNode->next)
            concatList.push(currentNode->data);
        return concatList;
    }
    RawList<T> slice(ptrdiff_t start = 0, std::optional<ptrdiff_t> _end = std::nullopt) const
    {
        RawList<T> sliceList(0);
        ptrdiff_t end = (_end.has_value() ? *_end : length);
        (start < 0 && start > -length) && (start += length) || start < 0 && (start = 0);
        (end < 0 && end > -length) && (end += length) || end > length && (end = length);
        if (start >= length || end <= start)
            return sliceList;
        Node *currentNode = head->operator+(start);
        for (ptrdiff_t i = 0; i < end - start; i++)
        {
            sliceList.push(currentNode->data);
            currentNode = currentNode->next;
        }
        return sliceList;
    }
    RawList<T> splice(ptrdiff_t start, std::optional<ptrdiff_t> _deleteCount = std::nullopt, const std::initializer_list<T> &newElements = {})
    {
        RawList<T> spliceList(0);
        (start < 0 && start > -length) && (start += length) || start < 0 && (start = 0);
        if (start >= length)
            return spliceList;
        ptrdiff_t deleteCount = (_deleteCount.has_value() ? *_deleteCount : length - start);
        deleteCount > length - start && (deleteCount = length - start) || deleteCount < 0 && (deleteCount = 0);
        const_cast<ptrdiff_t &>(length) = length - deleteCount + newElements.size();
        Node *startNodePrev, *endNodeNext;
        bool rebuildHead, rebuildTail;
        if (deleteCount > 0)
        {
            Node *currentNode = head->operator+(start);
            startNodePrev = currentNode->prev;
            rebuildHead = (currentNode == head);
            spliceList.head = currentNode;
            spliceList.head->prev = nullptr;
            for (ptrdiff_t i = 1; i < deleteCount; i++)
                currentNode = currentNode->next;
            endNodeNext = currentNode->next;
            rebuildTail = (currentNode == tail);
            spliceList.tail = currentNode;
            spliceList.tail->next = nullptr;
            const_cast<ptrdiff_t &>(spliceList.length) = deleteCount;
        }
        if (newElements.size() > 0)
        {
            auto [head, tail] = _IL2HeadTail(newElements);
            if (startNodePrev)
                startNodePrev->next = head;
            if (rebuildHead)
                this->head = head;
            if (endNodeNext)
                endNodeNext->prev = tail;
            if (rebuildTail)
                this->tail = tail;
            head->prev = startNodePrev;
            tail->next = endNodeNext;
        }
        else
        {
            if (startNodePrev)
                startNodePrev->next = endNodeNext;
            if (rebuildHead)
                head = endNodeNext;
            if (endNodeNext)
                endNodeNext->prev = startNodePrev;
            if (rebuildTail)
                tail = startNodePrev;
        }
        return spliceList;
    }
    RawList<T> &reverse()
    {
        Node *currentNode = head;
        while (currentNode)
        {
            std::swap(currentNode->prev, currentNode->next);
            // As we have swapped the previous pointer and next pointer,
            // we use "prev" which stands for the next now
            currentNode = currentNode->prev;
        }
        std::swap(head, tail);
        return *this;
    }
    RawList<T> &sort(const std::optional<std::function<ptrdiff_t(T, T)>> &_compareFn = std::nullopt)
    {
        if (!_compareFn.has_value())
            _quickSort(0, length - 1, std::less<T>{});
        else
        {
            auto &compareFn = *_compareFn;
            _quickSort(0, length - 1, [&compareFn](T a, T b)
                       { return compareFn(a, b) < 0; });
        }
        return *this;
    }
    RawList<T> &copyWithin(ptrdiff_t target, ptrdiff_t start, std::optional<ptrdiff_t> _end = std::nullopt)
    {
        ptrdiff_t end = (_end.has_value() ? *_end : length);
        (target < 0 && target > -length) && (target += length) || target < 0 && (target = 0);
        (start < 0 && start > -length) && (start += length) || start < 0 && (start = 0);
        (end < 0 && end > -length) && (end += length) || end > length && (end = length);
        if (target >= length || start >= length || end <= start || target == start)
            return *this;
        ptrdiff_t dist = std::min(end - start, end - target);
        T *temp = (T *)::operator new(dist * sizeof(T));
        Node *currentNode = head->operator+(start);
        for (ptrdiff_t i = 0; i < dist; i++, currentNode = currentNode->next)
            ::new (temp + i) T(std::move(currentNode->data));
        currentNode = head->operator+(target);
        for (ptrdiff_t i = 0; i < dist; i++, currentNode = currentNode->next)
            currentNode->data = std::move(temp[i]);
        ::operator delete(temp);
        return *this;
    }
    RawList<T> &fill(const T &value, ptrdiff_t start = 0, std::optional<ptrdiff_t> _end = std::nullopt)
    {
        ptrdiff_t end = (_end.has_value() ? *_end : length);
        (start < 0 && start > -length) && (start += length) || start < 0 && (start = 0);
        (end < 0 && end > -length) && (end += length) || end > length && (end = length);
        for (Node *currentNode = head->operator+(start); currentNode != nullptr && start < end; currentNode = currentNode->next, start++)
            currentNode->data = value;
        return *this;
    }
    //////////////////////////////////Operator
    T &operator[](ptrdiff_t index)
    {
        if (index >= length)
        {
            for (ptrdiff_t i = length; i < index + 1; i++)
                push(T{});
        }
        else if (index < 0)
            index = 0;
        return head->operator+(index)->data;
    }
    RawList<T> &operator=(const RawList<T> &otherList)
    {
        if (this == &otherList)
            return *this;
        RawList<T> newList{otherList};
        std::swap(const_cast<ptrdiff_t &>(length), const_cast<ptrdiff_t &>(newList.length));
        std::swap(head, newList.head);
        std::swap(tail, newList.tail);
        return *this;
    }
    RawList<T> &operator=(RawList<T> &&rvList)
    {
        std::swap(const_cast<ptrdiff_t &>(length), const_cast<ptrdiff_t &>(rvList.length));
        std::swap(head, rvList.head);
        std::swap(tail, rvList.tail);
        return *this;
    }
    friend std::ostream &operator<<(std::ostream &os, const RawList<T> &list)
    {
        Node *currentNode = list.head;
        os << "\n(" << list.length << ") [";
        while (currentNode)
        {
            os << currentNode->data << (currentNode->next ? ", " : "");
            currentNode = currentNode->next;
        }
        os << "]\n";
        return os;
    }
    //////////////////////////////////Desstructor
    ~RawList()
    {
        Node *currentNode = tail;
        while (currentNode)
        {
            Node *currentNodePrev = currentNode ? currentNode->prev : nullptr;
            currentNode->prev = nullptr;
            delete currentNode;
            currentNode = currentNodePrev;
        }
    }
};

#endif // _RAWLIST_H_