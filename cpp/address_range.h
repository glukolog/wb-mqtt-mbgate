#pragma once

#include <map>
#include <string>
#include <iostream>
#include <memory>
#include <utility>

/*!
 * Address range type template
 * Supports operating with address segments and store
 * additional data with each segment
 */
template<typename T> class TAddressRange
{
public:
    /*! 
     * Insert new data segment
     * \param start First address in segment
     * \param count Number of units in segment
     * \param obs User param for this segment
     * \return
     */
    bool insert(int start, unsigned count, T obs)
    {
        int end = start + count;

        /* check previous neighbor range to merge */
        auto prev = m.lower_bound(start);
        if (prev != m.begin())
            --prev;
        if (prev != m.end() && prev->second.first >= start && prev->first < start) {  // have intersection with previous segment
            if (prev->second.second != obs) 
                throw "Oops, observers doesn't match!";
            
            start = prev->first;

            m.erase(prev);
        }
        
        /* merge all inner segments */
        auto inner = m.upper_bound(start);
        while (inner != m.end() && inner->first < end) {  
            auto& current_observer = inner->second.second;
            auto current_end = inner->second.first;
            // check if there are different observers
            if (current_observer != obs)
                throw "Oops, observers doesn't match in merge!";

            if (current_end > end)
                end = current_end;

            // prepare iterator to remove
            auto to_remove = inner;
            inner++;
            m.erase(to_remove);
        }

        /* create new large segment */
        m.insert(make_pair(start, std::make_pair(end, obs)));

        return true;
    }

    /*!
     * Merge current range with external
     */
    bool insert(const TAddressRange& range)
    {
        bool ret = true;  
        for (auto& segment : range.m)
            ret = ret && insert(segment.first, segment.second.first - segment.first, segment.second.second);

        return ret;
    }

    bool insert(const TAddressRange& range, T obs)
    {
        bool ret = true;
        for (auto& segment : range.m)
            ret = ret && insert(segment.first, segment.second.first - segment.first, obs);

        return ret;
    }

    /*!
     * Check if segment is in this range
     * \param start First address in segment
     * \param count Number of units in segment
     * \return true is whole segment is in this range
     */
    bool in_range(int start, unsigned count = 1) const
    {
        auto prev = m.lower_bound(start);
        return (prev != m.end() && prev->second.first >= start + count);
    }

    /*!
     * Get user param for given segment
     * \param start First address in segment
     * \param count Number of units in segment
     * \return User parameter
     */
    T getParam(int start, int count = 1) const
    {
        auto prev = m.lower_bound(start);

        if (prev != m.end() && prev->second.first <= start + count)
                return prev->second.second;

        /* return PObserver(); /1* like a nullptr *1/ */
        throw "Oops, no such parameter";
    }

    bool operator==(const TAddressRange<T>& r) const
    {
        for (auto &segment : m) {
            auto v = r.m.find(segment.first);
            if (v == r.m.end() || v->second != segment.second)
                return false;
        }

        return true;
    }

    void clear()
    {
        m.clear();
    }


    
    template<typename U>
    friend std::ostream& operator<<(std::ostream &str, const TAddressRange<U>& range);

    

protected:
    std::map<int, std::pair<int, T>> m;
};

/* FIXME: remove this debug */
template<typename T>
std::ostream& operator<<(std::ostream& str, const TAddressRange<T>& range)
{
        str << "### Map info ###" << std::endl;
        for (const auto &p : range.m) {
                str << "[" << p.first << ", " << p.second.first << ")" << std::endl;
        }

        return str;
}