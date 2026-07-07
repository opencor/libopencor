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

#if defined(__linux__) && defined(__GNUC__)
// On glibc 2.38+, strtol() and strtoll() are redirected to __isoc23_strtol() and __isoc23_strtoll(), respectively,
// either via preprocessor macro (Intel) or via __asm__("__isoc23_strtol") on the declaration (ARM). The latter is a
// compiler-level redirect that cannot be undone with #undef, so calling strtol() inside our __isoc23_strtol() wrapper
// would cause infinite recursion on ARM. To bypass this, we forward-declare strtol()/strtoll() ourselves without the
// compiler-level redirect, giving us a direct reference to glibc's implementation unaffected by any asm redirect on the
// standard names.

extern "C" {
long strtol(const char *pString, char **pEndPtr, int pBase);
long long strtoll(const char *pString, char **pEndPtr, int pBase);

__attribute__((weak)) long __isoc23_strtol(const char *pString, char **pEndPtr, int pBase)
{
    return strtol(pString, pEndPtr, pBase);
}

__attribute__((weak)) long long __isoc23_strtoll(const char *pString, char **pEndPtr, int pBase)
{
    return strtoll(pString, pEndPtr, pBase);
}
}

#endif
