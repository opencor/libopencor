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

#if defined(BUILDING_USING_MSVC)
#    pragma warning(push)
#    pragma warning(disable: 4003)
#    pragma warning(disable: 4100)
#    pragma warning(disable: 4127)
#    pragma warning(disable: 4146)
#    pragma warning(disable: 4244)
#    pragma warning(disable: 4245)
#    pragma warning(disable: 4267)
#    pragma warning(disable: 4291)
#    pragma warning(disable: 4324)
#    pragma warning(disable: 4389)
#    pragma warning(disable: 4456)
#    pragma warning(disable: 4458)
#    pragma warning(disable: 4459)
#    pragma warning(disable: 4624)
#    pragma warning(disable: 4996)
#    pragma warning(disable: 5054)
#elif defined(BUILDING_USING_GNU)
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wdeprecated-enum-enum-conversion"
#    pragma GCC diagnostic ignored "-Wredundant-move"
#    pragma GCC diagnostic ignored "-Wunused-parameter"
#else
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wbitfield-enum-conversion"
#    pragma clang diagnostic ignored "-Wc++98-compat-pedantic"
#    pragma clang diagnostic ignored "-Wcast-align"
#    pragma clang diagnostic ignored "-Wcomma"
#    pragma clang diagnostic ignored "-Wdeprecated"
#    pragma clang diagnostic ignored "-Wdocumentation"
#    pragma clang diagnostic ignored "-Wdocumentation-deprecated-sync"
#    pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#    pragma clang diagnostic ignored "-Wduplicate-enum"
#    pragma clang diagnostic ignored "-Wenum-enum-conversion"
#    pragma clang diagnostic ignored "-Wexit-time-destructors"
#    pragma clang diagnostic ignored "-Wextra-semi"
#    pragma clang diagnostic ignored "-Wextra-semi-stmt"
#    pragma clang diagnostic ignored "-Wimplicit-int-conversion"
#    pragma clang diagnostic ignored "-Winconsistent-missing-destructor-override"
#    pragma clang diagnostic ignored "-Wmissing-noreturn"
#    pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#    pragma clang diagnostic ignored "-Wold-style-cast"
#    pragma clang diagnostic ignored "-Wpadded"
#    pragma clang diagnostic ignored "-Wreserved-id-macro"
#    pragma clang diagnostic ignored "-Wreserved-identifier"
#    pragma clang diagnostic ignored "-Wshadow"
#    pragma clang diagnostic ignored "-Wshadow-field"
#    pragma clang diagnostic ignored "-Wshadow-field-in-constructor"
#    pragma clang diagnostic ignored "-Wshift-sign-overflow"
#    pragma clang diagnostic ignored "-Wshorten-64-to-32"
#    pragma clang diagnostic ignored "-Wsign-conversion"
#    pragma clang diagnostic ignored "-Wsigned-enum-bitfield"
#    pragma clang diagnostic ignored "-Wsuggest-destructor-override"
#    pragma clang diagnostic ignored "-Wswitch-default"
#    pragma clang diagnostic ignored "-Wswitch-enum"
#    pragma clang diagnostic ignored "-Wtautological-type-limit-compare"
#    pragma clang diagnostic ignored "-Wtautological-unsigned-zero-compare"
#    pragma clang diagnostic ignored "-Wundefined-func-template"
#    pragma clang diagnostic ignored "-Wunused-but-set-variable"
#    pragma clang diagnostic ignored "-Wunused-parameter"
#    pragma clang diagnostic ignored "-Wweak-vtables"
#endif
