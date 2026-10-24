#include "languages.hpp"
#include <Qsci/qscilexerpython.h>
#include <Qsci/qscilexerruby.h>
#include <Qsci/qscilexerjavascript.h>
#include <Qsci/qscilexerjson.h>
#include <Qsci/qscilexermarkdown.h>
#include <Qsci/qscilexerhtml.h>
#include <Qsci/qscilexercss.h>
#include <Qsci/qscilexersql.h>
#include <Qsci/qscilexerproperties.h>
#include <Qsci/qscilexerdiff.h>
#include <Qsci/qscilexermakefile.h>
#include <Qsci/qscilexerbatch.h>
#include <Qsci/qscilexerbash.h>
#include <QColor>
#include <QFont>

// ========== KeywordLexer ==========

KeywordLexer::KeywordLexer(const LanguageDef& def, QObject* parent)
: QsciLexerCPP(parent, false)
{
    m_kw = def.keywords.join(' ').toUtf8();
    m_kw.append('\0');
    m_types = def.types.join(' ').toUtf8();
    m_types.append('\0');
}

const char* KeywordLexer::keywords(int set) const {
    if (set == 1) return m_kw.constData();
    if (set == 2) return m_types.constData();
    return QsciLexerCPP::keywords(set);
}

// ========== Definicje języków ==========

static const LanguageDef LANG_CPP = {
    {
        "alignas","alignof","and","and_eq","asm","auto","bitand","bitor","bool",
        "break","case","catch","char","char16_t","char32_t","class","compl",
        "concept","const","consteval","constexpr","constinit","const_cast",
        "continue","co_await","co_return","co_yield","decltype","default","delete",
        "do","double","dynamic_cast","else","enum","explicit","export","extern",
        "false","float","for","friend","goto","if","inline","int","long","mutable",
        "namespace","new","noexcept","not","not_eq","nullptr","operator","or",
        "or_eq","private","protected","public","register","reinterpret_cast",
        "requires","return","short","signed","sizeof","static","static_assert",
        "static_cast","struct","switch","template","this","thread_local","throw",
        "true","try","typedef","typeid","typename","union","unsigned","using",
        "virtual","void","volatile","wchar_t","while","xor","xor_eq"
    },
    {
        "size_t","ptrdiff_t","int8_t","int16_t","int32_t","int64_t",
        "uint8_t","uint16_t","uint32_t","uint64_t","intptr_t","uintptr_t"
    }
};

static const LanguageDef LANG_RUST = {
    {
        "as","async","await","break","const","continue","crate","dyn","else",
        "enum","extern","false","fn","for","if","impl","in","let","loop","match",
        "mod","move","mut","pub","ref","return","self","Self","static","struct",
        "super","trait","true","type","unsafe","use","where","while","yield",
        "macro_rules"
    },
    {
        "bool","char","str","u8","u16","u32","u64","u128","usize",
        "i8","i16","i32","i64","i128","isize","f32","f64",
        "Option","Some","None","Result","Ok","Err",
        "Vec","String","Box","Rc","Arc","RefCell","Mutex","RwLock",
        "HashMap","HashSet","BTreeMap","BTreeSet","VecDeque",
        "fmt","println","print","eprintln","eprint","format","vec","panic",
        "assert","assert_eq","assert_ne","debug_assert","todo","unimplemented",
        "unreachable","matches","write","writeln"
    }
};

static const LanguageDef LANG_GO = {
    {
        "break","case","chan","const","continue","default","defer","else",
        "fallthrough","for","func","go","goto","if","import","interface",
        "map","package","range","return","select","struct","switch","type","var"
    },
    {
        "bool","byte","complex64","complex128","error","float32","float64",
        "int","int8","int16","int32","int64","rune","string",
        "uint","uint8","uint16","uint32","uint64","uintptr",
        "true","false","iota","nil","make","new","len","cap","append","copy","delete",
        "print","println","panic","recover","close"
    }
};

static const LanguageDef LANG_ZIG = {
    {
        "addrspace","align","allowzero","and","anyframe","anytype","asm","async",
        "await","break","callconv","catch","comptime","const","continue","defer",
        "else","enum","errdefer","error","export","extern","fn","for","if",
        "inline","noalias","noinline","nosuspend","opaque","or","orelse",
        "packed","pub","resume","return","linksection","struct","suspend","switch",
        "test","threadlocal","try","union","unreachable","usingnamespace","var",
        "volatile","while"
    },
    {
        "bool","void","noreturn","type","anyerror","comptime_int","comptime_float",
        "c_int","c_uint","c_long","c_ulong","c_longlong","c_ulonglong",
        "c_short","c_ushort","c_char","c_uchar","c_float","c_double",
        "i8","i16","i32","i64","i128","isize",
        "u8","u16","u32","u64","u128","usize",
        "f16","f32","f64","f128",
        "true","false","null","undefined"
    }
};

