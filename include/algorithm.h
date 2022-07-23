#pragma once

#include "heap_algo.h"
#include "type_traits.h"
#include "algo.h"
#include "mini_vector.h"
#include "concepts.h"
#include <assert.h>
#include <functional>
#include <iostream>

namespace nano {

template<typename InputIter, typename OutputIter, 
        typename Comp = std::less<typename std::iterator_traits<InputIter>::value_type>>
OutputIter move_merge(InputIter first1, InputIter last1,
                InputIter first2, InputIter last2,
                OutputIter dest, const Comp& comp = Comp()) {
    return std::merge(std::move_iterator(first1), std::move_iterator(last2), 
            std::move_iterator(first2), std::move_iterator(last2), 
            dest, comp);
}

/**
 * @brief 插入排序二分搜索优化版
 * @tparam td::random_access_iterator 
 * @tparam std::less<typename std::iterator_traits<RandomAccessIter>::value_type> 
 * @param first 
 * @param last 
 * @param comp 
 */

template<std::random_access_iterator RandomAccessIter, 
        typename Comp = std::less<typename std::iterator_traits<RandomAccessIter>::value_type> >
void binary_insert_sort_dispatch(RandomAccessIter first, RandomAccessIter last, 
        RandomAccessIter start, const Comp& comp = Comp()) {
    for (; start != last; ++start) {
        RandomAccessIter pos = std::upper_bound(first, start, *start, comp);
        for (RandomAccessIter iter2 = start; iter2 != pos; --iter2) {
            std::swap(*iter2, *(iter2 - 1));
        }
    }
}

template<std::random_access_iterator RandomAccessIter, 
        typename Comp = std::less<typename std::iterator_traits<RandomAccessIter>::value_type> >
void binary_insert_sort(RandomAccessIter first, RandomAccessIter last, 
        const Comp& comp = Comp()) {
    RandomAccessIter start = std::is_sorted_until(first, last);
    binary_insert_sort_dispatch(first, last, start, comp);
}

/**
 * @brief 线性插入排序
 * @tparam BidirectionIter 
 * @tparam std::less<typename std::iterator_traits<BidirectionIter>::value_type> 
 * @param first 
 * @param last 
 * @param comp 
 */
template<std::bidirectional_iterator BidirectionIter, 
        typename Comp = std::less<typename std::iterator_traits<BidirectionIter>::value_type> >
void linear_insert_sort_dispatch(BidirectionIter first, BidirectionIter last, 
        BidirectionIter start, const Comp& comp = Comp()) {
    for (; start != last; ++start) {
        for (BidirectionIter iter1 = start; iter1 != first;) {
            BidirectionIter prev = iter1;
            --prev;
            if (comp(*iter1, *prev)) {
                std::swap(*iter1, *prev);
            } else {
                break;
            }
            iter1 = prev;
        }
    }
}

template<std::bidirectional_iterator BidirectionIter, 
        typename Comp = std::less<typename std::iterator_traits<BidirectionIter>::value_type> >
void linear_insert_sort(BidirectionIter first, BidirectionIter last, 
        const Comp& comp = Comp()) {
    BidirectionIter start = std::is_sorted_until(first, last, comp);
    linear_insert_sort_dispatch(first, last, start, comp);
}

/**
 * @brief 插入排序
 * @tparam Iter 
 * @tparam std::less<typename std::iterator_traits<Iter>::value_type> 
 * @param first 
 * @param last 
 * @param comp 
 */
template<typename Iter, typename Comp = std::less<typename std::iterator_traits<Iter>::value_type> >
void insert_sort(Iter first, Iter last, const Comp& comp = Comp()) {
    static_assert(is_bidirectional_iterator_v<Iter>, "random access or bidirectional iterator required");

    if constexpr(is_random_access_iterator_v<Iter>) {
        binary_insert_sort(first, last, comp);
    } else {
        linear_insert_sort(first, last, comp);
    }
}

/**
 * @brief 希尔排序
 * @tparam RandomAccessIter 
 * @tparam std::less<typename std::iterator_traits<RandomAccessIter>::value_type> 
 * @param first 
 * @param last 
 * @param comp 
 */
template<std::random_access_iterator RandomAccessIter, 
        typename Comp = std::less<typename std::iterator_traits<RandomAccessIter>::value_type>>
void shell_sort(RandomAccessIter first, RandomAccessIter last, 
        const Comp& comp = Comp()) {
    using DiffType = typename std::iterator_traits<RandomAccessIter>::difference_type;
    using VType = typename std::iterator_traits<RandomAccessIter>::value_type;
    
    DiffType n = last - first;
    for (DiffType gap = n / 2; gap > 0; gap /= 2) { //gap >>= 1
        for (DiffType i = gap; i < n; ++i) {
            VType tmp = *(first + i);
            DiffType j = i;
            for (; j >= gap && comp(tmp, *(first + j - gap)); j -= gap) {
                *(first + j) = std::move(*(first + j - gap));
            }
            *(first + j) = std::move(tmp);
        }
    }
}

/**
 * @brief 把a, b, c中间值移动到result
 * 
 * @tparam Iterator 
 * @tparam std::less<typename std::iterator_traits<Iterator>::value_type> 
 * @param result
 * @param a 
 * @param b 
 * @param c 
 * @param comp 
 */
template<typename Iterator, typename Comp = std::less<typename std::iterator_traits<Iterator>::value_type>>
void move_median_to_first(Iterator result, Iterator a, Iterator b,
        Iterator c, Comp comp = Comp()) {
    if (comp(*a, *b)) {
        if (comp(*b, *c)) {
            std::iter_swap(result, b);
        } else if (comp(*a, *c)) { 
            std::iter_swap(result, c);
        } else {
            std::iter_swap(result, a);
        }
    } else if (comp(*a, *c)) {
        std::iter_swap(result, a);
    } else if (comp(*b, *c)) {
        std::iter_swap(result, c);
    } else {
        std::iter_swap(result, b);
    }
}

template<std::random_access_iterator RandomAccessIter, 
        typename Comp = std::less<typename std::iterator_traits<RandomAccessIter>::value_type> >
RandomAccessIter unguarded_partition(RandomAccessIter first,
        RandomAccessIter last, RandomAccessIter pivot, Comp comp = Comp()) {
    while (true) {
        while (comp(*first, *pivot)) {
            ++first;
        }
        --last;
        while (comp(*pivot, *last)) {
            --last;
        }
        if (!(first < last)) {
            return first;
        }
        std::iter_swap(first, last);
        ++first;
    }
}

template<std::random_access_iterator RandomAccessIter, 
        typename Comp = std::less<typename std::iterator_traits<RandomAccessIter>::value_type>>
inline RandomAccessIter unguarded_partition_pivot(RandomAccessIter first,
        RandomAccessIter last, Comp comp = Comp()) {
    RandomAccessIter mid = first + (last - first) / 2;
    move_median_to_first(first, first + 1, mid, last - 1, comp);
    return unguarded_partition(first + 1, last, first, comp);
}

/**
 * @brief 快速排序递归最大深度
 * @attention C++17 required
 */
inline static constexpr int MAX_DEPTH = 32;

template<std::random_access_iterator RandomAccessIter, 
        typename Comp = std::less<typename std::iterator_traits<RandomAccessIter>::value_type> >
void introsort_dispatch(RandomAccessIter first, RandomAccessIter last, 
        int depth_limit, const Comp& comp = Comp()) {
    while (last - first > MAX_DEPTH) {
        if (0 == depth_limit) { //递归到一定深度，转为堆排序
	        heap_sort(first, last, comp);
	        return;
	    }
	    --depth_limit;
	    RandomAccessIter cut = unguarded_partition_pivot(first, last, comp);
	    introsort_dispatch(cut, last, depth_limit, comp);
	    last = cut;
    }
    insert_sort(first, last, comp);
}

/**
 * @brief 内省排序
 * @tparam RandomAccessIter 
 * @tparam std::less<typename std::iterator_traits<RandomAccessIter>::value_type> 
 * @param first 
 * @param last 
 * @param comp 
 */
template<std::random_access_iterator RandomAccessIter, 
        typename Comp = std::less<typename std::iterator_traits<RandomAccessIter>::value_type> >
void intro_sort(RandomAccessIter first, RandomAccessIter last, const Comp& comp = Comp()) {
    introsort_dispatch(first, last, log2(last - first) * 2, comp);
}

/**
 * @brief timsort, 参考jdk1.8
 * @tparam RandomAccessIter 
 * @tparam std::less<typename std::iterator_traits<RandomAccessIter>::value_type> 
 */
template<std::random_access_iterator RandomAccessIter, 
        typename Comp = std::less<typename std::iterator_traits<RandomAccessIter>::value_type>>
struct __TimSort{
public:
    using DiffType = typename std::iterator_traits<RandomAccessIter>::difference_type;
    using VType    = typename std::iterator_traits<RandomAccessIter>::value_type;

