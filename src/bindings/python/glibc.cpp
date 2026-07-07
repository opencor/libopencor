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
// On glibc 2.38+, strtoll() is defined as a macro that redirects to __isoc23_strtoll().

#    include <cstdlib>

#    undef strtoll

extern "C" long long __isoc23_strtoll(const char *pString, char **pEndPtr, int pBase)
{
    return strtoll(pString, pEndPtr, pBase);
}

#endif