static const LanguageDef LANG_D = {
    {
        "abstract","alias","align","asm","assert","auto","body","break","case",
        "cast","catch","class","const","continue","debug","default","delegate",
        "delete","deprecated","do","else","enum","export","extern","false",
        "final","finally","for","foreach","foreach_reverse","function","goto",
        "if","immutable","import","in","inout","interface","invariant","is","lazy",
        "macro","mixin","module","new","nothrow","null","out","override","package",
        "pragma","private","protected","public","pure","ref","return","scope",
        "shared","static","struct","super","switch","synchronized","template",
        "this","throw","true","try","typeid","typeof","union","unittest","version",
        "void","volatile","while","with","__FILE__","__LINE__","__gshared"
    },
    {
        "bool","byte","ubyte","short","ushort","int","uint","long","ulong",
        "cent","ucent","float","double","real","char","wchar","dchar",
        "string","wstring","dstring","size_t","ptrdiff_t"
    }
};

static const LanguageDef LANG_ODIN = {
    {
        "asm","defer","distinct","do","else","enum","for","foreign","if",
        "import","in","map","not_in","or_else","or_return","package","proc",
        "return","struct","switch","typeid","when","where","union","using","case"
    },
    {
        "bool","b8","b16","b32","b64","int","i8","i16","i32","i64","i128",
        "uint","u8","u16","u32","u64","u128","uintptr","f16","f32","f64",
        "complex32","complex64","complex128","string","cstring","rawptr",
        "true","false","nil","context","any"
    }
};

static const LanguageDef LANG_NIM = {
    {
        "addr","and","as","asm","bind","block","break","case","cast","concept",
        "const","continue","converter","defer","discard","distinct","div","do",
        "elif","else","end","enum","except","export","finally","for","from",
        "func","if","import","in","include","interface","is","isnot","iterator",
        "let","macro","method","mixin","mod","nil","not","notin","object","of",
        "or","out","proc","ptr","raise","ref","return","shl","shr","static","template",
        "try","tuple","type","using","var","when","while","with","without","xor","yield"
    },
    {
        "int","int8","int16","int32","int64","uint","uint8","uint16","uint32","uint64",
        "float","float32","float64","bool","char","string","byte","cint","cstring",
        "true","false","echo","new","newSeq","len","add","high","low"
    }
};

static const LanguageDef LANG_JAVA = {
    {
        "abstract","assert","break","case","catch","class","const","continue",
        "default","do","else","enum","extends","final","finally","for","goto",
        "if","implements","import","instanceof","interface","native","new",
        "package","private","protected","public","return","static","strictfp",
        "super","switch","synchronized","this","throw","throws","transient",
        "try","volatile","while","var","record","yield","sealed","permits"
    },
    {
        "boolean","byte","char","double","float","int","long","short","void",
        "true","false","null"
    }
};

static const LanguageDef LANG_CSHARP = {
    {
        "abstract","as","base","break","case","catch","checked","class","const",
        "continue","default","delegate","do","else","enum","event","explicit",
        "extern","finally","fixed","for","foreach","goto","if","implicit","in",
        "interface","internal","is","lock","namespace","new","operator","out",
        "override","params","private","protected","public","readonly","ref",
        "return","sealed","sizeof","stackalloc","static","struct","switch","this",
        "throw","try","typeof","unchecked","unsafe","using","virtual","volatile",
        "while","async","await","yield","var","dynamic","record","init","global",
        "partial","where","when","nameof"
    },
    {
        "bool","byte","sbyte","char","decimal","double","float","int","uint",
        "long","ulong","short","ushort","object","string","void","nint","nuint",
        "true","false","null"
    }
};

static const LanguageDef LANG_KOTLIN = {
    {
        "as","break","class","continue","do","else","false","for","fun","if",
        "in","interface","is","null","object","package","return","super","this",
        "throw","true","try","typealias","typeof","val","var","when","while",
        "by","catch","constructor","delegate","dynamic","field","file","finally",
        "get","import","init","param","property","receiver","set","setparam",
        "where","actual","abstract","annotation","companion","const","crossinline",
        "data","enum","expect","external","final","infix","inline","inner",
        "internal","lateinit","noinline","open","operator","out","override",
        "private","protected","public","reified","sealed","suspend","tailrec",
        "vararg"
    },
    {
        "Int","Long","Short","Byte","Float","Double","Boolean","Char","String",
        "Any","Nothing","Unit","Array","List","Map","Set"
    }
};

