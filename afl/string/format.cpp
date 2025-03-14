/**
  *  \file afl/string/format.cpp
  *  \brief Class afl::string::Format
  *
  *  The main advantage of a format string driven system over ad-hoc output
  *  and string concatenation is that format strings can be put
  *  in locale databases and be translated as whole, what can not be
  *  done as easily with `cout' snippets:
  *  <pre>
  *    cout << "Price: EUR " << n << " plus " << tax << "% VAT"
  *  </pre>
  *  require the three strings to be translated separately, which is
  *  more work and doesn't really work with a gettext-like approach.
  *
  *  With afl::string::Format, this gets
  *  <pre>
  *     cout << (Format("Price: EUR %d plus %d %% VAT") << n << tax)
  *  </pre>
  *  which is easier to translate.
  *
  *  The disadvantage of \c sprintf is that it is not typesafe and not extensible.
  *  How do we solve that? Instead of relying on the format string for types,
  *  we take type information from the compiler, and only use the format string as a guide.
  *  For example, using "%d" to output a number gets you the number in decimal, "%x" gets you lower-case hex.
  *  Unlike \c sprintf, however, "%s" does not try to output the thing as a string; in this case it'll get you decimal.
  *
  *  Extensibility is achieved by implementing the actual formatting in a FormatTraits template,
  *  which receives the parsed format placeholder in a FormatState object.
  *
  *  We support up to 10 (MAX_ARGS) arguments.
  *  An alternative to a static limit would have been making a linked list, which is less efficient.
  */

#include <ostream>
#include "afl/string/format.hpp"
#include "afl/string/formatstate.hpp"


/* Conver to string. This is the function where the actual formatting happens. */
afl::string::Format::operator String_t() const
{
    String_t result;

    const char* p = m_formatString;
    size_t argIndex = 0;

    uint32_t conditionState = 0;
    uint32_t conditionLevel = 0;

    FormatState state;

    while (const char* q = std::strchr(p, '%')) {
        // Append text leading up to here
        if (conditionState == 0) {
            result.append(p, static_cast<size_t>(q - p));
        }

        // Initial state
        // Format sequence: `%<index$><flags><width><.prec><code>'
        state.clear();
        bool show = true;
        p = q;
     again:                      // restart parsing after `$'
        while (1) {
            ++p;
            if (*p == '#') {
                state.setFlag(FormatState::AlternateFlag);
            } else if (*p == '0') {
                state.setFlag(FormatState::ZeroPadFlag);
            } else if (*p == '-') {
                state.setFlag(FormatState::LeftJustFlag);
            } else if (*p == ' ') {
                state.setFlag(FormatState::BlankFlag);
            } else if (*p == '+') {
                state.setFlag(FormatState::SignFlag);
            } else if (*p == '\'') {
                state.setFlag(FormatState::GroupFlag);
            } else if (*p == '!') {
                show = false;
            } else {
                break;
            }
        }

        // Parse width
        size_t width = 0;
        while (*p >= '0' && *p <= '9') {
            width = 10U*width + static_cast<size_t>(*p++ - '0');
        }

        if (*p == '$') {
            // %index$.... == insert named parameter, indexing starts at 0
            argIndex = width;
            state.setWidth(0);
            state.clearFlags();
            goto again;
        }

        state.setWidth(width);

        // Parse precision
        if (*p == '.') {
            size_t prec = 0;
            ++p;
            while (*p >= '0' && *p <= '9') {
                prec = 10U*prec + static_cast<size_t>(*p++ - '0');
            }
            state.setPrecision(prec);
            state.setFlag(FormatState::PrecisionFlag);
        }

        // Convert argument
        if (*p == 0) {
            break;                    // invalid format string
        }

        if (*p == '{') {
            // Condition test
            //   "%d %1{item%|items%}"
            ++conditionLevel;
            conditionState <<= 1;
            if (state.hasCondition(FormatState::Condition(width & 31))) {
                show = !show;
            }
            if (show) {
                // condition not valid
                conditionState |= 1;
            }
        } else if (*p == '|') {
            if (conditionLevel != 0) {
                conditionState ^= 1;
            }
        } else if (*p == '}') {
            if (conditionState != 0) {
                conditionState >>= 1;
                --conditionLevel;
            }
        } else if (conditionState == 0) {
            String_t here;
            if (*p == '%') {
                here = *p;
            } else {
                if (argIndex >= m_numArgs /*|| argIndex < 0*/) {
                    here = "<invalid>";
                } else {
                    state.setCode(*p);
                    here = m_args[argIndex].m_pFunction(m_args[argIndex].m_data, state);
                    ++argIndex;
                }
            }

            /* postprocess it */
            if (show) {
                if (here.length() < width) {
                    if (state.hasFlag(FormatState::LeftJustFlag)) {
                        here.append(width - here.length(), ' ');
                    } else {
                        here.insert(String_t::size_type(0), width - here.length(), ' ');
                    }
                }
                result += here;
            }
        } else {
            // skip this one
        }
        ++p;
    }
    result.append(p);
    return result;
}

/* Output formatted */
std::ostream&
operator<<(std::ostream& os, const afl::string::Format& fmt)
{
    return os << String_t(fmt);
}