    constexpr static int MIN_MERGE = 32;
    constexpr static int MIN_GALLOP = 7;

    __TimSort(const Comp& comp = Comp()) :
        m_minGallop(MIN_GALLOP),
        m_comp(comp) {
    }

public:
    void sort(RandomAccessIter first, RandomAccessIter last) {
        if (first <= last) {
            return;
        }
        
        m_first = first;
        m_last = last;
        RandomAccessIter oldFirst = first;
        // If array is small, do a "mini-TimSort" with no merges
        DiffType nRemaining = last - first;
        if (nRemaining < MIN_MERGE) {
            DiffType initRunLen = __count_and_make_run(first, last);
            binary_insert_sort_dispatch(first, last, first + initRunLen, m_comp);
            return;
        }

        // March over the array once, left to right, finding natural runs,
        // extending short natural runs to minRun elements, and merging runs
        // to maintain stack invariant.
        DiffType minRun = __min_run(nRemaining);
        do {
            // Identify next run
            DiffType runLen = __count_and_make_run(first, last);

            // If run is short, extend to min(minRun, nRemaining)
            if (runLen < minRun) {
                DiffType force = nRemaining <= minRun ? nRemaining : minRun;
                binary_insert_sort_dispatch(first, first + force, first + runLen, m_comp);
                runLen = force;
            }

            // Push run onto pending-run stack, and maybe merge
            m_runStack.emplace_back(first - oldFirst, runLen);
            __merge_collapse(oldFirst);

            // Advance to find next run
            first += runLen;
            nRemaining -= runLen;
        } while (nRemaining != 0);
        // Merge all remaining runs to complete sort
        __merge_force_collapse(oldFirst);
    }

private:
    /**
     * @brief for debug
     */
    template<typename OutputIter>
    void printSequence(OutputIter first, OutputIter last) {
        OutputIter statr = first;
        for (; first != last; ++first) {
            if (first != statr) {
                std::cout << ",";
            }
            std::cout << *first;
        }
        std::cout << std::endl;
    }
    
