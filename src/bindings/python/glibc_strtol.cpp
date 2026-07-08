/*
Copyright libOpenCOR contributors.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <stddef.h>
#include <stdint.h>

#if defined(__linux__) && defined(__GNUC__)
// On glibc 2.38+, strtol(), strtoll(), strtoul(), and strtoull() are redirected to __isoc23_strtol(),
// __isoc23_strtoll(), __isoc23_strtoul(), and __isoc23_strtoull(), respectively, either via preprocessor macro (Intel)
// or via __asm__("__isoc23_*") on the declaration (ARM). Both approaches would cause any call to strtol() inside our
// __isoc23_strtol() wrapper to resolve back to __isoc23_strtol() (either via macro expansion on Intel or via the asm
// redirect on ARM), leading to infinite recursion. To bypass this, we forward-declare the underlying strtol functions
// ourselves without the compiler-level redirect, giving us a direct reference to glibc's implementation that is
// unaffected by any macro or asm redirect on the standard names.
//
// Important: we do NOT include <stdlib.h> because on Intel, glibc redirects using preprocessor macros:
//   #define strtol __isoc23_strtol
//   #define strtoll __isoc23_strtoll
//   #define strtoul __isoc23_strtoul
//   #define strtoull __isoc23_strtoull
// which would macro-expand both our forward declarations and the return strtol() calls inside our wrappers, making
// them recursive. On ARM, glibc instead uses __asm__("__isoc23_strtol") on the declarations in <stdlib.h>, which would
// conflict with our own forward declarations (different asm labels on the same symbol). Either way, omitting <stdlib.h>
// entirely avoids both problems while still allowing us to forward-declare the real symbols.

extern "C" {
long strtol(const char *pString, char **pEndPtr, int pBase);
long long strtoll(const char *pString, char **pEndPtr, int pBase);
unsigned long strtoul(const char *pString, char **pEndPtr, int pBase);
unsigned long long strtoull(const char *pString, char **pEndPtr, int pBase);

__attribute__((weak)) long __isoc23_strtol(const char *pString, char **pEndPtr, int pBase)
{
    return strtol(pString, pEndPtr, pBase);
}

__attribute__((weak)) long long __isoc23_strtoll(const char *pString, char **pEndPtr, int pBase)
{
    return strtoll(pString, pEndPtr, pBase);
}

__attribute__((weak)) unsigned long __isoc23_strtoul(const char *pString, char **pEndPtr, int pBase)
{
    return strtoul(pString, pEndPtr, pBase);
}

__attribute__((weak)) unsigned long long __isoc23_strtoull(const char *pString, char **pEndPtr, int pBase)
{
    return strtoull(pString, pEndPtr, pBase);
}
}

#endif
