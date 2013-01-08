#include <mconfig/varlist.h>


using namespace M;

namespace MConfig {

void
Varlist::addEntry (ConstMemory const name,
                   ConstMemory const value,
                   bool        const with_value,
                   bool        const enable_section,
                   bool        const disable_section)
{
    if (with_value
        || (!enable_section && !disable_section))
    {
        Var * const var = new Var;
        assert (var);
        if (!with_value) {
            var->value_buf = NULL;
            var->value_len = 0;
        } else {
            if (value.len()) {
                var->value_buf = new Byte [value.len()];
                assert (var->value_buf);
                memcpy (var->value_buf, value.mem(), value.len());
            } else {
                var->value_buf = NULL;
            }
            var->value_len = value.len();
        }

        if (name.len()) {
            var->name_buf = new Byte [name.len()];
            assert (var->name_buf);
            memcpy (var->name_buf, name.mem(), name.len());
        } else {
            var->name_buf = NULL;
        }
        var->name_len = name.len();

        var_list.append (var);
    }

    if (enable_section || disable_section) {
        Section * const section = new Section;
        assert (section);

        if (name.len()) {
            section->name_buf = new Byte [name.len()];
            assert (section->name_buf);
            memcpy (section->name_buf, name.mem(), name.len());
        } else {
            section->name_buf = NULL;
        }
        section->name_len = name.len();

        if (disable_section)
            section->enabled = false;
        else
            section->enabled = true;

        section_list.append (section);
    }
}

Varlist::~Varlist ()
{
    {
        VarList::iterator iter (var_list);
        while (!iter.done()) {
            Var * const var = iter.next ();
            delete var;
        }
    }

    {
        SectionList::iterator iter (section_list);
        while (!iter.done()) {
            Section * const section = iter.next ();
            delete section;
        }
    }
}

}