    void printRunStack() {
        for (auto iter = m_runStack.begin();
            iter != m_runStack.end(); ++iter) {
            std::cout << "[" << iter->first 
                << "," << iter->second << "]" << std::endl;
        }
    }

    DiffType __min_run(DiffType n) {
        DiffType r = 0;      // Becomes 1 if any 1 bits are shifted off
        while (n >= MIN_MERGE) {
            r |= (n & 1);
            n >>= 1;
        }
        return n + r;
    }

    DiffType __count_and_make_run(RandomAccessIter first, RandomAccessIter last) {
        if (first == last) {
            return 0;
        }

        RandomAccessIter runHigh = first + 1;
        if (runHigh == last) {
            return 1;
        }
        
        if (m_comp(*(runHigh++), *first)) { // Descending
            //找递减序列
            while (runHigh != last && m_comp(*runHigh, *(runHigh - 1))) {
                ++runHigh;
            }
            std::reverse(first, runHigh); //翻转为递增序列
        } else {                              // Ascending
            //找递增序列
            while (runHigh != last && !m_comp(*runHigh, *(runHigh - 1))) {
                ++runHigh;
            }
        }

        return runHigh - first;
    }

    /**
     * @brief 找到在序列[first, last)中第一个大于等于key的位置
     * @tparam T 
     * @tparam RandomAccessIter 
     * @param key 
     * @param first 
     * @param last 
     * @param hint 
     * @return RandomAccessIter 
     */
    template<std::random_access_iterator RIter>
    RIter __gallop_left(const VType& key, RIter first, RIter last, 
            RIter hint) {
        DiffType lastOfs = 0;
        DiffType ofs = 1;
        if (m_comp(*hint, key)) { //*hint<key, 搜索hint的右边
            DiffType maxOfs = last - hint;
            while (ofs < maxOfs && m_comp(*(hint + ofs), key)) { //*(hint + ofs) < key
                lastOfs = ofs;
                ofs = (ofs << 1) + 1;
                if (ofs <= lastOfs) { // overflow
                    ofs = maxOfs;
                }
            }
            if (ofs > maxOfs) {
                ofs = maxOfs;
            }
            //Now *(hint + lastOfs) < key <= *(hint + ofs)
        } else { //key <= *hint, 搜索hint的左边
            DiffType maxOfs = (hint - first + 1); //hint需要包含
            while (ofs < maxOfs && !m_comp(*(hint - ofs), key)) { //key <= *(hint - ofs)
                lastOfs = ofs;
                ofs = (ofs << 1) + 1;
                if (ofs <= lastOfs) {   //overflow
                    ofs = maxOfs;
                }
            }
            if (ofs > maxOfs) {
                ofs = maxOfs;
            }
            //now *(hint - ofs) < key <= *(hint - lastOfs)
            //but both ofs and lastOfs should be negative
            DiffType tmp = ofs;
            ofs = -lastOfs;
            lastOfs = -tmp;
            //Now *(hint + lastOfs) < key <= *(hint + ofs)
        }

        // Now *(hint + lastOfs) < key <= *(hint + ofs), so key belongs somewhere
        // to the right of lastOfs but no farther right than ofs.  Do a binary
        // search, with invariant *(hint + lastOfs) < key <= *(hint + ofs).
        return std::lower_bound(hint + lastOfs + 1, hint + ofs, key, m_comp);
    }