static const LanguageDef LANG_TS = {
    {
        "break","case","catch","class","const","continue","debugger","default",
        "delete","do","else","export","extends","finally","for","function","if",
        "import","in","instanceof","let","new","return","super","switch","this",
        "throw","try","typeof","var","void","while","with","yield","async","await",
        "enum","implements","interface","package","private","protected","public",
        "static","abstract","as","from","of","readonly","declare","type","namespace",
        "module","keyof","infer","is","asserts","satisfies"
    },
    {
        "any","boolean","number","string","symbol","object","never","unknown",
        "undefined","null","true","false","void"
    }
};

static const LanguageDef LANG_CMAKE = {
    {
        "if","else","elseif","endif","foreach","endforeach","while","endwhile",
        "function","endfunction","macro","endmacro","return","break","continue",
        "set","unset","list","string","math","message","option","cmake_minimum_required",
        "cmake_policy","project","add_executable","add_library","add_subdirectory",
        "add_custom_command","add_custom_target","add_definitions","add_dependencies",
        "add_compile_options","add_compile_definitions","add_link_options",
        "target_link_libraries","target_include_directories","target_compile_features",
        "target_compile_options","target_compile_definitions","target_sources",
        "target_precompile_headers","include_directories","link_directories",
        "find_package","find_library","find_path","find_program","find_file",
        "install","export","file","configure_file","include","get_filename_component",
        "get_property","set_property","set_target_properties","get_target_property",
        "source_group","enable_testing","add_test","enable_language","try_compile",
        "try_run","check_include_file","check_function_exists","execute_process"
    },
    {
        "TRUE","FALSE","ON","OFF","YES","NO","Y","N","IGNORE","NOTFOUND",
        "REQUIRED","QUIET","STATIC","SHARED","MODULE","INTERFACE","PUBLIC","PRIVATE",
        "CACHE","FORCE","PARENT_SCOPE","CXX_STANDARD","CXX_STANDARD_REQUIRED","CXX_EXTENSIONS"
    }
};

// ========== Dispatcher ==========

QsciLexer* makeLexerForExtension(const QString& ext, QObject* parent) {
    // --- C-like (KeywordLexer = QsciLexerCPP z nadpisanymi słowami) ---
    if (ext == "c" || ext == "h" ||
        ext == "cpp" || ext == "cc" || ext == "cxx" || ext == "hpp" ||
        ext == "hxx" || ext == "inl" || ext == "ipp" || ext == "tpp")
        return new KeywordLexer(LANG_CPP, parent);
    if (ext == "rs")
        return new KeywordLexer(LANG_RUST, parent);
    if (ext == "go")
        return new KeywordLexer(LANG_GO, parent);
    if (ext == "zig")
        return new KeywordLexer(LANG_ZIG, parent);
    if (ext == "d")
        return new KeywordLexer(LANG_D, parent);
    if (ext == "odin")
        return new KeywordLexer(LANG_ODIN, parent);
    if (ext == "nim")
        return new KeywordLexer(LANG_NIM, parent);
    if (ext == "java")
        return new KeywordLexer(LANG_JAVA, parent);
    if (ext == "cs")
        return new KeywordLexer(LANG_CSHARP, parent);
    if (ext == "kt" || ext == "kts")
        return new KeywordLexer(LANG_KOTLIN, parent);
    if (ext == "ts" || ext == "tsx")
        return new KeywordLexer(LANG_TS, parent);
    if (ext == "cmake")
        return new KeywordLexer(LANG_CMAKE, parent);
    if (ext == "glsl" || ext == "vert" || ext == "frag" || ext == "geom" ||
        ext == "tesc" || ext == "tese" || ext == "comp")
        return new KeywordLexer(LANG_CPP, parent);

    // Fallback dla języków bez dedykowanego lexera — przynajmniej stringi/liczby
    if (ext == "dockerfile" || ext == "just" || ext == "meson" || ext == "nix" ||
        ext == "asm" || ext == "s" || ext == "nasm" ||
        ext == "ada" || ext == "adb" || ext == "ads" ||
        ext == "ld" || ext == "lds")
        return new KeywordLexer(LanguageDef{}, parent);

    // --- Dedykowane lexery Qsci ---
    if (ext == "py" || ext == "pyw")
        return new QsciLexerPython(parent);
    if (ext == "rb" || ext == "rake" || ext == "gemspec")
        return new QsciLexerRuby(parent);
    if (ext == "js" || ext == "mjs" || ext == "cjs" || ext == "jsx")
        return new QsciLexerJavaScript(parent);
    if (ext == "json")
        return new QsciLexerJSON(parent);
    if (ext == "md" || ext == "markdown")
        return new QsciLexerMarkdown(parent);
    if (ext == "html" || ext == "htm" || ext == "xhtml")
        return new QsciLexerHTML(parent);
    if (ext == "xml" || ext == "svg" || ext == "axaml" || ext == "xaml" ||
        ext == "csproj" || ext == "props" || ext == "targets" || ext == "resx" ||
        ext == "xsd" || ext == "xsl" || ext == "xslt" || ext == "ui" || ext == "glade" ||
        ext == "handlebars" || ext == "hbs" || ext == "ftl" || ext == "msbuild")
        return new QsciLexerHTML(parent);
    if (ext == "css" || ext == "scss" || ext == "sass" || ext == "less")
        return new QsciLexerCSS(parent);
    if (ext == "sql")
        return new QsciLexerSQL(parent);
    if (ext == "yaml" || ext == "yml" || ext == "toml" || ext == "ini" ||
        ext == "properties" || ext == "conf" || ext == "cfg" ||
        ext == "gitignore" || ext == "gitattributes" || ext == "editorconfig")
        return new QsciLexerProperties(parent);
    if (ext == "diff" || ext == "patch")
        return new QsciLexerDiff(parent);
    if (ext == "makefile" || ext == "mk" || ext == "make")
        return new QsciLexerMakefile(parent);
    if (ext == "bat" || ext == "cmd")
        return new QsciLexerBatch(parent);
    if (ext == "sh" || ext == "bash" || ext == "zsh" || ext == "fish" ||
        ext == "ksh" || ext == "bashrc" || ext == "zshrc")
        return new QsciLexerBash(parent);

    // Plain text
    return nullptr;
}

