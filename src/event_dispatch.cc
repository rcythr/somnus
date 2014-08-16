#include <somnus/event_dispatch.hpp>

#include <fstream>
#include <map>

using namespace somnus;

typedef std::vector<std::tuple<std::string, uint64_t>> ServiceList;

enum ParseState
{
    IDLE,
    SECTION,
    SECTION_END,
    SERVICE,
    ID
};

bool is_whitespace(char c) { return c == ' ' || c == '\t' || c == '\r'; }
bool is_alpha(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
bool is_numeric(char c) { return c >= '0' && c <= '9'; }
bool is_alphanum(char c) { return is_alpha(c) || is_numeric(c); }

#define PARSE_ERROR(Err) std::runtime_error(std::string("Line ") + std::to_string(lineno) + ": " Err);

EventDispatch::EventDispatch(std::string config_filename)
{
    std::ifstream input(config_filename.c_str());

    uint64_t lineno = 1;
    int64_t current_rope = -1;
    std::map<int64_t, ServiceList> ropes;
    for(std::string line; std::getline(input, line); )
    {
        ParseState state = IDLE;

        std::string buf1 = "";
        std::string buf2 = "";

        // Lex
        for(char c : line)
        {
            // Handle whitespace
            if(c == ' ' || c == '\t' || c == '\r')
                continue;

            // Handle comment
            if( c == '#' && (state == IDLE || state == ID || state == SECTION_END))
                break;

            switch(state)
            {
                case IDLE:
                {
                    if(c == '[')
                    {
                        state = SECTION;
                    }
                    else if(is_alphanum(c))
                    {
                        state = SERVICE;
                        buf1 += c;
                    }
                    else
                    {
                        throw PARSE_ERROR("Invalid symbol \'" + c + "\' found.");
                    }
                } break;

                case SERVICE:
                {
                    if(is_alphanum(c))
                    {
                        buf1 += c;
                    }
                    else if(c == '=')
                    {
                        state = ID;
                    }
                    else
                    {
                        throw PARSE_ERROR("Invalid symbol \'" + c + "\' found while parsing service name.");
                    }
                } break;

                case SECTION:
                {
                    if(is_numeric(c))
                    {
                        buf1 += c;
                    }
                    else if(c == ']')
                    {
                        state = SECTION_END;
                    }
                    else
                    {
                        throw PARSE_ERROR("Invalid symbol \'" + c + "\' found while parsing rope id.");
                    }
                } break;

                case ID:
                {
                    if(is_numeric(c))
                    {
                        buf2 += c;
                    }
                    else
                    {
                        throw PARSE_ERROR("Invalid symbol \'" + c + "\' found while parsing service ID.");
                    }
                } break;

                case SECTION_END:
                {
                    throw PARSE_ERROR("Invalid symbol \'" + c + "\' after Rope id.");
                } break;
            }
        }

        // Parse
        switch(state)
        {
            case SECTION_END:
            {
                if(buf1 == "")
                {
                    throw PARSE_ERROR("Expected rope ID between [ and ] symbols.");
                }
                else
                {
                    int64_t id = std::stol(buf1);
                    ropes[id] = ServiceList();
                    current_rope = id;
                }
            } break;
            
            case ID:
            {
                if(buf1 == "")
                {
                    throw PARSE_ERROR("Expected service name.");
                }
                else if(buf2 == "")
                {
                    throw PARSE_ERROR("Expected Id.");
                }
                else
                {
                    if(current_rope == -1)
                    {
                        throw PARSE_ERROR("Expected Rope Id before service.");
                    }

                    ropes[current_rope].push_back(std::tuple<std::string, int64_t>(buf1, std::stol(buf2)));
                }
            } break;

            case IDLE: break;
            case SERVICE: throw PARSE_ERROR("Expected assignment to service.");
            case SECTION: throw PARSE_ERROR("Expected ']' after rope id.");
        }

        ++lineno;
    }

    // Create Ropes. Associate with ServiceIDs.
    auto itr = ropes.begin();
    while(itr != ropes.end())
    {
        auto rope = std::make_shared<somnus::Rope>();
        for(auto entry : itr->second)
        {
            serviceToRope[std::get<1>(entry)] = rope;
        }
        ++itr;
    }
}

