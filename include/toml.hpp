#ifndef __TOML_HPP__
#define __TOML_HPP__

#include <iostream>

#include <iomanip>
#include <sstream>
#include <ostream>
#include <cassert>

#include <algorithm>
#include <chrono>
#include <ctime>
#include <memory>
#include <string>
#include <stdexcept>
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

    inline void accept(std::ostream& stream)
    {
      char offset_sign = (hour_offset_ > 0) ? ('+') : ('-');

      stream << year_ << "-" << month_ << "-" << day_ << "T";
      stream << hour_ << ":" << minute_ << ":" << second_;
      stream << offset_sign;
      stream << std::setw(2) << std::setfill('0') << std::abs(hour_offset_) << ":";
      stream << std::setw(2) << std::setfill('0') << std::abs(hour_offset_);
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

static inline std::ostream& operator << (std::ostream& stream, date_time& date)
{
  date.accept(stream);
  return stream;
}

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

    virtual void accept(std::ostream& stream) = 0;
private:
    data_type type_;
};

static inline std::ostream& operator << (std::ostream& stream, base& base_data)
{
  base_data.accept(stream);
  return stream;
}

template<class value_data, base::data_type type>
class value : public base
{
public:
    using value_type = value_data;

    value() : base(type)
    {}

    value(const value_data& data) : base(type), data_(data)
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

    virtual inline void accept(std::ostream&) override
    {}

protected:
    value_data data_;
};

template<>
inline void value<int, base::data_type::integer>::accept(std::ostream& stream)
{
  stream << data_;
}

template<>
inline void value<float, base::data_type::floaing>::accept(std::ostream& stream)
{
  stream << data_;
}

template<>
inline void value<std::string, base::data_type::string>::accept(std::ostream& stream)
{
  stream << data_;
}

template<>
inline void value<bool, base::data_type::boolean>::accept(std::ostream& stream)
{
  std::string bool_str = (data_) ? ("true") : ("false");
  stream << bool_str;
}

template<>
inline void value<date_time, base::data_type::date>::accept(std::ostream& stream)
{
  stream << data_;
}

using int_value = value<int, base::data_type::integer>;
using float_value = value<float, base::data_type::floaing>;
using string_value = value<std::string, base::data_type::string>;
using bool_value = value<bool, base::data_type::boolean>;
using date_time_value = value<date_time, base::data_type::date>;

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

class array;
template<>
inline bool base::is<array>() const
{
    return type_ == data_type::array;
}

class table;
template<>
inline bool base::is<table>() const
{
    return type_ == data_type::table;
}

class array : public value<std::vector<std::shared_ptr<base>>, base::data_type::array>
{
public:
    using base_type = value<std::vector<std::shared_ptr<base>>, base::data_type::array>;
    using container_type = typename base_type::value_type;
    using base_value_type = typename container_type::value_type::element_type;

    using iterator = typename container_type::iterator;
    using const_iterator = typename container_type::const_iterator;

    array() = default;
    
    array(const std::vector<std::shared_ptr<base>>& data) : base_type(data) 
    {}

    array(const array& other) : base_type(other)
    {}

    array(array&& other) : base_type(std::move(other))
    {}

    template<class value_data>
    inline void add(const value_data& data)
    {
        std::shared_ptr<value_data> data_ptr = std::make_shared<value_data>(data);
        data_.emplace_back(std::static_pointer_cast<base>(data_ptr));
    }

    template<class value_data>
    inline void add(const std::string& key, std::shared_ptr<value_data> value)
    {
        data_.emplace_back(std::static_pointer_cast<base>(value));
    }

    inline void add(const int& data)
    {
        add(int_value(data));
    }

    inline void add(const float& data)
    {
        add(float_value(data));
    }

    inline void add(const char* data)
    {
        add(std::string(data));
    }

    inline void add(const std::string& data)
    {
        add(string_value(data));
    }

    inline void add(const bool& data)
    {
        add(bool_value(data));
    }

