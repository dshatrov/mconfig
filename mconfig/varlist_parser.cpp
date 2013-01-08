/*  MConfig - C++ library for working with configuration files
    Copyright (C) 2012 Dmitry Shatrov

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


#include <libmary/libmary.h>

//#include <mycpp/cached_file.h>

#include <pargen/file_token_stream.h>
#include <pargen/parser.h>

#include <mconfig/varlist_pargen.h>
#include <mconfig/varlist_parser.h>


using namespace M;

namespace MConfig {

bool
varlist_word_token_match_func (ConstMemory const &token_mem,
                               void * const /* token_user_ptr */,
                               void * const /* user_data */)
{
    char const * const token = (char const*) token_mem.mem();
    if (token [0] == '=')
        return false;

    return true;
}

bool
varlist_accept_var_decl (VarList_VarDecl        * const var_decl,
		         Pargen::ParserControl  * const /* parser_control */,
		         void                   * const _varlist)
{
    Varlist * const varlist = static_cast <Varlist*> (_varlist);

    ConstMemory name;
    ConstMemory value;
    bool with_value = false;

    bool enable_section  = false;
    bool disable_section = false;

    switch (var_decl->var_decl_type) {
        case VarList_VarDecl::t_NameValue: {
            VarList_VarDecl_NameValue * const var_decl__name_value =
                    static_cast <VarList_VarDecl_NameValue*> (var_decl);

            if (VarList_SectionSpecifier * const section_specifier =
                        var_decl__name_value->sectionSpecifier)
            {
                switch (section_specifier->section_specifier_type) {
                    case VarList_SectionSpecifier::t_Enable:
                        enable_section = true;
                        break;
                    case VarList_SectionSpecifier::t_Disable:
                        disable_section = true;
                        break;
                }
            }

            name  = var_decl__name_value->name->any_token->token;
            value = var_decl__name_value->value->any_token->token;
            with_value = true;
        } break;
        case VarList_VarDecl::t_Name: {
            VarList_VarDecl_Name * const var_decl__name =
                    static_cast <VarList_VarDecl_Name*> (var_decl);

            if (VarList_SectionSpecifier * const section_specifier =
                        var_decl__name->sectionSpecifier)
            {
                switch (section_specifier->section_specifier_type) {
                    case VarList_SectionSpecifier::t_Enable:
                        enable_section = true;
                        break;
                    case VarList_SectionSpecifier::t_Disable:
                        disable_section = true;
                        break;
                }
            }

            name  = var_decl__name->name->any_token->token;
        } break;
        default:
            unreachable ();
    }

    logD_ (_func,
           "name: ", name, ", "
           "value: ", value, ", "
           "with_value: ", with_value, ", "
           "enable_section: ", enable_section, ", "
           "disable_section: ", disable_section);

    varlist->addEntry (name, value, with_value, enable_section, disable_section);

    return true;
}

Result VarlistParser::parseVarlist (ConstMemory   const filename,
                                    Varlist     * const varlist)
{
 try {
    NativeFile file;
    if (!file.open (filename, 0 /* open_flags */, FileAccessMode::ReadOnly)) {
        logE_ (_func, "Could not open ", filename, ": ", exc->toString());
        return Result::Failure;
    }

//    file = grab (new MyCpp::CachedFile (file, (1 << 14) /* page_size */, 64 /* max_pages */));

    Pargen::FileTokenStream token_stream (&file);

    Pargen::ParserElement *varlist_elem = NULL;
    Pargen::parse (&token_stream,
                   NULL /* lookup_data */,
                   varlist /* user_data */,
                   grammar,
                   &varlist_elem,
                   "default",
                   parser_config,
                   false /* debug_dump */);
 } catch (...) {
     logE_ (_func, "parsing exception");
     return Result::Failure;
 }

   return Result::Success;
}

VarlistParser::VarlistParser ()
{
 try {
    grammar = create_varlist_grammar ();
    Pargen::optimizeGrammar (grammar);
    parser_config = Pargen::createParserConfig (true /* upwards_jumps */);
 } catch (...) {
    logE_ (_func, "exception");
 }
}

}