// ========== OLED theme ==========

void applyOLEDThemeToLexer(QsciLexer* lexer) {
    if (!lexer) return;

    const QColor bg(0x00, 0x00, 0x00);
    lexer->setDefaultPaper(bg);
    lexer->setDefaultColor(QColor(0xcd, 0xd6, 0xf4));

    // Czarne tło dla wszystkich stylów — naprawia "białe placki"
    for (int i = 0; i < 128; ++i)
        lexer->setPaper(bg, i);

    if (auto* cpp = dynamic_cast<QsciLexerCPP*>(lexer)) {
        cpp->setColor(QColor(0x6c, 0x70, 0x86), QsciLexerCPP::Comment);
        cpp->setColor(QColor(0x6c, 0x70, 0x86), QsciLexerCPP::CommentLine);
        cpp->setColor(QColor(0x6c, 0x70, 0x86), QsciLexerCPP::CommentDoc);
        cpp->setColor(QColor(0x6c, 0x70, 0x86), QsciLexerCPP::CommentLineDoc);
        cpp->setColor(QColor(0x6c, 0x70, 0x86), QsciLexerCPP::CommentDocKeyword);
        cpp->setColor(QColor(0x6c, 0x70, 0x86), QsciLexerCPP::CommentDocKeywordError);

        cpp->setColor(QColor(0xfa, 0xb3, 0x87), QsciLexerCPP::Number);
        cpp->setColor(QColor(0x89, 0xb4, 0xfa), QsciLexerCPP::Keyword);
        cpp->setColor(QColor(0xf9, 0xe2, 0xaf), QsciLexerCPP::KeywordSet2);
        cpp->setColor(QColor(0xa6, 0xe3, 0xa1), QsciLexerCPP::DoubleQuotedString);
        cpp->setColor(QColor(0xa6, 0xe3, 0xa1), QsciLexerCPP::SingleQuotedString);
        cpp->setColor(QColor(0xf5, 0xc2, 0xe7), QsciLexerCPP::PreProcessor);
        cpp->setColor(QColor(0x94, 0xe2, 0xd5), QsciLexerCPP::Operator);
        cpp->setColor(QColor(0xcd, 0xd6, 0xf4), QsciLexerCPP::Identifier);
        cpp->setColor(QColor(0x89, 0xb4, 0xfa), QsciLexerCPP::GlobalClass);
    }

    lexer->setFont(QFont("JetBrains Mono", 11));
}