    /**
     * @brief 找到在序列[first, last)中第一个大于key的位置
     * @tparam T 
     * @tparam RIter 
     * @param key 
     * @param first 
     * @param last 
     * @param hint 
     * @return RIter 
     */
    template<std::random_access_iterator RIter>
    RIter __gallop_right(const VType& key, RIter first, RIter last, 
            RIter hint) {
        DiffType ofs = 1;
        DiffType lastOfs = 0;
        if (m_comp(key, *hint)) { //key < hint, 在hint左边查询
            // Gallop left until *(hint - ofs) <= key < *(hint - lastOfs)
            DiffType maxOfs = (hint - first + 1); //包含hint
            while (ofs < maxOfs && m_comp(key, *(hint - ofs))) { //key < *(hint - ofs)
                lastOfs = ofs;
                ofs = (ofs << 1) + 1;
                if (ofs <= lastOfs) {   // overflow
                    ofs = maxOfs;
                }
            }
            if (ofs > maxOfs) {
                ofs = maxOfs;
            }
            //Now *(hint - ofs) <= key < *(hint - lastOfs)
            //but both ofs and lastOfs should be negative
            DiffType tmp = ofs;
            ofs = -lastOfs;
            lastOfs = -tmp;
            //Now *(hint + lastOfs) <= key < *(hint + ofs)
        } else { // key >= *hint
            // Gallop right until *(hint + lastOfs) <= key < *(hint + ofs)
            int maxOfs = last - hint; 
            while (ofs < maxOfs && !m_comp(key, *(hint + ofs))) { //key >= *(hint + ofs)
                lastOfs = ofs;
                ofs = (ofs << 1) + 1;
                if (ofs <= lastOfs) {  // overflow
                    ofs = maxOfs;
                }
            }
            if (ofs > maxOfs) {
                ofs = maxOfs;
            }
            //Now *(hint + lastOfs) <= key < *(hint + ofs)
        }

        // Now *(hint + lastOfs) <= key < *(hint +  ofs), so key belongs somewhere to
        // the right of lastOfs but no farther right than ofs.  Do a binary
        // search, with invariant *(hint + lastOfs) <= key < *(hint + ofs).
        return std::upper_bound(hint + lastOfs + 1, hint + ofs, key, m_comp);
    }