    inline void add(const date_time& data)
    {
        add(date_time_value(data));
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

    inline iterator begin()
    {
        return data_.begin();
    }

    inline iterator end()
    {
        return data_.end();
    }

    inline const_iterator begin() const
    {
        return data_.begin();
    }

    inline const_iterator end() const
    {
        return data_.end();
    }

    virtual void accept(std::ostream& stream) override
    {
      stream << '[';
      const_iterator iterator = begin();
      while(iterator != end())
      {
        stream << *(*iterator);
        iterator += 1;
        if(iterator != end())
        {
          stream << ',';
        }
      }

      stream << ']';
    }
};

class table : public value<std::unordered_map<std::string, std::shared_ptr<base>>, base::data_type::table>
{
public:
    using base_type = value<std::unordered_map<std::string, std::shared_ptr<base>>, base::data_type::table>;
    using container_type = typename base_type::value_type;
        
    using iterator = typename container_type::iterator;
    using const_iterator = typename container_type::const_iterator;

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

    template<class value_data>
    inline bool add(const std::string& key, const value_data& value)
    {
        auto value_ptr = std::make_shared<value_data>(value);
        auto result = data_.emplace(key, std::static_pointer_cast<base>(value_ptr));
        return result.second;
    }

    template<class value_data>
    inline bool add(const std::string& key, std::shared_ptr<value_data> value)
    {
        auto result = data_.emplace(key, std::static_pointer_cast<base>(value));
        return result.second;
    }

    inline bool add(const std::string& key, const int& value)
    {
        return add(key, int_value(value));
    }

    inline bool add(const std::string& key, const float& value)
    {
        return add(key, float_value(value));
    }

    inline bool add(const std::string& key, const std::string& value)
    {
        return add(key, string_value(value));
    }

    inline bool add(const std::string& key, const bool& value)
    {
        return add(key, bool_value(value));
    }

    inline bool add(const std::string& key, const date_time& value)
    {
        return add(key, date_time_value(value));
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
        return data_.at(key);
    }

    inline std::shared_ptr<base> operator[](const std::string& key) const
    {
        return data_.at(key);
    }

    inline iterator begin()
    {
        return data_.begin();
    }

    inline iterator end()
    {
        return data_.end();
    }

    inline const_iterator begin() const
    {
        return data_.begin();
    }

    inline const_iterator end() const
    {
        return data_.end();
    }

    virtual inline void accept(std::ostream& stream) override
    {
      std::ostringstream child_stream;
      for(auto& i : data_)
      {
        if((*i.second).is<table>())
        {
          std::dynamic_pointer_cast<table>(i.second)->accept(child_stream, i.first);
          stream << child_stream.str();
        }
        else
        {
          stream << i.first << " = " << (*i.second) << std::endl;
        }
      }
    }

    inline void accept(std::ostream& stream, const std::string& table_name)
    {
      std::ostringstream child_stream;

      bool has_value = false;
      for(auto& i : data_)
      {
        if((*i.second).is<table>())
        {
          std::ostringstream child_table_name_stream;
          child_table_name_stream << table_name << "." << i.first;
          std::string child_table_name = child_table_name_stream.str();

          std::ostringstream child_element_stream;
          std::dynamic_pointer_cast<table>(i.second)->accept(child_element_stream, child_table_name);
          child_stream << child_element_stream.str();
        }
        else
        {
          if(!has_value)
          {
            stream << "[" << table_name << "]" << std::endl;
            has_value = true;
          }

          stream << i.first << " = " << (*i.second) << std::endl;
        }
      }

      stream << child_stream.str();
    }
};

class parse_exception : public std::runtime_error
{
public:
    parse_exception(const std::string& message) : std::runtime_error(message)
    {}

    parse_exception(const std::string& message, unsigned int line) : std::runtime_error(message + " at line " + std::to_string(line))
    {}
};

class parse
{
private:
    enum class parse_type
    {
        error,
        number,
        string,
        boolean,
        date,
        array,
        table 
    };

public:
    inline static table parse_str(const std::string& str)
    {
        std::istringstream stream = std::istringstream(str);
        return parse_impl(stream);
    }

private:
    inline static table parse_impl(std::istream& stream)
    {
        table root;
        std::string line;
        while(std::getline(stream, line))
        {
            std::cout << __FILE__ << "::" << __LINE__ << "  " << line << std::endl;
            parse_key_valeu_pair(root, line.begin(), line.end());
        }

        return root;
    }

