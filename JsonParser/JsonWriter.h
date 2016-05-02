
#ifndef JSON_WRITER_H
#define JSON_WRITER_H

#ifndef JSON_ADAPTER_H
#include "JsonAdapter.h"
#endif

namespace JSON
{
	//-----------------------------------------------------------------------------
	// base class for facade over eventual destination, 
	// e.g. a socket instead of a string
	class ISink
	{
	public:
		virtual ISink& operator<<(const char*) = 0;
		virtual ISink& operator<<(const std::string&) = 0;
		virtual ISink& operator<<(const std::wstring&) = 0;
		virtual ISink& operator<<(const char&) = 0;
		virtual ISink& operator<<(const int&) = 0;
		virtual ISink& operator<<(const unsigned int&) = 0;
		virtual ISink& operator<<(const long&) = 0;
		virtual ISink& operator<<(const double&) = 0;
		virtual ISink& operator<<(const bool&) = 0;

	};

	//-----------------------------------------------------------------------------
	// Simply a facade over stringer, writes directly to a std::string.
	class StringSink : public ISink
	{
		//
		Chordia::stringer* _sink;

	public:
		//
		StringSink(Chordia::stringer* sink) : _sink(sink)	{}
		//
		virtual ISink& operator<<(const char* arg)			{ (*_sink) << arg; return (*this); }
		virtual ISink& operator<<(const std::string& arg)	{ (*_sink) << arg; return (*this); }
		virtual ISink& operator<<(const std::wstring& arg)	{ (*_sink) << arg; return (*this); }
		virtual ISink& operator<<(const char& arg)			{ (*_sink) << arg; return (*this); }
		virtual ISink& operator<<(const int& arg)			{ (*_sink) << arg; return (*this); }
		virtual ISink& operator<<(const long& arg)			{ (*_sink) << arg; return (*this); }
		virtual ISink& operator<<(const unsigned int& arg)	{ (*_sink) << arg; return (*this); }
		virtual ISink& operator<<(const double& arg)		{ (*_sink) << arg; return (*this); }
		virtual ISink& operator<<(const bool& arg)			{ (*_sink) << arg; return (*this); }
	};

	//-------------------------------------------------------------------------
	// for serializing (writing) JSON

	class Writer : public Adapter
	{
		//---------------------------------------------------------------------
		ISink* _sink;

		//
		static const char* Quote() 
		{
			return "\"";
		}

	public:

		//---------------------------------------------------------------------
		//
		Writer(ISink* sink) : _sink(sink) {}

		//---------------------------------------------------------------------
		//
		virtual bool storing() { return true; }

		//---------------------------------------------------------------------
		// write a key/value pair with optional continuation
		virtual void serialize(const std::string& key,std::string& value,bool more) 
		{
			(*_sink) << "\"" << key << Quote() << ':' << Quote() << Chordia::escape(value) << Quote() << (more ? "," : "");
		}


		virtual void serialize(const std::string& key,std::wstring& value,bool more) 
		{
			(*_sink) << Quote() << key << Quote() << ':' << Quote() << Chordia::escape(Chordia::w2n(value)) << Quote() << (more ? "," : "");
		}

		virtual void serialize(const std::string& key,int& value,bool more) 
		{
			(*_sink) << Quote() << key << Quote() << ':' << value << (more ? "," : "");
		}
		
		virtual void serialize(const std::string& key,unsigned int& value,bool more) 
		{
			(*_sink) << Quote() << key << Quote() << ':' << value << (more ? "," : "");
		}

		virtual void serialize(const std::string& key,long& value,bool more) 
		{
			(*_sink) << Quote() << key << Quote() << ':' << value << (more ? "," : "");
		}

		virtual void serialize(const std::string& key,unsigned char& value,bool more)
		{
			(*_sink) << Quote() << key << Quote() << ':' << value << (more ? "," : "");
		}

		virtual void serialize(const std::string& key,double& value,bool more) 
		{
			(*_sink) << Quote() << key << Quote() << ':' << value << (more ? "," : "");
		}

		virtual void serialize(const std::string& key,bool& value,bool more) 
		{
			// literal true of false
			(*_sink) << Quote() << key << Quote() << ':' << value << (more ? "," : "");
		}

		//---------------------------------------------------------------------
		// write a literal
		virtual void serialize(const std::string& value)
		{
			(*_sink) << Quote() << value << Quote() ;
		}

		//---------------------------------------------------------------------
		// write a delimiter
		virtual void serialize(TokenType type)
		{
            bool ok = true;
			const char* delimiter = 0;
			switch (type)
			{
			case T_OBJ_BEGIN:
				delimiter = "{";
				break;
			case T_OBJ_END:
				delimiter = "}";
				break;
			case T_ARRAY_BEGIN:
				delimiter = "[";
				break;
			case T_ARRAY_END:
				delimiter = "]";
				break;
			case T_COMMA:
				delimiter = ",";
				break;
			case T_COLON:
				delimiter = ":";
				break;
            default:
                    ok = false;
			}
            if (ok)
            {
                (*_sink) << delimiter;
            }
		}


		//---------------------------------------------------------------------
		// serialize a string value. needs to handle escape sequences
		virtual void serialize(std::string& value)
		{
			(*_sink) << Quote() << Chordia::escape(value) << Quote() ;
		}

		//---------------------------------------------------------------------
		// needs to convert to JSON \u encoding for
		// any characters outside of UTF8

		virtual void serialize(std::wstring& value)
		{
			(*_sink) << Quote() << Chordia::escape(Chordia::w2n(value)) << Quote() ;
		}

		//---------------------------------------------------------------------
		virtual void serialize(int& value)
		{
			(*_sink) << value;
		}

		//---------------------------------------------------------------------
		virtual void serialize(unsigned int& value)
		{
			(*_sink) << value;
		}

		//---------------------------------------------------------------------
		virtual void serialize(long& value)
		{
			(*_sink) << value;
		}

		//---------------------------------------------------------------------
		virtual void serialize(double& value)
		{
			(*_sink) << value;
		}

		//---------------------------------------------------------------------
		virtual void serialize(bool& value)
		{
			(*_sink) << value;
		}
	};

	//-----------------------------------------------------------------------------
	// template the writer. usage is:
	// std::string json = JSON::producer<JSONExample>::convert(source);
	template <typename serializable_type, typename sink_type = StringSink>
	class producer
	{
		public:
		static std::string convert(serializable_type& source)
		{
			Chordia::stringer json;
			// by default serialize to a string
			sink_type ssi(&json);
			// create a writer
			JSON::Writer writer(&ssi);
			// serialize the instance
			source.serialize(writer);
			// and get the JSON string from the sink
			//
#ifdef _INTEGRATED
#ifdef _UNICODE
			return Chordia::convert_w(json.c_str());
#else
			return json.c_str();
#endif
#else
			return json.c_str();
#endif
		}
	};
}


#endif