    /**
     * @brief 从前往后合并两个run
     * @tparam RandomAccessIter 
     * @tparam Size 
     * @param first1 
     * @param last1 
     * @param first2 
     * @param last2 
     * @param dest 
     * @param minGallop 
     * @attention len1 means length of range [first1, last1)
     *            len2 means length of range [first2, last2)
     *            len1 <= len2 required and
     *            comp(*first2, *first1) is true(such as *first2 < *first1) and
     *            comp(*(last2 - 1), *(last1 - 1))(such as *(last2 - 1) < *(last1 - 1)) is true
     */
    void __merge_low(RandomAccessIter first1, RandomAccessIter last1, 
            RandomAccessIter first2, RandomAccessIter last2) {
        assert(last1 == first2);
        RandomAccessIter cursor2 = first2; 
        RandomAccessIter dest = first1;
        //把[first1, last1)拷贝到tmp中
        m_tmp.reserve(last1 - first1);
        move_copy(first1, last1, std::back_insert_iterator(m_tmp));
        typename mini_vector<VType>::iterator cursor1 = m_tmp.begin();

        // Move first element of second run and deal with degenerate cases
        *(dest++) = std::move(*(cursor2++));
        if (cursor2 == last2) {
            move_copy(m_tmp.begin(), m_tmp.end(), dest);
            return;
        }
        if (cursor1 + 1 == m_tmp.end()) { //*cursor1 is max element
            dest = move_copy(cursor2, last2, dest);
            *(dest) = std::move(*cursor1);  // Last elt of run 1 to end of merge
            return;
        }
        int minGallop = m_minGallop;
        while (true) {
            DiffType count1 = 0; // Number of times in a row that first run won
            DiffType count2 = 0; // Number of times in a row that second run won
            // Do the straightforward thing until (if ever) one run starts
            // winning consistently.
            do {
                if (m_comp(*cursor2, *cursor1)) {
                    *(dest++) = std::move(*(cursor2++));
                    count2++;
                    count1 = 0;
                    if (cursor2 == last2) {
                        goto out;
                    }
                } else {
                    *(dest++) = std::move(*(cursor1++));
                    count1++;
                    count2 = 0;
                    if (cursor1 + 1 == m_tmp.end()) {
                        goto out;
                    }
                }
            } while ((count1 | count2) < minGallop);
            
            // One run is winning so consistently that galloping may be a
            // huge win. So try that, and continue galloping until (if ever)
            // neither run appears to be winning consistently anymore.
            do {
                auto rbound = __gallop_right(*cursor2, cursor1, m_tmp.end(), cursor1);
                if (cursor1 != rbound) {
                    dest = move_copy(cursor1, rbound, dest);
                    cursor1 = rbound;
                    if ((m_tmp.end() - cursor1) <= 1) {  // len == 1 || len == 0
                        goto out;
                    }
                }
                *(dest++) = std::move(*(cursor2++));
                if (cursor2 == last2) {
                    goto out;
                }

                RandomAccessIter lbound = __gallop_left(*cursor1, cursor2, last2, cursor2);
                if (cursor2 != lbound) {
                    dest = move_copy(cursor2, lbound, dest);
                    cursor2 = lbound;
                    if (cursor2 == last2) { // len == 1 || len == 0
                        goto out;
                    }
                }
                *(dest++) = std::move(*(cursor1++));
                if (cursor1 + 1 == m_tmp.end()) {
                    goto out;
                }
                --minGallop;
            } while ((count1 >= static_cast<DiffType>(MIN_GALLOP)) | (count2 >= static_cast<DiffType>(MIN_GALLOP)));

            if (minGallop < 0) {
                minGallop = 0;
            }
            minGallop += 2;  // Penalize for leaving gallop mode
        }
    out:

        m_minGallop = minGallop < 1 ? 1 : minGallop;  // Write back to field
        DiffType len = (m_tmp.end() - cursor1);
        if (1 == len) {
            dest = move_copy(cursor2, last2, dest);
            *(dest) = std::move(*cursor1); //  Last elt of run 1 to end of merge
        } else if (0 == len) {
            //impossible except arguments are illegal
            std::abort();
        } else {
            move_copy(cursor1, m_tmp.end(), dest);
        }
        m_tmp.clear();
    }