    inline static void parse_key_valeu_pair(table& table, const std::string::iterator& line_start, const std::string::iterator& line_end)
    {
        auto key_start = consume_whitespace_toward_front(line_start, line_end);
        if(key_start == line_end || *key_start == '#')
        {
            return;
        }

        std::string::iterator value_start;
        std::string key = parse_key(value_start, key_start, line_end);

        std::cout << __FILE__ << "::" << __LINE__ << "  key=" << key << std::endl;

        value_start = consume_whitespace_toward_front(value_start, line_end);
        auto value = parse_value(value_start, line_end);

        std::cout << key << std::endl;
        table.add(key, value);;
    }

    inline static std::string parse_key(std::string::iterator& value_start, const std::string::iterator& start, const std::string::iterator& line_end)
    {
        auto equal = std::find_if(start, line_end, [](char c){ return c == '='; });
        auto key_end = equal - 1;
        key_end = consume_whitespace_toward_back(key_end, start);

        if(start == key_end)
        {
            std::cout << "=" << std::endl;
            std::cout << "TODO: thow exception" << std::endl;
        }

        std::string key(start, key_end);

        value_start = equal + 1;
        return key;
    }

    inline static std::shared_ptr<base> parse_value(const std::string::iterator& start, const std::string::iterator& line_end)
    {
        parse_type type = value_type(start);

        switch(type)
        {
        case parse_type::number:
            return parse_number(start, line_end);
            break;
        case parse_type::string:
            break;
        case parse_type::boolean:
            break;
        case parse_type::date:
            break;
        case parse_type::array:
            break;
        case parse_type::table:
            break;
        default:
            break;
        };

        return std::static_pointer_cast<base>(std::make_shared<int_value>(10000000000));
    }

    inline static parse_type value_type(const std::string::iterator& start)
    {
        if(is_number(*start) || *start == '+' || *start == '-')
        {
            return parse_type::number;
        }

        // TODO: thow exception
        return parse_type::error;
    }

    inline static std::shared_ptr<base> parse_number(const std::string::iterator& start, const std::string::iterator& line_end)
    {
        bool integer = true;
        std::string::iterator number_end = start;
        if(*start == '+' || *start == '-')
        {
            ++number_end;
        }

        bool first = true;
        while(number_end != line_end && *number_end != ' ' && *number_end != '\t' && *number_end != '#')
        {
            if(!is_number(*number_end))
            {
                if(!first && (*number_end == '.' || *number_end == '_'))
                {
                    ++number_end;
                    if(is_number(*number_end))
                    {
                        first = false;
                        ++number_end;
                        continue;
                    }
                    else
                    {
                        std::string error = "illegal format exception:" + std::string(start, line_end);
                        throw parse_exception(error);
                    }
                }
                else
                {
                    std::string error = "illegal format exception:" + std::string(start, line_end);
                    throw parse_exception(error);
                }
                break;
            } 
            first = false;
            ++number_end;
        }

        std::string number(start, number_end);
        number.erase(std::remove(number.begin(), number.end(), '_'), number.end());
        if(integer)
        {
            auto value = stoi(number);
            std::cout << __FILE__ << "::" << __LINE__ << "  " << value << std::endl;
            return std::static_pointer_cast<base>(std::make_shared<int_value>(value));
        }

        std::cout << __FILE__ << "::" << __LINE__ << "  " << std::string(start, number_end) << std::endl;
        return std::static_pointer_cast<base>(std::make_shared<int_value>(100));
    }

    inline static std::string::iterator consume_whitespace_toward_front(const std::string::iterator& start, const std::string::iterator& end)
    {
        std::string::iterator result = start;
        while (result != end && (*result == ' ' || *result == '\t'))
        {
            ++result;
        }

        return result;
    }

    inline static std::string::iterator consume_whitespace_toward_back(const std::string::iterator& end, const std::string::iterator& start)
    {
        std::string::iterator result = end;
        while (result != start && (*result == ' ' || *result == '\t'))
        {
            --result;
        }
        ++result;

        return result;
    }

    inline static bool is_number(char c)
    {
        return ('0' <= c && c <= '9');
    }
};


} // namespace toml

#endif 
