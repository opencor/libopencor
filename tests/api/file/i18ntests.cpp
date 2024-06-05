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

#include "tests/utils.h"

#include <libopencor>

TEST(I18nFileTest, arabicFiles)
{
    EXPECT_EQ(libOpenCOR::File::create(libOpenCOR::resourcePath("i18n/ملف/file.cellml"))->type(), libOpenCOR::File::Type::CELLML_FILE);
    EXPECT_EQ(libOpenCOR::File::create(libOpenCOR::resourcePath("i18n/ملف/file.sedml"))->type(), libOpenCOR::File::Type::SEDML_FILE);
    EXPECT_EQ(libOpenCOR::File::create(libOpenCOR::resourcePath("i18n/ملف/file.omex"))->type(), libOpenCOR::File::Type::COMBINE_ARCHIVE);
}

TEST(I18nFileTest, chineseFiles)
{
    EXPECT_EQ(libOpenCOR::File::create(libOpenCOR::resourcePath("i18n/文件夹/file.cellml"))->type(), libOpenCOR::File::Type::CELLML_FILE);
    EXPECT_EQ(libOpenCOR::File::create(libOpenCOR::resourcePath("i18n/文件夹/file.sedml"))->type(), libOpenCOR::File::Type::SEDML_FILE);
    EXPECT_EQ(libOpenCOR::File::create(libOpenCOR::resourcePath("i18n/文件夹/file.omex"))->type(), libOpenCOR::File::Type::COMBINE_ARCHIVE);
}

TEST(I18nFileTest, hindiFiles)
{
    EXPECT_EQ(libOpenCOR::File::create(libOpenCOR::resourcePath("i18n/फोल्डर/file.cellml"))->type(), libOpenCOR::File::Type::CELLML_FILE);
    EXPECT_EQ(libOpenCOR::File::create(libOpenCOR::resourcePath("i18n/फोल्डर/file.sedml"))->type(), libOpenCOR::File::Type::SEDML_FILE);
    EXPECT_EQ(libOpenCOR::File::create(libOpenCOR::resourcePath("i18n/फोल्डर/file.omex"))->type(), libOpenCOR::File::Type::COMBINE_ARCHIVE);
}

#ifdef BUILDING_ON_MACOS
TEST(I18nFileTest, japaneseFiles)
{
    // Note: the フォルダ folder was created on macOS and although it can be accessed on Windows, Linux, and macOS using
    //       their respective file manager, it can only be accessed programmatically on macOS. On Windows, for instance,
    //       a dakuten (and, most likely, a handakuten) is handled in a special way. So, this test will always fail on
    //       Windows. Something similar happens on Linux. So, in the end, we keep the macOS version of the folder name
    //       and only run this test on macOS.

    EXPECT_EQ(libOpenCOR::File::create(libOpenCOR::resourcePath("i18n/フォルダ/file.cellml"))->type(), libOpenCOR::File::Type::CELLML_FILE);
    EXPECT_EQ(libOpenCOR::File::create(libOpenCOR::resourcePath("i18n/フォルダ/file.sedml"))->type(), libOpenCOR::File::Type::SEDML_FILE);
    EXPECT_EQ(libOpenCOR::File::create(libOpenCOR::resourcePath("i18n/フォルダ/file.omex"))->type(), libOpenCOR::File::Type::COMBINE_ARCHIVE);
}
#endif

TEST(I18nFileTest, russianFiles)
{
    EXPECT_EQ(libOpenCOR::File::create(libOpenCOR::resourcePath("i18n/папка/file.cellml"))->type(), libOpenCOR::File::Type::CELLML_FILE);
    EXPECT_EQ(libOpenCOR::File::create(libOpenCOR::resourcePath("i18n/папка/file.sedml"))->type(), libOpenCOR::File::Type::SEDML_FILE);
    EXPECT_EQ(libOpenCOR::File::create(libOpenCOR::resourcePath("i18n/папка/file.omex"))->type(), libOpenCOR::File::Type::COMBINE_ARCHIVE);
}