    /**
     * @brief 从后往前合并两个run
     * @tparam RandomAccessIter 
     * @tparam Size 
     * @tparam std::less<typename std::iterator_traits<RandomAccessIter>::value_type> 
     * @param first1 
     * @param last1 
     * @param first2 
     * @param last2 
     * @param dest 
     * @param minGallop 
     * @param comp 
     * @attention same as __merge_low except len1 >= len2
     */
    void __merge_high(RandomAccessIter first1, RandomAccessIter last1, 
            RandomAccessIter first2, RandomAccessIter last2) {
        RandomAccessIter dest = last2 - 1;
        m_tmp.reserve(last2 - first2);
        move_copy(first2, last2, std::back_insert_iterator(m_tmp));

        RandomAccessIter cursor1 = last1 - 1;  
        typename mini_vector<VType>::iterator cursor2 = m_tmp.end() - 1;

        // Move last element of first run and deal with degenerate cases
        *(dest--) = std::move(*(cursor1--));
        if (cursor1 < first1) {
            move_copy_backward(m_tmp.begin(), m_tmp.end(), dest + 1);
            return;
        }
        if (m_tmp.begin() == cursor2) { //cursor2最小
            dest = move_copy_backward(first1, cursor1 + 1, dest + 1) - 1;
            *(dest) = std::move(*(cursor2));
            return;
        }

        int minGallop = m_minGallop;  // Use local variable for performance
        while (true) {
            DiffType count1 = 0; // Number of times in a row that first run won
            DiffType count2 = 0; // Number of times in a row that second run won
            // Do the straightforward thing until (if ever) one run
            // appears to win consistently.
            do {
                if (m_comp(*cursor2, *cursor1)) {
                    *(dest--) = *(cursor1--);
                    ++count1;
                    count2 = 0;
                    if (cursor1 < first1) {
                        goto out;
                    }
                } else {
                    *(dest--) = *(cursor2--);
                    ++count2;
                    count1 = 0;
                    if (cursor2 <= m_tmp.begin()) {
                        goto out;
                    }
                }
            } while ((count1 | count2) < minGallop);

            // One run is winning so consistently that galloping may be a
            // huge win. So try that, and continue galloping until (if ever)
            // neither run appears to be winning consistently anymore.
            do {
                RandomAccessIter ubound = __gallop_right(*(cursor2), first1, cursor1 + 1, first1);
                if (ubound != cursor1 + 1) {
                    dest = move_copy_backward(ubound, cursor1 + 1, dest + 1) - 1;
                    cursor1 = ubound - 1;
                    if (cursor1 < first1) {
                        goto out;
                    }
                }
                //Now [first1, cursor1] is less equal to *cursor2
                *(dest--) = std::move(*(cursor2--));
                if (cursor2 <= m_tmp.begin()) {
                    goto out;
                }

                auto lbound = __gallop_left(*cursor1, m_tmp.begin(), cursor2 + 1, cursor2);
                if (lbound != cursor2 + 1) {
                    dest = move_copy_backward(lbound, cursor2 + 1, dest + 1) - 1;
                    cursor2 = lbound - 1;
                    if (cursor2 <= m_tmp.begin()) {
                        goto out; // len2 == 1 || len2 == 0
                    }
                }
                *(dest--) = std::move(*(cursor1--));
                if (cursor1 < first1) {
                    goto out;
                }
                --minGallop;
            } while ((count1 >= MIN_GALLOP) | (count2 >= MIN_GALLOP));

            if (minGallop < 0) {
                minGallop = 0;
            }
            minGallop += 2;  // Penalize for leaving gallop mode
        }  // End of "outer" loop
    out:
        m_minGallop = minGallop < 1 ? 1 : minGallop;  // Write back to field

        DiffType len = (cursor2 - m_tmp.begin() + 1);
        if (1 == len) {
            dest = move_copy_backward(first1, cursor1 + 1, dest + 1) - 1;
            *(dest) = *(cursor2);  // Move first elt of run2 to front of merge
        } else if (0 == len) {
            std::abort();
            return;
        } else {
            move_copy_backward(m_tmp.begin(), cursor2 + 1, dest + 1);
        }
        m_tmp.clear();
    }

