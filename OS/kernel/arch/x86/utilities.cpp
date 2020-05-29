#include <utilities.h>

/*! @brief Count number of characters in string
 * @param Str
 * @return string length
 */
size_t Strlen(const char *Str)
{
    size_t i = 0;

    while (Str[i] != '\0')
        ++i;

    return i;
}

