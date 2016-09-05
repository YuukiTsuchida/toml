#ifndef __TOML_HPP__
#define __TOML_HPP__

#include <iostream>

#include <algorithm>
#include <chrono>
#include <ctime>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace toml 
{

class date_time
{
public:
    constexpr date_time() :
        year_(0),
        month_(0),
        day_(0),
        hour_(0),
        minute_(0),
        second_(0),
        hour_offset_(0),
        minute_offset_(0),
        current_time_(0)
    {}

    date_time(const date_time& other) :
        year_(other.year_),
        month_(other.month_),
        day_(other.day_),
        hour_(other.hour_),
        minute_(other.minute_),
        second_(other.second_),
        hour_offset_(other.hour_offset_),
        minute_offset_(other.minute_offset_),
        current_time_(other.current_time_)
    {}

    date_time(date_time&& other) :
        year_(std::move(other.year_)),
        month_(std::move(other.month_)),
        day_(std::move(other.day_)),
        hour_(std::move(other.hour_)),
        minute_(std::move(other.minute_)),
        second_(std::move(other.second_)),
        hour_offset_(std::move(other.hour_offset_)),
        minute_offset_(std::move(other.minute_offset_)),
        current_time_(std::move(other.current_time_))
    {}

    inline void set_date(uint32_t year, uint32_t month, uint32_t day)
    {
        set_date_time(std::move(year), std::move(month), std::move(day), hour_, minute_, second_);
    }

    inline void set_time(uint32_t hour, uint32_t minute, uint32_t second)
    {
        set_date_time(year_, month_, day_, std::move(hour), std::move(minute), std::move(second));
    }

    inline void set_date_time(uint32_t year, uint32_t month, uint32_t day, uint32_t hour, uint32_t minute, uint32_t second)
    {
        year_ = std::move(year);
        month_ = std::move(month);
        day_ = std::move(day);
        hour_ = std::move(hour);
        minute_ = std::move(minute);
        second_ = std::move(second);

        update_current_time();
    }
    
    inline void from_utc(time_t time)
    {
        current_time_ = time;
        std::tm* tm = std::gmtime(&time);
        from_utc(*tm);
    }

    inline void from_local(time_t time)
    {
        current_time_ = time;
        std::tm* tm = std::localtime(&time);
        from_local(*tm);
    }

    inline bool operator == (const date_time& other)
    {
        return (current_time_ == other.current_time_);
    }

    inline bool operator != (const date_time& other)
    {
        return !((*this) == other);
    }

    inline bool operator < (const date_time& other)
    {
        if(current_time_ < other.current_time_)
        {
            return true;
        }

        return false;
    }

    inline bool operator <= (const date_time& other)
    {
        if(current_time_ <= other.current_time_)
        {
            return true;
        }

        return false;
    }

    inline bool operator > (const date_time& other)
    {
        return !(*this <= other);
    }

    inline bool operator >= (const date_time& other)
    {
        return !(*this < other);
    }

    inline date_time& operator = (const date_time& other)
    {
        year_ = other.year_;
        month_ = other.month_;
        day_ = other.day_;
        hour_ = other.hour_;
        minute_ = other.minute_;
        second_ = other.second_;
        hour_offset_ = other.hour_offset_;
        minute_offset_ = other.minute_offset_;
        current_time_ = other.current_time_;
        
        return *this;
    }

    inline date_time& operator = (const date_time&& other)
    {
        year_ = std::move(other.year_);
        month_ = std::move(other.month_);
        day_ = std::move(other.day_);
        hour_ = std::move(other.hour_);
        minute_ = std::move(other.minute_);
        second_ = std::move(other.second_);
        hour_offset_ = std::move(other.hour_offset_);
        minute_offset_ = std::move(other.minute_offset_);
        current_time_ = std::move(other.current_time_);

        return *this;
    }

    static inline date_time now_from_utc()
    {
        auto now_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

        date_time date;
        date.from_utc(now_time);

        return date;
    }

    static inline date_time now_from_local()
    {
        auto now_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

        date_time date;
        date.from_local(now_time);

        return date;
    }

private:
    inline void update_current_time()
    {
      tm t;
      t.tm_year = year_ -1900;
      t.tm_mon = month_;
      t.tm_mday = day_;
      t.tm_hour = hour_;
      t.tm_min = minute_;
      t.tm_sec = second_;

      current_time_ = std::mktime(&t) - hour_offset_ * 3600 - minute_offset_ * 60;
    }

private:
    inline void from_utc(const tm& t)
    {
        year_ = t.tm_year + 1900;
        month_ = t.tm_mon;
        day_ = t.tm_mday;
        hour_ = t.tm_hour;
        minute_ = t.tm_min;
        second_ = t.tm_sec;
    }

    inline void from_local(const tm& t)
    {
        year_ = t.tm_year + 1900;
        month_ = t.tm_mon;
        day_ = t.tm_mday;
        hour_ = t.tm_hour;
        minute_ = t.tm_min;
        second_ = t.tm_sec;

        char buf[16];
        std::strftime(buf, 16, "%z", &t);

        int offset = std::stoi(buf);
        hour_offset_ = offset / 100;
        minute_offset_ = offset % 100;
    }
private:
    uint32_t year_;
    uint32_t month_;
    uint32_t day_;
    uint32_t hour_;
    uint32_t minute_;
    uint32_t second_;
    int32_t hour_offset_;
    int32_t minute_offset_;
    time_t current_time_;
};

class base
{
public:
    enum class data_type
    {
        integer,
        floaing,
        string,
        boolean,
        date,
        array,
        table 
    };

    base(data_type type) : type_(type)
    {}

    virtual ~base() {}

    inline data_type get_type() const
    {
        return type_;
    }

    template<class type>
    inline bool is() const
    {
        return false;
    }

private:
    data_type type_;
};

template<class value_data, base::data_type type>
class value : public base
{
public:
    using value_type = value_data;

    value() : base(type)
    {}

    value(value_data data) : base(type), data_(data)
    {}

    value(const value<value_data, type>& other) : base(type), data_(other.data_)
    {}

    value(value<value_data, type>&& other) : base(type), data_(std::move(other.data_))
    {}

    virtual ~value() {}

    inline value_data& get()
    {
        return data_;
    }

    inline const value_data& get() const
    {
        return data_;
    }

    inline value<value_data, type>& operator = (const value_data& data)
    {
        data_ = data;
        return *this;
    }

protected:
    value_data data_;
};

using int_value = value<int, base::data_type::integer>;
using float_value = value<float, base::data_type::floaing>;
using string_value = value<std::string, base::data_type::string>;
using bool_value = value<bool, base::data_type::boolean>;
using date_time_value = value<date_time, base::data_type::date>;

class array : public value<std::vector<std::shared_ptr<base>>, base::data_type::array>
{
public:
    using base_type = value<std::vector<std::shared_ptr<base>>, base::data_type::array>;
    using container_type = typename base_type::value_type;
    using base_value_type = typename container_type::value_type::element_type;

    using iteraotr = typename container_type::iterator;

    array() = default;
    
    array(const std::vector<std::shared_ptr<base>>& data) : base_type(data) 
    {}

    array(const array& other) : base_type(other)
    {}

    array(array&& other) : base_type(std::move(other))
    {}

    template<class value_data>
    inline void add(value_data data)
    {
        std::shared_ptr<value_data> data_ptr = std::make_shared<value_data>(data);
        data_.emplace_back(std::static_pointer_cast<base>(data_ptr));
    }

    template<class value_data>
    inline void add(std::shared_ptr<value_data> data)
    {
        data_.emplace_back(std::static_pointer_cast<base>(data));
    }

    inline size_t size() const
    {
        return data_.size();
    }

    template<class value_data>
    inline std::shared_ptr<value_data> get_at_as(int index)
    {
        std::shared_ptr<base> base_type_value = operator[](index);
        return std::dynamic_pointer_cast<value_data>(base_type_value);
    }

    template<class value_data>
    inline std::vector<std::shared_ptr<value_data>> get_array_as()
    {
        std::vector<std::shared_ptr<value_data>> result(data_.size());

        std::transform(data_.begin(), data_.end(), result.begin(),
                [&](std::shared_ptr<base> data){ return std::dynamic_pointer_cast<value_data>(data); });

        return result;
    }

    inline std::shared_ptr<base> operator[] (int index)
    {
        
        return data_[index];
    }

    inline iteraotr begin()
    {
        return data_.begin();
    }

    inline iteraotr end()
    {
        return data_.end();
    }
};

class table : public value<std::unordered_map<std::string, std::shared_ptr<base>>, base::data_type::table>
{
public:
    using base_type = value<std::unordered_map<std::string, std::shared_ptr<base>>, base::data_type::table>;
    using container_type = typename base_type::value_type;
        
    using iteraotr = typename container_type::iterator;

    table() = default;

    table(const std::unordered_map<std::string, std::shared_ptr<base>>& other) : base_type(other)
    {}

    table(std::unordered_map<std::string, std::shared_ptr<base>>&& other) : base_type(std::move(other))
    {}

    inline bool has(const std::string& key)
    {
        auto find_result = data_.find(key);
        return find_result != data_.end();
    }

    inline bool add(const std::string& key, std::shared_ptr<base> value)
    {
        std::cout << typeid(std::shared_ptr<base>::element_type).name() << std::endl;
        auto result = data_.emplace(key, value);

        return result.second;
    }

    template<class value_data>
    inline std::shared_ptr<value_data> get_as(const std::string& key)
    {
        if(!has(key))
        {

            return nullptr;
        }

        std::shared_ptr<base> base_type_value = data_[key];
        return std::dynamic_pointer_cast<value_data>(base_type_value);
    }

    template<class value_data>
    inline std::vector<std::shared_ptr<value_data>> get_as(const std::string& key)
    {
        if(!has(key))
        {
            return nullptr;
        }

        std::shared_ptr<array> array_type_value = std::dynamic_pointer_cast<array>(data_[key]);
        return array_type_value->get_array_as<value_data>();

    }

    template<class data_type>
    inline std::shared_ptr<data_type> create(const std::string& key)
    {
        if(has(key))
        {
            return nullptr;
        }

        auto new_data = std::make_shared<data_type>();
        add(key, new_data);

        return new_data;
    }

    inline std::shared_ptr<base> operator[](const std::string& key)
    {
        return data_[key];
    }

    inline iteraotr begin()
    {
        return data_.begin();
    }

    inline iteraotr end()
    {
        return data_.end();
    }

};

template<>
inline bool base::is<int>() const
{
    return type_ == data_type::integer;
}

template<>
inline bool base::is<float>() const
{
    return type_ == data_type::floaing;
}

template<>
inline bool base::is<std::string>() const
{
    return type_ == data_type::string;
}

template<>
inline bool base::is<bool>() const
{
    return type_ == data_type::boolean;
}

template<>
inline bool base::is<date_time>() const
{
    return type_ == data_type::date;
}

template<>
inline bool base::is<array>() const
{
    return type_ == data_type::array;
}

template<>
inline bool base::is<table>() const
{
    return type_ == data_type::table;
}

} // namespace toml

#endif 