    void __merge_at(RandomAccessIter rangeFirst, DiffType position) {
        // Record the length of the combined runs; if i is the 3rd-last
        // run now, also slide over the last run (which isn't involved
        // in this merge).  The current run (i+1) goes away in any case.
        RandomAccessIter first1 = rangeFirst + m_runStack[position].first;
        RandomAccessIter last1 = first1 + m_runStack[position].second;
        RandomAccessIter first2 = rangeFirst + m_runStack[position + 1].first;
        RandomAccessIter last2 = first2 + m_runStack[position + 1].second;

        assert(last1 == first2);

        m_runStack[position].second += m_runStack[position + 1].second;
        if (position + 3 == static_cast<DiffType>(m_runStack.size())) {
            m_runStack[position + 1] = m_runStack[position + 2];
        }
        m_runStack.pop_back();
        // Find where the first element of run2 goes in run1. Prior elements
        // in run1 can be ignored (because they're already in place).
        first1 = __gallop_right(*first2, first1, last1, first1);

        if (first1 == last1) {
            return;
        }

        // Find where the last element of run1 goes in run2. Subsequent elements
        // in run2 can be ignored (because they're already in place).
        last2 = __gallop_left(*(last1 - 1), first2, 
                last2, last2 - 1);
        if (first2 == last2) {
            return;
        }

        // Merge remaining runs, using m_tmp array with min(len1, len2) elements;
        if (last1 - first1 <= last2 - first2) {
            __merge_low(first1, last1, first2, last2);
        } else {
            __merge_high(first1, last1, first2, last2);
        }
    }

    void __merge_collapse(RandomAccessIter rangeFirst) {
        while (m_runStack.size() > 1) {
            DiffType n = static_cast<DiffType>(m_runStack.size()) - 2;
            if (n > 0 && m_runStack[n - 1].second <= m_runStack[n].second + m_runStack[n + 1].second) {
                if (m_runStack[n - 1].second < m_runStack[n + 1].second) {
                    --n;
                }
                __merge_at(rangeFirst, n);
            } else if (m_runStack[n].second <= m_runStack[n + 1].second) {
                __merge_at(rangeFirst, n);
            } else {
                break; // Invariant is established
            }
        }
    }

    void __merge_force_collapse(RandomAccessIter rangeFirst) {
        while (m_runStack.size() > 1) {
            DiffType n = static_cast<DiffType>(m_runStack.size()) - 2;
            if (n > 0 && m_runStack[n - 1].second < m_runStack[n + 1].second) {
                --n;
            }
            __merge_at(rangeFirst, n);
        }
    }

private:
    mini_vector<std::pair<DiffType, DiffType>> m_runStack; //position -> len
    mini_vector<VType> m_tmp;
    int m_minGallop;
    //for debug
    RandomAccessIter m_first;
    RandomAccessIter m_last;
    Comp m_comp;
};

template<std::random_access_iterator RandomAccessIter, 
        typename Comp = std::less<typename std::iterator_traits<RandomAccessIter>::value_type>>
void tim_sort(RandomAccessIter first, RandomAccessIter last, const Comp& comp = Comp()) {
    __TimSort<RandomAccessIter, Comp> __timsort(comp);
    __timsort.sort(first, last);
}

} //namespace nano