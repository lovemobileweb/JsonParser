
#ifndef JSON_READER_H
#define JSON_READER_H

#ifndef JSON_ADAPTER_H
#include "JsonAdapter.h"
#endif

namespace JSON
{

	//-----------------------------------------------------------------------------
	// for deserializing
	class Reader : public Adapter
	{
		// the scanner
		Scanner* m_pScanner;
		// token stack. we need 1 look-ahead
		std::stack<Token> m_tokens;
		//
		Token m_token;

		//-----------------------------------------------------------------------------
		TokenType Next()
		{
			if (m_tokens.empty() == false)
			{
				m_token = m_tokens.top();
				m_tokens.pop();
			}
			else
			{
				m_token = m_pScanner->Scan();
			}
			return m_token.type;
		}

		//-----------------------------------------------------------------------------
		// need lookahead for some cases, especially arrays which can be empty or
		// "key" : string|number|bool
		bool _Peek(TokenType type)
		{
			Token ret = m_pScanner->Scan();
			m_tokens.push(ret);
			return ret.type == type;
		}

		//-----------------------------------------------------------------------------
		inline void throw_if(bool condition,std::string msg)
		{
			if (condition == true)
			{
				Chordia::stringer sb;
				sb << "Error (" << m_pScanner->Line() << ':' << m_pScanner->Col() << ") " << msg;
				__PLATFORM_THROW(json_exception(sb.c_str()));
			}
		}

		//-----------------------------------------------------------------------------
		// do while (condition)
		bool loop_or_end(TokenType continue_type,TokenType end_type)
		{
			//
			TokenType next = Next();
			//
			// token type matches?
			if (next == end_type)
			{
				return false;
			}
			if (next == continue_type)
			{
				return true;
			}
			// no clean way to do this ....
			throw_if(true,"expecting ',' or ']'");
			// never reached
			return false;
		}

		//-----------------------------------------------------------------------------
		// core type checking primitive
		virtual void GetNext(TokenType type)	
		{
			// get the next token
			TokenType next = Next();
			// token type matches?
			throw_if(next != type,"GetNext: type mismatch");
		}

		//-----------------------------------------------------------------------------
		// primitive to read type-checked key/value pair as in "count" : 123
		void GetNext(const std::string& key,TokenType type,std::string& value,bool more)
		{
			// expecting a string token for "key"
			GetNext(T_STRING);
			// checked key name matches parsed value
			throw_if(key != m_token.text,"key does not match");
			// next token
			GetNext(T_COLON);
			// get the next token and match type
			GetNext(type);
			// conversions from text are performed one level further up the stack
			value = m_token.text;
			// this has to be one of ,]}
			if (more)
			{
				Next();
			}
		}

	public:

		//-----------------------------------------------------------------------------
		Reader(Scanner& scanner) : 
			m_pScanner(&scanner)
		{}

		//-----------------------------------------------------------------------------
		virtual bool storing() { return false; }

		//-----------------------------------------------------------------------------
		virtual bool more() 
		{ 
			return loop_or_end(T_COMMA,T_ARRAY_END); 
		}

		//-----------------------------------------------------------------------------
		virtual bool peek(TokenType type) 
		{ 
			return _Peek(type);
		}

		//-----------------------------------------------------------------------------
		// read and match a delimiter {}[],:
		virtual void serialize(TokenType delimiter) 
		{
			GetNext(delimiter);
		}

		//-----------------------------------------------------------------------------
		// primitive to read type-checked key/value pair
		virtual void serialize(const std::string& key,std::string& value,bool more)
		{
			GetNext(key,T_STRING,value,more);
		}


		//-----------------------------------------------------------------------------
		virtual void serialize(const std::string& key,std::wstring& value,bool more)
		{
			std::string ss;
			GetNext(key,T_STRING,ss,more);
			value = Chordia::n2w(ss);
		}
		
		virtual void serialize(const std::string& key,int& value,bool more)
		{
			std::string ss;
			GetNext(key,T_NUMBER,ss,more);
			value = Chordia::toInt(ss);
		}
		
		virtual void serialize(const std::string& key,unsigned int& value,bool more)
		{
			std::string ss;
			GetNext(key,T_NUMBER,ss,more);
			value = (unsigned int)Chordia::toInt(ss);
		}

		virtual void serialize(const std::string& key,long& value,bool more)
		{
			std::string ss;
			GetNext(key,T_NUMBER,ss,more);
			value = static_cast<long>(Chordia::toInt(ss));
		}

		virtual void serialize(const std::string& key,unsigned char& value,bool more)
		{
			std::string ss;
			GetNext(key,T_NUMBER,ss,more);
			value = static_cast<unsigned char>(Chordia::toInt(ss));
		}

		virtual void serialize(const std::string& key,double& value,bool more)
		{
			std::string ss;
			GetNext(key,T_NUMBER,ss,more);
			value = Chordia::toDouble(ss);
		}

		// 
		virtual void serialize(const std::string& key,bool& value,bool more)
		{
			std::string ss;
			GetNext(key,T_BOOLEAN,ss,more);
			value = (ss == "true");
		}

		// read a const value. does nothing with the parameter
		virtual void serialize(const std::string&)	
		{
			GetNext(T_STRING);
		}

		// read a quoted singleton value
		virtual void serialize(std::string& value)	
		{
			GetNext(T_STRING);
			// key matches parsed value
			value = m_token.text;
		}

		// strings will be quoted
		virtual void serialize(std::wstring& ret)	
		{
			//
			GetNext(T_STRING);
			// do conversion
			ret = Chordia::n2w(m_token.text);
		}

		virtual void serialize(int& value)	
		{
			GetNext(T_NUMBER);
			// key matches parsed value
			value = static_cast<char>(Chordia::toInt(m_token.text));
		}

		virtual void serialize(unsigned int& value)	
		{
			GetNext(T_NUMBER);
			// key matches parsed value
			value = static_cast<unsigned int>(Chordia::toInt(m_token.text));
		}

		virtual void serialize(long& value)	
		{
			GetNext(T_NUMBER);
			// key matches parsed value
			value = static_cast<long>(Chordia::toInt(m_token.text));
		}

		virtual void serialize(double& value)	
		{
			GetNext(T_NUMBER);
			// key matches parsed value
			value = Chordia::toDouble(m_token.text);
		}
		
		virtual void serialize(bool& value)	
		{
			GetNext(T_BOOLEAN);
			value = (m_token.text == "true");
		}
	};

	//-----------------------------------------------------------------------------
	// demonstrate the JSON reader
	template <typename serializable_type, typename source_type = StringSource>
	class consumer
	{
	public:
		static serializable_type convert(const std::string& json)
		{
			// create an instance
			serializable_type target;
			// set up the reader chain
			source_type ss(json.c_str(),json.size());
			// scanner
			JSON::Scanner scanner(ss);
			// and JSON reader
			JSON::Reader reader(scanner);
			// re-hydrate ...
			target.serialize(reader);
			// return the new instance
			return target;
		}
	};
}

#endif
