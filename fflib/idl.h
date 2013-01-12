struct student_t
{
    struct book_t
    {
        int16_t  pages;
        string    contents;
    };
    int8_t              age;
    float               grade;
    string              name;
    vector<string>      friends;
    map<string, book_t> books;
};

template<typename T>
class msg_dispacher_t
{
public:
    msg_dispather_t(const T& msg_handler_):
        m_msg_handler(msg_handler_)
    {
    }
    int dispath(const string& json_);
    
private:
    int foo_t_dispacher(json& val_)
    {
        //! more check for dispatcher
        //! foo_t msg = xxx;
        m_msg_handler.handle(msg);
    }

private:
    const T&            m_msg_handler;
    map<string, int>    m_reg_func;
};