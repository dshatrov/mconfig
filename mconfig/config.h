/*  MConfig - C++ library for working with configuration files
    Copyright (C) 2011 Dmitry Shatrov

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/


#ifndef __MCONFIG__CONFIG__H__
#define __MCONFIG__CONFIG__H__


#include <libmary/libmary.h>


namespace MConfig {

using namespace M;

class SectionEntry : public HashEntry<>
{
    friend class Section;
    friend class Config;

public:
    enum Type
    {
	Type_Option,
	Type_Section,
	Type_Invalid
    };

private:
    Ref<String> name_str;

    Type const type;

public:
    Type getType () const
    {
	return type;
    }

    ConstMemory getName () const
    {
	return name_str->mem();
    }

    SectionEntry (Type const type,
		  ConstMemory const &entry_name)
	: type (type)
    {
	name_str = grab (new String (entry_name));
    }

    virtual ~SectionEntry () {}
};

class Value : public IntrusiveListElement<>
{
private:
    Ref<String> value_str;

    enum CachedType {
	CachedType_None,
	CachedType_Double,
	CachedType_Int64,
	CachedType_Uint64
    };

    CachedType cached_type;
    bool bad_value;

    union {
	double cached_double;
	Int64  cached_int64;
	Uint64 cached_uint64;
    } cached_value;

public:
    void setValue (ConstMemory const &mem)
    {
	value_str = grab (new String (mem));
    }

    Result getAsDouble (double *ret_val);

    Result getAsInt64 (Int64 *ret_val);

    Result getAsUint64 (Uint64 *ret_val);

    Ref<String> getAsString ()
    {
	return value_str;
    }

    ConstMemory mem ()
    {
	return value_str->mem();
    }

    Value ()
	: cached_type (CachedType_None),
	  bad_value (false)
    {
    }
};

class Option : public SectionEntry
{
private:
    typedef IntrusiveList<Value> ValueList;

    ValueList value_list;

public:
    void addValue (ConstMemory const &mem)
    {
	Value * const value = new Value;
	value->setValue (mem);
	value_list.append (value);
    }

    void removeValues ()
    {
	{
	    ValueList::iter iter (value_list);
	    while (!value_list.iter_done (iter)) {
		Value * const value = value_list.iter_next (iter);
		delete value;
	    }
	}

	value_list.clear ();
    }

    Value* getValue ()
    {
	return value_list.getFirst();
    }

    void dump (OutputStream *outs,
	       unsigned      nest_level);

    Option (ConstMemory const &option_name)
	: SectionEntry (SectionEntry::Type_Option, option_name)
    {
    }

    ~Option ()
    {
	removeValues ();
    }

  // Iterators.

    class iter
    {
	friend class Option;

    private:
	ValueList::iter iter_;

    public:
	iter ()
	{
	}

	iter (Option &option)
	    : iter_ (option.value_list)
	{
	}
    };

    void iter_begin (iter &iter)
    {
	value_list.iter_begin (iter.iter_);
    }

    Value* iter_next (iter &iter)
    {
	return value_list.iter_next (iter.iter_);
    }

    bool iter_done (iter &iter)
    {
	return value_list.iter_done (iter.iter_);
    }
};

class Section : public SectionEntry
{
private:
    typedef Hash< SectionEntry,
		  Memory,
		  MemberExtractor< SectionEntry,
				   Ref<String>,
				   &SectionEntry::name_str,
				   Memory,
				   AccessorExtractor< String,
						      Memory,
						      &String::mem > >,
		  MemoryComparator<> >
	    SectionEntryHash;

    SectionEntryHash section_entry_hash;

public:
    SectionEntry* getSectionEntry (ConstMemory const &path,
				   bool create = false,
				   SectionEntry::Type section_entry_type = SectionEntry::Type_Invalid);

    Option* getOption (ConstMemory const &path,
		       bool create = false);

    Section* getSection (ConstMemory const &path,
			 bool create = false);

    SectionEntry* getSectionEntry_nopath (ConstMemory const &section_entry_name);

    Option* getOption_nopath (ConstMemory const &option_name,
			      bool create = false);

    Section* getSection_nopath (ConstMemory const &section_name,
				bool create = false);

    // Takes ownership of @option.
    void addOption (Option * const option)
    {
	section_entry_hash.add (option);
    }

    // Takes ownership of @section.
    void addSection (Section * const section)
    {
	section_entry_hash.add (section);
    }

    void removeSectionEntry (SectionEntry * const section_entry)
    {
	section_entry_hash.remove (section_entry);
	delete section_entry;
    }

    void dump (OutputStream *outs,
	       unsigned      nest_level = 0);

    void dumpBody (OutputStream *outs,
		   unsigned      nest_level = 0);

    Section (ConstMemory const &section_name)
	: SectionEntry (SectionEntry::Type_Section, section_name)
    {
    }

    ~Section ();

  // Iterators.

    class iter
    {
	friend class Section;

    private:
	SectionEntryHash::iter iter_;

    public:
	iter ()
	{
	}

	iter (Section &section)
	    : iter_ (section.section_entry_hash)
	{
	}
    };

    void iter_begin (iter &iter)
    {
	section_entry_hash.iter_begin (iter.iter_);
    }

    SectionEntry* iter_next (iter &iter)
    {
	return section_entry_hash.iter_next (iter.iter_);
    }

    bool iter_done (iter &iter)
    {
	return section_entry_hash.iter_done (iter.iter_);
    }
};

class GetResult
{
public:
    enum Value {
	Invalid = 0,
	Default,
	Success
    };
    operator Value () const { return value; }
    GetResult (Value const value) : value (value) {}
    GetResult () {}
private:
    Value value;
};

class Config
{
private:
    Section root_section;

public:
    // Helper method to avoid excessive explicit calls to getRootSection().
    Option* getOption (ConstMemory const &path,
		       bool const create = false)
    {
	return root_section.getOption (path, create);
    }

    // Helper method to avoid excessive explicit calls to getRootSection().
    Section* getSection (ConstMemory const &path,
			 bool const create = false)
    {
	return root_section.getSection (path, create);
    }

    Option* setOption (ConstMemory const &path,
		       ConstMemory const &value);

    ConstMemory getString (ConstMemory const &path);

    ConstMemory getString_default (ConstMemory const &path,
				   ConstMemory const &default_value)
    {
	ConstMemory const str = getString (path);
	if (str.len() == 0) {
	    return default_value;
	}
	return str;
    }

    GetResult getUint64 (ConstMemory const &path,
			 Uint64 *ret_value);

    GetResult getUint64_default (ConstMemory const &path,
				 Uint64 * const ret_value,
				 Uint64   const default_value)
    {
	GetResult const res = getUint64 (path, ret_value);
	if (res == GetResult::Default) {
	    if (ret_value) {
		*ret_value = default_value;
	    }
	    return GetResult::Default;
	}
	return res;
    }

    enum BooleanValue {
	Boolean_Invalid,
	Boolean_Default,
	Boolean_True,
	Boolean_False
    };

    BooleanValue getBoolean (ConstMemory const &path);

    Section* getRootSection ()
    {
	return &root_section;
    }

    void dump (OutputStream *outs,
	       unsigned      nest_level = 0);

    Config ()
	: root_section ("root")
    {
    }
};

}


#endif /* __MCONFIG__CONFIG__H__ */

